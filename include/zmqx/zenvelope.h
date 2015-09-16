#pragma once

#include <memory>
#include <czmq.h>

class ZEnvelope {
public:
	~ZEnvelope();

	zmsg_t* envelope() const;

	std::unique_ptr<ZEnvelope> clone() const;

	static std::unique_ptr<ZEnvelope> recv(void* sock);
	static int sendm(std::unique_ptr<ZEnvelope> envelope,void* sock);
	static int send_delimiter(void* sock);
	static int drop_delimiter(void* sock);
private:
	ZEnvelope() = delete;
	ZEnvelope(const ZEnvelope&) = delete;
	ZEnvelope& operator = (const ZEnvelope&) = delete;

	ZEnvelope(zmsg_t** p_msg);
private:
	zmsg_t*				m_envelope;
};

