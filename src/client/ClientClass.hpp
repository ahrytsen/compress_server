#ifndef CLIENTCLASS_HPP
# define CLIENTCLASS_HPP

# include "../MessageClass.hpp"
# include <cstdlib>
# include <iostream>
# include <string>
# include <boost/asio.hpp>
# include <boost/regex.hpp>

class Client {
	boost::asio::ip::tcp::socket & _socket;

	Client( void );
	Client( Client const & );
	Client&	operator=( Client const & );
	void	send_msg(Message & msg);
	void *	receive_payload( int32_t size );
	void	receive_msg( Message & msg );
	void	proccess(Message & msg);
public:
	Client( boost::asio::ip::tcp::socket & socket );
	void	run( void );
};

#endif
