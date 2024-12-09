cmake_minimum_required(VERSION 3.20)
project(fiber-benchmark)

include(cmake/CompileOptions.cmake)
include(cmake/Sanitize.cmake)

add_subdirectory(third_party)
add_subdirectory(server)
