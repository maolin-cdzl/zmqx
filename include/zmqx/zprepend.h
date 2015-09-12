#pragma once

#include <memory>
#include <czmq.h>

class ZPrepend {
public:
	ZPrepend();
	ZPrepend(zmsg_t** p_msg);
	~ZPrepend();

	int recv(void* sock);
	int sendm(void* sock);
	int shadow_sendm(void* sock) const;
	zmsg_t* content() const;

	static std::shared_ptr<ZPrepend> recv_new(void* sock);
	static int send_delimiter(void* sock);
	static int drop_delimiter(void* sock);
private:
	ZPrepend(const ZPrepend&) = delete;
	ZPrepend& operator = (const ZPrepend&) = delete;

private:
	zmsg_t*				m_pre;
};

