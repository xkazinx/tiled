# The tiled server for Linux (the deployed one: kazin.dev/projects/tiled). Windows
# builds the server from server.vcxproj (build.ps1); this file is for Linux, built in
# a container by build_linux.ps1 (Ubuntu 26.04, as the server, with libmysqlclient-dev):
#   cmake -S src/server -B build -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build
cmake_minimum_required(VERSION 3.21)
project(tiled_server CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TILED_SRC ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(KAZILIB ${TILED_SRC}/dep/kazilib)

file(GLOB SERVER_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
list(FILTER SERVER_SOURCES EXCLUDE REGEX "stdafx\\.cpp$")

# kazilib: enet (the Unix socket layer), the packet layer, the socket, the
# WebSocket gateway, the log's output
set(KAZILIB_SOURCES
	${KAZILIB}/enet/callbacks.cpp
	${KAZILIB}/enet/compress.cpp
	${KAZILIB}/enet/host.cpp
	${KAZILIB}/enet/list.cpp
	${KAZILIB}/enet/packet.cpp
	${KAZILIB}/enet/peer.cpp
	${KAZILIB}/enet/protocol.cpp
	${KAZILIB}/enet/unix.cpp
	${KAZILIB}/lib/shared/src/net/net_packet.cpp
	${KAZILIB}/lib/shared/src/net/socket.cpp
	${KAZILIB}/lib/shared/src/net/ws_gateway.cpp
	${KAZILIB}/lib/ml/src/log_out.cpp
)

# MySQL's client library (libmysqlclient-dev: mysql.h, libmysqlclient.so)
find_path(MYSQL_INCLUDE_DIR mysql.h PATH_SUFFIXES mysql REQUIRED)
find_library(MYSQL_LIBRARY NAMES mysqlclient REQUIRED)

add_executable(tiled_server ${SERVER_SOURCES} ${KAZILIB_SOURCES})
target_include_directories(tiled_server PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${TILED_SRC}/common
	${KAZILIB}/lib/shared/include
	${KAZILIB}/lib/ml/include
	${KAZILIB}/enet/include
	${KAZILIB}/lib/ml/src
	${MYSQL_INCLUDE_DIR}
)
# Every file starts with the precompiled header's include, as on Windows
target_precompile_headers(tiled_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h)
target_compile_definitions(tiled_server PRIVATE HAS_SOCKLEN_T=1)
find_package(Threads REQUIRED)
target_link_libraries(tiled_server PRIVATE Threads::Threads ${MYSQL_LIBRARY})
# The C++ runtime inside (no libstdc++ version needed on the box); libmysqlclient
# stays shared (the box's libmysqlclient24)
target_link_options(tiled_server PRIVATE -static-libstdc++ -static-libgcc)
