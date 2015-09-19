#pragma once

#include <string>
#include <vector>
#include <memory>
#include <czmq.h>

class ZEnvelope {
public:
	ZEnvelope();
	ZEnvelope(std::vector<std::string>&& envelopes);

	size_t size() const;
	const std::string& frame(size_t idx) const;

	int sendm(void* sock) const;

	static std::shared_ptr<ZEnvelope> recv(void* sock);
	static int sendm(const std::shared_ptr<ZEnvelope> envelope,void* sock);
private:
	std::vector<std::string>			m_envelopes;
};

