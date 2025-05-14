#include "websocket_client.hpp"

WebSocketClient::WebSocketClient(net::io_context& ioc)
	: resolver_(net::make_strand(ioc)), ws_(net::make_strand(ioc)) {
	std::cout << "WebSocketClient constructor called" << std::endl;
}

WebSocketClient::~WebSocketClient() {
	if (ws_.is_open()) {
		error_code ec;
		ws_.close(websocket::close_code::normal, ec);
		if (ec) {
			std::cerr << "Destructor close error: " << ec.message() << std::endl;
		}
	}
	std::cout << "WebSocketClient destructor called" << std::endl;
}

void WebSocketClient::connect(char const* host, char const* port) {
	std::cout << "Connecting to " << host << ":" << port << std::endl;

	host_ = host;
	port_ = port;

	// Look up the domain name
	resolver_.async_resolve(
		host,
		port,
		beast::bind_front_handler(
			&WebSocketClient::on_resolve_,
			shared_from_this()
		)
	);
}

void WebSocketClient::on_resolve_(error_code ec, tcp::resolver::results_type results) {
	if (ec) {
		std::cerr << "Resolve error: " << ec.message() << std::endl;
		return;
	}

	// Set the timeout for the operation
	beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(30));

	// Make the connection on the IP address we get from a lookup
	beast::get_lowest_layer(ws_).async_connect(
		results,
		beast::bind_front_handler(
			&WebSocketClient::on_connect_,
			shared_from_this()
		)
	);
}

void WebSocketClient::on_connect_(error_code ec, tcp::resolver::results_type::endpoint_type ep) {
	if (ec) {
		std::cerr << "Connect error: " << ec.message() << std::endl;
		return;
	}

	// Turn off the timeout on the tcp_stream, because
	// the websocket stream has its own timeout system.
	beast::get_lowest_layer(ws_).expires_never();

	// Set suggested timeout settings for the websocket
	ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));

	// Set a decorator to change the User-Agent of the handshake
	ws_.set_option(websocket::stream_base::decorator(
		[](websocket::request_type& req) {
			req.set(http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-client-async");
		}
	));

	host_ += ':' + std::to_string(ep.port());

	// Perform the websocket handshake
	ws_.async_handshake(
		host_,
		"/",
		beast::bind_front_handler(
			&WebSocketClient::on_handshake_,
			shared_from_this()
		)
	);
}

void WebSocketClient::on_handshake_(error_code ec) {
	if (ec) {
		std::cerr << "Handshake error: " << ec.message() << std::endl;
		return;
	}

	std::cout << "Handshake successful" << std::endl;

	// Start reading messages
	read_();

	if (on_connect_cb_) {
		on_connect_cb_();
	}
}

void WebSocketClient::close() {
	if (!ws_.is_open()) return;

	ws_.async_close(websocket::close_code::normal,
		beast::bind_front_handler(
			&WebSocketClient::on_close_,
			shared_from_this()
		)
	);
}

void WebSocketClient::on_close_(error_code ec) {
	if (ec) {
		std::cerr << "Close error: " << ec.message() << std::endl;
		on_error_cb_("Error disconnecing");
		return;
	}

	// The make_printable() function helps print a ConstBufferSequence
	std::cout << beast::make_printable(buffer_.data()) << std::endl;

	std::cout << "Connection closed" << std::endl;

	if (on_disconnect_cb_) {
		on_disconnect_cb_();
	}
}

void WebSocketClient::send_message(const std::string& message) {
	if (!ws_.is_open()) return;

	ws_.async_write(
		net::buffer(message),
		beast::bind_front_handler(
			&WebSocketClient::on_write_,
			shared_from_this()
		)
	);
}

void WebSocketClient::on_write_(error_code ec, std::size_t bytes_transferred) {
	boost::ignore_unused(bytes_transferred);

	if (ec) {
		std::cerr << "Write Error: " << ec.message() << std::endl;
		on_error_cb_("Error writing data");
		return;
	}

	std::cout << "Write successful" << std::endl;

	if (on_write_cb_) {
		on_write_cb_();
	}
}

void WebSocketClient::read_() {
	if (!ws_.is_open()) return;

	std::cout << "Reading message..." << std::endl;

	ws_.async_read(
		buffer_,
		beast::bind_front_handler(
			&WebSocketClient::on_read_,
			shared_from_this()
		)
	);
}

void WebSocketClient::on_read_(error_code ec, std::size_t bytes_transferred) {
	boost::ignore_unused(bytes_transferred);

	if (ec) {
		if (ec == net::error::operation_aborted || ec == websocket::error::closed) {
			std::cout << "[i] Read operation cancelled or WebSocket closed." << std::endl;
			return;
		}
		std::cerr << "Read error: " << ec.message() << std::endl;
		on_error_cb_("Error reading data");
		return;
	}

	std::cout << "Read successful" << std::endl;

	// Convert the buffer to a string
	std::string message = beast::buffers_to_string(buffer_.data());

	// Clear the buffer for the next read
	buffer_.consume(buffer_.size());

	std::cout << "Received message: " << message << std::endl;

	if (on_message_cb_) {
		on_message_cb_(message);
	}

	// continue reading next messages
	read_();
}

// ---------------
//  Listeners
// ---------------

void WebSocketClient::on_connect(std::function<void()> cb) {
	on_connect_cb_ = std::move(cb);
}

void WebSocketClient::on_disconnect(std::function<void()> cb) {
	on_disconnect_cb_ = std::move(cb);
}

void WebSocketClient::on_error(std::function<void(const std::string& error)> cb) {
	on_error_cb_ = std::move(cb);
}

void WebSocketClient::on_write(std::function<void()> cb) {
	on_write_cb_ = std::move(cb);
}

void WebSocketClient::on_message(std::function<void(const std::string&)> cb) {
	on_message_cb_ = std::move(cb);
}