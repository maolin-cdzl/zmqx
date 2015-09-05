#include "zmqx/zlooptimer.h"


ZLoopTimer::ZLoopTimer(zloop_t* loop) :
	m_loop(loop),
	m_tid(-1)
{
}

ZLoopTimer::~ZLoopTimer() {
	stop();
}

int ZLoopTimer::start(size_t tv,size_t times,const std::function<int()>& func) {
	if( m_tid != -1 )
		return -1;

	m_tid = zloop_timer(m_loop,tv,times,&ZLoopTimer::timerAdapter,this);
	if( -1 == m_tid )
		return -1;
	m_func = func;
	return 0;
}

void ZLoopTimer::stop() {
	if( m_tid != -1 ) {
		zloop_timer_end(m_loop,m_tid);
		m_tid = -1;
		m_func = nullptr;
	}
}

int ZLoopTimer::timerAdapter(zloop_t* loop,int timer_id,void* arg) {
	(void)loop;
	(void)timer_id;
	ZLoopTimer* self = (ZLoopTimer*)arg;
	assert(self->m_func);
	if( -1 == self->m_func() ) {
		self->m_tid = -1;
		self->m_func = nullptr;
		return -1;
	} else {
		return 0;
	}
}

