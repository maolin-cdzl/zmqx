#include "zmqx/zlooptimer.h"
#include "zmqx/zhelper.h"

// class ZLoopTimer
ZLoopTimer::ZLoopTimer(zloop_t* loop) :
	m_loop(loop),
	m_tid(-1),
	m_times(0)
{
}

ZLoopTimer::~ZLoopTimer() {
	stop();
}

int ZLoopTimer::start(uint64_t msec,size_t times,const std::function<int()>& func) {
	if( m_tid != -1 )
		return -1;

	m_tid = zloop_timer(m_loop,msec,times,&ZLoopTimer::timerAdapter,this);
	if( -1 == m_tid )
		return -1;
	m_func = func;
	m_times = times;
	return 0;
}

void ZLoopTimer::stop() {
	if( m_tid != -1 ) {
		zloop_timer_end(m_loop,m_tid);
		m_tid = -1;
		m_times = 0;
		m_func = nullptr;
	}
}

bool ZLoopTimer::isActive() const {
	return m_tid != -1;
}

int ZLoopTimer::rebind(const std::function<int()>& func) {
	if( m_tid != -1 ) {
		m_func = func;
		return 0;
	} else {
		return -1;
	}
}

int ZLoopTimer::timerAdapter(zloop_t* loop,int timer_id,void* arg) {
	(void)loop;
	(void)timer_id;
	ZLoopTimer* self = (ZLoopTimer*)arg;
	assert(self->m_func);

	bool finish = false;
	if( self->m_times > 0 ) {
		--self->m_times;
		if( self->m_times == 0 )
			finish = true;
	}

	int result = self->m_func();

	if( -1 == result || finish ) {
		self->m_tid = -1;
		self->m_times = 0;
		self->m_func = nullptr;
	}
	return result;
}

// class ZLoopTimeouter

ZLoopTimeouter::ZLoopTimeouter(zloop_t* loop) :
	m_loop(loop),
	m_tid(-1),
	m_tv_timeout(0)
{
}

ZLoopTimeouter::~ZLoopTimeouter() {
	stop();
}

int ZLoopTimeouter::start(uint64_t period,uint64_t timeout,const std::function<int()>& func) {
	if( m_tid != -1 )
		return -1;

	m_tid = zloop_timer(m_loop,period,0,&ZLoopTimeouter::timerAdapter,this);
	if( -1 == m_tid )
		return -1;
	m_func = func;
	m_tv_timeout = time_now() + timeout;
	return 0;
}

void ZLoopTimeouter::stop() {
	if( m_tid != -1 ) {
		zloop_timer_end(m_loop,m_tid);
		m_tid = -1;
		m_tv_timeout = 0;
		m_func = nullptr;
	}
}

bool ZLoopTimeouter::isActive() const {
	return m_tid != -1;
}

int ZLoopTimeouter::delay(uint64_t msec) {
	if( m_tid != -1 ) {
		m_tv_timeout = time_now() + msec;
		return 0;
	} else {
		return -1;
	}
}

int ZLoopTimeouter::rebind(const std::function<int()>& func) {
	if( m_tid != -1 ) {
		m_func = func;
		return 0;
	} else {
		return -1;
	}
}

int ZLoopTimeouter::timerAdapter(zloop_t* loop,int timer_id,void* arg) {
	(void)loop;
	(void)timer_id;
	ZLoopTimeouter* self = (ZLoopTimeouter*)arg;
	assert(self->m_func);

	int result = 0;
	if( time_now() >= self->m_tv_timeout ) {
		result = self->m_func();
		if( -1 == result ) {
			self->m_tid = -1;
			self->m_tv_timeout = 0;
			self->m_func = nullptr;
		}
	}

	return result;
}


