add_executable(server-fibers main.cpp)
target_link_libraries(server-fibers fiber Boost::asio Boost::beast)
