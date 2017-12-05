/******************************************************************* 
*  Copyright(c) 2017 
*  All rights reserved. 
* 
*  文件名称: Bmp.h 
*  简要描述: bmp位图相关处理 
* 
*  创建日期: 2017-08-04 
*  作者:  gongluck 
*  说明: 
* 
*  修改日期: 2017-08-14
*  作者:  gongluck
*  说明: 增强SaveBmpWithFileInfo的功能，检查扫描行
******************************************************************/
#ifndef __BMP_H__
#define __BMP_H__

#include <Windows.h>
#include <stdio.h>

//根据文件头和dib数据保存bmp文件
bool SaveBmpWithFileInfo(const char* filename,const BITMAPFILEHEADER* fileheader,const BITMAPINFO* bmpinfo,const void* bits,bool needChange = FALSE);
//根据位深度、像素宽、像素高和dib数据保存bmp文件
//深度24以下的不能处理（没有考虑有调色板的情况）
bool SaveBmpWithBitscountAndWH(const char* filename,int nBits,int width,int height,const void* bits,bool needChange = FALSE);
//由位图句柄（HBITMAP）保存成bmp图片
//可以用CreateDIBitmap、CreateDIBSection等函数获取HBITMAP
bool SavePicture(HBITMAP hbitmap,const char* filepath);
//将YV12转BGR24，没有效率可言（推荐使用ffmpeg）
bool YV12ToBGR24_Native(unsigned char* pYUV,unsigned char* pBGR24,int width,int height);

#endif 