#pragma once

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>

#include "shared_state.hpp"
#include "net.hpp"

// Forward declare the shared_state class
class shared_state;

// maintains a active WebSocket connection
class websocket_session : public std::enable_shared_from_this<websocket_session> {
	beast::flat_buffer buffer_;
	websocket::stream<tcp::socket> ws_;
	std::shared_ptr<shared_state> state_;
	std::vector<std::shared_ptr<std::string const>> queue_;

	void fail(error_code ec, char const* what);
	void on_accept(error_code ec);
	void on_read(error_code ec, std::size_t);
	void on_write(error_code ec, std::size_t);

public:
	websocket_session(
		tcp::socket socket,
		std::shared_ptr<shared_state> const& state
	);
	~websocket_session();

	template <class Body, class Allocator>
	void run(http::request<Body, http::basic_fields<Allocator>> req);

	void send(std::shared_ptr<std::string const> const& ss);
};

template <class Body, class Allocator>
void websocket_session::run(http::request<Body, http::basic_fields<Allocator>> req) {
	// Accept the websocket upgrade
	ws_.async_accept(
		req,
		std::bind(
			&websocket_session::on_accept,
			shared_from_this(),
			std::placeholders::_1
		)
	);
};