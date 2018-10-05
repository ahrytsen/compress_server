#ifndef MESSAGECLASS_HPP
# define MESSAGECLASS_HPP

# include <arpa/inet.h>
# include <iostream>

# define MAGIC_VALUE 0x53545259

class Message {
public:
	enum ByteOrder {
		host,
		network
	};
	enum	RequestCode {
		ping = 1,
		get_status,
		reset_status,
		compress,
	};
	enum	ResponceCode {
		request_ok,
		unknown_error,
		message_too_large,
		unsupported_request_type,
	};
	struct Header {
		uint32_t	magic_value;
		uint16_t	payload_length;
		uint16_t	code;
	};
	Message( void );
	Message( uint32_t const & payload_length, uint16_t const & code, void const * & payload );
	Header *		getHeader( void );
	void const*		getPayload( void ) const;
	uint16_t		getPayloadLength( void ) const;
	uint16_t		getCode( void ) const;
	void			toNetworkOrder( void );
	void			toHostOrder( void );
	void			setPayload( void * payload);
	~Message( void );
private:
	ByteOrder	_byte_order;
	Header		_header;
	void *		_payload;

	Message( Message const & );
	Message &		operator=( Message const & );
};

#endif
