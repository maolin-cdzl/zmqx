#include "zmqx/zdispatcher.h"
#include "zmqx/zprotobuf++.h"


ZDispatcher::ZDispatcher(zloop_t* loop) :
	m_loop(loop),
	m_sock(nullptr)
{
}

ZDispatcher::~ZDispatcher() {
	stop();
}

int ZDispatcher::start(zsock_t* sock,const std::shared_ptr<Dispatcher>& dispatcher) {
	assert( sock );
	assert( dispatcher );

	if( m_sock || dispatcher )
		return -1;


	if( 0 == zloop_reader(m_loop,sock,readableAdapter,this) ) {
		m_sock = sock;
		m_dispatcher = dispatcher;
		return 0;
	} else {
		return -1;
	}
}

int ZDispatcher::stop() {
	if( m_sock ) {
		zloop_reader_end(m_loop,m_sock);
		m_sock = nullptr;
		m_dispatcher.reset();
		return 0;
	} else {
		return -1;
	}
}


int ZDispatcher::onReadable() {
	auto msg = zpb_recv(m_sock);
	if( msg ) {
		m_dispatcher->deliver(msg);
	} else {
		m_dispatcher->trigger(-1);
	}
	return 0;
}

int ZDispatcher::readableAdapter(zloop_t* loop,zsock_t* reader,void* args) {
	(void)loop;
	(void)reader;
	ZDispatcher* self = (ZDispatcher*)args;
	return self->onReadable();
}

