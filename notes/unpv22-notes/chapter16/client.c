#include "data.h"

int main(int argc, const char* argv[])
{
    CLIENT* cl;
    square_in in;
    square_out out;
    struct timeval tv;

    if(argc != 3)
    {
        printf("usage : client <hostname> <integer-value>\n");
        exit(-1);
    }

    cl = clnt_create(argv[1], SQUARE_PROG, SQUARE_VERS, "udp");

    auth_destroy(cl->cl_auth);//销毁先前句柄关联的认证，空认证
    cl->cl_auth = authunix_create_default();//创建默认的Unix认证结构

    tv.tv_sec = 30;
    tv.tv_usec = 0;
    clnt_control(cl, CLSET_TIMEOUT, (char*)&tv);
    clnt_control(cl, CLSET_RETRY_TIMEOUT, (char*)&tv);

    if(clnt_control(cl, CLGET_TIMEOUT, (char*)&tv))
        printf("timeout = %ld sec, %ld usec\n", tv.tv_sec, tv.tv_usec);
    if(clnt_control(cl, CLGET_RETRY_TIMEOUT, (char*)&tv))
        printf("retry timeout = %ld sec, %ld usec\n", tv.tv_sec, tv.tv_usec);

    in.arg1 = atol(argv[2]);
    if(squareproc_2(&in, &out, cl) != RPC_SUCCESS)
    {
        printf("%s", clnt_sperror(cl, argv[1]));
        exit(-1);
    }
    printf("result : %ld\n", out.res1);

    return 0;
}
