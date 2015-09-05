#include "zmqx/zloopreader.h"


ZLoopReader::ZLoopReader(zloop_t* loop) :
	m_loop(loop),
	m_sock(nullptr)
{
}

ZLoopReader::~ZLoopReader() {
	stop();
}


int ZLoopReader::start(zsock_t* sock,const std::function<int(zsock_t*)>& func) {
	if( nullptr != m_sock )
		return -1;

	if( -1 == zloop_reader(m_loop,sock,&ZLoopReader::readableAdapter,this) ) {
		return -1;
	}
	m_sock = sock;
	m_func = func;
	return 0;
}

void ZLoopReader::stop() {
	if( m_sock ) {
		zloop_reader_end(m_loop,m_sock);
		m_sock = nullptr;
		m_func = nullptr;
	}
}

int ZLoopReader::rebind(const std::function<int(zsock_t*)>& func) {
	if( m_sock ) {
		m_func = func;
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
		return -1;
	} else {
		return 0;
	}
}

