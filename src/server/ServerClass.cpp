#include "ServerClass.hpp"

void	Server::incTotalCompressData_by( uint32_t const & size ) {
	boost::lock_guard<boost::mutex> lock(_server_mutex);
	_compress_data += size;
}

void	Server::incTotalReceiveData_by( uint32_t const & size ) {
	boost::lock_guard<boost::mutex> lock(_server_mutex);
	_received_total += size;
	_received_data += size;
}

void	Server::incTotalSent_by( uint32_t const & size ) {
	boost::lock_guard<boost::mutex> lock(_server_mutex);
	_sent_total += size;
}

void	Server::incTotalReceive_by( uint32_t const & size ) {
	boost::lock_guard<boost::mutex> lock(_server_mutex);
	_received_total += size;
}

uint32_t	Server::getTotalReceived( void ) const {
	boost::lock_guard<boost::mutex> lock(_server_mutex);
	return _received_total;
}

uint32_t	Server::getTotalSent( void ) const {
	boost::lock_guard<boost::mutex> lock(_server_mutex);
	return _sent_total;
}

uint8_t		Server::getCompressionRatio( void ) const {
	boost::lock_guard<boost::mutex> lock(_server_mutex);
	if(!_received_data) return 0;
	double tmp = static_cast<double>(_compress_data) / static_cast<double>(_received_data);
	return static_cast<uint8_t>(tmp * 100);
}

Message::ResponseCode	Server::getServerStatus( void ) const {
	boost::lock_guard<boost::mutex> lock(_server_mutex);
	return _status;
}

void		Server::setServerStatus(Message::ResponseCode code) {
	boost::lock_guard<boost::mutex> lock(_server_mutex);
	_status = code;
}

void		Server::resetCounters( void ) {
	boost::lock_guard<boost::mutex> lock(_server_mutex);
	_status = Message::response_ok;
	_received_total = 0;
	_sent_total = 0;
	_received_data = 0;
	_compress_data = 0;
}

void	Server::reject_payload( boost::asio::ip::tcp::socket & sock, uint16_t size ) {
	uint8_t buf[MAX_PAYLOAD_SIZE];
	while (size) {
		size -= sock.read_some(boost::asio::buffer(buf, MAX_PAYLOAD_SIZE));
	}
}

void *	Server::receive_payload( boost::asio::ip::tcp::socket & sock, uint16_t size ) {
	if ( !size )
		throw Server::RequestException( Message::empty_payload );
	else if ( size > MAX_PAYLOAD_SIZE) {
		reject_payload(sock, size);
		throw Server::RequestException( Message::message_too_large );
	}
	uint32_t	len;
	std::unique_ptr<uint8_t[]>	buf( new uint8_t[size + 1] );
	buf[size] = '\0';
	len = boost::asio::read(sock, boost::asio::buffer(buf.get(), size), boost::asio::transfer_exactly(size));
	incTotalReceiveData_by(len);
	return buf.release();
}

void	Server::get_request( boost::asio::ip::tcp::socket & sock, Message & request ) {
	boost::asio::read(sock,
					  boost::asio::buffer(request.getHeader(), sizeof(Message::Header)),
					  boost::asio::transfer_exactly(sizeof(Message::Header))
		);
	request.toHostOrder();
	incTotalReceive_by(sizeof(Message::Header));
	if ( !request.checkMagic()) throw Server::RequestException( Message::unsupported_request_type );
	if ( static_cast<Message::RequestCode>(request.getCode()) == Message::compress )
		request.setPayload(receive_payload(sock, request.getPayloadLength()));
	else if (request.getPayloadLength())
		reject_payload(sock, request.getPayloadLength());
}

void	Server::send_response( boost::asio::ip::tcp::socket & sock, Message & response ) {
	uint32_t	payload_len = response.getPayloadLength();
	response.toNetworkOrder();
	boost::asio::write(sock,
					   boost::asio::buffer(response.getHeader(), sizeof(Message::Header)),
					   boost::asio::transfer_exactly(sizeof(Message::Header))
		);
	incTotalSent_by(sizeof(Message::Header));
	if (payload_len && response.getPayload()) {
		payload_len = boost::asio::write(sock,
										 boost::asio::buffer(response.getPayload(), payload_len),
										 boost::asio::transfer_exactly(payload_len)
			);
		incTotalSent_by(payload_len);
	}
}

void	Server::session( uint16_t session_id, boost::shared_ptr< boost::asio::ip::tcp::socket > sock ) {
	for (;;)
	{
		try {
			Message	request;
			get_request(*sock, request);
			Message	response;
			proccess_request(request, response);
			send_response(*sock, response);

		}
		catch (Server::RequestException & e) {
			std::cout << "Session №" << session_id << ": " << e.what() << std::endl;
			Message	response(static_cast<uint16_t>(e.code()));
			send_response(*sock, response);
		}
		catch (boost::system::system_error & e) {
			if (e.code() != boost::asio::error::eof
				&& e.code() != boost::asio::error::connection_reset) {
				std::cerr << "Fatal error in session №" << session_id
						  << ": " << e.what() << std::endl;
				setServerStatus(Message::system_error);
			} else
				std::cout << "Session №" << session_id << " will be closed!" << std::endl;
			break ;
		}
		catch (std::bad_alloc & e) {
			std::cerr << "Fatal error in session №" << session_id
					  << ": bad allocation" << std::endl;
			setServerStatus(Message::bad_allocation);
		}
		catch (std::exception & e) {
			std::cerr << "Fatal error in session №" << session_id
						  << ": " << e.what() << std::endl;
			setServerStatus(Message::system_error);
			break ;
		}
		catch (...) {
			std::cerr << "Unknown fatal error in session №" << session_id << std::endl;
			setServerStatus(Message::system_error);
			break ;
		}
	}
}

void	Server::proccess_request( Message & request, Message & response ) {
	switch ( static_cast<Message::RequestCode>(request.getCode()) ) {
	case Message::ping:
		ping(response);
		break ;
	case Message::get_status:
		get_status(response);
		break ;
	case Message::reset_status:
		reset_status(response);
		break ;
	case Message::compress:
		compress(request, response);
		break ;
	default:
		throw Server::RequestException( Message::unsupported_request_type );
	}
}

void	Server::ping( Message & response ) {
	response.setCode(_status);
}

void	Server::get_status( Message & response ) {
	uint16_t	payload_size = 2 * sizeof(uint32_t) + sizeof(uint8_t);
	std::unique_ptr<uint8_t[]>	buf( new uint8_t[payload_size] );
	uint32_t	total_sent = getTotalSent();
	uint32_t	total_received = getTotalReceived();
	uint8_t		compress_ratio = getCompressionRatio();
	memcpy(buf.get(), &total_received, sizeof(total_received));
	memcpy(buf.get() + sizeof(uint32_t), &total_sent, sizeof(total_sent));
	memcpy(buf.get() + 2 * sizeof(uint32_t), &compress_ratio, sizeof(compress_ratio));
	response.setPayload(buf.release());
	response.setPayloadLength(payload_size);
}

void	Server::reset_status( Message & response ) {
	resetCounters();
	response.setCode(Message::response_ok);
}

void	Server::compress( Message & request, Message & response) {
	std::string	input(reinterpret_cast< char const* >(request.getPayload()));
	if (!boost::regex_search(input, boost::regex("^[a-z]+$")))
		throw Server::RequestException( Message::unsupported_payload );
	std::string	output (boost::regex_replace(input,
											 boost::regex("([a-z])\\1{2,}"),
											 [](boost::smatch const& match) {
												 return std::to_string(match[0].str().size()) + match[1].str();
											 }
							));
	response.setPayloadLength(output.size());
	char * tmp = new char[output.size()];
	memcpy(tmp, output.c_str(), output.size());
	incTotalCompressData_by(output.size());
	response.setPayload(tmp);
}

Server::Server( boost::asio::io_service & io_service ) :
	_status( Message::response_ok ),
	_received_total(0),
	_sent_total(0),
	_received_data(0),
	_compress_data(0),
	_io_service( io_service ),
	_acceptor( _io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), DEFAULT_PORT))
{}

Server::Server( boost::asio::io_service & io_service, uint16_t port ) :
	_status( Message::response_ok ),
	_received_total(0),
	_sent_total(0),
	_received_data(0),
	_compress_data(0),
	_io_service( io_service ),
	_acceptor( _io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{}

void	Server::run( void ) {
	for (uint16_t id = 1; ; id++)
	{
		boost::shared_ptr< boost::asio::ip::tcp::socket >
			sock(new boost::asio::ip::tcp::socket(_io_service));
		_acceptor.accept(*sock);
		boost::thread t(boost::bind(&Server::session, this, id, sock));
	}
}

Server::RequestException::RequestException( void )
	: _code(Message::unknown_error), _what("Unknown error!") {}

Server::RequestException::RequestException( Message::ResponseCode code )
	: _code(code) {
	switch(code) {
	case Message::response_ok:
		_what = "OK!";
		break ;
	case Message::message_too_large:
		_what = "Message Too Large";
		break ;
	case Message::unsupported_request_type:
		_what = "Unsupported Request Type";
		break ;
	case Message::unsupported_payload:
		_what = "Unsupported Payload Sequence";
		break ;
	case Message::unknown_error:
	default:
		_what = "Unknown Error";
	}
}

Server::RequestException::RequestException( RequestException const & e )
	: _code(e._code), _what(e._what) {}

Server::RequestException::~RequestException( void ) {}

Server::RequestException&	Server::RequestException::operator=( Server::RequestException const & e ) {
	_code = e._code;
	_what = e._what;
	return *this;
}

const char*	Server::RequestException::what( void ) const throw() {
	return _what.c_str();
}

Message::ResponseCode	Server::RequestException::code( void ) const {
	return _code;
}
