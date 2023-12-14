#include <iostream>

#include <boost/asio/read_until.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/log/trivial.hpp>

#include "server.h"
#include "../Common/common.h"

boost::filesystem::path combinePath(std::string folder, std::string filename)
{
	boost::system::error_code ec;
	boost::filesystem::path p = boost::filesystem::system_complete(folder, ec);
	if (ec || !boost::filesystem::is_directory(p) || !boost::filesystem::exists(p))
		return boost::filesystem::path();
	p /= filename;
	return p;
}

// Recursively find the location of a file on a given directory 
int FindFile(const boost::filesystem::path& directory,
	boost::filesystem::path& path,
	const std::string& filename)
{
	int found = -1;
	if (!boost::filesystem::is_directory(directory) || !boost::filesystem::exists(directory))
		return found;

	const boost::filesystem::path file = filename;
	const boost::filesystem::recursive_directory_iterator end;
	const boost::filesystem::recursive_directory_iterator dir_iter(directory);

	found = 0;

	const auto it = std::find_if(dir_iter,
		end,
		[&file](const boost::filesystem::directory_entry& e)
	{
		return e.path().filename() == file;
	});

	if (it != end)
	{
		path = it->path();
		found = 1;
	}

	return found;
}


Session::Session(TcpSocket socket, Server* server)
    : m_socket(std::move(socket)), m_pServer(server)
{
}

std::string buffer_to_string(const boost::asio::streambuf &buffer)
{
	using boost::asio::buffers_begin;
	auto bufs = buffer.data();
	std::string result(buffers_begin(bufs), buffers_begin(bufs) + buffer.size());
	return result;
}

void Session::doRead()
{
    auto self = shared_from_this();
    async_read_until(m_socket, m_requestBuf, "\n\n",
        [this, self](boost::system::error_code ec, size_t bytes)
        {
			if (!ec)
				processRead(bytes);
			else
				std::cout << std::endl << "Server: Client disconnected." << std::endl << std::endl;
        });
}

void Session::doWriteFile(const boost::system::error_code& ec)
{
	if (!ec) 
	{
		m_sourceFile.read(m_buf.data(), m_buf.size()); 
		if (m_sourceFile.fail() && !m_sourceFile.eof()) 
		{
		    auto msg = "Server: Failed while reading file";
			std::cout << msg << std::endl << std::endl;
			m_sourceFile.close();

			m_pServer->EndFile(m_fileName);

			m_fileName = "";
			m_fileSize = 0;

			doRead();
			return;
		}

		if (m_sourceFile.gcount())
		{
			auto buf = boost::asio::buffer(m_buf.data(), static_cast<size_t>(m_sourceFile.gcount()));
			writeBufferAndFile(buf);

			std::cout << "Server: Sending file: " << m_fileName << " bytes sent: " << m_sourceFile.gcount() << " total size: " << m_fileSize << " bytes" << std::endl;
		}
		else
		{
			m_sourceFile.close();

			m_pServer->EndFile(m_fileName);
			
			m_fileName = "";
			m_fileSize = 0;

			doRead();
		}
	}
	else
	{
		std::cout << "Server: Error: " << ec.message() << std::endl;

		m_sourceFile.close();

		m_pServer->EndFile(m_fileName);

		m_fileName = "";
		m_fileSize = 0;

		doRead();
	}
}

WIN32_FIND_DATAA GetFileInfo(char const *path) { 
	WIN32_FIND_DATAA data;

	HANDLE h;

	h = FindFirstFileA(path, &data);
	FindClose(h);
	return data;
}

bool Session::openFile(std::string const& path, size_t& size, time_t& lastWriteTime)
{
	m_sourceFile.close();
	
	m_sourceFile.open(path, std::ios_base::binary | std::ios_base::in, _SH_DENYNO);
	if (m_sourceFile.fail())
	    return false;

	//WIN32_FIND_DATAA data = GetFileInfo(fullpath.c_str());

	lastWriteTime = boost::filesystem::last_write_time(path);
	
	m_sourceFile.seekg(0, m_sourceFile.end);
	auto fileSize = m_sourceFile.tellg();
	m_sourceFile.seekg(0, m_sourceFile.beg);
	size = (size_t)fileSize;

	return true;
}

void Session::processRead(size_t bytesTransferred)
{
    std::istream requestStream(&m_requestBuf);
	std::string req = "";
	std::string fileName = "";
	requestStream >> req;
	requestStream >> fileName;

	std::string s = buffer_to_string(m_requestBuf);
	std::cout << std::endl << "Server: Received REQ: " << req << " File name: " << fileName << std::endl;
	
	m_requestBuf.consume(2);

	if (req == GET_FILE)
	{
		// BeginFile will find file in cache or "download" it from other "sources" (copy from source folders)

		if(!m_pServer->BeginFile(fileName))
		{
			// file not found in source or could not be copied !!!
			doRead();
			return;
		}

		time_t lastWriteTime = {};
		size_t fileSize = 0;

		auto full_path = combinePath(m_pServer->GetCacheFolder(), fileName);
		if (openFile(full_path.string(), fileSize, lastWriteTime))
		{
			m_fileName = fileName;
			m_fileSize = fileSize;
			std::ostream sendStream(&m_sendBuf);
			sendStream << fileName << "\n" << fileSize << "\n\n";
			writeBufferAndFile(m_sendBuf);
			return;
		}
	}
	else if (req == GET_FILE_INFO)
	{
		// BeginFile will find file in cache or "download" it from other "sources" (copy from source folders)

		if (!m_pServer->BeginFile(fileName))
		{
			// file not found in source or could not be copied !!!
			doRead();
			return;
		}

		size_t fileSize = 0;
		time_t lastWriteTime = {};

		auto full_path = combinePath(m_pServer->GetCacheFolder(), fileName);
		if (openFile(full_path.string(), fileSize, lastWriteTime))
		{
			m_fileName = fileName;
			m_fileSize = fileSize;
			std::ostream sendStream(&m_sendBuf);
			
			sendStream << fileName << "\n" << fileSize << "\n" << lastWriteTime << "\n\n"; 

			time_t t = lastWriteTime;
			tm tM;
			localtime_s(&tM, &t);
			std::cout << "Server: Sending file info: " << m_fileName << " Size: " << m_fileSize << " Date modified: " << tM.tm_year + 1900 << "-" << tM.tm_mon << "-" << tM.tm_mday << "_" << tM.tm_hour << ":" << tM.tm_min << ":" << tM.tm_sec << std::endl;
			
			m_sourceFile.close();
			writeBuffer(m_sendBuf);
			return;
		}
	}
	else if (req == INVALIDATE_FILE)
	{
		m_pServer->InvalidateFile(fileName);
	}
	else if (req == INVALIDATE_ALL)
	{
		m_pServer->InvalidateFiles();
	}

	doRead();
}


Server::Server(IoService& ioService, short port, std::string const& cacheFolder, const std::vector<std::string>& sourceFolders)
    : m_socket(ioService),
    m_acceptor(ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
    m_cacheFolder(cacheFolder),
	m_context(boost::asio::ssl::context::sslv23)
{
    std::cout << "Server: Started listening on port: " << port << "\nServer: Cache folder: " << cacheFolder << std::endl;
	std::cout << "Server: Source folders: " << std::endl;

	m_sourceFolders = sourceFolders;

	for (auto s : m_sourceFolders)
		std::cout << s << std::endl;

	std::cout << std::endl;

	createCacheFolder();

	m_context.set_options(
		boost::asio::ssl::context::default_workarounds
		| boost::asio::ssl::context::no_sslv2
		| boost::asio::ssl::context::single_dh_use);
	m_context.set_password_callback(std::bind(&Server::get_password, this));
	m_context.use_certificate_chain_file("server.pem");
	m_context.use_private_key_file("server.pem", boost::asio::ssl::context::pem);
	m_context.use_tmp_dh_file("server_dh4096.pem");

    doAccept();
}


void Server::doAccept()
{
    m_acceptor.async_accept(m_socket,
        [this](boost::system::error_code ec)
    {
        if (!ec)
            std::make_shared<Session>(boost::asio::ssl::stream<boost::asio::ip::tcp::socket>(std::move(m_socket), m_context), this)->start();

        doAccept();
    });
}


void Server::createCacheFolder()
{
    using namespace boost::filesystem;
    auto currentPath = path(m_cacheFolder);
    if (!exists(currentPath) && !create_directory(currentPath))
        std::cout << "Server: Coudn't create working directory: " << m_cacheFolder << std::endl << std::endl;
}


bool Server::CheckFileInCache(const std::string& filename)
{
	boost::filesystem::path found;
	boost::system::error_code ec;
	return FindFile(boost::filesystem::system_complete(m_cacheFolder, ec), found, filename);
}

bool Server::CheckFileInCache(const std::string& filename, boost::filesystem::path& found)
{
	boost::system::error_code ec;
	return FindFile(boost::filesystem::system_complete(m_cacheFolder, ec), found, filename);
}

bool Server::DownloadFile(const std::string& filename)
{
	// File already in cache
	if (CheckFileInCache(filename))
		return true;

	boost::filesystem::path dest = combinePath(m_cacheFolder, filename);
	
	boost::system::error_code ec;

	for (auto s : m_sourceFolders)
	{
		std::string req = "";
		boost::filesystem::path folder = s;
		folder = boost::filesystem::system_complete(folder, ec);
		boost::filesystem::path found;
		int res = FindFile(folder, found, filename);
		if (res > 0)
		{
			boost::filesystem::copy(found, dest, ec);
			return !ec;
		}
		else if (res > -1) // File not found in one the folder
			std::cerr << std::endl << "Server: File not found! " << req << " File name: " << filename << std::endl;
		else // Folder not found
			std::cerr << std::endl << "Server: Folder not found: " << req << folder << std::endl;
	}

	return false;
}

bool Server::BeginFile(const std::string& filename)
{
	boost::lock_guard<boost::mutex> lock{ mutex };

	if (!DownloadFile(filename))
		return false;

	int count = m_filesProcessed[filename];
	m_filesProcessed[filename] = count + 1;
	
	return true;
}

void Server::EndFile(const std::string& filename)
{
	boost::lock_guard<boost::mutex> lock{ mutex };

	int count = m_filesProcessed[filename];
	if (count > 1)
		m_filesProcessed[filename] = count - 1;
	else
		m_filesProcessed.erase(filename);
}

void Server::InvalidateFile(const std::string& filename)
{
	boost::thread t([this, filename]()
	{
		bool fileCanbeInvalidated = false;
		while (!fileCanbeInvalidated)
		{
			{
				boost::lock_guard<boost::mutex> lock{ mutex };
				fileCanbeInvalidated = !CheckFile(filename);

				if (fileCanbeInvalidated)
				{
					boost::filesystem::path p = combinePath(m_cacheFolder, filename);
					boost::system::error_code ec;
					boost::filesystem::remove(p, ec);
				}
			}

			Sleep(0);
		}
	});
}

void Server::InvalidateFiles()
{
	boost::thread t([this]()
	{
		bool filesCanbeInvalidated = false;
		while (!filesCanbeInvalidated)
		{
			{
				boost::lock_guard<boost::mutex> lock{ mutex };
				filesCanbeInvalidated = !CheckFile("");

				if (filesCanbeInvalidated)
				{
					boost::system::error_code ec;
					boost::filesystem::path p = boost::filesystem::system_complete(m_cacheFolder, ec);

					if (ec)
						break;

					if (!boost::filesystem::is_directory(p))
						break;
					if (!boost::filesystem::exists(p))
						break;

					for (boost::filesystem::directory_iterator end_dir_it, it(p); it != end_dir_it; ++it)
					{
						boost::filesystem::remove_all(it->path(), ec);
					}
				}
			}

			Sleep(0);
		}
	});
}

bool Server::CheckFile(const std::string& filename)
{
	if(filename.empty())
		return m_filesProcessed.size() > 0;
	std::map<std::string, int>::iterator it = m_filesProcessed.find(filename);
	return it != m_filesProcessed.end();
}

bool Server::CheckFiles()
{
	return CheckFile("");
}
