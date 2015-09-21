#pragma once

#include <czmq.h>
#include <google/protobuf/message.h>
#include <memory>

#include "zmqx/zenvelope.h"

int zpb_send(void* sock,const google::protobuf::Message& msg,bool delimiter=false);
int zpb_sendm(void* sock,const google::protobuf::Message& msg,bool delimiter=false);

std::shared_ptr<google::protobuf::Message> zpb_recv(void* sock,bool flush=false);

int zpb_recv(google::protobuf::Message& msg,void* sock,bool flush=false);

int zpb_send(void* sock,const std::shared_ptr<ZEnvelope>& envelope,const google::protobuf::Message& msg);
int zpb_sendm(void* sock,const std::shared_ptr<ZEnvelope>& envelope,const google::protobuf::Message& msg);

int zpb_pub_send(void* sock,const std::string& topic,const google::protobuf::Message& msg);
int zpb_pub_sendm(void* sock,const std::string& topic,const google::protobuf::Message& msg);

std::pair<std::string,std::shared_ptr<google::protobuf::Message>> zpb_sub_recv(void* sock);


