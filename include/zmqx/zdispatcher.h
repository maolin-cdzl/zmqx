#pragma once

#include <czmq.h>
#include "zmqx/dispatcher.h"
#include "zmqx/zloopreader.h"
#include "zmqx/zprepend.h"

class ZDispatcher {
public:
	ZDispatcher(zloop_t* loop);
	~ZDispatcher();

	int start(zsock_t* sock,const std::shared_ptr<Dispatcher>& dispatcher,bool read_pre=false);
	int start(zsock_t** p_sock,const std::shared_ptr<Dispatcher>& dispatcher,bool read_pre=false);
	void stop();
	zsock_t* socket() const;
	bool isActive() const;
	ZPrepend* prepend();

	int sendback(const google::protobuf::Message& msg);
	int shadow_sendback(const google::protobuf::Message& msg);
private:
	int onReadable(zsock_t* reader);
private:
	zloop_t*					m_loop;
	ZLoopReader					m_reader;
	std::shared_ptr<Dispatcher>	m_dispatcher;
	std::unique_ptr<ZPrepend>	m_prepend;
};


