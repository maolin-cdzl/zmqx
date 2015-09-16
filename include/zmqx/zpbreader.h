#pragma once

#include <czmq.h>
#include "zmqx/dispatcher.h"
#include "zmqx/zloopreader.h"
#include "zmqx/zenvelope.h"
#include "zmqx/zprotobuf++.h"


typedef base_dispatcher<zsock_t*> sock_dispatcher_t;

int zpb_deliver(const std::shared_ptr<sock_dispatcher_t>& dispatcher,zsock_t* sock);

typedef base_dispatcher<zsock_t*,std::unique_ptr<ZEnvelope>&> envelope_dispatcher_t;

int zpb_envelop_deliver(const std::shared_ptr<envelope_dispatcher_t>& dispatcher,zsock_t* sock);

std::shared_ptr<ZLoopReader> make_zpb_reader(zloop_t* loop,zsock_t* sock,const std::shared_ptr<sock_dispatcher_t>& dispatcher);

std::shared_ptr<ZLoopReader> make_zpb_reader(zloop_t* loop,zsock_t** sock,const std::shared_ptr<sock_dispatcher_t>& dispatcher);

std::shared_ptr<ZLoopReader> make_zpb_reader(zloop_t* loop,zsock_t* sock,const std::shared_ptr<envelope_dispatcher_t>& dispatcher);

std::shared_ptr<ZLoopReader> make_zpb_reader(zloop_t* loop,zsock_t** sock,const std::shared_ptr<envelope_dispatcher_t>& dispatcher);


