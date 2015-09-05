#include <glog/logging.h>
#include "zmqx/zdispatcher.h"
#include "zmqx/zprotobuf++.h"


ZDispatcher::ZDispatcher(zloop_t* loop) :
	m_loop(loop),
	m_reader(loop)
{
}

ZDispatcher::~ZDispatcher() {
	stop();
}

int ZDispatcher::start(zsock_t** p_sock,const std::shared_ptr<Dispatcher>& dispatcher) {
	assert( p_sock );
	assert( *p_sock );
	assert( dispatcher );

	if( m_reader.isActive() )
		return -1;

	if( 0 == m_reader.start(p_sock,std::bind<int>(&ZDispatcher::onReadable,this,std::placeholders::_1)) ) {
		m_dispatcher = dispatcher;
		return 0;
	} else {
		return -1;
	}
}

int ZDispatcher::start(zsock_t* sock,const std::shared_ptr<Dispatcher>& dispatcher) {
	assert( sock );
	assert( dispatcher );

	if( m_reader.isActive() )
		return -1;

	if( 0 == m_reader.start(sock,std::bind<int>(&ZDispatcher::onReadable,this,std::placeholders::_1)) ) {
		m_dispatcher = dispatcher;
		return 0;
	} else {
		return -1;
	}
}

void ZDispatcher::stop() {
	m_reader.stop();
	m_dispatcher.reset();
}

zsock_t* ZDispatcher::socket() const {
	return m_reader.socket();
}

bool ZDispatcher::isActive() const {
	return m_reader.isActive();
}

int ZDispatcher::onReadable(zsock_t* reader) {
	auto msg = zpb_recv(reader);
	if( msg ) {
		m_dispatcher->deliver(msg);
	} else {
		m_dispatcher->trigger(-1);
	}
	return 0;
}

