#include "epng.h"

#include <png.h>

#include "entis.h"

void entis_write_png_from_buffer(const char* file) {
  uint32_t** frame_buffer = entis_get_frame_buffer();
  uint32_t width = entis_width();
  uint32_t height = entis_height();
  FILE* out = fopen(file, "wb");
  if (!out) {
    return;
  }
  png_structp png =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) {
    fclose(out);
    return;
  }
  png_infop info = png_create_info_struct(png);
  if (!info) {
    fclose(out);
    return;
  }
  if (setjmp(png_jmpbuf(png))) {
    fclose(out);
    return;
  }
  uint8_t** byte_data = (png_bytepp)malloc(height * sizeof(png_bytep));
  for (uint32_t i = 0; i < height; ++i) {
    byte_data[i] = (png_bytep)malloc(6 * width * sizeof(png_byte));
  }
  for (uint32_t y = 0; y < height; ++y) {
    for (uint32_t x = 0; x < width; ++x) {
      uint32_t red = ((frame_buffer[y][x] >> 16) & 0XFF) * 256;
      uint32_t green = ((frame_buffer[y][x] >> 8) & 0XFF) * 256;
      uint32_t blue = ((frame_buffer[y][x]) & 0XFF) * 256;
      uint32_t id = x * 6;
      byte_data[y][id] = (uint8_t)(floor((double)red / 256.0));
      byte_data[y][id + 1] = (uint8_t)(red % 256);
      byte_data[y][id + 2] = (uint8_t)(floor((double)green / 256.0));
      byte_data[y][id + 3] = (uint8_t)(green % 256);
      byte_data[y][id + 4] = (uint8_t)(floor((double)blue / 256.0));
      byte_data[y][id + 5] = (uint8_t)(blue % 256);
    }
  }
  png_init_io(png, out);
  png_set_IHDR(png, info, width, height, 16, PNG_COLOR_TYPE_RGB,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);
  png_write_info(png, info);
  png_write_image(png, byte_data);
  png_write_end(png, NULL);
  png_free_data(png, info, PNG_FREE_ALL, -1);
  png_destroy_write_struct(&png, (png_infopp)NULL);
  fclose(out);
}
void entis_read_png_to_buffer(const char* file) {
  uint8_t header[8];
  FILE* load = fopen(file, "rb");
  if (!load) {
    entis_warning("File \"%s\" not found", file);
    return;
  }
  fread(header, 1, 8, load);
  if (png_sig_cmp(header, 0, 8)) {
    entis_warning("\"%s\" is not a valid PNG file", file);
    fclose(load);
    return;
  }
  png_structp png =
      png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) {
    fclose(load);
    return;
  }
  png_infop info = png_create_info_struct(png);
  if (!info) {
    fclose(load);
    return;
  }
  if (setjmp(png_jmpbuf(png))) {
    fclose(load);
    return;
  }
  png_init_io(png, load);
  png_set_sig_bytes(png, 8);
  png_read_info(png, info);

  uint32_t width = png_get_image_width(png, info);
  uint32_t height = png_get_image_height(png, info);
  entis_resize(width, height);
  png_byte color = png_get_color_type(png, info);
  png_byte depth = png_get_bit_depth(png, info);

  png_read_update_info(png, info);

  if (setjmp(png_jmpbuf(png))) {
    entis_warning("Error during PNG reading");
    fclose(load);
    return;
  }

  uint8_t** byte_data = (png_bytepp)malloc(height * sizeof(png_bytep));
  for (uint32_t i = 0; i < height; ++i) {
    byte_data[i] = (png_bytep)malloc(png_get_rowbytes(png, info));
  }
  png_read_image(png, byte_data);
  uint8_t step = 3;
  switch (color) {
    case 0:
      step = 1;
      break;
    case 2:
      step = 3;
      break;
    case 4:
      step = 2;
      break;
    case 6:
      step = 4;
      break;
    default:
      step = 3;
      break;
  }
  if (depth == 16) {
    step *= 2;
  }
  uint32_t** frame_buffer = entis_get_frame_buffer();
  for (uint32_t y = 0; y < height; ++y) {
    for (uint32_t x = 0; x < width; ++x) {
      if (depth == 8) {
        frame_buffer[y][x] = ((uint32_t)byte_data[y][step * x] << 16) |
                             ((uint32_t)byte_data[y][step * x + 1] << 8) |
                             (uint32_t)byte_data[y][step * x + 2];
      } else if (depth == 16) {
        frame_buffer[y][x] = (((uint32_t)byte_data[y][step * x + 1] << 8 |
                               (uint32_t)byte_data[y][step * x])
                              << 16) |
                             (((uint32_t)byte_data[y][step * x + 3] << 8 |
                               (uint32_t)byte_data[y][step * x + 2])
                              << 8) |
                             ((uint32_t)byte_data[y][step * x + 5] << 8 |
                              (uint32_t)byte_data[y][step * x + 4]);
      }
    }
  }
  fclose(load);
}
