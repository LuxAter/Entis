#include "frame_buffer.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "bmp.h"

static unsigned width_ = 0, height_ = 0;
static uint32_t fg_ = 0xffffff, bg_ = 0x000000;
static uint32_t** frame_buffer_ = NULL;

bool entis_fb_init(unsigned width, unsigned height) {
  width_ = width;
  height_ = height;
  frame_buffer_ = (uint32_t**)malloc(height_ * sizeof(uint32_t*));
  if (frame_buffer_ == NULL) return false;
  for (uint32_t i = 0; i < height_; ++i) {
    frame_buffer_[i] = (uint32_t*)malloc(width_ * sizeof(uint32_t));
    if (frame_buffer_[i] == NULL) {
      for (uint32_t j = 0; j < i; ++j) {
        free(frame_buffer_[i]);
      }
      free(frame_buffer_);
      return false;
    }
    for (uint32_t j = 0; j < width_; ++j) {
      frame_buffer_[i][j] = bg_;
    }
  }
  return true;
}
bool entis_fb_term() {
  for (uint32_t i = 0; i < height_; ++i) {
    free(frame_buffer_[i]);
  }
  free(frame_buffer_);
  return true;
}

bool entis_fb_resize(unsigned width, unsigned height) {
  for (uint32_t i = 0; i < height_; ++i) {
    free(frame_buffer_[i]);
  }
  free(frame_buffer_);
  width_ = width;
  height_ = height;
  frame_buffer_ = (uint32_t**)malloc(height_ * sizeof(uint32_t*));
  if (frame_buffer_ == NULL) return false;
  for (uint32_t i = 0; i < height_; ++i) {
    frame_buffer_[i] = (uint32_t*)malloc(width_ * sizeof(uint32_t));
    if (frame_buffer_[i] == NULL) {
      for (uint32_t j = 0; j < i; ++j) {
        free(frame_buffer_[i]);
      }
      free(frame_buffer_);
      return false;
    }
    for (uint32_t j = 0; j < width_; ++j) {
      frame_buffer_[i][j] = bg_;
    }
  }
  return true;
}
unsigned entis_fb_width() { return width_; }
unsigned entis_fb_height() { return height_; }
void entis_fb_size(unsigned* width, unsigned* height) {
  *width = width_;
  *height = height_;
}

void entis_fb_color(uint32_t color) { fg_ = color; }
void entis_fb_background(uint32_t color) { bg_ = color; }

void entis_fb_pixel(unsigned x, unsigned y) {
  if (x < width_ && y < height_) {
    frame_buffer_[y][x] = fg_;
  }
}
void entis_fb_ipixel(int x, int y) {
  if (x >= 0 && x < width_ && y >= 0 && y < height_) {
    frame_buffer_[y][x] = fg_;
  }
}
void entis_fb_dpixel(double x, double y) {
  entis_fb_ipixel((int)(x), (int)(y));
}

/* TODO: Implement safty measures earlier, so that the point dosen't need to be
 * checked every time. <16-02-19, Arden Rasmussen> */
void entis_fb_line_low(unsigned x0, unsigned y0, unsigned x1, unsigned y1) {
  int dx = x1 - x0, dy = y1 - y0, yi = 1;
  if (dy < 0) {
    yi = -1;
    dy = -dy;
  }
  int d = 2 * dy - dx;
  int y = y0;
  for (int x = x0; x <= x1; ++x) {
    if (x < width_ && y < height_) {
      frame_buffer_[y][x] = fg_;
    }
    if (d > 0) {
      y += yi;
      d -= (2 * dx);
    }
    d += (2 * dy);
  }
}
void entis_fb_line_high(unsigned x0, unsigned y0, unsigned x1, unsigned y1) {
  int dx = x1 - x0, dy = y1 - y0, xi = 1;
  if (dx < 0) {
    xi = -1;
    dx = -dx;
  }
  int d = 2 * dx - dy;
  int x = x0;
  for (int y = y0; y <= y1; ++y) {
    if (x < width_ && y < height_) {
      frame_buffer_[y][x] = fg_;
    }
    if (d > 0) {
      x += xi;
      d -= (2 * dy);
    }
    d += (2 * dx);
  }
}
void entis_fb_line(unsigned x0, unsigned y0, unsigned x1, unsigned y1) {
  if (labs((int64_t)y1 - (int64_t)y0) < labs((int64_t)x1 - (int64_t)x0)) {
    if (x0 > x1) {
      entis_fb_line_low(x1, y1, x0, y0);
    } else {
      entis_fb_line_low(x0, y0, x1, y1);
    }
  } else {
    if (y0 > y1) {
      entis_fb_line_high(x1, y1, x0, y0);
    } else {
      entis_fb_line_high(x0, y0, x1, y1);
    }
  }
}
void entis_fb_iline(int x0, int y0, int x1, int y1) {
  if ((x0 < 0 && x1 < 0) || (y0 < 0 && y1 < 0) ||
      (x0 >= width_ && x1 >= width_) || (y0 >= height_ && y1 >= height_)) {
    return;
  } else if (x0 < 0 || x1 < 0 || y0 < 0 || y1 < 0) {
    if (x0 < 0) {
      x0 = (double)(y1 - y0) / (x1 - x0) * -x0 + y0;
    } else {
      x1 = (double)(y1 - y0) / (x1 - x0) * -x0 + y0;
    }
    if (y0 < 0) {
      y0 = (double)(x1 - x0) / (y1 - y0) * -y0 + x0;
    } else {
      y1 = (double)(x1 - x0) / (y1 - y0) * -y0 + x0;
    }
  }
  entis_fb_line(x0, y0, x1, y1);
}
void entis_fb_dline(double x0, double y0, double x1, double y1) {
  entis_fb_iline((int)x0, (int)y0, (int)x1, (int)y1);
}

void entis_fb_tri(unsigned x0, unsigned y0, unsigned x1, unsigned y1,
                  unsigned x2, unsigned y2) {
  entis_fb_line(x0, y0, x1, y1);
  entis_fb_line(x1, y1, x2, y2);
  entis_fb_line(x2, y2, x0, y0);
}
void entis_fb_itri(int x0, int y0, int x1, int y1, int x2, int y2) {
  entis_fb_iline(x0, y0, x1, y1);
  entis_fb_iline(x1, y1, x2, y2);
  entis_fb_iline(x2, y2, x0, y0);
}
void entis_fb_dtri(double x0, double y0, double x1, double y1, double x2,
                   double y2) {
  entis_fb_dline(x0, y0, x1, y1);
  entis_fb_dline(x1, y1, x2, y2);
  entis_fb_dline(x2, y2, x0, y0);
}

void entis_fb_rect(unsigned x, unsigned y, unsigned w, unsigned h) {
  entis_fb_line(x, y, x + w, y);
  entis_fb_line(x, y, x, y + h);
  entis_fb_line(x + w, y, x + w, y + h);
  entis_fb_line(x, y + h, x + w, y + y);
}
void entis_fb_irect(int x, int y, int w, int h) {
  entis_fb_iline(x, y, x + w, y);
  entis_fb_iline(x, y, x, y + h);
  entis_fb_iline(x + w, y, x + w, y + h);
  entis_fb_iline(x, y + h, x + w, y + y);
}
void entis_fb_drect(double x, double y, double w, double h) {
  entis_fb_dline(x, y, x + w, y);
  entis_fb_dline(x, y, x, y + h);
  entis_fb_dline(x + w, y, x + w, y + h);
  entis_fb_dline(x, y + h, x + w, y + y);
}

void entis_fb_polygon(unsigned cx, unsigned cy, unsigned sides) {}
void entis_fb_ipolygon(int cx, int cy, int sides) {}
void entis_fb_dpolygon(double cx, double cy, double sides) {}

void entis_fb_circle(unsigned cx, unsigned cy, unsigned r) {
  unsigned x = r - 1;
  unsigned y = 0;
  int err = (x * x) - (r * r) + r;
  while (x >= y) {
    if (cx + x < width_ && cy + y < height_)
      frame_buffer_[cy + y][cx + x] = fg_;
    if (cx - x < width_ && cy + y < height_)
      frame_buffer_[cy + y][cx - x] = fg_;
    if (cx + y < width_ && cy + x < height_)
      frame_buffer_[cy + x][cx + y] = fg_;
    if (cx - y < width_ && cy + x < height_)
      frame_buffer_[cy + x][cx - y] = fg_;
    if (cx - y < width_ && cy - x < height_)
      frame_buffer_[cy - x][cx - y] = fg_;
    if (cx + y < width_ && cy - x < height_)
      frame_buffer_[cy - x][cx + y] = fg_;
    if (cx - x < width_ && cy - y < height_)
      frame_buffer_[cy - y][cx - x] = fg_;
    if (cx + x < width_ && cy - y < height_)
      frame_buffer_[cy - y][cx + x] = fg_;
    if (err <= 0) {
      y++;
      err += 2 * y + 1;
    } else {
      x--;
      err -= x * 2 + 1;
    }
  }
}
void entis_fb_icircle(int cx, int cy, int r) {
  if (!(cx < 0 || cy < 0 || cx >= width_ || cy >= height_)) {
    entis_fb_circle((unsigned)cx, (unsigned)cy, (unsigned)(r > 0 ? r : -r));
  }
}
void entis_fb_dcircle(double cx, double cy, double r) {
  if (!(cx < 0 || cy < 0 || cx >= width_ || cy >= height_)) {
    entis_fb_circle((unsigned)cx, (unsigned)cy, (unsigned)(r > 0 ? r : -r));
  }
}

void entis_fb_poly(unsigned* x, unsigned* y, unsigned n) {
  for (unsigned i = 1; i < n; ++i) {
    entis_fb_line(x[i - 1], y[i - 1], x[i], y[i]);
  }
  entis_fb_line(x[0], y[0], x[n - 1], y[n - 1]);
}
void entis_fb_ipoly(int* x, int* y, unsigned n) {}
void entis_fb_dpoly(double* x, double* y, unsigned n) {}

bool entis_fb_write_bmp(const char* file_name) {
  return entis_bmp_write(file_name, width_, height_, frame_buffer_);
}
