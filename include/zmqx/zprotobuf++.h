#pragma once

#include <czmq.h>
#include <google/protobuf/message.h>
#include <memory>


int zpb_send(void* sock,const google::protobuf::Message& msg,bool delimiter=false);
int zpb_sendm(void* sock,const google::protobuf::Message& msg,bool delimiter=false);

std::shared_ptr<google::protobuf::Message> zpb_recv(void* sock);

int zpb_recv(google::protobuf::Message& msg,void* sock);


