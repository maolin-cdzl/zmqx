#pragma once

#include <functional>
#include <czmq.h>


class ZLoopTimer {
public:
	ZLoopTimer(zloop_t* loop);
	~ZLoopTimer();

	int start(uint64_t msec,size_t times,const std::function<int()>& func);
	void stop();
	bool isActive() const;

	int rebind(const std::function<int()>& func);
private:
	static int timerAdapter(zloop_t* loop,int timer_id,void* arg);
private:
	zloop_t*						m_loop;
	int								m_tid;
	size_t							m_times;
	std::function<int()>			m_func;
};

class ZLoopTimeouter {
public:
	ZLoopTimeouter(zloop_t* loop);
	~ZLoopTimeouter();

	// if callback func return -1,timer stop,0 means keep go on
	int start(uint64_t period,uint64_t timeout,const std::function<int()>& func);
	void stop();
	bool isActive() const;
	int delay(uint64_t msec);

	int rebind(const std::function<int()>& func);
private:
	static int timerAdapter(zloop_t* loop,int timer_id,void* arg);

private:
	zloop_t*						m_loop;
	int								m_tid;
	uint64_t						m_tv_timeout;
	std::function<int()>			m_func;
};


