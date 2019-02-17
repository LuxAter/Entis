#include "entis.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "frame_buffer.h"

static unsigned width_ = 0, height_ = 0;
static unsigned module_flags_ = 0;

static uint32_t fg_ = 0xffffff, bg_ = 0x000000;

bool entis_init(unsigned width, unsigned height, unsigned flags) {
  width_ = width;
  height_ = height;
  module_flags_ = flags;
  bool state = true;
  if ((flags & ENTIS_FB) != 0) {
    state &= entis_fb_init(width, height);
  }
  if ((flags & ENTIS_XCB) != 0) {
  }
  if ((flags & ENTIS_TTF) != 0) {
  }
  if ((flags & ENTIS_PNG) != 0) {
  }
  return state;
}

bool entis_term() {
  bool state = true;
  if ((module_flags_ & ENTIS_FB) != 0) {
    state &= entis_fb_term();
  }
  if ((module_flags_ & ENTIS_XCB) != 0) {
  }
  if ((module_flags_ & ENTIS_TTF) != 0) {
  }
  if ((module_flags_ & ENTIS_PNG) != 0) {
  }
  width_ = 0;
  height_ = 0;
  return state;
}

void entis_set_flag(unsigned flag) { module_flags_ |= flag; }
bool entis_get_flag(unsigned flag) { return ((module_flags_ & flag) != 0); }

bool entis_resize(unsigned width, unsigned height) {
  bool state = true;
  width_ = width;
  height_ = height;
  if ((module_flags_ & ENTIS_FB) != 0) {
    state &= entis_fb_resize(width, height);
  }
  if ((module_flags_ & ENTIS_XCB) != 0) {
  }
  return state;
}
unsigned entis_width() { return width_; }
unsigned entis_height() { return height_; }
void entis_size(unsigned* width, unsigned* height) {
  *width = width_;
  *height = height_;
}

void entis_sleep(double s) {
  double secs;
  double nano = modf(s, &secs) * 1e9;
  nanosleep(&(struct timespec){(int)secs, (int)nano}, NULL);
}

void entis_color_int(uint32_t color) {
  fg_ = color;
  if ((module_flags_ & ENTIS_FB) != 0) {
    entis_fb_color(color);
  }
  if ((module_flags_ & ENTIS_XCB) != 0) {
  }
}
void entis_color_rgb(uint8_t r, uint8_t g, uint8_t b) {
  entis_color_int(((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b);
}
void entis_color_drgb(double r, double g, double b) {
  entis_color_int(((uint32_t)(r * 255) << 16) | ((uint32_t)(g * 255) << 8) |
                  (uint32_t)(b * 255));
}
void entis_background_int(uint32_t color) {
  bg_ = color;
  if ((module_flags_ & ENTIS_FB) != 0) {
    entis_fb_background(color);
  }
  if ((module_flags_ & ENTIS_XCB) != 0) {
  }
}
void entis_background_rgb(uint8_t r, uint8_t g, uint8_t b) {
  entis_background_int(((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b);
}
void entis_background_drgb(double r, double g, double b) {
  entis_background_int(((uint32_t)(r * 255) << 16) |
                       ((uint32_t)(g * 255) << 8) | (uint32_t)(b * 255));
}

bool entis_write_bmp(const char* file_name){
  if((module_flags_ & ENTIS_FB) != 0){
    return entis_fb_write_bmp(file_name);
  }
  return false;
}
