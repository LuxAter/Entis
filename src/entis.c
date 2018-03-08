#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>

#include "entis.h"
#include "event.h"

static xcb_connection_t* connection_;
static xcb_screen_t* screen_;

static xcb_window_t window_;
static xcb_pixmap_t pixmap_;
static xcb_gcontext_t gcontext_, pixmap_gcontext_, pixmap_bg_gcontext_;

static uint16_t width_, height_;

void entis_init(const char* title, unsigned int w, unsigned int h,
                uint32_t value_mask, void* value_list) {
  if (connection_ != NULL) {
    fprintf(stderr, "Connection to X server already established!\n");
  }
  connection_ = xcb_connect(NULL, NULL);
  if (connection_ == NULL) {
    fprintf(stderr, "Failed to connect to X server\n");
    exit(1);
  } else if (xcb_connection_has_error(connection_)) {
    fprintf(stderr, "Error while attempting to connect to X server\n");
    xcb_disconnect(connection_);
    exit(1);
  }
  screen_ = xcb_setup_roots_iterator(xcb_get_setup(connection_)).data;
  window_ = xcb_generate_id(connection_);
  xcb_void_cookie_t cookie;
  if (value_mask != 0) {
    cookie = xcb_create_window(connection_, XCB_COPY_FROM_PARENT, window_,
                               screen_->root, 0, 0, w, h, 0,
                               XCB_WINDOW_CLASS_INPUT_OUTPUT,
                               screen_->root_visual, value_mask, value_list);
  } else {
    uint32_t internal_value[1] = {
        XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS |
        XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION |
        XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW |
        XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
        XCB_EVENT_MASK_STRUCTURE_NOTIFY};
    cookie = xcb_create_window(
        connection_, XCB_COPY_FROM_PARENT, window_, screen_->root, 0, 0, w, h,
        0, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen_->root_visual,
        XCB_CW_EVENT_MASK, internal_value);
  }
  xcb_generic_error_t* error_check = xcb_request_check(connection_, cookie);
  if (error_check != NULL) {
    fprintf(stderr, "[ERROR %u] Failed to generage xcb window\n",
            error_check->error_code);
    xcb_destroy_window(connection_, window_);
    exit(1);
  }
  free(error_check);
  pixmap_ = xcb_generate_id(connection_);
  cookie = xcb_create_pixmap(connection_, screen_->root_depth, pixmap_,
                             screen_->root, w, h);
  error_check = xcb_request_check(connection_, cookie);
  if (error_check != NULL) {
    fprintf(stderr, "[ERROR %u] Failed to generate xcb pixmap\n",
            error_check->error_code);
  }
  free(error_check);
  pixmap_gcontext_ = xcb_generate_id(connection_);
  cookie = xcb_create_gc(connection_, pixmap_gcontext_, screen_->root, 0, NULL);
  error_check = xcb_request_check(connection_, cookie);
  if (error_check != NULL) {
    fprintf(stderr,
            "[ERROR %u] Failed to generage xcb pixmap graphics context\n",
            error_check->error_code);
  }
  free(error_check);
  pixmap_bg_gcontext_ = xcb_generate_id(connection_);
  cookie = xcb_create_gc(connection_, pixmap_bg_gcontext_, screen_->root,
                         XCB_GC_FOREGROUND, (uint32_t[]){0x000000});
  error_check = xcb_request_check(connection_, cookie);
  if (error_check != NULL) {
    fprintf(stderr,
            "[ERROR %u] Failed to generage xcb pixmap graphics context\n",
            error_check->error_code);
  }
  free(error_check);
  xcb_poly_fill_rectangle(connection_, pixmap_, pixmap_bg_gcontext_, 1,
                          (xcb_rectangle_t[]){{0, 0, w, h}});
  gcontext_ = xcb_generate_id(connection_);
  cookie = xcb_create_gc(connection_, gcontext_, screen_->root, 0, NULL);
  error_check = xcb_request_check(connection_, cookie);
  if (error_check != NULL) {
    fprintf(stderr, "[ERROR %u] Failed to generage xcb graphics context\n",
            error_check->error_code);
  }
  free(error_check);
  if (strlen(title) != 0) {
    xcb_change_property(connection_, XCB_PROP_MODE_REPLACE, window_,
                        XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(title),
                        title);
  }
  width_ = w;
  height_ = h;
  xcb_map_window(connection_, window_);
}

void entis_term() {
  if (connection_ == NULL) {
    fprintf(stderr, "No connection to X server\n");
  }
  xcb_destroy_window(connection_, window_);
  xcb_disconnect(connection_);
}

bool entis_connection_valid() {
  if (connection_ == NULL) {
    return false;
  }
  return true;
}
void entis_flush() { xcb_flush(connection_); }

xcb_connection_t* entis_get_connection() { return connection_; }
xcb_screen_t* entis_get_screen() { return screen_; }
xcb_window_t entis_get_window() { return window_; }
xcb_pixmap_t entis_get_pixmap() { return pixmap_; }
xcb_gcontext_t entis_get_gcontext() { return gcontext_; }

void entis_set_color(uint32_t color) {
  uint32_t values[1] = {color};
  xcb_void_cookie_t cookie =
      xcb_change_gc(connection_, pixmap_gcontext_, XCB_GC_FOREGROUND, values);
  xcb_generic_error_t* error_check = xcb_request_check(connection_, cookie);
  if (error_check != NULL) {
    fprintf(stderr, "[ERROR %u] Failed to change xcb graphics context color\n",
            error_check->error_code);
  }
}
void entis_set_color_rgb(uint32_t r, uint32_t g, uint32_t b) {
  entis_set_color(0x010000 * r + 0x000100 * g + b);
}
void entis_set_color_drgb(double r, double g, double b) {
  entis_set_color(0x010000 * (uint32_t)(255 * r) +
                  0x000100 * (uint32_t)(256 * g) + (uint32_t)(256 * b));
}

void entis_set_background(uint32_t color) {
  uint32_t values[1] = {color};
  xcb_void_cookie_t cookie = xcb_change_window_attributes(
      connection_, window_, XCB_CW_BACK_PIXEL, values);
  xcb_generic_error_t* error_check = xcb_request_check(connection_, cookie);
  if (error_check != NULL) {
    fprintf(stderr, "[ERROR %u] Failed to change xcb window background color\n",
            error_check->error_code);
  }
  free(error_check);
  cookie = xcb_change_gc(connection_, pixmap_bg_gcontext_, XCB_GC_FOREGROUND,
                         values);
  error_check = xcb_request_check(connection_, cookie);
  if (error_check != NULL) {
    fprintf(stderr, "[ERROR %u] Failed to change xcb graphics context color\n",
            error_check->error_code);
  }
  free(error_check);
}
void entis_set_background_rgb(uint32_t r, uint32_t g, uint32_t b) {
  entis_set_background(0x010000 * r + 0x000100 * g + b);
}
void entis_set_background_drgb(double r, double g, double b) {
  entis_set_background(0x010000 * (uint32_t)(255 * r) +
                       0x000100 * (uint32_t)(256 * g) + (uint32_t)(256 * b));
}

void entis_reload_pixmap(uint32_t w, uint32_t h) {
  xcb_free_pixmap(connection_, pixmap_);
  xcb_void_cookie_t cookie = xcb_create_pixmap(connection_, screen_->root_depth,
                                               pixmap_, screen_->root, w, h);
  xcb_generic_error_t* error_check = xcb_request_check(connection_, cookie);
  if (error_check != NULL) {
    fprintf(stderr, "[ERROR %u] Failed to resize xcb pixmap\n",
            error_check->error_code);
  }
  free(error_check);
  width_ = w;
  height_ = h;
}

void entis_point(uint16_t x, uint16_t y) {
  xcb_point_t points[1] = {{x, y}};
  xcb_poly_point(entis_get_connection(), XCB_COORD_MODE_ORIGIN, pixmap_,
                 pixmap_gcontext_, 1, points);
}
void entis_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
  // TODO
}

void entis_copy_pixmap() {
  xcb_void_cookie_t cookie = xcb_copy_area(
      connection_, pixmap_, window_, gcontext_, 0, 0, 0, 0, width_, height_);
  xcb_generic_error_t* error_check =
      xcb_request_check(entis_get_connection(), cookie);
  if (error_check != NULL) {
    fprintf(stderr, "[ERROR %u] Failed to copy pixmap to window\n",
            error_check->error_code);
  }
}

void entis_clear() {
  xcb_poly_fill_rectangle(connection_, pixmap_, pixmap_bg_gcontext_, 1,
                          (xcb_rectangle_t[]){{0, 0, width_, height_}});
}

EntisEvent entis_wait_event() {
  entis_copy_pixmap();
  entis_flush();
  EntisEvent event = entis_event_wait_event();
  while (true) {
    switch (event.type) {
      case ENTIS_EXPOSE: {
        entis_copy_pixmap();
        entis_flush();
        break;
      }
      case ENTIS_REPARENT_NOTIFY: {
        break;
      }
      case ENTIS_CONFIGURE_NOTIFY: {
        if (event.configure.width != width_ ||
            event.configure.height != height_) {
          entis_reload_pixmap(event.configure.width, event.configure.height);
        }
        break;
      }
      case ENTIS_MAP_NOTIFY: {
        break;
      }
      case ENTIS_NO_EXPOSURE: {
        break;
      }
      default: { return event; }
    }
    event = entis_event_wait_event();
  }
  return event;
}

EntisEvent entis_poll_event() {
  entis_copy_pixmap();
  entis_flush();
  EntisEvent event = entis_event_poll_event();
  while (true) {
    switch (event.type) {
      case ENTIS_EXPOSE: {
        entis_copy_pixmap();
        entis_flush();
        break;
      }
      case ENTIS_REPARENT_NOTIFY: {
        break;
      }
      case ENTIS_CONFIGURE_NOTIFY: {
        if (event.configure.width != width_ ||
            event.configure.height != height_) {
          entis_reload_pixmap(event.configure.width, event.configure.height);
        }
        break;
      }
      case ENTIS_MAP_NOTIFY: {
        break;
      }
      case ENTIS_NO_EXPOSURE: {
        return (EntisEvent){ENTIS_NO_EVENT};
        break;
      }
      default: { return event; }
    }
    event = entis_event_poll_event();
  }
  return event;
}

EntisEvent entis_wait_event_type(uint32_t type) {
  EntisEvent event = entis_wait_event();
  while ((event.type & type) == false) {
    event = entis_wait_event();
  }
  return event;
}

EntisEvent entis_poll_event_type(uint32_t type) {
  EntisEvent event = entis_poll_event();
  while ((event.type & type) == false && event.type != ENTIS_NO_EVENT) {
    event = entis_poll_event();
  }
  return event;
}

entis_key_event entis_wait_key() {
  return entis_wait_event_type(ENTIS_KEY_PRESS | ENTIS_KEY_RELEASE).key;
}
entis_key_event entis_poll_key() {
  return entis_poll_event_type(ENTIS_KEY_PRESS | ENTIS_KEY_RELEASE).key;
}

entis_button_event entis_wait_button() {
  return entis_wait_event_type(ENTIS_BUTTON_PRESS | ENTIS_BUTTON_RELEASE)
      .button;
}
entis_button_event entis_poll_button() {
  return entis_poll_event_type(ENTIS_BUTTON_PRESS | ENTIS_BUTTON_RELEASE)
      .button;
}

void entis_clear_events() {
  EntisEvent event = entis_poll_event();
  while (event.type != ENTIS_NO_EVENT && event.type != ENTIS_NO_EXPOSURE) {
    event = entis_poll_event();
  }
}
