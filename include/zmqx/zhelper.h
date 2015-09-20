#pragma once

#include <string>
#include <czmq.h>

uint64_t time_now();

int zmq_wait_timeouted(void* sock,int ev,long timeout);

int zsock_connect_m(void* sock,size_t count,char* addrs[]);

//Pop frame off front of message, If next frame is empty, pops and destroys that empty frame.
std::string zmq_pop_router_identity(zmsg_t* msg);

std::string new_uuid();


// it's multi-thread safe
std::string new_short_identity();

// it's multi-thread safe
uint64_t new_short_bin_identity();

#define zmq_wait_readable(s,tv)			zmq_wait_timeouted((s),ZMQ_POLLIN,(tv))
#define zmq_wait_writable(s,tv)			zmq_wait_timeouted((s),ZMQ_POLLOUT,(tv))

