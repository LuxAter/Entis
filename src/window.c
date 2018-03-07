#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>

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
    xcb_destroy_window(entis_get_connection(), win.window);
    return win;
  }
  win.is_valid = true;
  entis_create_pixmap(&win);
  entis_create_graphics_context(&win, 0, NULL);
  if (strlen(win.title) != 0) {
    entis_change_window_name(&win);
  }
  xcb_map_window(entis_get_connection(), win.window);
  return win;
}

void entis_destroy_window(struct entis_window* win) {
  xcb_destroy_window(entis_get_connection(), win->window);
}

void entis_change_window_name(struct entis_window* win) {
  if (win->is_valid == false) {
    fprintf(stderr, "[ERROR] Window not created\n");
    return;
  }
  if (!entis_connection_valid()) {
    fprintf(stderr, "[ERROR] xcb connection not established\n");
    return;
  }
  xcb_change_property(entis_get_connection(), XCB_PROP_MODE_REPLACE,
                      win->window, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
                      strlen(win->title), win->title);
}

void entis_create_pixmap(struct entis_window* win) {
  win->pixmap = xcb_generate_id(entis_get_connection());
  xcb_get_geometry_cookie_t geom_cookie =
      xcb_get_geometry(entis_get_connection(), win->window);
  xcb_get_geometry_reply_t* geom =
      xcb_get_geometry_reply(entis_get_connection(), geom_cookie, NULL);
  xcb_void_cookie_t cookie =
      xcb_create_pixmap(entis_get_connection(), geom->depth, win->pixmap,
                        win->window, win->w, win->h);
  xcb_generic_error_t* error_check =
      xcb_request_check(entis_get_connection(), cookie);
  if (error_check != NULL) {
    fprintf(stderr, "[ERROR %u] Failed to generate xcb pixmap\n",
            error_check->error_code);
  }
  free(geom);
}

void entis_create_graphics_context(struct entis_window* win,
                                   uint32_t value_mask, void* value_list) {
  if (win->is_valid == false) {
    fprintf(stderr, "[ERROR] Window not created\n");
    return;
  }
  if (!entis_connection_valid()) {
    fprintf(stderr, "[ERROR] xcb connection not established\n");
    return;
  }
  win->graphics_context = xcb_generate_id(entis_get_connection());
  xcb_void_cookie_t cookie =
      xcb_create_gc(entis_get_connection(), win->graphics_context,
                    entis_get_screen()->root, value_mask, value_list);
  xcb_generic_error_t* error_check =
      xcb_request_check(entis_get_connection(), cookie);
  if (error_check != NULL) {
    fprintf(stderr, "[ERROR %u] Failed to generage xcb graphics context\n",
            error_check->error_code);
  }
}

void entis_set_color_int(struct entis_window* win, uint32_t color) {
  if (win->is_valid == false) {
    fprintf(stderr, "[ERROR] Window not created\n");
    return;
  }
  uint32_t values[1] = {color};
  xcb_void_cookie_t cookie = xcb_change_gc(
      entis_get_connection(), win->graphics_context, XCB_GC_FOREGROUND, values);
  xcb_generic_error_t* error_check =
      xcb_request_check(entis_get_connection(), cookie);
  if (error_check != NULL) {
    fprintf(stderr, "[ERROR %u] Failed to change xcb graphics context color\n",
            error_check->error_code);
  }
}
void entis_set_color(struct entis_window* win, struct entis_color color) {}
void entis_set_color_palette(struct entis_window* win,
                             struct entis_palette palette) {}
void entis_set_color_palette_index(struct entis_window* win, int index) {}

void entis_display_window(struct entis_window* win) {
  xcb_void_cookie_t cookie =
      xcb_copy_area(entis_get_connection(), win->pixmap, win->window,
                    win->graphics_context, 0, 0, 0, 0, win->w, win->h);
  xcb_generic_error_t* error_check =
      xcb_request_check(entis_get_connection(), cookie);
  if (error_check != NULL) {
    fprintf(stderr, "[ERROR %u] Failed to copy pixmap to window\n",
            error_check->error_code);
  }
}
