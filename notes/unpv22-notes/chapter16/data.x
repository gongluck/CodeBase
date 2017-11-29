struct square_in
{
    long arg1;
};

struct square_out
{
    long res1;
};

program SQUARE_PROG/*RPC程序*/
{
    version SQUARE_VERS
    {
        square_out SQUAREPROC(square_in) = 1;/*过程*/
    }=2;/*版本号*/
}=0x31230000;/*程序号*/
