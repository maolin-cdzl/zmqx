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

class ZmqXTest: public testing::Test {
protected:
	static void SetUpTestCase() {
		receiver = zsock_new(ZMQ_SUB);
		CHECK_NOTNULL(receiver);
		zsock_set_subscribe(receiver,"");
		CHECK_NE(-1,zsock_connect(receiver,"tcp://127.0.0.1:7777")) << "connect receiver failed";

		sender = zsock_new(ZMQ_PUB);
		CHECK_NOTNULL(sender);
		CHECK_NE(-1,zsock_bind(sender,"tcp://127.0.0.1:7777")) << "bind sender failed";
		//make sure connected
		sleep(1);

		zstr_send(sender,"test");
		char* str = zstr_recv(receiver);
		CHECK_STREQ("test",str);
		zstr_free(&str);
	}
	static void TearDownTestCase() {
		zsock_destroy(&sender);
		zsock_destroy(&receiver);
	}

	static zsock_t*			sender;
	static zsock_t*			receiver;
};

zsock_t* ZmqXTest::sender = nullptr;
zsock_t* ZmqXTest::receiver = nullptr;

// ASSERT_XXX can not used in function not return void!


static void default_process_helper(const std::shared_ptr<google::protobuf::Message>& msg) {
	ASSERT_NE(msg,nullptr);
}

static int default_process(const std::shared_ptr<google::protobuf::Message>& msg) {
	default_process_helper(msg);
	return 0;
}

static void hello_process_helper(const std::shared_ptr<google::protobuf::Message>& msg) {
	ASSERT_NE( msg,nullptr);
	ASSERT_EQ(test::Hello::descriptor()->full_name(),msg->GetTypeName());
	ASSERT_STREQ(TEST_STR,std::dynamic_pointer_cast<test::Hello>(msg)->str().c_str());
}

static int hello_process(void* arg,const std::shared_ptr<google::protobuf::Message>& msg) {
	hello_process_helper(msg);
	if( arg ) {
		*(int*)arg = 1;
	}
	return 0;
}

static void shutdown_process_helper(const std::shared_ptr<google::protobuf::Message>& msg) {
	ASSERT_NE( msg,nullptr);
	ASSERT_EQ(test::Shutdown::descriptor()->full_name(),msg->GetTypeName());
}

static int shutdown_process(const std::shared_ptr<google::protobuf::Message>& msg) {
	shutdown_process_helper(msg);
	return -1;
}

class Processer {
public:
	int processDefault(const std::shared_ptr<google::protobuf::Message>& msg) {
		return default_process(msg);
	}

	int processHello(void* arg,const std::shared_ptr<google::protobuf::Message>& msg) {
		return hello_process(arg,msg);
	}

	int processShutdown(const std::shared_ptr<google::protobuf::Message>& msg) {
		return shutdown_process(msg);
	}
};


TEST_F(ZmqXTest,ZPBPlusPlus) {
	test::Hello hello;
	hello.set_str(TEST_STR);

	// case 1
	ASSERT_EQ(0,zpb_send(sender,hello));

	auto msg = zpb_recv(receiver);
	ASSERT_NE(nullptr,msg);
	ASSERT_EQ(test::Hello::descriptor()->full_name(),msg->GetTypeName());
	ASSERT_STREQ(TEST_STR,std::dynamic_pointer_cast<test::Hello>(msg)->str().c_str());

	// case 2
	ASSERT_EQ(0,zpb_send(sender,hello));
	
	hello.Clear();
	ASSERT_EQ(0,zpb_recv(hello,receiver));
	ASSERT_STREQ(TEST_STR,hello.str().c_str());

	// case 3
	test::Empty empty;
	ASSERT_EQ(0,zpb_send(sender,empty));
	
	msg = zpb_recv(receiver);
	ASSERT_NE(nullptr,msg);
	ASSERT_EQ(test::Empty::descriptor()->full_name(),msg->GetTypeName());

	// case 4
	ASSERT_EQ(0,zpb_send(sender,empty));
	ASSERT_EQ(0,zpb_recv(empty,receiver));

	// case 5
	zstr_send(sender,"bad message");
	ASSERT_EQ(nullptr,zpb_recv(receiver));

	// case 6
	zstr_send(sender,"bad message");
	ASSERT_EQ(-1,zpb_recv(empty,receiver));

	// case 7
	ASSERT_EQ(0,zpb_send(sender,hello,true));
	ASSERT_NE(nullptr,zpb_recv(receiver));

	// case 8
	ASSERT_EQ(0,zpb_send(sender,hello,true));
	ASSERT_EQ(0,zpb_recv(hello,receiver));

	// case 9
	ASSERT_EQ(0,zpb_sendm(sender,hello,false));
	ASSERT_EQ(0,zpb_send(sender,hello));
	ASSERT_NE(nullptr,zpb_recv(receiver));
	ASSERT_NE(nullptr,zpb_recv(receiver));

	// case 10
	ASSERT_EQ(0,zpb_sendm(sender,hello,true));
	ASSERT_EQ(0,zpb_send(sender,hello,true));
	ASSERT_NE(nullptr,zpb_recv(receiver));
	ASSERT_NE(nullptr,zpb_recv(receiver));
}

TEST_F(ZmqXTest,ZPB_Reliability) {
	test::Hello hello;
	hello.set_str(TEST_STR);

	// case 1
	ASSERT_EQ(0,zstr_sendm(sender,""));
	ASSERT_EQ(0,zstr_sendm(sender,""));
	ASSERT_EQ(0,zpb_send(sender,hello,true));
	ASSERT_NE(nullptr,zpb_recv(receiver));

	// case 2
	zstr_sendm(sender,"what ever");
	zstr_send(sender,"xxxxxxxxx");
	ASSERT_EQ(nullptr,zpb_recv(receiver));
	ASSERT_EQ(0,zpb_send(sender,hello));
	ASSERT_NE(nullptr,zpb_recv(receiver));

	// case 3
	zstr_send(sender,"#pb");
	ASSERT_EQ(nullptr,zpb_recv(receiver));
	ASSERT_EQ(0,zpb_send(sender,hello));
	ASSERT_NE(nullptr,zpb_recv(receiver));

	// case 4
	zstr_sendm(sender,"#pb");
	zstr_send(sender,"xxxxxxx");
	ASSERT_EQ(nullptr,zpb_recv(receiver));
	ASSERT_EQ(0,zpb_send(sender,hello));
	ASSERT_NE(nullptr,zpb_recv(receiver));

	// case 5
	zstr_sendm(sender,"#pb");
	zstr_sendm(sender,test::Hello::descriptor()->full_name().c_str());
	zstr_send(sender,"xxxxxxxx");
	ASSERT_EQ(nullptr,zpb_recv(receiver));
	ASSERT_EQ(0,zpb_send(sender,hello));
	ASSERT_NE(nullptr,zpb_recv(receiver));
}

TEST_F(ZmqXTest,Dispatcher) {
	auto disp = std::make_shared<Dispatcher>();
	int flag = 0;

	disp->set_default(default_process);
	disp->register_processer(test::Hello::descriptor()->full_name(),std::bind<int>(&hello_process,&flag,std::placeholders::_1));

	// case 1
	auto hello = std::make_shared<test::Hello>();
	hello->set_str(TEST_STR);
	
	ASSERT_EQ(0,disp->deliver(hello));
	ASSERT_EQ(1,flag);

	// case 2
	auto empty = std::make_shared<test::Empty>();
	ASSERT_EQ(0,disp->deliver(empty));

	// case 3
	Processer proc;
	disp->set_default(std::bind(&Processer::processDefault,&proc,std::placeholders::_1));
	disp->register_processer(test::Hello::descriptor(),std::bind(&Processer::processHello,&proc,&flag,std::placeholders::_1));

	flag = 0;
	ASSERT_EQ(0,disp->deliver(hello));
	ASSERT_EQ(flag,1);

	ASSERT_EQ(0,disp->deliver(empty));

	disp.reset();
}

TEST_F(ZmqXTest,ZDispatcher) {
	zloop_t* loop = zloop_new();
	auto zdisp = std::make_shared<ZDispatcher>(loop);
	auto disp = std::make_shared<Dispatcher>();

	Processer proc;
	int flag = 0;

	disp->set_default(std::bind(&Processer::processDefault,&proc,std::placeholders::_1));
	disp->register_processer(test::Hello::descriptor(),std::bind(&Processer::processHello,&proc,&flag,std::placeholders::_1));
	disp->register_processer(test::Shutdown::descriptor(),std::bind(&Processer::processShutdown,&proc,std::placeholders::_1));

	// case 1
	test::Hello hello;
	hello.set_str(TEST_STR);
	ASSERT_EQ(0,zpb_send(sender,hello));

	test::Empty empty;
	
	ASSERT_EQ(0,zpb_send(sender,empty));

	test::Shutdown shutdown;
	ASSERT_EQ(0,zpb_send(sender,shutdown));

	zdisp->start(receiver,disp);

	ASSERT_EQ(-1,zloop_start(loop));

	ASSERT_EQ(1,flag);

	zdisp.reset();
	disp.reset();
	zloop_destroy(&loop);
}



int main(int argc, char **argv) {
	google::InitGoogleLogging(argv[0]);
	zsys_init();
	::testing::InitGoogleTest(&argc, argv);
	int result = RUN_ALL_TESTS();
	zsys_shutdown();
	return result;
}
