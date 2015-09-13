#include <glog/logging.h>
#include "zmqx/dispatcher.h"

using namespace std::placeholders;

Dispatcher::Dispatcher() :
	m_default(std::bind<int>(&Dispatcher::defaultMessage,_1)),
	m_err_handler(std::bind<int>(&Dispatcher::defaultError,_1))
{
}

Dispatcher::~Dispatcher() {
}

void Dispatcher::set_default(const processer_t& proc) {
	m_default = proc;
}

void Dispatcher::unset_default() {
	m_default = std::bind<int>(&Dispatcher::defaultMessage,_1); 
}

void Dispatcher::set_error_handler(const error_handler_t& handler) {
	m_err_handler = handler;
}

void Dispatcher::unset_error_handler() {
	m_err_handler = std::bind<int>(&Dispatcher::defaultError,_1);
}

void Dispatcher::register_processer(const std::string& type,const processer_t& proc) {
	const google::protobuf::Descriptor* desc = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(type);

	CHECK_NOTNULL( desc );
	register_processer(desc,proc);
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

	CHECK_NOTNULL( desc );
	unregister_processer(desc);
}

int Dispatcher::deliver(const std::shared_ptr<google::protobuf::Message>& msg) {
	auto it = m_processers.find( msg->GetDescriptor() );
	if( it != m_processers.end() ) {
		return (it->second)(msg);
	} else {
		return m_default(msg);
	}
}

int Dispatcher::trigger(int err) {
	return m_err_handler(err);
}

int Dispatcher::defaultMessage(const std::shared_ptr<google::protobuf::Message>& msg) {
	LOG(WARNING) << "Dispatcher handle unprocess message: " << msg->GetTypeName();
	return 0;
}

int Dispatcher::defaultError(int err) {
	LOG(WARNING) << "Dispatcher handle error: " << err;
	return 0;
}


