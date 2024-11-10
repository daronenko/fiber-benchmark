#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include <iostream>
#include <string>
#include <thread>

namespace asio = boost::asio;
namespace beast = boost::beast;
using tcp = asio::ip::tcp;

void handle_client(tcp::socket socket) {
    try {
        beast::flat_buffer buffer;
        beast::http::request<beast::http::string_body> req;

        beast::http::read(socket, buffer, req);

        beast::http::response<beast::http::string_body> res{
            beast::http::status::ok, req.version()};
        res.set(beast::http::field::server, "ThreadServer");
        res.set(beast::http::field::content_type, "text/plain");
        res.body() = "OK";
        res.prepare_payload();

        beast::http::write(socket, res);
    
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

[[noreturn]] void server(asio::io_context& io_context, unsigned short port) {
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));
    std::cerr << "Server is listening on port " << port << "..." << std::endl;

    while (true) {
        tcp::socket socket(io_context);
        acceptor.accept(socket);
        std::thread([socket = std::move(socket)]() mutable {
            handle_client(std::move(socket));
        }).detach();
    }
}

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: ./server-http <port>" << std::endl;
            return 1;
        }

        uint16_t port = std::atoi(argv[1]);
        asio::io_context io_context;
        server(io_context, port);

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
