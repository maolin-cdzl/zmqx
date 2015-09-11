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

int ZDispatcher::start(zsock_t** p_sock,const std::shared_ptr<Dispatcher>& dispatcher,const source_reader_t& source_reader) {
	assert( p_sock );
	assert( *p_sock );
	assert( dispatcher );

	if( m_reader.isActive() )
		return -1;

	if( 0 == m_reader.start(p_sock,std::bind<int>(&ZDispatcher::onReadable,this,std::placeholders::_1)) ) {
		m_dispatcher = dispatcher;
		m_source_reader = source_reader;
		return 0;
	} else {
		return -1;
	}
}

int ZDispatcher::start(zsock_t* sock,const std::shared_ptr<Dispatcher>& dispatcher,const source_reader_t& source_reader) {
	assert( sock );
	assert( dispatcher );

	if( m_reader.isActive() )
		return -1;

	if( 0 == m_reader.start(sock,std::bind<int>(&ZDispatcher::onReadable,this,std::placeholders::_1)) ) {
		m_dispatcher = dispatcher;
		m_source_reader = source_reader;
		return 0;
	} else {
		return -1;
	}
}

void ZDispatcher::stop() {
	m_reader.stop();
	m_dispatcher.reset();
	m_source_reader = nullptr;
	m_source.clear();
}

zsock_t* ZDispatcher::socket() const {
	return m_reader.socket();
}

bool ZDispatcher::isActive() const {
	return m_reader.isActive();
}

const std::string& ZDispatcher::source() const {
	return m_source;
}

int ZDispatcher::onReadable(zsock_t* reader) {
	if( m_source_reader ) {
		m_source = m_source_reader(reader);
		if( m_source.empty() ) {
			return m_dispatcher->trigger(-1);
		}
		DLOG(INFO) << "";
	}

	auto msg = zpb_recv(reader);
	if( msg ) {
		return m_dispatcher->deliver(msg);
	} else {
		LOG(WARNING) << "ZDispatcher recv Protobuf message failed";
		return m_dispatcher->trigger(-1);
	}
}

