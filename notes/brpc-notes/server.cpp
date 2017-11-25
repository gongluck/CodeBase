#include "../../output/include/butil/logging.h"
#include "../../output/include/brpc/server.h"
#include "echo.pb.h"

class MyEchoService : public EchoService
{
public:
    virtual void Echo(::google::protobuf::RpcController* controller,
                      const ::EchoRequest* request,
                      ::EchoResponse* response,
                      ::google::protobuf::Closure* done)
    {
        brpc::ClosureGuard done_guard(done);
        brpc::Controller* cntl = static_cast<brpc::Controller*>(controller);
        response->set_message(request->message());

        LOG(INFO) << "remote_side=" << cntl->remote_side();
        printf("remote_side=%s\n", 
               butil::endpoint2str(cntl->remote_side()).c_str());
    }
};

int main(int argc, char* argv[])
{
    brpc::Server server;
    MyEchoService myechoservice;

    if(server.AddService(&myechoservice, brpc::SERVER_OWNS_SERVICE) != 0)
    {
        LOG(FATAL) << "Fail to add myechoservice";
        return -1;
    }

    if(server.Start("localhost:9000", NULL) != 0)
    {
        LOG(ERROR) << "Fail to start EchoServer";
        return -1;
    }

    server.RunUntilAskedToQuit();

    return 0;
}
