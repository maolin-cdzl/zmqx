CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

project(ZmqXTest)

set(CMAKE_C_FLAGS "-std=c99 -fPIC ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "-std=c++11 -fPIC ${CMAKE_CXX_FLAGS}")

add_executable(test_zmqx
	test_zmqx.cxx
	test.pb.cc
	test.pb.h
)

target_include_directories(test_zmqx
	PRIVATE $<TARGET_PROPERTY:zmqx,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(test_zmqx LINK_PRIVATE zmqx gtest)
