#include "http_session.hpp"
#include "websocket_session.hpp"
#include "net.hpp"

http_session::http_session(
    tcp::socket socket,
    std::shared_ptr<shared_state> const& state
) : socket_(std::move(socket)), state_(state) {
}

// Return a reasonable mime type based on the extension of a file.
beast::string_view mime_type(boost::beast::string_view path) {
    using boost::beast::iequals;
    auto const ext = [&path]
        {
            auto const pos = path.rfind(".");
            if (pos == boost::beast::string_view::npos)
                return boost::beast::string_view{};
            return path.substr(pos);
        }();
    if (iequals(ext, ".htm"))  return "text/html";
    if (iequals(ext, ".html")) return "text/html";
    if (iequals(ext, ".php"))  return "text/html";
    if (iequals(ext, ".css"))  return "text/css";
    if (iequals(ext, ".txt"))  return "text/plain";
    if (iequals(ext, ".js"))   return "application/javascript";
    if (iequals(ext, ".json")) return "application/json";
    if (iequals(ext, ".xml"))  return "application/xml";
    if (iequals(ext, ".swf"))  return "application/x-shockwave-flash";
    if (iequals(ext, ".flv"))  return "video/x-flv";
    if (iequals(ext, ".png"))  return "image/png";
    if (iequals(ext, ".jpe"))  return "image/jpeg";
    if (iequals(ext, ".jpeg")) return "image/jpeg";
    if (iequals(ext, ".jpg"))  return "image/jpeg";
    if (iequals(ext, ".gif"))  return "image/gif";
    if (iequals(ext, ".bmp"))  return "image/bmp";
    if (iequals(ext, ".ico"))  return "image/vnd.microsoft.icon";
    if (iequals(ext, ".tiff")) return "image/tiff";
    if (iequals(ext, ".tif"))  return "image/tiff";
    if (iequals(ext, ".svg"))  return "image/svg+xml";
    if (iequals(ext, ".svgz")) return "image/svg+xml";
    return "application/text";
}

// Append an HTTP rel-path to a local filesystem path.
// The returned path is normalized for the platform.
std::string path_cat(
    beast::string_view base,
    beast::string_view path
) {
    if (base.empty()) {
        return path;
    }
    std::string result = base;
#if BOOST_MSVC
    char constexpr path_separator = '\\';
    if (result.back() == path_separator)
        result.resize(result.size() - 1);
    result.append(path.data(), path.size());
    for (auto& c : result)
        if (c == '/')
            c = path_separator;
#else
    char constexpr path_separator = '/';
    if (result.back() == path_separator)
        result.resize(result.size() - 1);
    result.append(path.data(), path.size());
#endif
    return result;
}

// This function produces an HTTP response for the given
// request. The type of the response object depends on the
// contents of the request, so the interface requires the
// caller to pass a generic lambda for receiving the response.
template<class Body, class Allocator, class Send>
void handle_request(
    boost::beast::string_view doc_root,
    http::request<Body, http::basic_fields<Allocator>>&& req,
    Send&& send
) {
    // Returns a bad request response
    auto const bad_request = [&req](boost::beast::string_view why) {
        http::response<http::string_body> res{ http::status::bad_request, req.version() };
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = std::string(why);
        res.prepare_payload();
        return res;
        };

    // Returns a not found response
    auto const not_found = [&req](boost::beast::string_view target) {
        http::response<http::string_body> res{ http::status::not_found, req.version() };
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "The resource '" + std::string(target) + "' was not found.";
        res.prepare_payload();
        return res;
        };

    // Returns a server error response
    auto const server_error = [&req](boost::beast::string_view what) {
        http::response<http::string_body> res{ http::status::internal_server_error, req.version() };
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "An error occurred: '" + std::string(what) + "'";
        res.prepare_payload();
        return res;
        };

    // Make sure we can handle the method
    if (req.method() != http::verb::get &&
        req.method() != http::verb::head)
        return send(bad_request("Unknown HTTP-method"));

    // Request path must be absolute and not contain "..".
    if (req.target().empty() ||
        req.target()[0] != '/' ||
        req.target().find("..") != boost::beast::string_view::npos)
        return send(bad_request("Illegal request-target"));

    // Build the path to the requested file
    std::string path = path_cat(doc_root, req.target());
    if (req.target().back() == '/')
        path.append("index.html");

    // Attempt to open the file
    error_code ec;
    http::file_body::value_type body;
    body.open(path.c_str(), beast::file_mode::scan, ec);

    // Handle the case where the file doesn't exist
    if (ec == boost::system::errc::no_such_file_or_directory)
        return send(not_found(req.target()));

    // Handle an unknown error
    if (ec)
        return send(server_error(ec.message()));

    // Cache the size since we need it after the move
    auto const size = body.size();

    // Respond to HEAD request
    if (req.method() == http::verb::head)
    {
        http::response<http::empty_body> res{ http::status::ok, req.version() };
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, mime_type(path));
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        return send(std::move(res));
    }

    // Respond to GET request
    http::response<http::file_body> res{
        std::piecewise_construct,
        std::make_tuple(std::move(body)),
        std::make_tuple(http::status::ok, req.version())
    };

    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, mime_type(path));
    res.content_length(size);
    res.keep_alive(req.keep_alive());
    return send(std::move(res));
}

void http_session::run() {
    // Read a request
    http::async_read(
        socket_,
        buffer_,
        req_,
        [self = shared_from_this()](error_code ec, std::size_t bytes) {
            self->on_read(ec, bytes);
        }
    );
}

void http_session::fail(error_code ec, char const* what) {
    // Don't report on cancelled operations
    if (ec == net::error::operation_aborted) {
        return;
    }
    std::cerr << what << ": " << ec.message() << "\n";
}

void http_session::on_read(error_code ec, std::size_t) {
    // Connection closed
    if (ec == http::error::end_of_stream) {
        socket_.shutdown(tcp::socket::shutdown_send, ec);
        return;
    }

    if (ec) {
        return fail(ec, "read");
    }

    // Handle websocket upgrade
    if (websocket::is_upgrade(req_)) {
        // create a new websocket session by transferring the socket
        std::make_shared<websocket_session>(
            std::move(socket_),
            state_
        )->run(std::move(req_));
        return;
    }

    // Handle HTTP request
    handle_request(
        state_->doc_root(),
        std::move(req_),
        [this](auto&& response) {
            using response_type = typename std::decay<decltype(response)>::type;
            auto sp = std::make_shared<response_type>(std::forward<decltype(response)>(response));

#if 0
            // NOTE This causes an ICE in gcc 7.3
            // Write the response
            http::async_write(this->socket_, *sp,
                [self = shared_from_this(), sp](error_code ec, std::size_t bytes) {
                    self->on_write(ec, bytes, sp->need_eof());
                });
#else
            // Write the response
            auto self = shared_from_this();
            http::async_write(this->socket_, *sp,
                [self, sp](error_code ec, std::size_t bytes) {
                    self->on_write(ec, bytes, sp->need_eof());
                });
#endif
        }
    );
}

void http_session::on_write(error_code ec, std::size_t, bool close) {
    if (ec) {
        return fail(ec, "write");
    }

    if (close) {
        // This means the connection is closed
        socket_.shutdown(tcp::socket::shutdown_send, ec);
        return;
    }

    // clear the content of the request,
    // otherwise the read behaviour is undefined
    req_ = {};

    http::async_read(
        socket_,
        buffer_,
        req_,
        [self = shared_from_this()](error_code ec, std::size_t bytes) {
            self->on_read(ec, bytes);
        }
    );
}
