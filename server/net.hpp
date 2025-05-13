#pragma once

#include <boost/asio.hpp>
#include <boost/beast.hpp>

// namespace aliases and typedefs
// Boost/Asio
namespace net = boost::asio;
using tcp = net::ip::tcp;
using error_code = boost::system::error_code;

// Boost/Beast
namespace beast = boost::beast;
namespace http = boost::beast::http;
namespace websocket = boost::beast::websocket;
