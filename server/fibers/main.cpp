#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

#include <fiber/sched/thread_pool.hpp>
#include <fiber/core/sched/go.hpp>
#include <fiber/sync/wait_group.hpp>

#include <iostream>
#include <utility>
#include <thread>

using asio::ip::tcp;
using namespace fiber;

void handle_client(tcp::socket socket) {
  static const size_t max_length = 1024;

  try {
    for (;;) {
      char data[max_length];

      std::error_code error;
      size_t length = socket.read_some(asio::buffer(data), error);

      if (error == asio::stream_errc::eof) {
        break;
      } else if (error) {
        throw std::system_error(error);
      }

      asio::write(socket, asio::buffer(data, length));
    }
  } catch (std::exception& e) {
    std::cerr << "Exception in thread: " << e.what() << std::endl;
  }
}

[[noreturn]] void server(asio::io_context& io_context, unsigned short port,
                         sched::ThreadPool& pool) {
  tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));
  std::cerr << "Server is listening on port " << port << "..." << std::endl;

  while (true) {
    tcp::socket socket(io_context);
    acceptor.accept(socket);
    Go(pool, [socket = std::move(socket)]() mutable {
      handle_client(std::move(socket));
    });
  }
}

int main(int argc, char* argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "Usage: ./server-threads <port>" << std::endl;
      return 1;
    }

    uint16_t port = std::atoi(argv[1]);

    asio::io_context io_context;

    const size_t kThreads = 4;
    sched::ThreadPool pool{kThreads};

    pool.Start();
    server(io_context, port, pool);
    pool.Stop();

  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  return 0;
}
