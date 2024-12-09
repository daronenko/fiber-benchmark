#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace asio = boost::asio;
namespace beast = boost::beast;
using tcp = asio::ip::tcp;

constexpr size_t MAX_THREADS = 4;
std::queue<tcp::socket> connection_queue;
std::mutex queue_mutex;
std::condition_variable queue_cv;

void handle_client(tcp::socket socket) {
  try {
    beast::flat_buffer buffer;
    beast::http::request<beast::http::string_body> req;
    beast::http::read(socket, buffer, req);

    beast::http::response<beast::http::string_body> res{beast::http::status::ok,
                                                        req.version()};
    res.set(beast::http::field::server, "ThreadServer");
    res.set(beast::http::field::content_type, "text/plain");
    res.body() = "OK";
    res.prepare_payload();

    beast::http::write(socket, res);
  } catch (const std::exception& e) {
    std::cerr << "Exception in handle_client: " << e.what() << std::endl;
  }
}

[[noreturn]] void worker_thread(asio::io_context& io_context) {
  for (;;) {
    tcp::socket socket(io_context);
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      queue_cv.wait(lock, [] {
        return !connection_queue.empty();
      });
      socket = std::move(connection_queue.front());
      connection_queue.pop();
    }
    handle_client(std::move(socket));
  }
}

[[noreturn]] void server(asio::io_context& io_context, unsigned short port) {
  tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));
  for (size_t i = 0; i < MAX_THREADS; ++i) {
    std::thread(worker_thread, std::ref(io_context)).detach();
  }
  while (true) {
    tcp::socket socket(io_context);
    acceptor.accept(socket);
    {
      std::lock_guard<std::mutex> lock(queue_mutex);
      connection_queue.push(std::move(socket));
    }
    queue_cv.notify_one();
  }
}

int main(int argc, char* argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "Usage: ./server-http <port>" << std::endl;
      return 1;
    }

    uint16_t port = static_cast<uint16_t>(std::atoi(argv[1]));
    asio::io_context io_context;
    server(io_context, port);

  } catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  return 0;
}
