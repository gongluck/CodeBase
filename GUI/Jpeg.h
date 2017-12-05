/******************************************************************* 
*  Copyright(c) 2017 
*  All rights reserved. 
* 
*  文件名称: Jpeg.h 
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
#ifndef __JPEG_H__
#define __JPEG_H__

#include <Windows.h>
#include <stdio.h>
#include "jpeglib.h"

//#pragma comment(lib,"libjpeg.lib")

/*
    1.outbuf为空时，返回解码内存需要的大小，并且通过参数返回位深度、图宽和图高
    2.outbuf不为空时，解码数据到outbuf
 */
int DecodeJpeg2Bmp(const char* filename,void* outbuf,int bufsize,int* nBits,int* width,int* height);

#endif