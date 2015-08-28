#pragma once

#include <memory>
#include <unordered_map>
#include <google/protobuf/message.h>



class Dispatcher {
public:
	typedef void (*pb_default_process_fn)(const std::shared_ptr<google::protobuf::Message>& msg,int err,void* args);
	typedef void (*pb_msg_process_fn)(const std::shared_ptr<google::protobuf::Message>& msg,void* args);
public:
	Dispatcher();
	~Dispatcher();

	void set_default(pb_default_process_fn proc,void* args);
	void unset_default();

	void register_processer(const google::protobuf::Descriptor* desc,pb_msg_process_fn proc,void* args);
	void register_processer(const std::string& type,pb_msg_process_fn proc,void* args);
	void unregister_processer(const google::protobuf::Descriptor* desc);
	void unregister_processer(const std::string& type);

	int deliver(const std::shared_ptr<google::protobuf::Message>& msg);
	int trigger(int err);
private:
	struct processer_t {
		pb_msg_process_fn		proc;
		void*					args;
	};
private:
	pb_default_process_fn		m_def_proc;
	void*						m_def_args;
	std::unordered_map<const google::protobuf::Descriptor*,processer_t> m_processers;
};

