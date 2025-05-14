#pragma once

#include <memory>
#include <string>
#include <iostream>
#include <functional>

#include "net.hpp"

class WebSocketClient : public std::enable_shared_from_this<WebSocketClient> {
	std::string host_;
	std::string port_;
	tcp::resolver resolver_;
	websocket::stream<beast::tcp_stream> ws_;
	beast::flat_buffer buffer_;

	void on_resolve_(error_code ec, tcp::resolver::results_type results);
	void on_connect_(error_code ec, tcp::resolver::results_type::endpoint_type ep);
	void on_handshake_(error_code ec);
	void on_close_(error_code ec);
	void on_write_(error_code ec, std::size_t bytes_transferred);
	void read_();
	void on_read_(error_code ec, std::size_t bytes_transferred);

	// callbacks
	std::function<void()> on_connect_cb_;
	std::function<void()> on_disconnect_cb_;
	std::function<void(const std::string& error)> on_error_cb_;
	std::function<void()> on_write_cb_;
	std::function<void(const std::string&)> on_message_cb_;

public:
	explicit
	WebSocketClient(net::io_context& ioc);

	~WebSocketClient();

	void connect(char const* host, char const* port);
	void close();
	void send_message(const std::string& message);

	// Listeners
	void on_connect(std::function<void()> cb);
	void on_disconnect(std::function<void()> cb);
	void on_error(std::function<void(const std::string& error)> cb);
	void on_write(std::function<void()> cb);
	void on_message(std::function<void(const std::string&)> cb);
};