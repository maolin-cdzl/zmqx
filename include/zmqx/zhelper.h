#pragma once

#include <czmq.h>

#ifdef __cplusplus
extern "C" {
#endif

uint64_t time_now();

int zmq_wait_timeouted(void* sock,int ev,long timeout);

int zsock_connect_m(void* sock,size_t count,char* addrs[]);

#define zmq_wait_readable(s,tv)			zmq_wait_timeouted((s),ZMQ_POLLIN,(tv))
#define zmq_wait_writable(s,tv)			zmq_wait_timeouted((s),ZMQ_POLLOUT,(tv))

#ifdef __cplusplus
}
#endif