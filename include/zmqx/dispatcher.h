#pragma once

#include <memory>
#include <unordered_map>
#include <google/protobuf/message.h>

typedef int (*pb_msg_process_fn)(const std::shared_ptr<google::protobuf::Message>& msg,void* args);

class Dispatcher {
public:
	Dispatcher();
	~Dispatcher();

	void set_default(pb_msg_process_fn proc,void* args);
	void unset_default();

	void register_processer(const google::protobuf::Descriptor* desc,pb_msg_process_fn proc,void* args);
	void register_processer(const std::string& type,pb_msg_process_fn proc,void* args);
	void unregister_processer(const google::protobuf::Descriptor* desc);
	void unregister_processer(const std::string& type);

	int deliver(const std::shared_ptr<google::protobuf::Message>& msg);
private:
	struct processer_t {
		pb_msg_process_fn		proc;
		void*					args;
	};
private:
	processer_t					m_default;
	std::unordered_map<const google::protobuf::Descriptor*,processer_t> m_processers;
};

