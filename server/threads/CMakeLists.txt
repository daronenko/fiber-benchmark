add_executable(server-threads main.cpp)
target_link_libraries(server-threads Boost::asio Boost::beast)
