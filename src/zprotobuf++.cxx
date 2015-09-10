#include "zmqx/zprotobuf++.h"
#include <google/protobuf/descriptor.h>

static std::shared_ptr<google::protobuf::Message> create_message(const std::string& type_name);

int zpb_send(void* sock,const google::protobuf::Message& msg,bool more) {
	zframe_t* fr = nullptr;
	do {
		if( -1 == zstr_sendm(sock,"#pb") )
			break;
		if( -1 == zstr_sendm(sock,msg.GetTypeName().c_str()) )
			break;

		const size_t fsize = msg.ByteSize();
		fr = zframe_new(nullptr,fsize);
		if( fsize > 0 ) {
			if( ! msg.SerializeToArray(zframe_data(fr),zframe_size(fr)) ) {
				break;
			}
		}
		return zframe_send(&fr,sock,(more ? ZMQ_SNDMORE : 0));
	} while( 0 );

	if( fr ) {
		zframe_destroy(&fr);
	}
	zsock_flush(sock);
	return -1;
}

std::shared_ptr<google::protobuf::Message> zpb_recv(void* sock) {
	zframe_t* fr = nullptr;
	char* msg_name = nullptr;
	do {
		// magic part
		fr = zframe_recv(sock);
		if( nullptr == fr )
			break;
		if( ! zframe_streq(fr,"#pb") )
			break;
		zframe_destroy(&fr);

		// name part
		if( ! zsock_rcvmore(sock) )
			break;
		msg_name = zstr_recv(sock);
		if( nullptr == msg_name )
			break;

		// body part
		if( ! zsock_rcvmore(sock) )
			break;
		fr = zframe_recv(sock);
		if( nullptr == fr )
			break;
		

		auto msg = create_message(msg_name);
		zstr_free(&msg_name);

		if( zframe_size(fr) > 0 ) {
			if( ! msg->ParseFromArray( zframe_data(fr), zframe_size(fr) ) ) {
				break;
			}
		} else if( ! msg->IsInitialized() ) {
			break;
		}
		zframe_destroy(&fr);
		return msg;
	} while(0);

	if( msg_name ) {
		zstr_free(&msg_name);
	}
	if( fr ) {
		zframe_destroy(&fr);
	}
	zsock_flush(sock);

	return nullptr;
}

int zpb_recv(google::protobuf::Message& msg,void* sock) {
	zframe_t* fr = nullptr;
	do {
		// magic part
		fr = zframe_recv(sock);
		if( nullptr == fr )
			break;
		if( ! zframe_streq(fr,"#pb") )
			break;
		zframe_destroy(&fr);

		// name part
		if( ! zsock_rcvmore(sock) )
			break;
		fr = zframe_recv(sock);
		if( nullptr == fr )
			break;
		if( ! zframe_streq(fr,msg.GetTypeName().c_str()) )
			break;
		zframe_destroy(&fr);

		// body part
		if( ! zsock_rcvmore(sock) )
			break;
		fr = zframe_recv(sock);
		if( nullptr == fr )
			break;

		if( zframe_size(fr) > 0 ) {
			if( ! msg.ParseFromArray( zframe_data(fr), zframe_size(fr) ) ) {
				break;
			}
		} else if( ! msg.IsInitialized() ) {
			break;
		}
		zframe_destroy(&fr);
		return 0;
	} while(0);

	if( fr ) {
		zframe_destroy(&fr);
	}
	zsock_flush(sock);
	return -1;
}

static std::shared_ptr<google::protobuf::Message> create_message(const std::string& type_name)
{
	const google::protobuf::Descriptor* descriptor = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(type_name);
	if (descriptor) {
		const google::protobuf::Message* prototype = google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
		if (prototype) {
			return std::shared_ptr<google::protobuf::Message>(prototype->New());
		}
	}
	return nullptr;
}
