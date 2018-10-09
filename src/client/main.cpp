#include "ClientClass.hpp"

int main(int ac, char** av)
{
	try {
		if (ac != 3) {
			std::cerr << "Usage: client <host> <port>" <<std::endl;
			return 1;
		}
		boost::asio::io_service io_service;
		boost::asio::ip::tcp::resolver resolver(io_service);
		boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), av[1], av[2]);
		boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query);
		boost::asio::ip::tcp::socket s(io_service);
		boost::asio::connect(s, it);

		Client	client(s);
		client.run();
	}
	catch (std::exception& e) {
		std::cerr << "Fatal error: " << e.what() << std::endl;
	}
	return 0;
}
