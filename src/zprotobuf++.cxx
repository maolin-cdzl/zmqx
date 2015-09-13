#include "zmqx/zprotobuf++.h"
#include <google/protobuf/descriptor.h>
#include <glog/logging.h>

static std::shared_ptr<google::protobuf::Message> create_message(const std::string& type_name);

static int zpb_do_send(void* sock,const google::protobuf::Message& msg,bool delimiter,bool more) {
	zframe_t* fr = nullptr;
	do {
		if( delimiter ) {
			if( -1 == zstr_sendm(sock,"") ) {
				DLOG(ERROR) << "send delimiter failed";
				break;
			}
		}
		if( -1 == zstr_sendm(sock,"#pb") ) {
			DLOG(ERROR) << "send magic failed";
			break;
		}
		if( -1 == zstr_sendm(sock,msg.GetTypeName().c_str()) ) {
			DLOG(ERROR) << "send message name failed";
			break;
		}

		const size_t fsize = msg.ByteSize();
		fr = zframe_new(nullptr,fsize);
		if( fsize > 0 ) {
			if( ! msg.SerializeToArray(zframe_data(fr),zframe_size(fr)) ) {
				DLOG(ERROR) << "serialize message failed";
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

int zpb_send(void* sock,const google::protobuf::Message& msg,bool delimiter) {
	return zpb_do_send(sock,msg,delimiter,false);
}

int zpb_sendm(void* sock,const google::protobuf::Message& msg,bool delimiter) {
	return zpb_do_send(sock,msg,delimiter,true);
}

std::shared_ptr<google::protobuf::Message> zpb_recv(void* sock) {
	zframe_t* fr = nullptr;
	char* msg_name = nullptr;
	do {
		// magic part
		bool magic_good = false;
		do {
			fr = zframe_recv(sock);
			if( nullptr == fr ) {
				DLOG(WARNING) << "zpb magic frame null";
				break;
			}

			if( zframe_size(fr) > 0 ) {
				if( zframe_streq(fr,"#pb") ) {
					magic_good = true;
				} else {
					DLOG(WARNING) << "zpb magic mismatched";
				}
				zframe_destroy(&fr);
				break;
			}
			DLOG(INFO) << "drop empty frame";
			zframe_destroy(&fr);
			if( ! zsock_rcvmore(sock) ) {
				DLOG(WARNING) << "zpb socket no more frame when recv magic";
				break;
			}
		} while(true);

		if( ! magic_good ) {
			break;
		}

		// name part
		if( ! zsock_rcvmore(sock) ) {
			DLOG(WARNING) << "zpb socket no more frame when recv message name";
			break;
		}
		msg_name = zstr_recv(sock);
		if( nullptr == msg_name ) {
			DLOG(WARNING) << "zpb message name frame null";
			break;
		}

		// body part
		if( ! zsock_rcvmore(sock) ) {
			DLOG(WARNING) << "zpb socket no more frame when recv message body";
			break;
		}
		fr = zframe_recv(sock);
		if( nullptr == fr ) {
			DLOG(WARNING) << "zpb body frame null";
			break;
		}

		auto msg = create_message(msg_name);
		if( msg == nullptr ) {
			DLOG(WARNING) << "zpb can not create message: " << msg_name;
			break;
		}
		zstr_free(&msg_name);

		if( zframe_size(fr) > 0 ) {
			if( ! msg->ParseFromArray( zframe_data(fr), zframe_size(fr) ) ) {
				DLOG(WARNING) << "zpb message parse error";
				break;
			}
		} else if( ! msg->IsInitialized() ) {
			DLOG(WARNING) << "zpb empty message is not initialized";
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
		bool magic_good = false;
		do {
			fr = zframe_recv(sock);
			if( nullptr == fr ) {
				DLOG(WARNING) << "zpb magic frame null";
				break;
			}

			if( zframe_size(fr) > 0 ) {
				if( zframe_streq(fr,"#pb") ) {
					magic_good = true;
				} else {
					DLOG(WARNING) << "zpb magic mismatched";
				}
				zframe_destroy(&fr);
				break;
			}
			DLOG(INFO) << "drop empty frame";
			zframe_destroy(&fr);
			if( ! zsock_rcvmore(sock) ) {
				DLOG(WARNING) << "zpb socket no more frame when recv magic";
				break;
			}
		} while(true);

		if( ! magic_good ) {
			break;
		}

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
