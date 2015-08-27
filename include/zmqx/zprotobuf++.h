#pragma once

#include <czmq.h>
#include <google/protobuf/message.h>
#include <memory>


int zpb_send(void* sock,const google::protobuf::Message& msg,bool more=false);

std::shared_ptr<google::protobuf::Message> zpb_recv(void* sock);


