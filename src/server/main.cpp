#include "ServerClass.hpp"
#include <iostream>

int	main( void ) {
	try {
		boost::asio::io_service	io_service;
		Server server(io_service);
		server.run();
	}
	catch (std::exception & e) {
		std::cout << "Fatal error: " << e.what() << std::endl;
	}
}
