#pragma once

#include <memory>
#include <string>
#include <unordered_set>

// Forward declare the websocket_session class
class websocket_session;

// holds the server data
class shared_state {
	std::string doc_root_;
	// only one thread can access the sessions_ at a time
	std::unordered_set<websocket_session*> sessions_;

public:
	explicit
		shared_state(std::string doc_root);

	std::string const& doc_root() const noexcept {
		return doc_root_;
	}

	void join(websocket_session& session);
	void leave(websocket_session& session);
	void send(std::string message);
};