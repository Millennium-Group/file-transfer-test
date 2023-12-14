#pragma once

#include <array>
#include <fstream>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>


class Client
{
public:
    using IoContext = boost::asio::io_context;
    using TcpResolver = boost::asio::ip::tcp::resolver;
    using TcpSocket = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;

    Client(IoContext& io_context,
        boost::asio::ssl::context& context, const boost::asio::ip::tcp::resolver::results_type endpoints,
        std::string const& path);

	bool sendRequest(std::string const& req, std::string const& filename);
	int isConnected()
	{
		return m_connected;
	}

private:
    bool verify_certificate(bool preverified,
        boost::asio::ssl::verify_context& ctx)
    {
        // The verify callback can be used to check whether the certificate that is
        // being presented is valid for the peer. For example, RFC 2818 describes
        // the steps involved in doing this for HTTPS. Consult the OpenSSL
        // documentation for more details. Note that the callback is called once
        // for each certificate in the certificate chain, starting from the root
        // certificate authority.

        // In this example we will simply print the certificate's subject name.
        char subject_name[256];
        X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
        X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
        std::cout << "Verifying " << subject_name << "\n";

        return preverified;
    }
    void handshake(const boost::asio::ip::tcp::endpoint& endpoint)
    {
        m_socket.async_handshake(boost::asio::ssl::stream_base::client,
            [this, endpoint](const boost::system::error_code& error)
            {
                if (!error)
                {
                    m_connected = 2;
                    std::cout << "Client: Connected to server on port: " << endpoint.port() << std::endl;
                }
                else
                {
                    m_connected = 0;
                    std::cout << "Handshake failed: " << error.message() << "\n";
                }
            });
    }

    void doConnect(const boost::asio::ip::tcp::resolver::results_type endpoints);

    template<class Buffer>
    size_t writeBuffer(Buffer& buffer);

	void doReadFileContent(size_t bytesTransferred);

    TcpResolver m_ioContext;
    TcpSocket m_socket;
    enum { MessageSize = 1024 };
    std::array<char, MessageSize> m_buf;
    boost::asio::streambuf m_request;
    
    std::string m_path;
	int m_connected;

	std::ofstream m_outputFile;
	size_t m_fileSize;
	std::string m_fileName;
};


template<class Buffer>
size_t Client::writeBuffer(Buffer& buffer)
{
	return boost::asio::write(m_socket, buffer);
}
