#include <stdio.h>
#include <stdlib.h>
#include <xcb/xcb.h>
#include <string.h>

#include "entis.h"
#include "window.h"

struct entis_window entis_create_window(const char* title, unsigned int x,
                                        unsigned int y, unsigned int w,
                                        unsigned int h, uint32_t value_mask,
                                        void* value_list) {
  entis_window win;
  win.title = title;
  win.is_valid = false;
  win.x = x;
  win.y = y;
  win.w = w;
  win.h = h;
  if (!entis_connection_valid()) {
    fprintf(stderr, "[ERROR] xcb connection not established\n");
    return win;
  }
  win.window = xcb_generate_id(entis_get_connection());
  xcb_void_cookie_t cookie = xcb_create_window(
      entis_get_connection(), XCB_COPY_FROM_PARENT, win.window,
      entis_get_screen()->root, x, y, w, h, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT,
      entis_get_screen()->root_visual, value_mask, value_list);
  xcb_generic_error_t* error_check =
      xcb_request_check(entis_get_connection(), cookie);
  if (error_check != NULL) {
    fprintf(stderr, "[ERROR %u] Failed to generage xcb window\n",
            error_check->error_code);
  }
  win.is_valid = true;
  xcb_map_window(entis_get_connection(), win.window);
  if (strlen(win.title) != 0){
    entis_change_window_name(&win);
  }
  return win;
}

void entis_destroy_window(struct entis_window win) {
  xcb_destroy_window(entis_get_connection(), win.window);
}

void entis_change_window_name(struct entis_window* win) {
  if (win->is_valid == false) {
    fprintf(stderr, "[ERROR] Window not created\n");
    return;
  }
  xcb_change_property(entis_get_connection(), XCB_PROP_MODE_REPLACE,
                      win->window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
                      strlen(win->title), win->title);
}
