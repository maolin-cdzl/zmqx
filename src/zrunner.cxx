#include <glog/logging.h>
#include "zmqx/zrunner.h"


ZRunner::ZRunner() :
	m_actor(nullptr)
{
}

ZRunner::~ZRunner() {
	stop();
}

int ZRunner::start(const std::function<void(zsock_t*)>& routine) {
	CHECK( routine != nullptr );
	if( m_actor ) {
		return -1;
	}
	m_routine = routine;
	m_actor =  zactor_new(&ZRunner::run,this);
	if( m_actor ) {
		return 0;
	} else {
		m_routine = nullptr;
		return -1;
	}
}

void ZRunner::stop() {
	if( m_actor ) {
		zactor_destroy(&m_actor);
	}
	m_routine = nullptr;
}

void ZRunner::run(zsock_t* pipe,void* arg) {
	ZRunner* self = (ZRunner*) arg;
	self->m_routine(pipe);
}

