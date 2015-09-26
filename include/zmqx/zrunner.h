#pragma once

#include <czmq.h>
#include <functional>


class ZRunner {
public:
	ZRunner();
	~ZRunner();

	int start(const std::function<void(zsock_t*)>& routine);
	void stop();

	inline zactor_t* actor() const {
		return m_actor;
	}

	inline zsock_t* socket() const {
		if( m_actor ) {
			return zactor_sock(m_actor);
		}
		return nullptr;
	}
private:
	static void run(zsock_t* pipe,void* arg);
private:
	zactor_t*						m_actor;
	std::function<void(zsock_t*)>	m_routine;
};

