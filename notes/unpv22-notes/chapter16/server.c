#include <unistd.h>
#include "data.h"

square_out* squareproc_1_svc(square_in* inp, struct svc_req* rqstp)
{
    static square_out out;

    printf("arg = %ld\n", inp->arg1);
    sleep(5);
    out.res1 = inp->arg1 * inp->arg1;
    printf("arg = %ld\n", inp->arg1);

    return &out;
}

bool_t squareproc_2_svc(square_in* inp, square_out* outp, struct svc_req* rqstp)
{
    printf("arg = %ld\n", inp->arg1);
    if(rqstp->rq_cred.oa_flavor == AUTH_SYS)
    {
        struct authunix_parms* au;
        au = (struct authunix_parms*)rqstp->rq_clntcred;
        printf("AUTH_SYS: host %s, uid %ld, gid %ld\n",
               au->aup_machname, (long)au->aup_uid, (long)au->aup_gid);
    }
    sleep(5);
    outp->res1 = inp->arg1 * inp->arg1;
    printf("arg = %ld\n", inp->arg1);
    
    return TRUE;
}

int square_prog_2_freeresult(SVCXPRT* transp, xdrproc_t xdr_result, caddr_t result)
{
    xdr_free(xdr_result, result);

    return 1;
}
