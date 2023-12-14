#include <string>
#include <iostream>

#include <boost/asio/read_until.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/log/trivial.hpp>

#include "client.h"
#include "../Common/common.h"
#include <boost/bind/placeholders.hpp>
using std::placeholders::_1;
using std::placeholders::_2;

void createStoreFolder(std::string folder)
{
	using namespace boost::filesystem;
	auto currentPath = path(folder);
	if (!exists(currentPath) && !create_directory(currentPath))
		std::cout << "Client: Coudn't create working directory: " << folder << std::endl;
}


Client::Client(IoContext& ioContext, boost::asio::ssl::context& context, const boost::asio::ip::tcp::resolver::results_type endpoints,
    std::string const& path)
    : m_ioContext(ioContext), m_socket(ioContext, context),
    m_path(path)
{
	createStoreFolder(m_path);
	m_connected = 1;

	m_socket.set_verify_mode(boost::asio::ssl::verify_peer);
	m_socket.set_verify_callback(
		std::bind(&Client::verify_certificate, this, _1, _2));

    doConnect(endpoints);
}

void Client::doReadFileContent(size_t bytesTransferred)
{
	if (bytesTransferred > 0) {
		m_outputFile.write(m_buf.data(), static_cast<std::streamsize>(bytesTransferred));

		if (m_outputFile.tellp() >= static_cast<std::streamsize>(m_fileSize))
		{
			m_outputFile.close();
			std::cout << "Client: Received file: " << m_fileName << std::endl;
			m_fileName = "";
			m_fileSize = 0;
			return;
		}
	}
	
	size_t bytes = m_socket.read_some(boost::asio::buffer(m_buf.data(), m_buf.size()));
	doReadFileContent(bytes);
}

bool Client::sendRequest(std::string const& req, std::string const& filename)
{
	boost::asio::streambuf request;

	std::ostream requestStream(&request);
	requestStream << req << "\n" << filename << "\n\n";
	if (!writeBuffer(request))
		return false;

	if (req == GET_FILE)
	{
		boost::asio::streambuf requestBuf;
		boost::system::error_code ec;
		boost::asio::read_until(m_socket, requestBuf, "\n\n", ec);
		if (ec)
			return false;
		
		std::istream stream(&requestBuf);
		stream >> m_fileName;
		m_fileName = filename;
		stream >> m_fileSize;
		stream.read(m_buf.data(), 2);

		boost::filesystem::path p = m_path;
		p = boost::filesystem::system_complete(p, ec);
		if (ec || !boost::filesystem::is_directory(p) || !boost::filesystem::exists(p))
			return false;
		p /= filename;
		m_outputFile.open(p.string(), std::ios_base::binary | std::ios_base::out);
		size_t bytes = m_socket.read_some(boost::asio::buffer(m_buf.data(), m_buf.size()));
		if (bytes)
			doReadFileContent(bytes);
	}
	else if (req == GET_FILE_INFO)
	{
		boost::asio::streambuf requestBuf;
		boost::system::error_code ec;
		boost::asio::read_until(m_socket, requestBuf, "\n\n", ec);
		if (ec)
			return false;

		std::istream stream(&requestBuf);
		stream >> m_fileName;
		m_fileName = filename;
		stream >> m_fileSize;
		
		std::string lastWriteTime = "";
		stream >> lastWriteTime;

		stream.read(m_buf.data(), 2);

		time_t t = std::atoi(lastWriteTime.c_str());
		tm tM;
		localtime_s(&tM, &t);
		std::cout << "Client: File info: " << m_fileName << " Size: " << m_fileSize << " Date modified: " << tM.tm_year + 1900 << "-" << tM.tm_mon << "-" << tM.tm_mday << "_" << tM.tm_hour << ":" << tM.tm_min << ":" << tM.tm_sec << std::endl;
		
		m_fileName = "";
		m_fileSize = 0;
	}

	return true;
}

void Client::doConnect(const boost::asio::ip::tcp::resolver::results_type endpoints)
{
    boost::asio::async_connect(m_socket.lowest_layer(), endpoints,
        [this](boost::system::error_code ec, const boost::asio::ip::tcp::endpoint& endpoint)
        {
            if (!ec) {
				handshake(endpoint);
            } else {
				m_connected = 0;
                std::cout << "Client: Coudn't connect to host. Please run server "
                    "or check network connection.\n";
            }
        });
}

