#pragma once

#include <czmq.h>
#include <google/protobuf/message.h>
#include <memory>

#include "zmqx/zenvelope.h"

int zpb_send(void* sock,const google::protobuf::Message& msg,bool delimiter=false);
int zpb_sendm(void* sock,const google::protobuf::Message& msg,bool delimiter=false);

std::shared_ptr<google::protobuf::Message> zpb_recv(void* sock);

int zpb_recv(google::protobuf::Message& msg,void* sock);

int zpb_send(void* sock,std::unique_ptr<ZEnvelope> envelope,const google::protobuf::Message& msg);
int zpb_sendm(void* sock,std::unique_ptr<ZEnvelope> envelope,const google::protobuf::Message& msg);

int zpb_pub_send(void* sock,const std::string& envelope,const google::protobuf::Message& msg);
int zpb_pub_sendm(void* sock,const std::string& envelope,const google::protobuf::Message& msg);

std::pair<std::string,std::shared_ptr<google::protobuf::Message>> zpb_sub_recv(void* sock);


