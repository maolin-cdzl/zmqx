#include "zmqx/zprotobuf.h"
#include <czmq.h>

struct zpbc_t {
	zhash_t*		pbmsg_hash;
};

zpbc_t* zpbc_new() {
	zpbc_t* self = (zpbc_t*) malloc(sizeof(zpbc_t));
	self->pbmsg_hash = zhash_new();

	return self;
}


void zpbc_destroy(zpbc_t** p_self) {
	if( NULL == p_self || NULL == *p_self )
		return;

	zhash_destroy(&(*p_self)->pbmsg_hash);
	free(*p_self);
	*p_self = NULL;
}

int zpbc_register(zpbc_t* self,const ProtobufCMessageDescriptor* descriptor) {
	assert( self );
	assert( descriptor );

	void* p = zhash_lookup(self->pbmsg_hash,descriptor->name);
	if( NULL == p ) {
		zhash_insert(self->pbmsg_hash,descriptor->name,(void*)descriptor);
	}
	return 0;
}



int zpbc_send(const ProtobufCMessage* m,void* dest,int more) {
	assert( m );
	assert( dest );

	zframe_t* fr = NULL;
	const size_t fsize = protobuf_c_message_get_packed_size(m);
	do {
		if( -1 == zstr_sendm(dest,"#pb") )
			break;
		if( -1 == zstr_sendm(dest,m->descriptor->name) )
			break;

		fr = zframe_new(NULL,fsize);
		if( fsize > 0 ) {
			if( fsize != protobuf_c_message_pack(m,(uint8_t*)zframe_data(fr)) ) {
				break;
			}
		}
		return zframe_send(&fr,dest,(more ? ZMQ_SNDMORE : 0));
	} while( 0 );

	if( fr ) {
		zframe_destroy(&fr);
	}
	zsock_flush(dest);
	return -1;
}


ProtobufCMessage* zpbc_recv(zpbc_t* self,void* source) {
	assert( self );
	assert( source );

	ProtobufCMessage* m = NULL;
	zframe_t* fr = NULL;
	char* name = NULL;
	const ProtobufCMessageDescriptor* p = NULL;

	do {
		// magic part
		fr = zframe_recv(source);
		if( NULL == fr )
			break;
		if( ! zframe_streq(fr,"#pb") )
			break;
		zframe_destroy(&fr);

		// name part
		if( ! zsock_rcvmore(source) )
			break;
		name = zstr_recv(source);
		if( NULL == name )
			break;
		p = (const ProtobufCMessageDescriptor*) zhash_lookup(self->pbmsg_hash,name);
		if( NULL == p )
			break;
		zstr_free(&name);

		// body part
		if( ! zsock_rcvmore(source) )
			break;
		fr = zframe_recv(source);
		if( NULL == fr )
			break;
		m = protobuf_c_message_unpack(p,NULL,zframe_size(fr),zframe_data(fr));
		zframe_destroy(&fr);

		return m;
	} while(0);

	if( name ) {
		zstr_free(&name);
	}
	if( fr ) {
		zframe_destroy(&fr);
	}
	zsock_flush(source);
	return NULL;
}


