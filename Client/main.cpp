#include <iostream>

#include <boost/asio/io_service.hpp>

#include "client.h"
#include "../Common/common.h"

int main(int argc, char* argv[])
{
	auto address = "";
	if(argc > 1)
		address = argv[1];
    auto port = std::to_string(PORT_NUM);
	if (argc > 2)
		port = argv[2];
    auto folder = "./ClientStore";
	if (argc > 3)
		folder = argv[3];

	std::string const cert = "-----BEGIN CERTIFICATE-----\n"
		"MIIDVDCCAjygAwIBAgIUX3cVQ47QyJp7SOy0RPzP743W9MwwDQYJKoZIhvcNAQEL\n"
		"BQAwOzELMAkGA1UEBhMCQVUxDDAKBgNVBAgMA05TVzEPMA0GA1UEBwwGU3lkbmV5\n"
		"MQ0wCwYDVQQKDARhc2lvMB4XDTIxMTExMTIxMTA1MloXDTI2MTExMDIxMTA1Mlow\n"
		"OzELMAkGA1UEBhMCQVUxDDAKBgNVBAgMA05TVzEPMA0GA1UEBwwGU3lkbmV5MQ0w\n"
		"CwYDVQQKDARhc2lvMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAyURD\n"
		"LjKxTCkapmWhY0bP1NaaOPzIJTTB0dzREOlmRmBmiHpW7DaRx7qBm6jYDKQ7OCbz\n"
		"30/j8K4TjHOLIwxzXhXMYTJOcN2giPHNUBvm9oEuDAhYgltArJQnBBEH+3C1hCIv\n"
		"1+uhTWo0HpGXTeJnvboTZ1YgmbOgr6lMhNiu9QmPX885DxWf6sDq8mRgCDX2x8sk\n"
		"Ls0HuLSo88Osjx532yEhnrZgexsByhoRD3yrKHV5mWpaunk5BMsP/XMuQHayFmbO\n"
		"siqnHJoL1znGVH003PcBGmEDmoIUqhLiBi2gWGu1pmckP9loqQUTEn0aLOVclHf4\n"
		"slWq344zh4tJCpQMfQIDAQABo1AwTjAdBgNVHQ4EFgQUfiX1CMQrGDi9mIBAg9cg\n"
		"m0RwLJUwHwYDVR0jBBgwFoAUfiX1CMQrGDi9mIBAg9cgm0RwLJUwDAYDVR0TBAUw\n"
		"AwEB/zANBgkqhkiG9w0BAQsFAAOCAQEAnDnVNSb8z/pNFaZ6YAZ+ukfNT3jjbGm1\n"
		"10BOLqJj8s5A8/JkwjaWhky/DuGXDywgEvzXC18aNAxASeqO8h9pAZtszu6NWB4s\n"
		"h3r+dEQakMacxrZ+jBL/cYLrUv9r3KMPKxaDnplkamqFA/9eNmoV7vDyGtGPZuD6\n"
		"oTROtQqqDSrxthCkqnibAfusi7RmlCdvJa0kVK7krKJZAhi8W9f32+lBPv9oq3Ns\n"
		"dAxnOj/D3UnhNoIt0EdjqUdLo2U39dt5s5Syj2rFUAgfbc02Rwx65kq8AjTRlW/M\n"
		"KDpGsifwIB8b7wActMUO8c3GptptNVWmFm5+Mmk54P//P3tIAx9KXw==        \n"
		"-----END CERTIFICATE-----                                       \n"
		"-----BEGIN ENCRYPTED PRIVATE KEY-----                           \n"
		"MIIFHDBOBgkqhkiG9w0BBQ0wQTApBgkqhkiG9w0BBQwwHAQIPcVUeQ7ZElgCAggA\n"
		"MAwGCCqGSIb3DQIJBQAwFAYIKoZIhvcNAwcECGKlFVN6/gIlBIIEyJPeknsA5dvV\n"
		"WK27AZzs4wM6WrsD6ba+kPJyZTpon5pn4eoTiw4UCvo7C+21G9jCqbIbDrgTWHwH\n"
		"zu6YrBFTZgRUpdLkLsyUsp4UJrZ8xJ7N/jWlG763CyluFE5oBFLz2Vt/DSDSaWdA\n"
		"sKdxua/1kvw+KYoDqlObMFIspM5TrndcMnWkOgyvQAvnH0NZXFFBa4QGFrwWDrCo\n"
		"m12CzMLwaPMrNFBffCTqZnL1AajT+EYqPTi+cQ5mkRpZ2cuzhdug2eki1KkD12ZN\n"
		"3d8Ehl8bfbLEqRE/HnggleRRt7ia1xkzruqQp14JA2UsrioGMF5nvWgWrbyHfpui\n"
		"KrCsDwDelrArW/GCki53QBQMuH8Q1YmoNrRZwlG257eA1LiJvmxRyIjKYu0RP+Q8\n"
		"EOldycy51VsPVPApUbv9r4IJldrIJiwlmRxxM36uKtFhfojnsWzJORDYI8C37uKi\n"
		"UEPiD4GSEH6C0lGO94IoZPjl9z/rZ0qZx1VRHl3nxZc0AQvvj9bWMbyRwsgb6wod\n"
		"P8JSy6uEib9UxuenNHzTd48GcNhJbhOqd4IV0XKhi8W1Kil3mMdc3QAwKaLTx12/\n"
		"1GrbMui061uyeM6Rf6Xao0PApDnUNmxcFSTsoarG0yH7Q0WZMgKTDCCGhhtZKlE6\n"
		"x7pRsnxiFaIpoh32EVIRI+ZXh2rXBcwa2ew0aEccRXtPFvcmdjevkrHuCggc+M+Y\n"
		"seFqTHVGWf8eS6o08w095DboD0vFpZXZMRfycTbA7BiE4NYE/uc7v5cH3Ax9l2ef\n"
		"zG7o9idDt+/RX7OcetxDFw4eQbq7PfjvrfXS1DcRUEyJ04emh7oxlkAUUNsFtabN\n"
		"T0ggvHxcQWkYRE5oPlkbgpwKpK4LDcApXKFwCDKPur2W5Q7KHRfDLtSvZvYarJum\n"
		"8j2pGpEis/bdTih29ofNsX6a0Yo5Tlj+9+1c/6/Xi7XvRk/Vbgkoa3iVQ3ckdCuZ\n"
		"vO7cRYZBBs6W/Ti3hWFzPEX9FfcnSUp9bEnH4ASnmUcp8PDBJYeaI6HqpO3XbkbF\n"
		"l70eDNakd2cDNjQzkFpBT7HO+HtqU3xNt9K0z2gMB7iTDVFyIzh26joCR2O8tiqS\n"
		"wUJFkoLPb74GSB7WzE+gb4jXX6n9609PUoR3f896mM34uX3CsY8lA+/0ZGpHnFDA\n"
		"ToKdmz6WKIAw0E20nyzLuLwPZgj7HLcR7zza4raipe9QxIdyJr5O+jzGt+OjSM9b\n"
		"K1agibRE5DChqQ+P+ikOc6nG7UNyn+lKSjGEbwuzi8F0iugMgcTc/vYO8OWDNGpd\n"
		"D1euA5OuVPdfatFa16Fyr4MJJIfE83C4/kSj05fdoyb6pJkOjHhUppVMe+ES5kwl\n"
		"YI8RES2XVJzUSxnWsIM613YlMgIZ9xgcuIADnO7gaKJ4RQG+9wJ853Uo4+n89K7F\n"
		"Y6KzihuYAUkbAw1pPo1TODom7A/gB9stqRUSQlZWmIgtnJ8wPjt/we0gzPM8LQzb\n"
		"ye4KOLjH5iquFc4MU4TtN3gvp9P5R9UFrGeMImS5eMUmBQHckDNdIAtMj7M1wUpR\n"
		"JVrzDXWDjC21sLn95NtNMPb+FRlt/biTV3IE3ZmX0kbuCRoH7b7hhR41Zpoajwl0\n"
		"FqYigB5gnVodGUWuBgDUqA==\n"
		"-----END ENCRYPTED PRIVATE KEY-----\n"
		;

    try {
		boost::asio::io_context io_context;

        boost::asio::ip::tcp::resolver resolver(io_context);
		auto endpoints = resolver.resolve(address, port);

		boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);

		boost::system::error_code ec;
		ctx.add_certificate_authority(
			boost::asio::buffer(cert.data(), cert.size()), ec);
		if (ec)
			return - 1;

        Client client(io_context, ctx, endpoints, folder);

		bool res = false;

		while (client.isConnected())
		{
			if (GetConsoleWindow() == GetForegroundWindow() && GetAsyncKeyState(VK_ESCAPE))
				break;
			
			io_context.poll();
			
			if (client.isConnected() > 1)
			{
				res = client.sendRequest(GET_FILE, "1.jpg");
				if (!res)
					break;
				res = client.sendRequest(GET_FILE_INFO, "1.jpg");
				if (!res)
					break;
				res = client.sendRequest(INVALIDATE_FILE, "1.jpg");
				if (!res)
					break;

				res = client.sendRequest(GET_FILE, "1.jpg");
				if (!res)
					break;

				res = client.sendRequest(GET_FILE_INFO, "1.jpg");
				if (!res)
					break;

				res = client.sendRequest(GET_FILE, "2.jpg");
				if (!res)
					break;
				res = client.sendRequest(GET_FILE_INFO, "2.jpg");
				if (!res)
					break;

				res = client.sendRequest(GET_FILE, "3.jpg");
				if (!res)
					break;
				res = client.sendRequest(GET_FILE_INFO, "3.jpg");
				if (!res)
					break;

				res = client.sendRequest(INVALIDATE_ALL, "ALL");
				if (!res)
					break;
			}

			Sleep(10);
		}

    } catch (std::fstream::failure& e) {
        std::cerr << e.what() << "\n";
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

	std::cout << "Disconnected! Shutting down. \n";
    return 0;
}
