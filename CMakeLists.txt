CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

project(zmqx)

set(CMAKE_C_FLAGS "-std=c99 -fPIC ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "-std=c++11 -fPIC ${CMAKE_CXX_FLAGS}")

add_library(zmqx STATIC
	src/zprotobuf.c
	src/zhelper.cxx
	src/zprotobuf++.cxx
	src/zpbreader.cxx
	src/zloopreader.cxx
	src/zlooptimer.cxx
	src/zenvelope.cxx
	include/zmqx/zhelper.h
	include/zmqx/zprotobuf.h
	include/zmqx/zprotobuf++.h
	include/zmqx/dispatcher.h
	include/zmqx/zpbreader.h
	include/zmqx/zloopreader.h
	include/zmqx/zlooptimer.h
	include/zmqx/zenvelope.h
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
	uuid
)

add_subdirectory(test)
