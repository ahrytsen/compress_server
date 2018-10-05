#include "ServerClass.hpp"

bool	Server::get_request(boost::asio::ip::tcp::socket & sock, Message & request) {
	boost::system::error_code error;
	_received_total += boost::asio::read(sock, boost::asio::buffer(request.getHeader(), sizeof(Message::Header)),
										 boost::asio::transfer_exactly(sizeof(Message::Header)), error);
	if (error == boost::asio::error::eof) return false;
	else throw boost::system::system_error(error);

	return true;
}

void	Server::send_responce() {

}

void	Server::session( boost::shared_ptr< boost::asio::ip::tcp::socket > sock ) {
	try
	{
		for (;;)
		{
			Message	request;
			if (!get_request(*sock, request)) break;

			//boost::asio::write(*sock, boost::asio::buffer(reinterpret_cast<void*>(&header),
			//											  sizeof(Message::Header)));
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception in thread: " << e.what() << "\n";
	}
}

std::string	Server::compressPayload( char const * bufer, uint16_t size) {
	std::string	input(bufer, size);
	return boost::regex_replace(input, boost::regex("([a-z])\\1{2,}"),
								[](boost::smatch const& match) {
									return std::to_string(match[0].str().size()) + match[1].str();
								});
}

Server::Server( boost::asio::io_service & io_service ) :
	_io_service( io_service ),
	_acceptor( _io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), DEFAULT_PORT)) {}

Server::Server( boost::asio::io_service & io_service, uint16_t port ) :
	_io_service( io_service ),
	_acceptor( _io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {}

void	Server::run( void ) {
	char const * lol("aaaaaaabbccccccccccccccccccccccccccccccccccccccccccccccccccdddddiiif");
	std::cout << compressPayload(lol, strlen(lol)) << std::endl;
	for (;;)
	{
		boost::shared_ptr< boost::asio::ip::tcp::socket > sock(new boost::asio::ip::tcp::socket(_io_service));
		_acceptor.accept(*sock);
		boost::thread t(boost::bind(&Server::session, this, sock));
	}
}
