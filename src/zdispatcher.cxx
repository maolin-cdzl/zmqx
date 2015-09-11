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

int ZDispatcher::start(zsock_t** p_sock,const std::shared_ptr<Dispatcher>& dispatcher,bool read_pre) {
	assert( p_sock );
	assert( *p_sock );
	assert( dispatcher );

	if( m_reader.isActive() )
		return -1;

	if( 0 == m_reader.start(p_sock,std::bind<int>(&ZDispatcher::onReadable,this,std::placeholders::_1)) ) {
		m_dispatcher = dispatcher;
		if( read_pre ) {
			m_prepend = std::move(std::unique_ptr<ZPrepend>(new ZPrepend()));
		}
		return 0;
	} else {
		return -1;
	}
}

int ZDispatcher::start(zsock_t* sock,const std::shared_ptr<Dispatcher>& dispatcher,bool read_pre) {
	assert( sock );
	assert( dispatcher );

	if( m_reader.isActive() )
		return -1;

	if( 0 == m_reader.start(sock,std::bind<int>(&ZDispatcher::onReadable,this,std::placeholders::_1)) ) {
		m_dispatcher = dispatcher;
		if( read_pre ) {
			m_prepend.reset(new ZPrepend());
		}
		return 0;
	} else {
		return -1;
	}
}

void ZDispatcher::stop() {
	m_reader.stop();
	m_dispatcher.reset();
	m_prepend.reset();
}

zsock_t* ZDispatcher::socket() const {
	return m_reader.socket();
}

bool ZDispatcher::isActive() const {
	return m_reader.isActive();
}

ZPrepend* ZDispatcher::prepend() {
	return m_prepend.get();
}

int ZDispatcher::sendback(const google::protobuf::Message& msg) {
	do {
		if( m_prepend ) {
			if( -1 == m_prepend->sendm(m_reader.socket()) ) {
				break;
			}
		}
		return zpb_send(m_reader.socket(),msg);
	} while(0);

	zsock_flush(m_reader.socket());
	return -1;
}

int ZDispatcher::onReadable(zsock_t* reader) {
	if( m_prepend ) {
		if( -1 == m_prepend->recv(reader) ) {
			return m_dispatcher->trigger(-1);
		}
	}

	auto msg = zpb_recv(reader);
	if( msg ) {
		return m_dispatcher->deliver(msg);
	} else {
		LOG(WARNING) << "ZDispatcher recv Protobuf message failed";
		return m_dispatcher->trigger(-1);
	}
}

