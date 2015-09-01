#pragma once

#include <memory>
#include <functional>
#include <unordered_map>
#include <google/protobuf/message.h>


class Dispatcher {
public:
	typedef void (*pb_default_process_fn)(const std::shared_ptr<google::protobuf::Message>& msg,int err,void* args);
	typedef void (*pb_msg_process_fn)(const std::shared_ptr<google::protobuf::Message>& msg,void* args);

	typedef std::function<void(const std::shared_ptr<google::protobuf::Message>&,int)>	default_process_t;
	typedef std::function<void(const std::shared_ptr<google::protobuf::Message>&)>		processer_t;
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
	void trigger(int err);

	template<typename C,typename F>
	void register_member_processer(const google::protobuf::Descriptor* desc,F func,C* obj) {
		processer_t pt = std::bind(func,obj,std::placeholders::_1);
		m_processers[desc] = pt;
	}

	template<typename C,typename F>
	void set_member_default(F func,C* obj) {
		m_default = std::bind(func,obj,std::placeholders::_1,std::placeholders::_2);
	}
private:
	static void defaultNothing(const std::shared_ptr<google::protobuf::Message>& msg,int err,void* args);
private:
	default_process_t	m_default;
	std::unordered_map<const google::protobuf::Descriptor*,processer_t> m_processers;
};

