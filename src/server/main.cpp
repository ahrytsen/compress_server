#include "ServerClass.hpp"
#include <iostream>

int	main( int ac, char** av ) {
	try {
		boost::asio::io_service	io_service;
		if (ac < 2) {
			Server server(io_service);
			server.run();
		} else {
			Server server(io_service, std::stoi(av[1]));
			server.run();
		}
	}
	catch (std::exception & e) {
		std::cout << "Fatal error: " << e.what() << std::endl;
	}
}
