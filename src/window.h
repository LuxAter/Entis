#ifndef ENTIS_WINDOW_H_
#define ENTIS_WINDOW_H_

#include <xcb/xcb.h>
#include <stdbool.h>

#include "color.h"

typedef struct entis_window{
  bool is_valid;
  unsigned int x, y, w, h;
  const char* title;
  struct entis_palette* palette;
  xcb_window_t window;
  xcb_pixmap_t pixmap;
  xcb_gcontext_t graphics_context;
} entis_window;

struct entis_window entis_create_window(const char* title, unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint32_t value_mask, void* value_list);

void entis_destroy_window(struct entis_window* win);

void entis_change_window_name(struct entis_window* win);

void entis_create_pixmap(struct entis_window* win);
void entis_create_graphics_context(struct entis_window* win, uint32_t value_mask, void* value_list);

void entis_set_color_int(struct entis_window* win, uint32_t color);
void entis_set_color(struct entis_window* win, struct entis_color color);
void entis_set_color_palette(struct entis_window* win, struct entis_palette palette);
void entis_set_color_palette_index(struct entis_window* win, int index);

void entis_display_window(struct entis_window* win);

#endif  // ENTIS_WINDOW_H_
