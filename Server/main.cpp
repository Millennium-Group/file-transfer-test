#include <iostream>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ssl.hpp>

#include "server.h"
#include "../Common/common.h"

#define CACHE_FOLDER "./Cache"
#define SOURCE_FOLDER "./Source"

int main(int argc, char* argv[])
{
    //if (argc < 2) {
    //    std::cerr << "Usage: server <port> [cacheFolder]\n";
    //    return 1;
    //}

	auto port_num = PORT_NUM;
	if (argc > 1)
		port_num = std::stoi(argv[1]);
	
	auto cache_folder = CACHE_FOLDER;
	if (argc > 2)
		cache_folder = argv[2];

	std::vector<std::string> sourceFolders;
	int c = 3;
	while(c < argc)
		sourceFolders.push_back(argv[c++]);

	if (sourceFolders.empty())
		sourceFolders.push_back(SOURCE_FOLDER);

    try {
        boost::asio::io_service ioService;

        Server server(ioService, port_num, cache_folder, sourceFolders);

        ioService.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
