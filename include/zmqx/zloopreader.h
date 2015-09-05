#pragma once

#include <functional>
#include <czmq.h>

class ZLoopReader {
public:
	ZLoopReader(zloop_t* loop);
	~ZLoopReader();

	int start(zsock_t* sock,const std::function<int(zsock_t*)>& func);
	void stop();
private:
	static int readableAdapter(zloop_t* loop,zsock_t* reader,void* arg);
private:
	zloop_t*									m_loop;
	zsock_t*									m_sock;
	std::function<int(zsock_t*)>				m_func;
};

