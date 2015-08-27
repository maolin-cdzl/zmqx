#include "zmqx/zprotobuf++.h"
#include <google/protobuf/descriptor.h>

static google::protobuf::Message* create_message(const std::string& type_name);
int zpb_send(void* sock,const google::protobuf::Message& msg,bool more) {
	std::string blk( msg.SerializeAsString() );

	zmsg_t* zmsg = zmsg_new();
	zmsg_addstr(zmsg,"#pb");
	zmsg_addstr(zmsg,msg.GetTypeName().c_str());
	zmsg_addstr(zmsg,"");
	zmsg_addmem(zmsg,blk.c_str(),blk.size());

	if( more ) {
		return zmsg_sendm(&zmsg,sock);
	} else {
		return zmsg_send(&zmsg,sock);
	}
}

std::shared_ptr<google::protobuf::Message> zpb_recv(void* sock) {
	std::shared_ptr<google::protobuf::Message> msg;
	google::protobuf::Message* pm = nullptr;
	
	zmsg_t* zmsg = zmsg_recv(sock);

	do {
		zframe_t* fr = zmsg_first(zmsg);
		if( nullptr == fr )
			break;
		if( ! zframe_streq(fr,"#pb") )
			break;

		fr = zmsg_next(zmsg);
		if( nullptr == fr )
			break;
		char* str = zframe_strdup(fr);
		if( nullptr == str )
			break;
		pm = create_message(str);
		free(str);
		if( ! pm )
			break;

		fr = zmsg_next(zmsg);
		if( nullptr == fr )
			break;
		if( zframe_size(fr) != 0 )
			break;
		
		fr = zmsg_next(zmsg);
		if( nullptr == fr )
			break;

		if( zframe_size(fr) > 0 ) {
			if( ! pm->ParseFromArray( zframe_data(fr), zframe_size(fr) ) ) {
				break;
			}
		}

		msg = std::shared_ptr<google::protobuf::Message>( pm );
		pm = nullptr;
	} while(0);

	if( zmsg ) {
		zmsg_destroy(&zmsg);
	}
	if( pm ) {
		delete pm;
	}

	return msg;
}

static google::protobuf::Message* create_message(const std::string& type_name)
{
	google::protobuf::Message* message = NULL;
	const google::protobuf::Descriptor* descriptor = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(type_name);
	if (descriptor) {
		const google::protobuf::Message* prototype = google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
		if (prototype) {
			message = prototype->New();
		}
	}
	return message;
}
