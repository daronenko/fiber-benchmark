include(FetchContent)

# --------------------------------------------------------------------

# Offline mode (uncomment next line to enable)
# set(FETCHCONTENT_FULLY_DISCONNECTED ON)

# set(FETCHCONTENT_QUIET OFF)

# --------------------------------------------------------------------

# Libraries

# --------------------------------------------------------------------

message(STATUS "FetchContent: fiber")

FetchContent_Declare(
        fiber
        GIT_REPOSITORY https://github.com/daronenko/fiber.git
        GIT_TAG main
)
FetchContent_MakeAvailable(fiber)

# --------------------------------------------------------------------

# message(STATUS "FetchContent: asio")

# FetchContent_Declare(
#         asio
#         GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
#         GIT_TAG asio-1-31-0
# )
# FetchContent_MakeAvailable(asio)

# add_library(asio INTERFACE)
# target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)

# --------------------------------------------------------------------

message(STATUS "FetchContent: boost")

set(BOOST_INCLUDE_LIBRARIES asio beast)
set(BOOST_ENABLE_CMAKE ON)

FetchContent_Declare(
  Boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_TAG boost-1.84.0
  GIT_PROGRESS TRUE
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(Boost)
