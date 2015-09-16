#include <glog/logging.h>
#include "zmqx/zenvelope.h"

ZEnvelope::ZEnvelope(zmsg_t** p_msg) :
	m_envelope(*p_msg)
{
	*p_msg = nullptr;
}

ZEnvelope::~ZEnvelope() {
	if( m_envelope ) {
		zmsg_destroy(&m_envelope);
	}
}

zmsg_t* ZEnvelope::envelope() const {
	return m_envelope;
}

std::unique_ptr<ZEnvelope> ZEnvelope::clone() const {
	zmsg_t* msg = nullptr;
	if( m_envelope ) {
		msg = zmsg_dup(m_envelope);
	}
	return std::unique_ptr<ZEnvelope>(new ZEnvelope(&msg));
}

std::unique_ptr<ZEnvelope> ZEnvelope::recv(void* sock) {
	CHECK_NOTNULL(sock);
	zmsg_t* msg = zmsg_new();

	do {
		zframe_t* fr = zframe_recv(sock);
		if( nullptr == fr )
			break;
		const size_t len = zframe_size(fr);
		zmsg_append(msg,&fr);
		if( ! zsock_rcvmore(sock) ) {
			break;
		}
		if( len == 0 ) {
			return std::move(std::unique_ptr<ZEnvelope>(new ZEnvelope(&msg)));
		}
	} while( 1 );

	if( msg ) {
		zmsg_destroy(&msg);
	}
	return nullptr;
}

int ZEnvelope::sendm(std::unique_ptr<ZEnvelope> envelope,void* sock) {
	CHECK_NOTNULL(sock);
	CHECK(envelope);

	return zmsg_sendm(&envelope->m_envelope,sock);
}


int ZEnvelope::send_delimiter(void* sock) {
	return zstr_sendm(sock,"");
}

int ZEnvelope::drop_delimiter(void* sock) {
	zframe_t* fr = nullptr;
	do {
		fr = zframe_recv(sock);
		if( nullptr == fr )
			break;
		const size_t len = zframe_size(fr);
		zframe_destroy(&fr);

		if( ! zsock_rcvmore(sock) ) {
			break;
		}
		if( len == 0 ) {
			return 0;
		}
	} while( 1 );

	zsock_flush(sock);
	return -1;
}


