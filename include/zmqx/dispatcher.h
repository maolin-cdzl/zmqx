#pragma once

#include <memory>
#include <functional>
#include <unordered_map>
#include <google/protobuf/message.h>


class Dispatcher {
public:
	typedef std::function<int(int)>	error_handler_t;
	typedef std::function<int(const std::shared_ptr<google::protobuf::Message>&)> processer_t;
public:
	Dispatcher();
	~Dispatcher();

	void set_default(const processer_t& proc);
	void unset_default();

	void set_error_handler(const error_handler_t& handler);
	void unset_error_handler();

	void register_processer(const google::protobuf::Descriptor* desc,const processer_t& proc);
	void register_processer(const std::string& type,const processer_t& proc);

	void unregister_processer(const google::protobuf::Descriptor* desc);
	void unregister_processer(const std::string& type);

	int deliver(const std::shared_ptr<google::protobuf::Message>& msg);
	int trigger(int err);
private:
	static int defaultMessage(const std::shared_ptr<google::protobuf::Message>& msg);
	static int defaultError(int err);
private:
	processer_t					m_default;
	error_handler_t				m_err_handler;
	std::unordered_map<const google::protobuf::Descriptor*,processer_t> m_processers;
};

