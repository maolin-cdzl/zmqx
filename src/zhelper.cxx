#include <time.h>
#include <assert.h>
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

	assert(sock);

	pi[0].socket = zsock_resolve(sock);
	pi[0].fd = 0;
	pi[0].events = ev;

	return zmq_poll(pi,1,timeout);
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
