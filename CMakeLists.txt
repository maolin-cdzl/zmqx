CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

project(zmqx)

set(CMAKE_C_FLAGS "-std=c99 -fPIC ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "-std=c++11 -fPIC ${CMAKE_CXX_FLAGS}")

add_library(zmqx STATIC
	src/zhelper.c
	src/zprotobuf.c
	src/zprotobuf++.cxx
	src/dispatcher.cxx
	src/zdispatcher.cxx
	include/zmqx/zhelper.h
	include/zmqx/zprotobuf.h
	include/zmqx/zprotobuf++.h
	include/zmqx/dispatcher.h
	include/zmqx/zdispatcher.h
)

target_include_directories(zmqx
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(zmqx PRIVATE _POSIX_C_SOURCE=200809L )

target_link_libraries(zmqx PUBLIC
	czmq
	zmq
	protobuf-c
	protobuf
	glog
	gflags
)

add_subdirectory(test)
