#include "zmqx/dispatcher.h"

Dispatcher::Dispatcher() {
	unset_default();
}

Dispatcher::~Dispatcher() {
}

void Dispatcher::set_default(pb_msg_process_fn proc,void* args) {
	m_default.proc = proc;
	m_default.args = args;
}

void Dispatcher::unset_default() {
	m_default.proc = nullptr;
	m_default.args = nullptr;
}

void Dispatcher::register_processer(const google::protobuf::Descriptor* desc,pb_msg_process_fn proc,void* args) {
	assert( desc );
	assert( proc );

	processer_t pt;
	pt.proc = proc;
	pt.args = args;

	m_processers.insert( std::make_pair(desc,pt) );
}

void Dispatcher::register_processer(const std::string& type,pb_msg_process_fn proc,void* args) {

	const google::protobuf::Descriptor* desc = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(type);

	assert( desc );
	register_processer(desc,proc,args);
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
		it->second.proc(msg,it->second.args);
		return 1;
	} else if( m_default.proc ) {
		m_default.proc(msg,m_default.args);
		return 0;
	} else {
		return -1;
	}
}

