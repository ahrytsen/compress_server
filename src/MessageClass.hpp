#ifndef MESSAGECLASS_HPP
# define MESSAGECLASS_HPP

# include <arpa/inet.h>
# include <iostream>

# define MAGIC_VALUE 0x53545259

class Message {
public:
	enum	RequestCode {
		ping = 1,
		get_status,
		reset_status,
		compress,
	};
	enum	ResponseCode {
		response_ok = 0,
		unknown_error,
		message_too_large,
		unsupported_request_type,
		empty_payload = 33,
		unsupported_payload,
//server errors(non-request specific):
		bad_allocation,
		system_error,
	};
	struct Header {
		uint32_t	magic_value;
		uint16_t	payload_length;
		uint16_t	code;
	};
	Message( void );
	Message ( uint16_t const & code );
	Message( uint32_t const & payload_length, uint16_t const & code, void const * & payload );
	Header *		getHeader( void );
	bool			checkMagic( void );
	uint16_t		getCode( void ) const;
	void const*		getPayload( void ) const;
	uint16_t		getPayloadLength( void ) const;
	void			toNetworkOrder( void );
	void			toHostOrder( void );
	void			setPayload( void * payload);
	void			setPayloadLength( uint16_t length );
	void			setCode( ResponseCode code );
	~Message( void );
private:
	Header		_header;
	void *		_payload;

	Message( Message const & );
	Message &		operator=( Message const & );
};

#endif
