#include <string>
#include <memory>
#include <gtest/gtest.h>
#include <glog/logging.h>
#include <czmq.h>
#include "test.pb.h"
#include "zmqx/zprotobuf++.h"
#include "zmqx/dispatcher.h"
#include "zmqx/zdispatcher.h"

static const char* TEST_STR = "hello world";

static zsock_t*			sender = nullptr;
static zsock_t*			receiver = nullptr;

class ZmqXEnv : public testing::Environment {
public:
	// Override this to define how to set up the environment.
	virtual void SetUp() {
		receiver = zsock_new_sub("tcp://127.0.0.1:7777","");
		sender = zsock_new_pub("tcp://127.0.0.1:7777");
		//make sure sub connected pub
		sleep(1);
	}
	// Override this to define how to tear down the environment.
	virtual void TearDown() {
		zsock_destroy(&sender);
		zsock_destroy(&receiver);
	}
};

static void default_process(const std::shared_ptr<google::protobuf::Message>& msg,int err,void* arg) {
	if( err == 0 ) {
		ASSERT_NE(msg,nullptr);
	} else {
		ASSERT_EQ(msg,nullptr);
	}
}

static void hello_process(const std::shared_ptr<google::protobuf::Message>& msg,void* arg) {
	ASSERT_NE( msg,nullptr);
	ASSERT_EQ(test::Hello::descriptor()->full_name(),msg->GetTypeName());
	ASSERT_STREQ(TEST_STR,std::dynamic_pointer_cast<test::Hello>(msg)->str().c_str());

	if( arg ) {
		*(int*)arg = 1;
	}
}

static void shutdown_process(const std::shared_ptr<google::protobuf::Message>& msg,void* arg) {
	ASSERT_NE( msg,nullptr);
	ASSERT_EQ(test::Shutdown::descriptor()->full_name(),msg->GetTypeName());

	zsys_interrupted = 1;
}

class Processer {
public:
	void processDefault(const std::shared_ptr<google::protobuf::Message>& msg,int err) {
		default_process(msg,err,nullptr);
	}

	void processHello(const std::shared_ptr<google::protobuf::Message>& msg) {
		hello_process(msg,nullptr);
	}

	void processShutdown(const std::shared_ptr<google::protobuf::Message>& msg) {
		shutdown_process(msg,nullptr);
	}
};


TEST(ZmqXTest,ZPBPlusPlus) {
	int result = -1;
	test::Hello hello;
	hello.set_str(TEST_STR);

	// case 1
	result = zpb_send(sender,hello);
	ASSERT_EQ( result, 0 );

	auto msg = zpb_recv(receiver);
	ASSERT_NE( msg,nullptr);
	ASSERT_EQ(test::Hello::descriptor()->full_name(),msg->GetTypeName());
	ASSERT_STREQ(TEST_STR,std::dynamic_pointer_cast<test::Hello>(msg)->str().c_str());

	// case 2
	result = zpb_send(sender,hello);
	ASSERT_EQ( result, 0 );
	
	hello.Clear();
	result = zpb_recv(hello,receiver);
	ASSERT_EQ( result, 0 );
	ASSERT_STREQ(TEST_STR,hello.str().c_str());

	// case 3
	test::Empty empty;
	result = zpb_send(sender,empty);
	ASSERT_EQ( result,0);
	
	msg = zpb_recv(receiver);
	ASSERT_NE( msg,nullptr);
	ASSERT_EQ(test::Empty::descriptor()->full_name(),msg->GetTypeName());

	// case 4
	result = zpb_send(sender,empty);
	ASSERT_EQ( result,0);
	
	result = zpb_recv(empty,receiver);
	ASSERT_EQ( result,0);

	// case 5
	zstr_send(sender,"bad message");
	msg = zpb_recv(receiver);
	ASSERT_EQ(msg,nullptr);

	// case 6
	zstr_send(sender,"bad message");
	result = zpb_recv(empty,receiver);
	ASSERT_EQ(result,-1);
}

TEST(ZmqXTest,Dispatcher) {
	auto disp = std::make_shared<Dispatcher>();
	int flag = 0;
	int result = -1;

	disp->set_default(default_process,nullptr);
	disp->register_processer(test::Hello::descriptor()->full_name(),hello_process,&flag);

	// case 1
	auto hello = std::make_shared<test::Hello>();
	hello->set_str(TEST_STR);

	result = disp->deliver(hello);
	ASSERT_EQ(result,1);
	ASSERT_EQ(flag,1);

	// case 2
	auto empty = std::make_shared<test::Empty>();
	result = disp->deliver(empty);
	ASSERT_EQ(result,0);

	// case 3
	Processer proc;
	disp->set_member_default(&Processer::processDefault,&proc);
	disp->register_member_processer(test::Hello::descriptor(),&Processer::processHello,&proc);

	result = disp->deliver(hello);
	ASSERT_EQ(result,1);

	result = disp->deliver(empty);
	ASSERT_EQ(result,0);

	disp.reset();
}

TEST(ZmqXTest,ZDispatcher) {
	zloop_t* loop = zloop_new();
	auto zdisp = std::make_shared<ZDispatcher>(loop);
	auto disp = std::make_shared<Dispatcher>();

	Processer proc;
	disp->set_member_default(&Processer::processDefault,&proc);
	disp->register_member_processer(test::Hello::descriptor(),&Processer::processHello,&proc);
	disp->register_member_processer(test::Shutdown::descriptor(),&Processer::processShutdown,&proc);

	int result = -1;

	// case 1
	test::Hello hello;
	hello.set_str(TEST_STR);
	result = zpb_send(sender,hello);
	ASSERT_EQ( result, 0 );

	test::Empty empty;
	result = zpb_send(sender,empty);
	ASSERT_EQ( result,0);

	test::Shutdown shutdown;
	result = zpb_send(sender,shutdown);
	ASSERT_EQ( result,0);

	zdisp->start(receiver,disp);

	result = zloop_start(loop);
	ASSERT_EQ(result,0);

	zdisp.reset();
	disp.reset();
	zloop_destroy(&loop);
}



int main(int argc, char **argv) {
	google::InitGoogleLogging(argv[0]);
	zsys_init();
	::testing::AddGlobalTestEnvironment(new ZmqXEnv());
	::testing::InitGoogleTest(&argc, argv);
	int result = RUN_ALL_TESTS();
	zsys_shutdown();
	return result;
}