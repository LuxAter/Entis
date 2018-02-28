#ifndef ENTIS_WINDOW_H_
#define ENTIS_WINDOW_H_

#include <xcb/xcb.h>
#include <stdbool.h>

typedef struct entis_window{
  const char* title;
  xcb_window_t window;
  unsigned int x, y, w, h;
  bool is_valid;
} entis_window;

struct entis_window entis_create_window(const char* title, unsigned int x, unsigned int y, unsigned int w, unsigned int h, uint32_t value_mask, void* value_list);

void entis_destroy_window(struct entis_window win);

void entis_change_window_name(struct entis_window* win);

#endif  // ENTIS_WINDOW_H_
