CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

project(ZmqXTest)

set(CMAKE_C_FLAGS "-std=c99 -fPIC ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "-std=c++11 -fPIC ${CMAKE_CXX_FLAGS}")

# configure risproto
set(PROTOBUF_IMPORT_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

set(ZMQX_TEST_PROTO 
	${CMAKE_CURRENT_SOURCE_DIR}/test.proto 
)

build_pb_cxx_sources(${CMAKE_CURRENT_BINARY_DIR}/include/zmqx ${CMAKE_CURRENT_BINARY_DIR}/src TEST_PROTO_SRCS TEST_PROTO_HDRS ${ZMQX_TEST_PROTO})


add_executable(test_zmqx
	test_zmqx.cxx
	${TEST_PROTO_SRCS}
	${TEST_PROTO_HDRS}
)

target_include_directories(test_zmqx
	PRIVATE $<TARGET_PROPERTY:zmqx,INTERFACE_INCLUDE_DIRECTORIES>
	PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include
	PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include/zmqx
)

target_link_libraries(test_zmqx LINK_PRIVATE zmqx gtest)
