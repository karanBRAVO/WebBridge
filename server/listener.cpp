#include "listener.hpp"
#include "shared_state.hpp"
#include "http_session.hpp"
#include "net.hpp"

listener::listener(
	net::io_context& ioc,
	tcp::endpoint endpoint,
	std::shared_ptr<shared_state> const& state
) : acceptor_(ioc), socket_(ioc), state_(state) {
	error_code ec;

	// Open the acceptor
	acceptor_.open(endpoint.protocol(), ec);

	if (ec) {
		fail(ec, "open");
		return;
	}

	// Allow address reuse
	acceptor_.set_option(
		net::socket_base::reuse_address(true),
		ec
	);

	if (ec) {
		fail(ec, "set_option");
		return;
	}

	// Bind to the server address
	acceptor_.bind(endpoint, ec);

	if (ec) {
		fail(ec, "bind");
		return;
	}

	// Start listening for connections
	acceptor_.listen(
		net::socket_base::max_listen_connections,
		ec
	);

	if (ec) {
		fail(ec, "listen");
		return;
	}
}

void listener::fail(error_code ec, char const* what) {
	// Don't report on cancelled operations
	if (ec == net::error::operation_aborted) {
		return;
	}

	std::cerr << what << ": " << ec.message() << "\n";
}

void listener::on_accept(error_code ec) {
	if (ec) {
		fail(ec, "accept");
	}
	else {
		// Launch a new connection for this session
		std::make_shared<http_session>(
			std::move(socket_),
			state_
		)->run();
	}

	// Accept the next connection
	acceptor_.async_accept(
		socket_,
		[self = shared_from_this()](error_code ec) {
			self->on_accept(ec);
		});
}

void listener::run() {
	// Start accepting the connections
	acceptor_.async_accept(
		socket_,
		[self = shared_from_this()](error_code ec) {
			self->on_accept(ec);
		});
}