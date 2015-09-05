#include "zmqx/zloopreader.h"


ZLoopReader::ZLoopReader(zloop_t* loop) :
	m_loop(loop),
	m_sock(nullptr),
	m_state("idle")
{
}

ZLoopReader::~ZLoopReader() {
	stop();
}


int ZLoopReader::start(zsock_t** p_sock,const std::function<int(zsock_t*)>& func,const std::string& state) {
	if( nullptr != m_sock || nullptr == p_sock || nullptr == *p_sock )
		return -1;

	if( -1 == zloop_reader(m_loop,*p_sock,&ZLoopReader::readableAdapter,this) ) {
		return -1;
	}
	m_sock = *p_sock;
	m_func = func;
	m_state = state;
	*p_sock = nullptr;
	return 0;
}

void ZLoopReader::stop() {
	if( m_sock ) {
		zloop_reader_end(m_loop,m_sock);
		zsock_destroy(&m_sock);
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
	assert( self->m_func );
	if( -1 == self->m_func(reader) ) {
		self->m_sock = nullptr;
		self->m_func = nullptr;
		self->m_state = "idle";
		return -1;
	} else {
		return 0;
	}
}

