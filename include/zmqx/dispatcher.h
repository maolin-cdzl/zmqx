#pragma once

#include <memory>
#include <functional>
#include <unordered_map>
#include <google/protobuf/message.h>


class Dispatcher {
public:
	typedef int (*pb_default_process_fn)(const std::shared_ptr<google::protobuf::Message>& msg,int err,void* args);
	typedef int (*pb_msg_process_fn)(const std::shared_ptr<google::protobuf::Message>& msg,void* args);

	typedef std::function<int (const std::shared_ptr<google::protobuf::Message>&,int)>	default_process_t;
	typedef std::function<int (const std::shared_ptr<google::protobuf::Message>&)>		processer_t;
public:
	Dispatcher();
	~Dispatcher();

	void set_default(pb_default_process_fn proc,void* args);
	void set_default(const default_process_t& proc);
	void unset_default();

	void register_processer(const google::protobuf::Descriptor* desc,pb_msg_process_fn proc,void* args);
	void register_processer(const std::string& type,pb_msg_process_fn proc,void* args);
	void register_processer(const google::protobuf::Descriptor* desc,const processer_t& proc);

	void unregister_processer(const google::protobuf::Descriptor* desc);
	void unregister_processer(const std::string& type);

	int deliver(const std::shared_ptr<google::protobuf::Message>& msg);
	int trigger(int err);
private:
	static int defaultNothing(const std::shared_ptr<google::protobuf::Message>& msg,int err,void* args);
private:
	default_process_t	m_default;
	std::unordered_map<const google::protobuf::Descriptor*,processer_t> m_processers;
};

