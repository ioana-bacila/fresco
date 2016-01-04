/*
 * BPG decoder command line utility
 *
 * Copyright (c) 2014 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <getopt.h>
#include <inttypes.h>

/* define it to include PNG output
 #define USE_PNG

 #ifdef USE_PNG
 #include <png.h>
 #endif
 */

#include "libbpg.h"
#include "bpgdec.h"
#include "libavutil/mem.h"
#include "bmputil/bmp.h"

void ppm_save_to_file(BPGDecoderContext *img, const char *filename) {
	BPGImageInfo img_info_s, *img_info = &img_info_s;
	FILE *f;
	int w, h, y;
	uint8_t *rgb_line;

	bpg_decoder_get_info(img, img_info);

	w = img_info->width;
	h = img_info->height;

	rgb_line = malloc(3 * w);

	f = fopen(filename, "wb");
	if (!f) {
		fprintf(stderr, "%s: I/O error\n", filename);
		exit(1);
	}

	fprintf(f, "P6\n%d %d\n%d\n", w, h, 255);

	bpg_decoder_start(img, BPG_OUTPUT_FORMAT_RGB24);
	for (y = 0; y < h; y++) {
		bpg_decoder_get_line(img, rgb_line);
		fwrite(rgb_line, 1, w * 3, f);
	}
	fclose(f);

	free(rgb_line);
}

int ppm_save_to_buf(BPGDecoderContext *img, const uint8_t **buf,
		unsigned int *buf_len) {
	BPGImageInfo img_info_s, *img_info = &img_info_s;
	int w, h, y;
	int header_alloc = 255, header_len, size_of_line;
	uint8_t *rgb_line;
	char header[255];

	memset(header, 0, header_alloc);
	bpg_decoder_get_info(img, img_info);

	w = img_info->width;
	h = img_info->height;
	size_of_line = 3 * w;
	rgb_line = malloc(size_of_line);

	snprintf(header, header_alloc, "P6\n%d %d\n%d\n", w, h, header_alloc);
	*buf_len = size_of_line * h + strlen(header);
	*buf = (uint8_t*) malloc(*buf_len);

	if (*buf == NULL) {
		printf("Could not allocate output buffer");
		return -1;
	}

	header_len = strlen(header);
	memcpy(*buf, header, header_len);

	int idx = 0;
	bpg_decoder_start(img, BPG_OUTPUT_FORMAT_RGB24);
	for (y = 0; y < h; y++) {
		bpg_decoder_get_line(img, rgb_line);
		memcpy((*buf) + size_of_line + idx, rgb_line, size_of_line);
		idx += size_of_line;
	}

	free(rgb_line);

	return 0;
}

int bmp_save_to_buf(BPGDecoderContext *img, uint8_t** bmp_buf, unsigned int *buf_len) {
	BPGImageInfo img_info_s, *img_info = &img_info_s;
	int w, h, y, size_of_line, x;
	uint8_t *rgb_line/*, *bmp_line*/;
	uint8_t swap;

	int bmp_header_len = sizeof(Bitmap);
	Bitmap *bmp = (Bitmap*) calloc(1, bmp_header_len);
	bmp->fileheader.signature[0] = 'B';
	bmp->fileheader.signature[1] = 'M';
	bmp->fileheader.fileoffset_to_pixelarray = bmp_header_len;

	if (bpg_decoder_get_info(img, img_info) < 0) {
		return -1;
	}

	w = img_info->width;
	h = img_info->height;

	bmp->bitmapinfoheader.dib_header_size = sizeof(BmpInfoHeader);
	bmp->bitmapinfoheader.width = w;
	bmp->bitmapinfoheader.height = h;
	bmp->bitmapinfoheader.planes = 1;
	bmp->bitmapinfoheader.bits_per_pixel = BITS_PER_PIXEL;
	bmp->bitmapinfoheader.compression = 0;
	bmp->bitmapinfoheader.image_size = w * h * (24 / 8);
	bmp->bitmapinfoheader.x_pixelpermeter = X_PIXESLPERMETER;
	bmp->bitmapinfoheader.y_pixelpermeter = Y_PIXESLPERMETER;
	bmp->bitmapinfoheader.num_colors_pallette = 0;

	int padding = 0;
	int scanlinebytes = w * 3;
	while ((scanlinebytes + padding) % sizeof(uint32_t) != 0) {
		padding++;
	}

	// get the padded scanline width
	size_of_line = scanlinebytes + padding;
	rgb_line = malloc(size_of_line);
	if (rgb_line == NULL) {
		printf("Could not allocate the RGB line buffer! \n");
		return -1;
	}

	bmp->fileheader.file_size = size_of_line * h + bmp_header_len;

	*buf_len = bmp->fileheader.file_size;
	*bmp_buf = malloc(*buf_len);

	if (*bmp_buf == NULL) {
		printf("Could not allocate the RGB line buffer! \n");
		free(rgb_line);
		return -1;
	}

	memset(*bmp_buf, 0, *buf_len);
	memcpy(*bmp_buf, bmp, bmp_header_len);

	bpg_decoder_start(img, BPG_OUTPUT_FORMAT_RGB24);
	int idx = 0, buf_idx;
	for (y = 0; y < h; y++) {
		bpg_decoder_get_line(img, rgb_line);

		// RGB needs to be BGR
		for (x = 0; x < size_of_line; x += 3) {
			swap = rgb_line[x + 2];
			rgb_line[x + 2] = rgb_line[x]; // swap r and b
			rgb_line[x] = swap; // swap b and r
		}

		idx += size_of_line;
		buf_idx = (*buf_len - idx);
		memcpy((*bmp_buf) + buf_idx, rgb_line, size_of_line);
	}
//	FILE *f = fopen("out.bmp", "ab+");
//	fwrite (*bmp_buf, 1, *buf_len, f);
//	fclose(f);
	free(rgb_line);
}

int* get_bmp_buf_size(int w, int h) {
	// find the number of padding bytes
	int line_len, padding = 0;
	int line_bytes_len = w * 3;
	while ((line_bytes_len + padding) % sizeof(uint32_t) != 0) {
		padding++;
	}

	line_len = line_bytes_len + padding;

	int *buf_len = line_len * h + sizeof(Bitmap);

	return buf_len;
}

int bpg_decoder_decode_buf(uint8_t* buf_enc, unsigned int enc_len,
		uint8_t** buf_dec, unsigned int* dec_len, DecodeImageFormatEnum format) {
	BPGDecoderContext *img = bpg_decoder_open();

	if (bpg_decoder_decode(img, buf_enc, buf_dec) < 0) {
		fprintf(stderr, "Could not decode image\n");
		return -1;
	}

	switch (format) {
	case BMP: {
		return bmp_save_to_buf(img, buf_dec, dec_len);
	}
	case PPM: {
		return ppm_save_to_buf(img, buf_dec, dec_len);
	}
	default: {
		return 1; // not a valid format
	}
	}

	bpg_decoder_close(img);

}

#if 0
int main(int argc, char **argv) {
	FILE *f, *of;
	BPGDecoderContext *img;
	uint8_t *header, *buf_enc, *buf_dec;
	int buf_len, bit_depth, c, show_info;
	const char *outfilename, *filename;

	filename = "./img/lena_q23.bpg";

	f = fopen(filename, "rb");
	if (!f) {
		fprintf(stderr, "Could not open %s\n", filename);
		exit(1);
	}

	fseek(f, 0, SEEK_END);
	buf_len = ftell(f);
	fseek(f, 0, SEEK_SET);

	buf_enc = malloc(buf_len);
	if (fread(buf_enc , 1, buf_len, f) != buf_len) {
		fprintf(stderr, "Error while reading file\n");
		exit(1);
	}

	fclose(f);

	header = malloc(255);
	memcpy(header, buf_enc, 255);

	BPGImageInfo *info = av_mallocz(sizeof(BPGImageInfo));
	BPGExtensionData **ext = av_mallocz(sizeof(BPGExtensionData));
	int idx = bpg_decoder_get_info_from_buf(info, NULL, header, buf_len);

	img = bpg_decoder_open();
	copy_context_from_info(img, info, NULL);
	int ret = bpg_decoder_decode_body(img, buf_enc, buf_len - idx, idx);

	int *dec_len = get_bmp_buf_size(512, 512);
	bmp_save_to_buf(img, &buf_dec, &dec_len);

	free(header);
	return 0;
}
#endif

