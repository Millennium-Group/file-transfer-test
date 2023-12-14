#pragma once

#include <array>
#include <fstream>
#include <string>
#include <memory>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

class Server;
class Session
    : public std::enable_shared_from_this<Session>
{
public:
    //using TcpSocket = boost::asio::ip::tcp::socket;
	using TcpSocket = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;

    Session(TcpSocket socket, Server* server);

    void start()
    {
		doHandshake();
    }

private:
	void doHandshake()
	{
		auto self(shared_from_this());
		m_socket.async_handshake(boost::asio::ssl::stream_base::server,
			[this, self](const boost::system::error_code& error)
			{
				if (!error)
				{
					std::cout << std::endl << "Client connected!" << std::endl << std::endl;
					doRead();
				}
			});
	}

	void doWriteFile(const boost::system::error_code& ec);
    void doRead();
    void processRead(size_t bytesTransferred);
	bool openFile(std::string const& path, size_t& size, time_t& lastWriteTime);

	template<class Buffer>
	void writeBuffer(Buffer& buffer)
	{
		auto self(shared_from_this());
		boost::asio::async_write(m_socket,
		    buffer,
		    [this, self](boost::system::error_code ec, size_t /*length*/)
		    {
				m_pServer->EndFile(m_fileName);

				m_fileName = "";
				m_fileSize = 0;
				
				doRead();
		    });
	}

	template<class Buffer>
	void writeBufferAndFile(Buffer& buffer)
	{
		auto self(shared_from_this());
		boost::asio::async_write(m_socket,
			buffer,
			[this, self](boost::system::error_code ec, size_t /*length*/)
		{
			doWriteFile(ec);
		});
	}

    TcpSocket m_socket;
    enum { MaxLength = 40960 };
    std::array<char, MaxLength> m_buf;
    boost::asio::streambuf m_requestBuf;
	boost::asio::streambuf m_sendBuf;
    std::ofstream m_outputFile;
    size_t m_fileSize;
    std::string m_fileName;
	std::ifstream m_sourceFile;
	Server* m_pServer = NULL;
};


class Server
{
	friend class Session;

public:
    using TcpSocket = boost::asio::ip::tcp::socket;
    using TcpAcceptor = boost::asio::ip::tcp::acceptor;
    using IoService = boost::asio::io_service;

    Server(IoService& ioService, short port, std::string const& cacheFolder, const std::vector<std::string>& sourceFolders);

	const std::string& GetCacheFolder()
	{
		return m_cacheFolder;
	}

	std::string get_password() const
	{
		return "test";
	}

private:
	bool CheckFileInCache(const std::string& filename);
	bool CheckFileInCache(const std::string& filename, boost::filesystem::path& found);
	bool DownloadFile(const std::string& filename);

	bool BeginFile(const std::string& filename);
	void EndFile(const std::string& filename);

	void InvalidateFile(const std::string& filename);
	void InvalidateFiles();

	bool CheckFile(const std::string& filename);
	bool CheckFiles();


    void doAccept();
    void createCacheFolder();

    TcpSocket m_socket;
    TcpAcceptor m_acceptor;
	boost::asio::ssl::context m_context;

    std::string m_cacheFolder;
	std::vector<std::string> m_sourceFolders;

	boost::mutex mutex;
	std::map<std::string, int> m_filesProcessed;
};
