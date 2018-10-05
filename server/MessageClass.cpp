#include "MessageClass.hpp"

Message::Message( void ) :
	_byte_order(host), _payload(nullptr) {
	_header.magic_value = MAGIC_VALUE;
	_header.payload_length = 0;
	_header.code = 0;
}


Message::Message( uint32_t const & payload_length, uint16_t const & code, void const * & payload ) :
	_byte_order(host), _payload(&payload) {
	_header.magic_value = MAGIC_VALUE;
	_header.payload_length = payload_length;
	_header.code = code;
}

Message::Header *	Message::getHeader( void ) {
	return &_header;
}

void const*		Message::getPayload( void ) const {
	return _payload;
}

uint16_t		Message::getPayloadLength( void ) const {
	return _header.payload_length;
}

void			Message::toNetworkOrder( void ) {
	if (_byte_order == host) {
		_header.magic_value = htonl(_header.magic_value);
		_header.payload_length = htons(_header.payload_length);
		_header.code = htons(_header.code);
		_byte_order = network;
	}
}

void			Message::toHostOrder( void ) {
	if (_byte_order == network) {
		_header.magic_value = ntohl(_header.magic_value);
		_header.payload_length = ntohs(_header.payload_length);
		_header.code = ntohs(_header.code);
		_byte_order = host;
	}
}

void			Message::setPayload( void * payload) {
	_payload = payload;
}

Message::~Message( void ) {
	delete reinterpret_cast<uint8_t*>(_payload);
}
