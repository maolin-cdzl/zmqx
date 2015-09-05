#pragma once

#include <string>
#include <functional>
#include <czmq.h>

class ZLoopReader {
public:
	ZLoopReader(zloop_t* loop);
	~ZLoopReader();

	int start(zsock_t** p_sock,const std::function<int(zsock_t*)>& func,const std::string& state="");
	int start(zsock_t* sock,const std::function<int(zsock_t*)>& func,const std::string& state="");
	void stop();
	bool isActive() const;
	const std::string& state() const;
	zsock_t* socket() const;

	int rebind(const std::function<int(zsock_t*)>& func,const std::string& state="");
private:
	static int readableAdapter(zloop_t* loop,zsock_t* reader,void* arg);
private:
	zloop_t*									m_loop;
	zsock_t*									m_sock;
	bool										m_own_sock;
	std::string									m_state;
	std::function<int(zsock_t*)>				m_func;
};

