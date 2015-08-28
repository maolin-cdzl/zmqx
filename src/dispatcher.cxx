#include "zmqx/dispatcher.h"

Dispatcher::Dispatcher() {
	unset_default();
}

Dispatcher::~Dispatcher() {
}

void Dispatcher::set_default(pb_default_process_fn proc,void* args) {
	m_def_proc = proc;
	m_def_args = args;
}

void Dispatcher::unset_default() {
	m_def_proc = nullptr;
	m_def_args = nullptr;
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
	} else if( m_def_proc ) {
		m_def_proc(msg,0,m_def_args);
		return 0;
	} else {
		return -1;
	}
}

int Dispatcher::trigger(int err) {
	if( m_def_proc ) {
		m_def_proc(nullptr,err,m_def_args);
		return 0;
	} else {
		return -1;
	}
}

