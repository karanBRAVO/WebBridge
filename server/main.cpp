#include <memory>
#include <iostream>

#include "listener.hpp"
#include "shared_state.hpp"
#include "net.hpp"

int main(int argc, char* argv[]) {
	// Usage:
	// ./chat_server <address> <port> <doc_root>
	if (argc != 4) {
		std::cerr <<
			"Usage: websocket-chat-server <address> <port> <doc_root>\n" <<
			"Example:\n" <<
			"    websocket-chat-server 0.0.0.0 8080 .\n";
		return EXIT_FAILURE;
	}

	auto address = net::ip::make_address(argv[1]);
	auto port = static_cast<unsigned short>(std::atoi(argv[2]));
	auto doc_root = argv[3];

	// The io_context is required for all I/O
	net::io_context ioc;

	// Create and launch a listening port
	std::make_shared<listener>(
		ioc,
		tcp::endpoint{ address, port },
		std::make_shared<shared_state>(doc_root)
	)->run();

	ioc.run();

	return EXIT_SUCCESS;
}
