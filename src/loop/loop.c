#include "loop.h"

#include "../entis.h"

uint32_t width = 500, height = 500;
uint32_t mouseX = 0, mouseY = 0;

void size(uint32_t w, uint32_t h) {
  entis_resize(w, h);
  width = entis_width();
  height = entis_height();
}

void fill(uint32_t c) { entis_color_int(c); }
void background(uint32_t c) {
  entis_background_int(c);
  entis_clear();
}

void circle(uint32_t w, uint32_t h, int32_t r) { entis_circle_fill(w, h, r); }
