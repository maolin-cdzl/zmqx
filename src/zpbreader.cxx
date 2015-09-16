#include "zmqx/zpbreader.h"


int zpb_deliver(const std::shared_ptr<sock_dispatcher_t>& dispatcher,zsock_t* sock) {
	auto msg = zpb_recv(sock);
	if( msg ) {
		return dispatcher->deliver(msg,sock);
	}
	return dispatcher->trigger(-1);
}


int zpb_envelop_deliver(const std::shared_ptr<envelope_dispatcher_t>& dispatcher,zsock_t* sock) {
	auto envelope = ZEnvelope::recv(sock);
	if( envelope ) {
		auto msg = zpb_recv(sock);
		if( msg ) {
			return dispatcher->deliver(msg,sock,envelope);
		}
	}
	return dispatcher->trigger(-1);
}

std::shared_ptr<ZLoopReader> make_zpb_reader(zloop_t* loop,zsock_t* sock,const std::shared_ptr<sock_dispatcher_t>& dispatcher) {
	auto reader = std::make_shared<ZLoopReader>(loop);

	if( 0 == reader->start(sock,std::bind<int>(&zpb_deliver,dispatcher,std::placeholders::_1)) ) {
		return reader;
	} else {
		return nullptr;
	}
}

std::shared_ptr<ZLoopReader> make_zpb_reader(zloop_t* loop,zsock_t** sock,const std::shared_ptr<sock_dispatcher_t>& dispatcher) {
	auto reader = std::make_shared<ZLoopReader>(loop);

	if( 0 == reader->start(sock,std::bind<int>(&zpb_deliver,dispatcher,std::placeholders::_1)) ) {
		return reader;
	} else {
		return nullptr;
	}
}

std::shared_ptr<ZLoopReader> make_zpb_reader(zloop_t* loop,zsock_t* sock,const std::shared_ptr<envelope_dispatcher_t>& dispatcher) {
	auto reader = std::make_shared<ZLoopReader>(loop);

	if( 0 == reader->start(sock,std::bind<int>(&zpb_envelop_deliver,dispatcher,std::placeholders::_1)) ) {
		return reader;
	} else {
		return nullptr;
	}
}

std::shared_ptr<ZLoopReader> make_zpb_reader(zloop_t* loop,zsock_t** sock,const std::shared_ptr<envelope_dispatcher_t>& dispatcher) {
	auto reader = std::make_shared<ZLoopReader>(loop);

	if( 0 == reader->start(sock,std::bind<int>(&zpb_envelop_deliver,dispatcher,std::placeholders::_1)) ) {
		return reader;
	} else {
		return nullptr;
	}
}



