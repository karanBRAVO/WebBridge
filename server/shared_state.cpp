#include "shared_state.hpp"
#include "websocket_session.hpp"

shared_state::shared_state(std::string doc_root) : doc_root_(std::move(doc_root)) {}

// Add the new session to the set of sessions
void shared_state::join(websocket_session& session) {
	sessions_.insert(&session);
}

// Removes the session from the set of sessions
void shared_state::leave(websocket_session& session) {
	sessions_.erase(&session);
}

// Sends a message to all sessions
void shared_state::send(std::string message) {
	auto const ss = std::make_shared<std::string const>(std::move(message));
	for (auto session : sessions_) {
		session->send(ss);
	}
}