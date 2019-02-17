#include "bmp.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool entis_bmp_write(const char* file_name, unsigned width, unsigned height,
                     uint32_t** buffer) {
  unsigned file_size = 54 + 3 * width * height;

  uint8_t* byte_data = (uint8_t*)malloc(3 * width * height);
  if (byte_data == NULL) {
    return false;
  }
  memset(byte_data, 0, 3 * width * height);
  for (unsigned x = 0; x < width; ++x) {
    for (unsigned y = 0; y < height; ++y) {
      unsigned x_ind = x, y_ind = (height - 1) - y;
      uint8_t r = ((buffer[y][x] >> 16) & 0XFF);
      uint8_t g = ((buffer[y][x] >> 8) & 0XFF);
      uint8_t b = ((buffer[y][x]) & 0XFF);
      byte_data[(x_ind + y_ind * width) * 3 + 0] = b;
      byte_data[(x_ind + y_ind * width) * 3 + 1] = g;
      byte_data[(x_ind + y_ind * width) * 3 + 2] = r;
    }
  }

  unsigned char file_header[14] = {'B', 'M', 0, 0,  0, 0, 0,
                                   0,   0,   0, 54, 0, 0, 0};
  unsigned char info_header[40] = {40, 0, 0, 0, 0, 0, 0,  0,
                                   0,  0, 0, 0, 1, 0, 24, 0};
  unsigned char pad[3] = {0, 0, 0};

  file_header[2] = (unsigned char)(file_size);
  file_header[3] = (unsigned char)(file_size >> 8);
  file_header[4] = (unsigned char)(file_size >> 14);
  file_header[5] = (unsigned char)(file_size >> 24);

  info_header[4] = (unsigned char)(width);
  info_header[5] = (unsigned char)(width >> 8);
  info_header[6] = (unsigned char)(width >> 16);
  info_header[7] = (unsigned char)(width >> 24);
  info_header[8] = (unsigned char)(height);
  info_header[9] = (unsigned char)(height >> 8);
  info_header[10] = (unsigned char)(height >> 16);
  info_header[11] = (unsigned char)(height >> 24);

  FILE* out = fopen(file_name, "wb");
  if (out == NULL) {
    free(byte_data);
    return false;
  }
  fwrite(file_header, 1, 14, out);
  fwrite(info_header, 1, 40, out);
  for (unsigned i = 0; i < height; ++i) {
    fwrite(byte_data + (width * (height - i - 1) * 3), 3, width, out);
    fwrite(pad, 1, (4 - (width * 3) % 4) % 4, out);
  }
  free(byte_data);
  fclose(out);
  return true;
}
