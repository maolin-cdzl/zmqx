#include "zmqx/zprepend.h"

ZPrepend::ZPrepend() :
	m_pre(nullptr)
{
}

ZPrepend::ZPrepend(zmsg_t** p_msg) :
	m_pre(*p_msg)
{
	*p_msg = nullptr;
}

ZPrepend::~ZPrepend() {
	if( m_pre ) {
		zmsg_destroy(&m_pre);
	}
}

int ZPrepend::recv(void* sock) {
	assert(sock);
	if( m_pre != nullptr ) {
		zmsg_destroy(&m_pre);
	}

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
			m_pre = msg;
			return 0;
		}
	} while( 1 );

	if( msg ) {
		zmsg_destroy(&msg);
	}
	return -1;
}

int ZPrepend::sendm(void* sock) {
	assert(sock);
	if( m_pre ) {
		return zmsg_sendm(&m_pre,sock);
	} else {
		return -1;
	}
}

int ZPrepend::shadow_sendm(void* sock) const {
	assert(sock);
	if( m_pre ) {
		zmsg_t* msg = zmsg_dup(m_pre);
		if( msg ) {
			return zmsg_sendm(&msg,sock);
		}
	}
	return -1;
}

zmsg_t* ZPrepend::content() const {
	return m_pre;
}

std::shared_ptr<ZPrepend> ZPrepend::recv_new(void* sock) {
	assert(sock);
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
			return std::make_shared<ZPrepend>(&msg);
		}
	} while( 1 );

	if( msg ) {
		zmsg_destroy(&msg);
	}
	return nullptr;
}

int ZPrepend::send_delimiter(void* sock) {
	return zstr_sendm(sock,"");
}

int ZPrepend::drop_delimiter(void* sock) {
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

	return -1;
}


