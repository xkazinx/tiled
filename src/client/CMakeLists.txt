# The tiled client for a web browser (WebAssembly, Emscripten). Windows builds the
# client from client.vcxproj (build.ps1); this file is the web one, run by
# build_web.ps1 against kazilib's SFML web build (kazilib web/build_deps.ps1):
#   emcmake cmake -S src/client -B <build> -G Ninja -DSFML_DIR=...
# Output: index.html / .js / .wasm / .data (the font) - the folder the server's
# gateway serves (deploy\server\web\) and kazin.dev/projects/tiled.
cmake_minimum_required(VERSION 3.21)
project(tiled_web CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TILED_SRC ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(KAZILIB ${TILED_SRC}/dep/kazilib)
set(SFML_STATIC_LIBRARIES ON)
find_package(SFML 3 REQUIRED COMPONENTS Graphics)
include(${KAZILIB}/web/kazilib_web.cmake)

file(GLOB CLIENT_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
list(FILTER CLIENT_SOURCES EXCLUDE REGEX "stdafx\\.cpp$")

add_executable(tiled ${CLIENT_SOURCES})
kazilib_web_target(tiled)
# Dear ImGui and imgui-SFML (kazilib's: drawn through SFML in a browser)
kazilib_web_imgui(tiled)
target_compile_definitions(tiled PRIVATE CLIENT)
target_include_directories(tiled PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${TILED_SRC}/common)
target_link_libraries(tiled PRIVATE SFML::Graphics)
target_link_options(tiled PRIVATE
	"--preload-file=${TILED_SRC}/../deploy/data/arial.ttf@/data/arial.ttf")
