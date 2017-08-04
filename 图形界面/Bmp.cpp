/******************************************************************* 
*  Copyright(c) 2017 
*  All rights reserved. 
* 
*  文件名称: Bmp.cpp 
*  简要描述: bmp位图相关处理 
* 
*  创建日期: 2017-08-04 
*  作者:  gongluck 
*  说明: 
* 
*  修改日期: 
*  作者:  
*  说明: 
******************************************************************/
#include "Bmp.h"

//根据文件头和dib数据保存bmp文件
bool SaveBmpWithFileInfo(const char* filename,const BITMAPFILEHEADER* fileheader,const BITMAPINFO* bmpinfo,const void* bits)
{
    FILE* fp = NULL;
    if(filename == NULL || fileheader == NULL || bmpinfo == NULL || bits == NULL)
        return false;
    if((fp = fopen(filename,"wb")) == NULL)
        return false;
    fwrite(fileheader,sizeof(BITMAPFILEHEADER),1,fp);
    fwrite(bmpinfo,sizeof(BITMAPINFO),1,fp);
    fwrite(bits,bmpinfo->bmiHeader.biSizeImage,1,fp);
    fclose(fp);
    return true;
}

//根据位深度、像素宽、像素高和dib数据保存bmp文件
//深度24以下的不能处理（没有考虑有调色板的情况）
bool SaveBmpWithBitscountAndWH(const char* filename,int nBits,int width,int height,const void* bits)
{
    BITMAPFILEHEADER header = {0};
    BITMAPINFO info = {0};

    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = nBits;
    info.bmiHeader.biCompression = BI_RGB;
    info.bmiHeader.biSizeImage = (width * nBits + 31) / 32 * 4 * height;

    header.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) + info.bmiHeader.biSizeImage;
    header.bfType = 0x4d42;//"BM"
    header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO);

    return SaveBmpWithFileInfo(filename,&header,&info,bits);
}

//由位图句柄（HBITMAP）保存成bmp图片
//可以用CreateDIBitmap、CreateDIBSection等函数获取HBITMAP
bool SavePicture(HBITMAP hbitmap,const char* filepath)
{  
    HDC hdc= GetDC(NULL);  
    BITMAP bitmap = {0};  
    WORD clrbits = 0;  
    BITMAPINFO* bitmapinfo = NULL;  
    BITMAPFILEHEADER fileheader;  
    DWORD temp;  
    void* buf = NULL;  
    FILE* fp = NULL;  

    GetObject(hbitmap,sizeof(BITMAP),&bitmap);  
    clrbits = bitmap.bmBitsPixel * bitmap.bmPlanes;//每点的颜色位数*平面数  
    if(clrbits == 1) ;  
    else if(clrbits <= 4) clrbits = 4;  
    else if(clrbits <= 8) clrbits = 8;  
    else if(clrbits <= 16) clrbits = 16;  
    else if(clrbits <= 24) clrbits = 24;  
    else clrbits = 32;  

    if(clrbits < 24)//24位以下需要调色板  
       bitmapinfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*(1<<clrbits));  
    else  
       bitmapinfo = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD));  

    bitmapinfo->bmiHeader.biBitCount = clrbits;//图像颜色位数  
    bitmapinfo->bmiHeader.biClrImportant = 0;  
    bitmapinfo->bmiHeader.biClrUsed = (clrbits<24) ? (1<<clrbits) : 0;//使用的颜色数  
    bitmapinfo->bmiHeader.biCompression = BI_RGB;//压缩方式,BI_RGB不压缩  
    bitmapinfo->bmiHeader.biHeight = bitmap.bmHeight;//位图高度  
    bitmapinfo->bmiHeader.biPlanes = bitmap.bmPlanes;//设备平面数  
    bitmapinfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);//BITMAPINFOHEADER结构体大小  
    bitmapinfo->bmiHeader.biSizeImage = (bitmap.bmWidth * clrbits + 31) / 32 * 4 * bitmap.bmHeight;//位图数据大小  
    bitmapinfo->bmiHeader.biWidth = bitmap.bmWidth;//位图宽度  
    bitmapinfo->bmiHeader.biXPelsPerMeter = bitmapinfo->bmiHeader.biYPelsPerMeter = 0;  

    fileheader.bfType = 0x4d42;  
    fileheader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) 
        + sizeof(RGBQUAD)*(bitmapinfo->bmiHeader.biClrUsed==0 ? 0 : bitmapinfo->bmiHeader.biClrUsed)  
        + bitmapinfo->bmiHeader.biSizeImage;  
    fileheader.bfReserved1 = fileheader.bfReserved2 = 0;  
    fileheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) +   
        sizeof(RGBQUAD)*(bitmapinfo->bmiHeader.biClrUsed==0 ? 0 : bitmapinfo->bmiHeader.biClrUsed);  

    buf = malloc(bitmapinfo->bmiHeader.biSizeImage);  
    GetDIBits(hdc,hbitmap,0,bitmap.bmHeight,buf,bitmapinfo,DIB_RGB_COLORS);

    fp = fopen(filepath,"wb");  
    fwrite(&fileheader,sizeof(BITMAPFILEHEADER),1,fp);  
    fwrite(bitmapinfo,sizeof(BITMAPINFO),1,fp);  
    fwrite(buf,bitmapinfo->bmiHeader.biSizeImage,1,fp);  
    fclose(fp);  
    free(buf);
    free(bitmapinfo);
    ReleaseDC(NULL,hdc);  
    return true;
}  

//将YV12转BGR24，没有效率可言（推荐使用ffmpeg）
bool YV12ToBGR24_Native(unsigned char* pYUV,unsigned char* pBGR24,int width,int height)
{
    if (width < 1 || height < 1 || pYUV == NULL || pBGR24 == NULL)
        return false;
    const long len = width * height;
    unsigned char* yData = pYUV;
    unsigned char* vData = &yData[len];
    unsigned char* uData = &vData[len >> 2];

    int bgr[3];
    int yIdx,uIdx,vIdx,idx;
    for (int i = 0;i < height;i++){
        for (int j = 0;j < width;j++){
            yIdx = i * width + j;
            vIdx = (i/2) * (width/2) + (j/2);
            uIdx = vIdx;

            bgr[0] = (int)(yData[yIdx] + 1.732446 * (uData[vIdx] - 128));                                    // b分量
            bgr[1] = (int)(yData[yIdx] - 0.698001 * (uData[uIdx] - 128) - 0.703125 * (vData[vIdx] - 128));    // g分量
            bgr[2] = (int)(yData[yIdx] + 1.370705 * (vData[uIdx] - 128));                                    // r分量

            for (int k = 0;k < 3;k++){
                idx = (i * width + j) * 3 + k;
                if(bgr[k] >= 0 && bgr[k] <= 255)
                    pBGR24[idx] = bgr[k];
                else
                    pBGR24[idx] = (bgr[k] < 0)?0:255;
            }
        }
    }
    return true;
}