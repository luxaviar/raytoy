#include <stdint.h>
#define STB_IMAGE_IMPLEMENTATION
#include "3rdparty/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "3rdparty/stb_image_write.h"

uint8_t* image_load(char const *filename, int *x, int *y, int *comp, int req_comp) {
    uint8_t* data = (uint8_t*)stbi_load(filename, x, y, comp, req_comp);
    return data;
}

float* image_loadf(char const *filename, int *x, int *y, int *comp, int req_comp) {
    float* data = (float*)stbi_loadf(filename, x, y, comp, req_comp);
    return data;
}

void image_free(void *data) {
    stbi_image_free(data);
}

int write_png_image(char const *filename, int x, int y, int comp, const void *data, int stride_bytes) {
    return stbi_write_png(filename, x, y, comp, data, stride_bytes);
}

void set_flip_vertically_on_load(int flag) {
    stbi_set_flip_vertically_on_load(flag);
}
