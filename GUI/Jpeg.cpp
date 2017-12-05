/******************************************************************* 
*  Copyright(c) 2017 
*  All rights reserved. 
* 
*  文件名称: Jpeg.cpp
*  简要描述: 使用Jpeglib
* 
*  创建日期: 2017-08-14 
*  作者:  gongluck 
*  说明: 
* 
*  修改日期: 
*  作者: 
*  说明: 
******************************************************************/
#include "Jpeg.h"

int DecodeJpeg2Bmp(const char* filename,void* outbuf,int bufsize,int* nBits,int* width,int* height)
{
    if(filename == NULL)
        return -1;

    //声明并初始化解压缩对象，同时制定错误信息管理器
    jpeg_decompress_struct cinfo = {0};
    jpeg_error_mgr error = {0};
    cinfo.err = jpeg_std_error(&error);
    jpeg_create_decompress(&cinfo);

    //打开jpg图像文件，并指定为解压缩对象的源文件
    FILE* finput = fopen(filename,"rb");
    if(finput == NULL)
        return -2;

    jpeg_stdio_src(&cinfo,finput);

    //读取图像信息
    jpeg_read_header(&cinfo,TRUE);

    //根据图像信息申请一个图像缓冲区
    if(outbuf == NULL)
    {
        *nBits = cinfo.num_components * 8;
        *width = cinfo.image_width;
        *height = cinfo.image_height;
        jpeg_destroy_decompress(&cinfo);
        fclose(finput);
        return sizeof(BYTE)*cinfo.image_width*(cinfo.image_height+1)*cinfo.num_components;
    }
        
    JSAMPROW data = (JSAMPROW)outbuf;

    //开始解压缩
    jpeg_start_decompress(&cinfo);
    JSAMPROW row_pointer[1];
    while (cinfo.output_scanline < cinfo.output_height)
    {
        row_pointer[0] = &data[(cinfo.output_scanline)*cinfo.image_width*cinfo.num_components];
        jpeg_read_scanlines(&cinfo,row_pointer ,1);
    }
    jpeg_finish_decompress(&cinfo);

    //释放资源
    jpeg_destroy_decompress(&cinfo);
    fclose(finput);
    return 0;
}