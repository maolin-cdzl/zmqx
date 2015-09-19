#include <glog/logging.h>
#include "zmqx/zenvelope.h"

ZEnvelope::ZEnvelope() {
}

ZEnvelope::ZEnvelope(std::vector<std::string>&& envelopes) :
	m_envelopes(std::move(envelopes))
{
}

size_t ZEnvelope::size() const {
	return m_envelopes.size();
}

const std::string& ZEnvelope::frame(size_t idx) const {
	return m_envelopes[idx];
}

int ZEnvelope::sendm(void* sock) const {
	for(auto it=m_envelopes.begin(); it != m_envelopes.end(); ++it) {
		CHECK(! it->empty());
		zframe_t* fr = zframe_new(it->c_str(),it->size());
		if( -1 == zframe_send(&fr,sock,ZFRAME_MORE) ) {
			DLOG(FATAL) << "Send envelope failed";
			if( fr ) {
				zframe_destroy(&fr);
			}
			return -1;
		}
	}
	
	if( -1 == zstr_sendm(sock,"") ) {
		DLOG(FATAL) << "Send envelope delimiter failed: " << errno;
		return -1;
	}
	return 0;
}

std::shared_ptr<ZEnvelope> ZEnvelope::recv(void* sock) {
	CHECK_NOTNULL(sock);

	std::vector<std::string> envelopes;

	zframe_t* fr = nullptr;
	do {
		fr = zframe_recv(sock);
		if( nullptr == fr )
			break;
		if( ! zsock_rcvmore(sock) ) {
			break;
		}
		const size_t len = zframe_size(fr);
		if( len == 0 ) {
			zframe_destroy(&fr);
			return std::make_shared<ZEnvelope>(std::move(envelopes));
		} else {
			envelopes.push_back(std::string((const char*)zframe_data(fr),zframe_size(fr)));
			zframe_destroy(&fr);
		}
	} while( 1 );

	if( fr ) {
		zframe_destroy(&fr);
	}
	zsock_flush(sock);
	return nullptr;
}

int ZEnvelope::sendm(const std::shared_ptr<ZEnvelope> envelope,void* sock) {
	CHECK_NOTNULL(sock);
	CHECK(envelope);

	return envelope->sendm(sock);
}



