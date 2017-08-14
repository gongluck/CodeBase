#include "stdafx.h"
#include "codec_jpeg.h"
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int d_jpeg_file_decode(const char* path, d_jpeg_buffer** buff_out, COLOR_TYPE colt/*=COLT_BGR*/)
{
	if ( path == NULL || buff_out == NULL )
		return -1;

	*buff_out = NULL;

	jpeg_decompress_struct cinfo = {0};
	jpeg_error_mgr jerr;
	FILE *f = fopen(path, "rb");
	if ( f == NULL )
		return -2;	

	cinfo.err = jpeg_std_error(&jerr);

	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, f);
	int ret = jpeg_read_header(&cinfo, true);
	if ( ret < 0 )
	{
		jpeg_finish_decompress(&cinfo);
		fclose(f);
		return -3;
	}
	if ( !jpeg_start_decompress(&cinfo) )
	{
		jpeg_finish_decompress(&cinfo);
		fclose(f);
		return -4;
	}

	*buff_out = (d_jpeg_buffer*)malloc(sizeof(d_jpeg_buffer));
	if ( buff_out == NULL )
	{
		jpeg_finish_decompress(&cinfo);
		fclose(f);
		return -5;
	}

	int row_stride = cinfo.output_width * cinfo.output_components;
	int row_bytes = (row_stride+ 3) >> 2 << 2;
	if ( row_stride <= 0 )
	{
		jpeg_finish_decompress(&cinfo);
		fclose(f);
		return -6;
	}
	(*buff_out)->data = (unsigned char*)malloc(row_bytes*cinfo.output_height);
	memset((*buff_out)->data, 0, row_bytes*cinfo.output_height);
	if ( (*buff_out)->data == NULL )
	{
		free(*buff_out);
		*buff_out = NULL;
		jpeg_finish_decompress(&cinfo);
		fclose(f);
		return -7;
	}

 	JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);
 	
 
	int idx = 0;
	if (colt == COLT_BGR)
	{
		while ( cinfo.output_scanline < cinfo.output_height )
		{
			jpeg_read_scanlines(&cinfo, buffer, 1);
			for (unsigned int i=0; i<cinfo.image_width; ++i)
			{
				*((*buff_out)->data+idx+i*3) = *((unsigned char*)*buffer+i*3+2);
				*((*buff_out)->data+idx+i*3+1) = *((unsigned char*)*buffer+i*3+1);
				*((*buff_out)->data+idx+i*3+2) = *((unsigned char*)*buffer+i*3);
			}
			idx += row_bytes;
		}	
	}
	else if (colt == COLT_RGB)
	{
		while ( cinfo.output_scanline < cinfo.output_height )
		{
			jpeg_read_scanlines(&cinfo, buffer, 1);
			memcpy((*buff_out)->data+idx, *buffer, row_stride);				
			idx += row_bytes;
		}	
	}


	(*buff_out)->w = cinfo.output_width;
	(*buff_out)->h = cinfo.output_height;
	(*buff_out)->pitch = row_stride;
	(*buff_out)->len = cinfo.output_height * row_bytes;

	jpeg_finish_decompress(&cinfo);   
	jpeg_destroy_decompress(&cinfo);

	fclose(f);
	f = NULL;

	return 0;
}

int d_jpeg_mem_decode(const char* pData, int nLen, d_jpeg_buffer** buff_out)
{
	if (pData == NULL || nLen <= 0 || buff_out == NULL)
		return -1;

	*buff_out = NULL;

	jpeg_decompress_struct cinfo = {0};
	jpeg_error_mgr jerr;
	
	cinfo.err = jpeg_std_error(&jerr);

	jpeg_create_decompress(&cinfo);
	jpeg_mem_src(&cinfo, (unsigned char*)pData, nLen);
	int ret = jpeg_read_header(&cinfo, true);
	if ( ret < 0 )
	{
		jpeg_finish_decompress(&cinfo);
		return -3;
	}
	if ( !jpeg_start_decompress(&cinfo) )
	{
		jpeg_finish_decompress(&cinfo);
		return -4;
	}

	*buff_out = (d_jpeg_buffer*)malloc(sizeof(d_jpeg_buffer));
	if ( buff_out == NULL )
	{
		jpeg_finish_decompress(&cinfo);
		return -5;
	}

	int row_stride = cinfo.output_width * cinfo.output_components;
	int row_bytes = (row_stride+ 3) >> 2 << 2;
	if ( row_stride <= 0 )
	{
		jpeg_finish_decompress(&cinfo);
		return -6;
	}
	(*buff_out)->data = (unsigned char*)malloc(row_bytes*cinfo.output_height);
	memset((*buff_out)->data, 0, row_bytes*cinfo.output_height);
	if ( (*buff_out)->data == NULL )
	{
		free(*buff_out);
		*buff_out = NULL;
		jpeg_finish_decompress(&cinfo);
		return -7;
	}

	JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);


	int idx = 0;
	while ( cinfo.output_scanline < cinfo.output_height )
	{
		jpeg_read_scanlines(&cinfo, buffer, 1);
		//		memcpy((*buff_out)->data+idx, *buffer, row_stride);		
		for (unsigned int i=0; i<cinfo.image_width; ++i)
		{
			*((*buff_out)->data+idx+i*3) = *((unsigned char*)*buffer+i*3+2);
			*((*buff_out)->data+idx+i*3+1) = *((unsigned char*)*buffer+i*3+1);
			*((*buff_out)->data+idx+i*3+2) = *((unsigned char*)*buffer+i*3);
		}
		idx += row_bytes;
	}	

	(*buff_out)->w = cinfo.output_width;
	(*buff_out)->h = cinfo.output_height;
	(*buff_out)->pitch = row_stride;
	(*buff_out)->len = cinfo.output_height * row_bytes;

	jpeg_finish_decompress(&cinfo);   
	jpeg_destroy_decompress(&cinfo);

	
	return 0;
}

int d_jpeg_file_encode_rgb24(const char* path, d_jpeg_buffer* buff_in, int quality/*=50*/)
{
	if ( path == NULL || buff_in == NULL || buff_in->data == NULL )
		return -1;

	buff_in->pitch = (buff_in->w * 3 + 3)>>2<<2;
	if ( buff_in->len > buff_in->pitch * buff_in->h )
		return -2;

	jpeg_compress_struct cinfo = {0};
	jpeg_error_mgr jerr;
	FILE *f = fopen(path, "wb");
	if ( f == NULL )
		return -3;	


	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, f);

	cinfo.image_width = buff_in->w; 	/* image width and height, in pixels */
	cinfo.image_height = buff_in->h;
	cinfo.input_components = 3;		/* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB;

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);
	jpeg_start_compress(&cinfo, TRUE);

	int row_stride = buff_in->pitch;
	JSAMPROW samp_buff[1];

	unsigned char *data_rgb = (unsigned char *)malloc(buff_in->pitch*buff_in->h);
	if ( data_rgb == NULL )
	{
		jpeg_finish_compress(&cinfo);
		fclose(f);
		jpeg_destroy_compress(&cinfo);
		return -4;
	}
	memcpy(data_rgb, buff_in->data, buff_in->len);
	
	while (cinfo.next_scanline < cinfo.image_height) 
	{
		samp_buff[0] = data_rgb + cinfo.next_scanline * row_stride;
		jpeg_write_scanlines(&cinfo, samp_buff, 1);
	}

	jpeg_finish_compress(&cinfo);

	fclose(f);

	jpeg_destroy_compress(&cinfo);

	free(data_rgb);

	return 0;
}


int d_jpeg_mem_encode_bgr24(const char* path, d_jpeg_buffer* buff_in, int quality/*=50*/)
{
	if ( path == NULL || buff_in == NULL || buff_in->data == NULL )
		return -1;
	
	buff_in->pitch = (buff_in->w * 3 + 3)>>2<<2;
	if ( buff_in->len > buff_in->pitch * buff_in->h )
		return -2;
	
	jpeg_compress_struct cinfo = {0};
	jpeg_error_mgr jerr;
	FILE *f = fopen(path, "wb");
	if ( f == NULL )
		return -3;	

	
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, f);
	
	cinfo.image_width = buff_in->w; 	/* image width and height, in pixels */
	cinfo.image_height = buff_in->h;
	cinfo.input_components = 3;		/* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB;

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);
	jpeg_start_compress(&cinfo, TRUE);

	int row_stride = buff_in->pitch;
	JSAMPROW samp_buff[1];

	unsigned char *data_rgb = (unsigned char *)malloc(buff_in->pitch*buff_in->h);
	if ( data_rgb == NULL )
	{
		jpeg_finish_compress(&cinfo);
		fclose(f);
		jpeg_destroy_compress(&cinfo);
		return -4;
	}
	memcpy(data_rgb, buff_in->data, buff_in->len);
	//bgr->rgb
	unsigned char val = 0;
	for (int y=0; y<buff_in->h; ++y)
	{
		for (int x=0; x<buff_in->w; ++x)
		{
			val = *(data_rgb+y*row_stride + x * 3);
			*(data_rgb+y*row_stride + x * 3) = *(data_rgb+y*row_stride + x * 3 + 2);
			*(data_rgb+y*row_stride + x * 3 + 2) = val;
		}
	}

	while (cinfo.next_scanline < cinfo.image_height) 
	{
		samp_buff[0] = data_rgb + cinfo.next_scanline * row_stride;
		jpeg_write_scanlines(&cinfo, samp_buff, 1);
	}
	
	jpeg_finish_compress(&cinfo);

	fclose(f);

	jpeg_destroy_compress(&cinfo);

	free(data_rgb);

	return 0;
}

int d_jpeg_mem_encode_rgb24(d_jpeg_buffer* buf_in, void** buf_out, int* len_out, int quality/*=50*/)
{
	if (buf_in == NULL || buf_out == NULL || len_out == NULL)
		return -1;

	buf_in->pitch = (buf_in->w * 3 + 3)>>2<<2;
	if ( buf_in->len > buf_in->pitch * buf_in->h )
		return -2;

	jpeg_compress_struct cinfo = {0};
	jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_mem_dest(&cinfo, (unsigned char**)buf_out, (unsigned long*)len_out);

	cinfo.image_width = buf_in->w; 	/* image width and height, in pixels */
	cinfo.image_height = buf_in->h;
	cinfo.input_components = 3;		/* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB;

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);
	jpeg_start_compress(&cinfo, TRUE);

	int row_stride = buf_in->pitch;
	JSAMPROW samp_buff[1];

	unsigned char *data_rgb = (unsigned char *)malloc(buf_in->pitch*buf_in->h);
	if ( data_rgb == NULL )
	{
		jpeg_finish_compress(&cinfo);
		jpeg_destroy_compress(&cinfo);
		return -4;
	}
	memcpy(data_rgb, buf_in->data, buf_in->len);

	//bgr->rgb
	unsigned char val = 0;
	for (int y=0; y<buf_in->h; ++y)
	{
		for (int x=0; x<buf_in->w; ++x)
		{
			val = *(data_rgb+y*row_stride + x * 3);
			*(data_rgb+y*row_stride + x * 3) = *(data_rgb+y*row_stride + x * 3 + 2);
			*(data_rgb+y*row_stride + x * 3 + 2) = val;
		}
	}

	while (cinfo.next_scanline < cinfo.image_height) 
	{
		samp_buff[0] = data_rgb + cinfo.next_scanline * row_stride;
		jpeg_write_scanlines(&cinfo, samp_buff, 1);
	}

	while (cinfo.next_scanline < cinfo.image_height) 
	{
		samp_buff[0] = data_rgb + cinfo.next_scanline * row_stride;
		jpeg_write_scanlines(&cinfo, samp_buff, 1);
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	free(data_rgb);


	return 0;
}

int d_jpeg_mem_encode_brg24(d_jpeg_buffer* buf_in, void** buf_out, int* len_out, int quality/*=50*/)
{
	if (buf_in == NULL || buf_out == NULL || len_out == NULL)
		return -1;

	buf_in->pitch = (buf_in->w * 3 + 3)>>2<<2;
	if ( buf_in->len > buf_in->pitch * buf_in->h )
		return -2;

	jpeg_compress_struct cinfo = {0};
	jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	jpeg_mem_dest(&cinfo, (unsigned char**)buf_out, (unsigned long*)len_out);

	cinfo.image_width = buf_in->w; 	/* image width and height, in pixels */
	cinfo.image_height = buf_in->h;
	cinfo.input_components = 3;		/* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB;

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE);
	jpeg_start_compress(&cinfo, TRUE);

	int row_stride = buf_in->pitch;
	JSAMPROW samp_buff[1];

	unsigned char *data_rgb = (unsigned char *)malloc(buf_in->pitch*buf_in->h);
	if ( data_rgb == NULL )
	{
		jpeg_finish_compress(&cinfo);
		jpeg_destroy_compress(&cinfo);
		return -4;
	}
	memcpy(data_rgb, buf_in->data, buf_in->len);

	while (cinfo.next_scanline < cinfo.image_height) 
	{
		samp_buff[0] = data_rgb + cinfo.next_scanline * row_stride;
		jpeg_write_scanlines(&cinfo, samp_buff, 1);
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	free(data_rgb);


	return 0;
}

int d_jpeg_swap_updown(d_jpeg_buffer* buff)
{
	if (!buff)
		return -1;

	unsigned char *tmp = new unsigned char[buff->len];

	memset(tmp, 0, buff->len);

	for (int i=0; i<buff->h; ++i)
	{
		memcpy(tmp+i*buff->pitch, buff->data+(buff->h-1-i) * buff->pitch, buff->pitch);
	}
	memcpy(buff->data, tmp, buff->len);

	delete[] tmp;

	return 0;
}

void d_jpeg_free_buffer(d_jpeg_buffer** buff)
{
	if ( buff != NULL )
	{
		if ( *buff != NULL )
		{
			if ( (*buff)->data != NULL )
			{
				free((*buff)->data);
				(*buff)->data = NULL;
			}
			free(*buff);
			*buff = NULL;
		}		
	}
}

void d_jpeg_free_mem(void** p)
{
	if (p)
	{
		free(*p);
		*p = NULL;
	}
}