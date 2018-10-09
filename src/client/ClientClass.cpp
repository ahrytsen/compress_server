#include "ClientClass.hpp"

Client::Client( boost::asio::ip::tcp::socket & socket )
	: _socket(socket) {}

void	Client::send_msg( Message & msg ) {
	uint32_t	payload_len = msg.getPayloadLength();
	msg.toNetworkOrder();
	boost::asio::write(_socket,
					   boost::asio::buffer(msg.getHeader(), sizeof(Message::Header)),
					   boost::asio::transfer_exactly(sizeof(Message::Header))
		);
	if (payload_len && msg.getPayload())
		boost::asio::write(_socket,
						   boost::asio::buffer(msg.getPayload(), payload_len),
						   boost::asio::transfer_exactly(payload_len)
			);
}

void *	Client::receive_payload( int32_t size ) {
	std::unique_ptr<uint8_t[]>	buf( new uint8_t[size + 1] );
	buf[size] = '\0';
	boost::asio::read(_socket, boost::asio::buffer(buf.get(), size), boost::asio::transfer_exactly(size));
	return buf.release();
}

void	Client::receive_msg( Message & msg ) {
	boost::asio::read(_socket,
					  boost::asio::buffer(msg.getHeader(), sizeof(Message::Header)),
					  boost::asio::transfer_exactly(sizeof(Message::Header))
		);
	msg.toHostOrder();
	if (msg.getPayloadLength())
		msg.setPayload(receive_payload(msg.getPayloadLength()));
}

void	Client::proccess(Message & msg) {
	Message::RequestCode tmp_code = static_cast<Message::RequestCode>(msg.getCode());
	send_msg(msg);
	Message	response;
	receive_msg(response);
	std::cout << "Header:" << std::endl
			  << "payload length: " << response.getPayloadLength() << std::endl
			  << "code: " << response.getCode() << std::endl;
	if (tmp_code == Message::get_status && response.getPayload()) {
		const uint32_t * total_received = reinterpret_cast< const uint32_t* >(response.getPayload());
		const uint32_t * total_sent = reinterpret_cast< const uint32_t* >(response.getPayload()) + 1;
		const uint8_t * ratio = reinterpret_cast< const uint8_t* >(
			reinterpret_cast<const uint32_t *>(response.getPayload()) + 2);
		std::cout << "Payload:" << std::endl
				  << "total recived=" << *total_received << std::endl
				  << "total sent=" << *total_sent << std::endl
				  << "compression ratio=" << static_cast<int>(*ratio) << '%' << std::endl;
	}
	else if (response.getPayload()) {
		std::cout << "Payload:" << std::endl
				  << reinterpret_cast<const char*>(response.getPayload())
				  << std::endl;
	}
}

void	Client::run() {
	boost::regex	reg("^\\s*(\\S+)(\\s+(\\S+))?\\s*$");
	std::string		cmd;
	for(;;) {
		boost::smatch	match;
		std::cerr << "$> ";
		std::getline(std::cin, cmd);
		Message	msg;
		if (boost::regex_match(cmd, match, reg)) {
			if (match[1].str() == "ping")
				msg.setCode(static_cast<Message::ResponseCode>(Message::ping));
			else if (match[1].str() == "get_status")
				msg.setCode(static_cast<Message::ResponseCode>(Message::get_status));
			else if (match[1].str() == "reset_status")
				msg.setCode(static_cast<Message::ResponseCode>(Message::reset_status));
			else if (match[1] == "compress")
				msg.setCode(static_cast<Message::ResponseCode>(Message::compress));
			else
				msg.setCode(static_cast<Message::ResponseCode>(Message::compress + 1));
			if (match[3].str().size()) {
				char *tmp  = new char[match[3].str().size()];
				memcpy(tmp, match[3].str().c_str(), match[3].str().size());
				msg.setPayload(tmp);
				msg.setPayloadLength(match[3].str().size());
			}
			proccess(msg);
		}
		cmd.clear();
	}
}
