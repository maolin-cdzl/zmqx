#include <glog/logging.h>
#include "zmqx/zloopreader.h"


ZLoopReader::ZLoopReader(zloop_t* loop) :
	m_loop(loop),
	m_sock(nullptr),
	m_own_sock(false),
	m_state("idle")
{
}

ZLoopReader::~ZLoopReader() {
	stop();
}

int ZLoopReader::start(zsock_t* sock,const std::function<int(zsock_t*)>& func,const std::string& state) {
	CHECK_NOTNULL(sock);

	if( nullptr != m_sock ) {
		DLOG(FATAL) << "m_sock is not NULL";
		return -1;
	}

	if( -1 == zloop_reader(m_loop,sock,&ZLoopReader::readableAdapter,this) ) {
		DLOG(FATAL) << "Can not start reader: " << errno;
		return -1;
	}
	m_sock = sock;
	m_func = func;
	m_state = state;
	m_own_sock = false;
	return 0;
}

int ZLoopReader::start(zsock_t** p_sock,const std::function<int(zsock_t*)>& func,const std::string& state) {
	if( nullptr == p_sock )
		return -1;

	if( 0 == start(*p_sock,func,state) ) {
		*p_sock = nullptr;
		m_own_sock = true;
		return 0;
	} else {
		return -1;
	}
}

zsock_t* ZLoopReader::socket() const {
	return m_sock;
}

void ZLoopReader::stop() {
	if( m_sock ) {
		zloop_reader_end(m_loop,m_sock);
		if( m_own_sock ) {
			zsock_destroy(&m_sock);
		} else {
			m_sock = nullptr;
		}
		m_func = nullptr;
		m_state = "idle";
	}
}

bool ZLoopReader::isActive() const {
	return ( m_sock != nullptr );
}

const std::string& ZLoopReader::state() const {
	return m_state;
}

int ZLoopReader::rebind(const std::function<int(zsock_t*)>& func,const std::string& state) {
	if( m_sock ) {
		m_func = func;
		m_state = state;
		return 0;
	} else {
		return -1;
	}
}

int ZLoopReader::readableAdapter(zloop_t* loop,zsock_t* reader,void* arg) {
	(void)loop;
	ZLoopReader* self = (ZLoopReader*)arg;
	CHECK( self->m_func );
	if( -1 == self->m_func(reader) ) {
		if( self->m_own_sock ) {
			zsock_destroy(&self->m_sock);
		} else {
			self->m_sock = nullptr;
		}
		self->m_func = nullptr;
		self->m_state = "idle";
		return -1;
	} else {
		return 0;
	}
}

