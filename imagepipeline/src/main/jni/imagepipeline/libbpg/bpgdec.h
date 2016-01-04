typedef enum {
	BMP,
	PPM
} DecodeImageFormatEnum;

int bpg_decoder_decode_buf(uint8_t* buf_enc, unsigned int enc_len,
		uint8_t** buf_dec, unsigned int* dec_len, DecodeImageFormatEnum format);

int* get_bmp_buf_size(int w, int h);

int bmp_save_to_buf(BPGDecoderContext *img, uint8_t** bmp_buf, unsigned int *buf_len);

void ppm_save_to_file(BPGDecoderContext *img, const char *filename);
int ppm_save_to_buf(BPGDecoderContext *img, const uint8_t **buf,
		unsigned int *buf_len);


