#include <glog/logging.h>
#include "zmqx/dispatcher.h"

using namespace std::placeholders;

Dispatcher::Dispatcher() :
	m_default(std::bind<int>(&Dispatcher::defaultNothing,_1,_2,nullptr))
{
}

Dispatcher::~Dispatcher() {
}

void Dispatcher::set_default(pb_default_process_fn proc,void* args) {
	m_default = std::bind<int>(proc,_1,_2,args);
}

void Dispatcher::set_default(const default_process_t& proc) {
	m_default = proc;
}

void Dispatcher::unset_default() {
	m_default = std::bind<int>(&Dispatcher::defaultNothing,_1,_2,nullptr); 
}

void Dispatcher::register_processer(const google::protobuf::Descriptor* desc,pb_msg_process_fn proc,void* args) {
	assert( desc );
	assert( proc );

	register_processer(desc,std::bind<int>(proc,_1,args));
}

void Dispatcher::register_processer(const std::string& type,pb_msg_process_fn proc,void* args) {

	const google::protobuf::Descriptor* desc = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(type);

	assert( desc );
	register_processer(desc,std::bind<int>(proc,_1,args));
}

void Dispatcher::register_processer(const google::protobuf::Descriptor* desc,const processer_t& proc) {
	m_processers[desc] = proc;
}

void Dispatcher::unregister_processer(const google::protobuf::Descriptor* desc) {
	auto it = m_processers.find(desc);
	if( it != m_processers.end() ) {
		m_processers.erase(it);
	}
}

void Dispatcher::unregister_processer(const std::string& type) {
	const google::protobuf::Descriptor* desc = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(type);

	assert( desc );
	unregister_processer(desc);
}

int Dispatcher::deliver(const std::shared_ptr<google::protobuf::Message>& msg) {
	auto it = m_processers.find( msg->GetDescriptor() );
	if( it != m_processers.end() ) {
		return (it->second)(msg);
	} else {
		return m_default(msg,0);
	}
}

int Dispatcher::trigger(int err) {
	return m_default(nullptr,err);
}

int Dispatcher::defaultNothing(const std::shared_ptr<google::protobuf::Message>& msg,int err,void*) {
	if( msg ) {
		LOG(WARNING) << "Dispatcher handle unprocess message: " << msg->GetTypeName();
	} else {
		LOG(WARNING) << "Dispatcher handle error: " << err;
	}
	return 0;
}

