#pragma once

#include <memory>
#include <iostream>

#include "net.hpp"

// Forward declaration
class shared_state;

// accepts incoming connections
class listener : public std::enable_shared_from_this<listener> {
	tcp::acceptor acceptor_;
	tcp::socket socket_;
	std::shared_ptr<shared_state> state_;

	void fail(error_code ec, char const* what);
	void on_accept(error_code ec);

public:
	listener(
		net::io_context& ioc,
		tcp::endpoint endpoint,
		std::shared_ptr<shared_state> const& state
	);

	void run();
};