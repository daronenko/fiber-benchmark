#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

#include <iostream>
#include <utility>
#include <thread>

using asio::ip::tcp;

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
      std::cerr << "Usage: ./server-threads <port>" << std::endl;
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
