#include <fiber/sync/wait_group.hpp>
#include <fiber/sched/thread_pool.hpp>
#include <fiber/core/sched/go.hpp>

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include <iostream>
#include <string>
#include <thread>

using namespace fiber;

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
        res.set(beast::http::field::server, "FiberServer");
        res.set(beast::http::field::content_type, "text/plain");
        res.body() = "OK";
        res.prepare_payload();

        beast::http::write(socket, res);

    } catch (const std::exception& e) {
        std::cerr << "Exception in fiber: " << e.what() << std::endl;
    }
}

[[noreturn]] void server(asio::io_context& io_context, fiber::sched::ThreadPool& pool, unsigned short port) {
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));
    std::cerr << "Server is listening on port " << port << "..." << std::endl;

    for (;;) {
        tcp::socket socket(io_context);
        acceptor.accept(socket);

        Go(pool, [sock = std::move(socket)]() mutable {
            handle_client(std::move(sock));
        });
    }
}

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: ./server-http <port>" << std::endl;
            return 1;
        }

        unsigned short port = static_cast<unsigned short>(std::atoi(argv[1]));

        asio::io_context io_context;

        const size_t kThreads = std::thread::hardware_concurrency();
        fiber::sched::ThreadPool pool{kThreads};
        
        pool.Start();
        server(io_context, pool, port);
        pool.Stop();

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
