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

	zmsg_t* zmsg = zmsg_new();
	zmsg_addstr(zmsg,"#pb");
	zmsg_addstr(zmsg,m->descriptor->name);
	zmsg_addstr(zmsg,"");

	size_t len = protobuf_c_message_get_packed_size(m);
	if( len ) {
		zframe_t* dfr = zframe_new(NULL,len);
		protobuf_c_message_pack(m,(uint8_t*)zframe_data(dfr));
		zmsg_append(zmsg,&dfr);
	} else {
		zmsg_addstr(zmsg,"");
	}
	if( more ) {
		return zmsg_sendm(&zmsg,dest);
	} else {
		return zmsg_send(&zmsg,dest);
	}
}


ProtobufCMessage* zpbc_recv(zpbc_t* self,void* source) {
	assert( self );
	assert( source );

	ProtobufCMessage* m = NULL;
	char* name = NULL;
	const ProtobufCMessageDescriptor* p = NULL;

	zmsg_t* zmsg = zmsg_recv(source);
	do {
		zframe_t* fr = zmsg_first(zmsg);
		if( NULL == fr )
			break;
		if( ! zframe_streq(fr,"#pb") )
			break;

		fr = zmsg_next(zmsg);
		if( NULL == fr )
			break;
		name = zframe_strdup(fr);
		if( NULL == name )
			break;
		p = (const ProtobufCMessageDescriptor*) zhash_lookup(self->pbmsg_hash,name);
		if( NULL == p )
			break;

		fr = zmsg_next(zmsg);
		if( NULL == fr )
			break;
		if( zframe_size(fr) != 0 )
			break;
		
		fr = zmsg_next(zmsg);
		if( NULL == fr )
			break;

		m = protobuf_c_message_unpack(p,NULL,zframe_size(fr),zframe_data(fr));
	} while(0);

	if( name ) {
		free(name);
	}
	if( zmsg ) {
		zmsg_destroy(&zmsg);
	}

	return m;
}


