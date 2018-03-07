#ifndef ENTIS_PRIMITIVES_
#define ENTIS_PRIMITIVES_

#include <stdbool.h>
#include <xcb/xcb.h>

#include "window.h"

void entis_set_auto_display(bool setting);

void entis_point(struct entis_window* win, unsigned int x, unsigned int y);
void entis_fill_rectangle(struct entis_window* win, unsigned int x,
                          unsigned int y, unsigned int w, unsigned int h);

#endif  // ENTIS_PRIMITIVES_
