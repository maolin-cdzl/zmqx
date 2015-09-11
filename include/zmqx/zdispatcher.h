#pragma once

#include <czmq.h>
#include "zmqx/dispatcher.h"
#include "zmqx/zloopreader.h"

class ZDispatcher {
public:
	typedef std::function<std::string(zsock_t*)>		source_reader_t;
public:
	ZDispatcher(zloop_t* loop);
	~ZDispatcher();

	int start(zsock_t* sock,const std::shared_ptr<Dispatcher>& dispatcher,const source_reader_t& source_reader=source_reader_t());
	int start(zsock_t** p_sock,const std::shared_ptr<Dispatcher>& dispatcher,const source_reader_t& source_reader=source_reader_t());
	void stop();
	zsock_t* socket() const;
	bool isActive() const;
	const std::string& source() const;
private:
	int onReadable(zsock_t* reader);
private:
	zloop_t*					m_loop;
	ZLoopReader					m_reader;
	std::shared_ptr<Dispatcher>	m_dispatcher;
	source_reader_t				m_source_reader;
	std::string					m_source;
};


