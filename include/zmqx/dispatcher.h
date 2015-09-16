#pragma once

#include <memory>
#include <functional>
#include <unordered_map>
#include <google/protobuf/message.h>
#include <glog/logging.h>


template<typename ...Args>
class base_dispatcher {
	typedef std::function<int(int)>	error_handler_t;
	typedef std::function<int(const std::shared_ptr<google::protobuf::Message>&,Args...)> processer_t;
public:
	base_dispatcher() :
		m_default(nullptr),
		m_err_handler(nullptr)
	{
	}

	~base_dispatcher() {
	}

	void set_default(const processer_t& proc) {
		m_default = proc;
	}
	void unset_default() {
		m_default = nullptr;
	}

	void set_error_handler(const error_handler_t& handler) {
		m_err_handler = handler;
	}
	void unset_error_handler() {
		m_err_handler = nullptr;
	}

	void register_processer(const google::protobuf::Descriptor* desc,const processer_t& proc) {
		m_processers[desc] = proc;
	}

	void register_processer(const std::string& type,const processer_t& proc) {
		const google::protobuf::Descriptor* desc = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(type);

		CHECK_NOTNULL( desc );
		register_processer(desc,proc);
	}

	void unregister_processer(const google::protobuf::Descriptor* desc) {
		auto it = m_processers.find(desc);
		if( it != m_processers.end() ) {
			m_processers.erase(it);
		}
	}

	void unregister_processer(const std::string& type) {
		const google::protobuf::Descriptor* desc = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(type);

		CHECK_NOTNULL( desc );
		unregister_processer(desc);
	}

	int deliver(const std::shared_ptr<google::protobuf::Message>& msg,Args... args) {
		auto it = m_processers.find( msg->GetDescriptor() );
		if( it != m_processers.end() ) {
			return (it->second)(msg,args...);
		} else if( m_default ) {
			return m_default(msg,args...);
		} else {
			LOG(WARNING) << "base_dispatcher handle unprocess message: " << msg->GetTypeName();
			return 0;
		}
	}

	int trigger(int err) {
		if( m_err_handler ) {
			return m_err_handler(err);
		} else {
			LOG(WARNING) << "base_dispatcher handle error: " << err;
			return 0;
		}
	}
private:
	processer_t					m_default;
	error_handler_t				m_err_handler;
	std::unordered_map<const google::protobuf::Descriptor*,processer_t> m_processers;
};


template<>
class base_dispatcher<> {
public:
	typedef std::function<int(int)>	error_handler_t;
	typedef std::function<int(const std::shared_ptr<google::protobuf::Message>&)> processer_t;
public:
	base_dispatcher() :
		m_default(nullptr),
		m_err_handler(nullptr)
	{
	}

	~base_dispatcher() {
	}

	void set_default(const processer_t& proc) {
		m_default = proc;
	}
	void unset_default() {
		m_default = nullptr;
	}

	void set_error_handler(const error_handler_t& handler) {
		m_err_handler = handler;
	}
	void unset_error_handler() {
		m_err_handler = nullptr;
	}

	void register_processer(const google::protobuf::Descriptor* desc,const processer_t& proc) {
		m_processers[desc] = proc;
	}

	void register_processer(const std::string& type,const processer_t& proc) {
		const google::protobuf::Descriptor* desc = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(type);

		CHECK_NOTNULL( desc );
		register_processer(desc,proc);
	}

	void unregister_processer(const google::protobuf::Descriptor* desc) {
		auto it = m_processers.find(desc);
		if( it != m_processers.end() ) {
			m_processers.erase(it);
		}
	}

	void unregister_processer(const std::string& type) {
		const google::protobuf::Descriptor* desc = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(type);

		CHECK_NOTNULL( desc );
		unregister_processer(desc);
	}

	int deliver(const std::shared_ptr<google::protobuf::Message>& msg) {
		auto it = m_processers.find( msg->GetDescriptor() );
		if( it != m_processers.end() ) {
			return (it->second)(msg);
		} else if( m_default ) {
			return m_default(msg);
		} else {
			LOG(WARNING) << "base_dispatcher handle unprocess message: " << msg->GetTypeName();
			return 0;
		}
	}

	int trigger(int err) {
		if( m_err_handler ) {
			return m_err_handler(err);
		} else {
			LOG(WARNING) << "base_dispatcher handle error: " << err;
			return 0;
		}
	}

private:
	processer_t					m_default;
	error_handler_t				m_err_handler;
	std::unordered_map<const google::protobuf::Descriptor*,processer_t> m_processers;
};


