#include "websocket_session.hpp"

websocket_session::websocket_session(
	tcp::socket socket,
	std::shared_ptr<shared_state> const& state
) : ws_(std::move(socket)), state_(state) {
}

void websocket_session::fail(error_code ec, char const* what) {
	if (ec == net::error::operation_aborted || ec == websocket::error::closed) {
		return;
	}

	std::cerr << what << ": " << ec.message() << "\n";
}

void websocket_session::on_accept(error_code ec) {
	if (ec) {
		return fail(ec, "accept");
	}

	// Add the new session to the shared state
	state_->join(*this);

	// Read a message
	ws_.async_read(
		buffer_,
		[sp = shared_from_this()](error_code ec, std::size_t bytes) {
			sp->on_read(ec, bytes);
		}
	);
}

void websocket_session::on_read(error_code ec, std::size_t) {
	if (ec) {
		return fail(ec, "read");
	}

	// send the message to all sessions
	state_->send(beast::buffers_to_string(buffer_.data()));

	// Clear the buffer
	buffer_.consume(buffer_.size());

	// Read the next message
	ws_.async_read(
		buffer_,
		[sp = shared_from_this()](error_code ec, std::size_t bytes) {
			sp->on_read(ec, bytes);
		}
	);
}

void websocket_session::send(std::shared_ptr<std::string const> const& ss) {
	// add to queue
	queue_.push_back(ss);

	// already writing
	if (queue_.size() > 1) {
		return;
	}

	ws_.async_write(
		net::buffer(*queue_.front()),
		[sp = shared_from_this()](error_code ec, std::size_t bytes) {
			sp->on_write(ec, bytes);
		}
	);
}

void websocket_session::on_write(error_code ec, std::size_t) {
	if (ec) {
		return fail(ec, "write");
	}

	// remove the message from the queue
	queue_.erase(queue_.begin());

	if (!queue_.empty()) {
		ws_.async_write(
			net::buffer(*queue_.front()),
			[sp = shared_from_this()](error_code ec, std::size_t bytes) {
				sp->on_write(ec, bytes);
			}
		);
	}
}

websocket_session::~websocket_session() {
	state_->leave(*this);
}