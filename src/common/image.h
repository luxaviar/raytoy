#pragma once

void set_flip_vertically_on_load(int flag);

uint8_t* image_load(char const *filename, int *x, int *y, int *comp, int req_comp);
float* image_loadf(char const *filename, int *x, int *y, int *comp, int req_comp);

void image_free(void *data);

int write_png_image(char const *filename, int x, int y, int comp, const void *data, int stride_bytes);
