#ifndef __CODEC_JPEG_H__
#define __CODEC_JPEG_H__
//////////////////////////////////////////////////////////////////////////
#include "jpeglib.h"
#include <setjmp.h>
#pragma comment(lib, "libjpeg.lib")
//////////////////////////////////////////////////////////////////////////
typedef struct d_jpeg_buffer
{
	unsigned char *data;
	int len;
	int w;
	int h;
	int pitch;
}d_jpeg_buffer;

typedef enum COLOR_TYPE
{
	COLT_BGR,
	COLT_RGB
}COLOR_TYPE;
//////////////////////////////////////////////////////////////////////////
int d_jpeg_file_decode(const char* path, d_jpeg_buffer** buff_out, COLOR_TYPE colt=COLT_BGR);	//must call jpeg_free_buffer to free memory by caller
int d_jpeg_mem_decode(const char* pData, int nLen, d_jpeg_buffer** buff_out);	//must call jpeg_free_buffer to free memory by caller
int d_jpeg_file_encode_rgb24(const char* path, d_jpeg_buffer* buff_in, int quality=50);	//encode rgb data to jpeg file
int d_jpeg_file_encode_bgr24(const char* path, d_jpeg_buffer* buff_in, int quality=50);	//encode bgr data to jpeg file
int d_jpeg_mem_encode_rgb24(d_jpeg_buffer* buf_in, void** buf_out, int* len_out, int quality=50);
int d_jpeg_mem_encode_bgr24(d_jpeg_buffer* buf_in, void** buf_out, int* len_out, int quality=50);
int d_jpeg_swap_updown(d_jpeg_buffer* buff);		//ÉÏÏÂ½»»»

void d_jpeg_free_buffer(d_jpeg_buffer** buff);
void d_jpeg_free_mem(void** p);
/////////////////////////////////////////;/////////////////////////////////
#endif