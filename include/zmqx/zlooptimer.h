#pragma once

#include <functional>
#include <czmq.h>


class ZLoopTimer {
public:
	ZLoopTimer(zloop_t* loop);
	~ZLoopTimer();

	int start(size_t tv,size_t times,const std::function<int()>& func);
	void stop();
private:
	static int timerAdapter(zloop_t* loop,int timer_id,void* arg);
private:
	zloop_t*						m_loop;
	int								m_tid;
	std::function<int()>			m_func;
};

