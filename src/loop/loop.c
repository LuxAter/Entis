#include "loop.h"

#include "../entis.h"

uint32_t width = 500, height = 500;
uint32_t mouseX = 0, mouseY = 0;

double framerate = 60;
bool focused = true;

double sleep_time_ = 1.0 / 60.0;
bool loop_ = true;

void noLoop() { loop_ = false; }
void loop() { loop_ = true; }

void redraw() { draw(); }

void frameRate(double fr) {
  framerate = fr;
  sleep_time_ = 1.0 / fr;
}

void delay(uint32_t ts) {
  double s = (double)ts / 1000.0;
  entis_sleep(s);
}

void size(uint32_t w, uint32_t h) {
  entis_resize(w, h);
  width = entis_width();
  height = entis_height();
}
void fullScreen() { entis_toggle_fullscreen(); }

void fill(uint32_t c) { entis_color_int(c); }
void background(uint32_t c) {
  entis_background_int(c);
  entis_clear();
}

void circle(uint32_t w, uint32_t h, int32_t r) { entis_circle_fill(w, h, r); }
void line(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2) {
  entis_line(x1, y1, x2, y2);
}
void point(uint32_t x, uint32_t y) { entis_point(x, y); }
void rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
  entis_rectangle_fill(x, y, w, h);
}
void triangle(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t x3,
              uint32_t y3) {
  entis_triangle_fill(x1, y1, x2, y2, x3, y3);
}
