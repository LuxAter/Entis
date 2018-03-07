#include <stdbool.h>
#include <xcb/xcb.h>

#include "entis.h"
#include "window.h"

static bool auto_display_ = false;

void entis_set_auto_display(bool setting) { auto_display_ = setting; }

void entis_point(struct entis_window* win, unsigned int x, unsigned int y) {
  xcb_point_t points[1] = {{x, y}};
  xcb_poly_point(entis_get_connection(), XCB_COORD_MODE_ORIGIN, win->pixmap,
                 win->graphics_context, 1, points);
}

void entis_fill_rectangle(struct entis_window* win, unsigned int x,
                          unsigned int y, unsigned int w, unsigned int h) {
  xcb_rectangle_t rects[1] = {{x, y, w, h}};
  xcb_poly_fill_rectangle(entis_get_connection(), win->pixmap,
                          win->graphics_context, 1, rects);
}
