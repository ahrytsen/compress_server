#ifndef SERVERCLASS_HPP
# define SERVERCLASS_HPP

# include <boost/asio.hpp>
# include <boost/regex.hpp>
# include <boost/thread/thread.hpp>
# include <string>
# include <mutex>
# include "MessageClass.hpp"

# define DEFAULT_PORT 4000

class Server {
	std::mutex						_server_mutex;
	uint16_t						_status;
	uint32_t						_received_total;
	uint32_t						_sent_total;
	uint32_t	 					_received_data;
	uint32_t						_compress_data;
	boost::asio::io_service &		_io_service;
	boost::asio::ip::tcp::acceptor	_acceptor;

	Server( void );
	Server( Server const & );
	Server &						operator=( Server const & );
	bool							get_request( boost::asio::ip::tcp::socket & sock, Message & request );
	void							send_responce();
	void							session( boost::shared_ptr< boost::asio::ip::tcp::socket > sock );
	std::string						compressPayload( char const * bufer, uint16_t size);
public:
	Server( boost::asio::io_service & io_service );
	Server( boost::asio::io_service & io_service, uint16_t port );
	void	run( void );
	~Server() {};
};

#endif
