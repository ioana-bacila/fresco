
#define BITS_PER_PIXEL 24
#define X_PIXESLPERMETER 0x130B
#define Y_PIXESLPERMETER 0x130B

#pragma pack(push,1)

typedef struct {
    uint8_t signature[2];
    uint32_t file_size;
    uint32_t reserved;
    uint32_t fileoffset_to_pixelarray;
} BmpFileHeader;

typedef struct {
    uint32_t dib_header_size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bits_per_pixel;
    uint32_t compression;
    uint32_t image_size;
    uint32_t y_pixelpermeter;
    uint32_t x_pixelpermeter;
    uint32_t num_colors_pallette;
    uint32_t most_imp_color;
} BmpInfoHeader;

typedef struct {
    BmpFileHeader fileheader;
    BmpInfoHeader bitmapinfoheader;
} Bitmap;

#pragma pack(pop)
