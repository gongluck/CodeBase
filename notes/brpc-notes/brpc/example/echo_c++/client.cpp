#include "../../output/include/butil/logging.h"
#include "../../output/include/brpc/channel.h"
#include "echo.pb.h"

int main(int argc, char* argv[])
{
    brpc::Channel channel;

    if(channel.Init("localhost:9000", NULL) != 0)
    {
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }

    EchoService_Stub stub(&channel);
    EchoRequest request;
    EchoResponse response;
    brpc::Controller cntl;

    request.set_message(argv[1]);
    stub.Echo(&cntl, &request, &response, NULL);
    if(!cntl.Failed())
    {
        LOG(INFO) << "Received from " << cntl.remote_side()
            << " to " << cntl.local_side();
        printf("Response : %s\n", response.message().c_str());
    }
    else
        LOG(WARNING) << cntl.ErrorText();

    return 0;
}
