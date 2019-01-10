#ifndef ENTIS_FPT_H_
#define ENTIS_FPT_H_

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "entis.h"

static inline int G_init_graphics(int w, int h) {
  return entis_init(w, h, ENTIS_TTF | ENTIS_XCB);
}

static bool (*G_close)() = entis_term;
static void (*G_clear)() = entis_clear;

static void (*G_color)(uint32_t) = entis_color_int;
static void (*G_rgb)(double, double, double) = entis_color_drgb;
static void (*Gi_rgb)(uint8_t, uint8_t, uint8_t) = entis_color_rgb;

static void (*G_pixel)(uint32_t, uint32_t) = entis_point;
static void (*G_point)(uint32_t, uint32_t) = entis_point;

static void (*G_line)(uint32_t, uint32_t, uint32_t, uint32_t) = entis_line;
static void (*G_triangle)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
                          uint32_t) = entis_triangle;
static void (*G_rectangle)(uint32_t, uint32_t, uint32_t,
                           uint32_t) = entis_rectangle;
static void (*G_circle)(uint32_t, uint32_t, uint32_t) = entis_circle;
static void (*G_polygon)(uint32_t*, uint32_t*, uint32_t) = entis_poly;

static void (*G_fill_triangle)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
                               uint32_t) = entis_triangle_fill;
static void (*G_fill_rectangle)(uint32_t, uint32_t, uint32_t,
                                uint32_t) = entis_rectangle_fill;
static void (*G_fill_circle)(uint32_t, uint32_t, uint32_t) = entis_circle_fill;
static void (*G_fill_polygon)(uint32_t*, uint32_t*, uint32_t) = entis_poly_fill;

static uint32_t (*G_font_pixel_height)() = entis_glyph_height;
static uint32_t (*G_strign_pixel_width)(const char*) = entis_text_width;
static void (*G_draw_string)(uint32_t x, uint32_t y,
                             const char* str) = entis_text;

static void (*G_save_image_to_file)(const char* filename) = entis_write_png;

static inline void Gi_wait_click(int p[2]) {
  entis_event ev = entis_wait_event_type(ENTIS_BUTTON_PRESS);
  p[0] = ev.button.x;
  p[1] = ev.button.y;
}
static inline void G_wait_click(double p[2]) {
  entis_event ev = entis_wait_event_type(ENTIS_BUTTON_PRESS);
  p[0] = (double)ev.button.x;
  p[1] = (double)ev.button.y;
}
static inline int G_wait_key() {
  entis_event ev = entis_wait_event_type(ENTIS_KEY_PRESS);
  return (int)ev.key.keysym;
}

#endif  // ENTIS_FPT_H_
