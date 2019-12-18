cmake_minimum_required(VERSION 3.10)
project(cppSRTFramingExample)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)
include_directories(${MY_SOURCE_DIR} ./srt/)
link_directories(./efp/ ./srt/)
add_executable(cppSRTFramingClient client.cpp)
target_link_libraries(cppSRTFramingClient libefp.a libsrt.a ${OPENSSL_LIBRARIES} libsrtnet.a)

add_executable(cppSRTFramingServer server.cpp)
target_link_libraries(cppSRTFramingServer libefp.a libsrt.a ${OPENSSL_LIBRARIES} libsrtnet.a)