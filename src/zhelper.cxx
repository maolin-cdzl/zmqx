#include <time.h>
#include <mutex>
#include <random>
#include <chrono>
#include <glog/logging.h>
#include <uuid/uuid.h>
#include "zmqx/zhelper.h"

uint64_t time_now() {
	uint64_t now;
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC,&ts);

	now = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
	return now;
}


int zmq_wait_timeouted(void* sock,int ev,long timeout) {
	zmq_pollitem_t pi[1];

	CHECK_NOTNULL(sock);

	pi[0].socket = zsock_resolve(sock);
	pi[0].fd = 0;
	pi[0].events = ev;

	return zmq_poll(pi,1,timeout);
}

std::string zmq_pop_router_identity(zmsg_t* msg) {
	CHECK_NOTNULL(msg);
	std::string id;

	if( zmsg_size(msg) > 0 ) {
		zframe_t* fr = zmsg_pop(msg);
		if( fr ) {
			id.assign((const char*)zframe_data(fr),zframe_size(fr));
			zframe_destroy(&fr);

			fr = zmsg_first(msg);
			if( zframe_size(fr) == 0 ) {
				fr = zmsg_pop(msg);
				zframe_destroy(&fr);
			}
		}
	}

	return std::move(id);
}

int zsock_connect_m(void* sock,size_t count,char* addrs[]) {
	void* s = zsock_resolve(sock);
	for(size_t i=0; i < count; ++i) {
		if( -1 == zmq_connect(s,addrs[i]) )
			return -1;
	}
	return 0;
}

std::string new_uuid() {
    uuid_t uuid;
    uuid_generate_random ( uuid );
    char s[37];
    uuid_unparse( uuid, s );
    return s;
}

uint64_t new_short_bin_identity() {
	static std::mutex						s_mutex;
	static std::default_random_engine		s_generator(std::chrono::system_clock::now().time_since_epoch().count());

	std::uniform_int_distribution<uint64_t> id_random(0,UINT64_MAX);
	s_mutex.lock();
	uint64_t id = id_random(s_generator);
	s_mutex.unlock();
	return id;
}

std::string new_short_identity() {
	std::stringstream ss;
	ss << std::hex << new_short_bin_identity();
	return ss.str();
}

