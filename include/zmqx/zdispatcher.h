#pragma once

#include <czmq.h>
#include "zmqx/dispatcher.h"
#include "zmqx/zloopreader.h"

class ZDispatcher {
public:
	ZDispatcher(zloop_t* loop);
	~ZDispatcher();

	int start(zsock_t* sock,const std::shared_ptr<Dispatcher>& dispatcher);
	int start(zsock_t** p_sock,const std::shared_ptr<Dispatcher>& dispatcher);
	void stop();
	zsock_t* socket() const;
	bool isActive() const;
private:
	int onReadable(zsock_t* reader);
private:
	zloop_t*					m_loop;
	ZLoopReader					m_reader;
	std::shared_ptr<Dispatcher>	m_dispatcher;
};


