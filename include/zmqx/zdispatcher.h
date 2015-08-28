#include <czmq.h>
#include "zmqx/dispatcher.h"

class ZDispatcher {
public:
	ZDispatcher(zloop_t* loop);

	int start(zsock_t* sock,const std::shared_ptr<Dispatcher>& dispatcher);
	int stop();

private:
	int onReadable();
	static int readableAdapter(zloop_t* loop,zsock_t* reader,void* args);
private:
	zloop_t*					m_loop;
	zsock_t*					m_sock;
	std::shared_ptr<Dispatcher>	m_dispatcher;
};


