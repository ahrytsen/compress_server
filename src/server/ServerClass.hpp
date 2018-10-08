#ifndef SERVERCLASS_HPP
# define SERVERCLASS_HPP

# include <boost/asio.hpp>
# include <boost/regex.hpp>
# include <boost/thread/thread.hpp>
# include <string>
# include <mutex>
# include "../MessageClass.hpp"

# define DEFAULT_PORT 4000
# define MAX_PAYLOAD_SIZE 0x8000

class Server {
	mutable boost::mutex			_server_mutex;
	Message::ResponseCode			_status;
	uint32_t						_received_total;
	uint32_t						_sent_total;
	uint32_t	 					_received_data;
	uint32_t						_compress_data;
	boost::asio::io_service &		_io_service;
	boost::asio::ip::tcp::acceptor	_acceptor;

	Server( void );
	Server( Server const & );
	Server &						operator=( Server const & );
	void							incTotalCompressData_by( uint32_t const & size );
	void							incTotalReceiveData_by( uint32_t const & size );
	void							incTotalSent_by( uint32_t const & size );
	void							incTotalReceive_by( uint32_t const & size );
	uint32_t						getTotalReceived( void ) const;
	uint32_t						getTotalSent( void ) const;
	uint8_t							getCompressionRatio( void ) const;
	Message::ResponseCode			getServerStatus( void ) const;
	void							setServerStatus(Message::ResponseCode code);
	void							resetCounters( void );
	void							reject_payload( boost::asio::ip::tcp::socket & sock, uint32_t size );
	void*							receive_payload( boost::asio::ip::tcp::socket & sock, uint32_t size );
	void							get_request( boost::asio::ip::tcp::socket & sock, Message & request );
	void							proccess_request( Message & request, Message & response );
	void							send_response( boost::asio::ip::tcp::socket & sock, Message & response );
	void							session( uint16_t session_id, boost::shared_ptr< boost::asio::ip::tcp::socket > sock );
	void							ping( Message & response );
	void							get_status( Message & response );
	void							reset_status( Message & response );
	void							compress( Message & request, Message & response );
public:
	Server( boost::asio::io_service & io_service );
	Server( boost::asio::io_service & io_service, uint16_t port );
	void	run( void );
	~Server() {};

	class	RequestException : public std::exception {
		Message::ResponseCode	_code;
		std::string				_what;
	public:
		RequestException( void );
		RequestException( Message::ResponseCode code );
		RequestException( RequestException const & e );
		virtual ~RequestException( void );
		RequestException&	operator=( RequestException const & e );
		virtual Message::ResponseCode code() const;
		virtual const char* what() const throw();
	};

	struct	Status {
		uint32_t	total_received;
		uint32_t	total_sent;
		uint8_t		compression_ratio;
	};
};

#endif
