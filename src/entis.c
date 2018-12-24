#include "entis.h"

#include <stdbool.h>
#include <stdint.h>

#include <xcb/xcb.h>

#include "error.h"

static xcb_connection_t* connection_ = NULL;
static xcb_screen_t* screen_ = NULL;
static xcb_window_t window_;
static xcb_pixmap_t pixmap_;
static xcb_gcontext_t gcontext_, fg_gcontext_, bg_gcontext_;

static FT_Library library_;
static FT_Face face_;

static bool xcb_ = false;
static bool ft_ = false;

static uint32_t fg_ = 0xffffff, bg_ = 0x000000;
static uint32_t width_ = 0, height_ = 0;

static uint32_t** frame_buffer_ = NULL;

bool entis_init(uint32_t width, uint32_t height, uint32_t flags) {
  width_ = width;
  height_ = height;
  frame_buffer_ = (uint32_t**)malloc(height * sizeof(uint32_t*));
  for (uint32_t i = 0; i < height; ++i) {
    frame_buffer_[i] = (uint32_t*)malloc(width * sizeof(uint32_t));
  }
  if ((flags & ENTIS_XCB) != 0) {
    entis_init_xcb("Entis");
  }
  if ((flags & ENTIS_TTF) != 0) {
    entis_init_ft();
  }
  return true;
}

bool entis_term() {
  if (xcb_ == true && !entis_term_xcb()) {
    return false;
  }
  if (ft_ == true && !entis_term_ft()) {
    return false;
  }
  for (uint32_t i = 0; i < height_; ++i) {
    free(frame_buffer_[i]);
  }
  free(frame_buffer_);
  return true;
}

bool entis_init_xcb(const char* title) {
  if (connection_ != NULL) {
    entis_warning("Connection to XCB already established");
    return false;
  }
  connection_ = xcb_connect(NULL, NULL);
  if (connection_ == NULL) {
    entis_error("Failed to connect to XCB");
    return false;
  } else if (xcb_connection_has_error(connection_)) {
    entis_error("Error while attempting to connect to XCB");
    xcb_disconnect(connection_);
    return false;
  }
  screen_ = xcb_setup_roots_iterator(xcb_get_setup(connection_)).data;
  window_ = xcb_generate_id(connection_);
  uint32_t internal_value[1] = {
      XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS |
      XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_ENTER_WINDOW |
      XCB_EVENT_MASK_LEAVE_WINDOW | XCB_EVENT_MASK_KEY_PRESS |
      XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_STRUCTURE_NOTIFY};
  xcb_void_cookie_t cookie = xcb_create_window(
      connection_, XCB_COPY_FROM_PARENT, window_, screen_->root, 0, 0, width_,
      height_, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen_->root_visual,
      XCB_CW_EVENT_MASK, internal_value);
  xcb_generic_error_t* error_check = xcb_request_check(connection_, cookie);
  if (error_check != NULL) {
    entis_error("(%u)Failed to generate XCB window", error_check->error_code);
    xcb_destroy_window(connection_, window_);
    xcb_disconnect(connection_);
    return false;
  }
  free(error_check);
  pixmap_ = xcb_generate_id(connection_);
  cookie = xcb_create_pixmap(connection_, screen_->root_depth, pixmap_,
                             screen_->root, width_, height_);
  error_check = xcb_request_check(connection_, cookie);
  if (error_check != NULL) {
    entis_error("(%u) Failed to generate XCB pixmap", error_check->error_code);
    xcb_destroy_window(connection_, window_);
    xcb_disconnect(connection_);
    return false;
  }
  free(error_check);
  gcontext_ = xcb_generate_id(connection_);
  cookie = xcb_create_gc(connection_, gcontext_, screen_->root, 0, NULL);
  error_check = xcb_request_check(connection_, cookie);
  if (error_check != NULL) {
    entis_error("(%u) Failed to generate XCB graphics context",
                error_check->error_code);
    xcb_destroy_window(connection_, window_);
    xcb_disconnect(connection_);
    return false;
  }
  free(error_check);
  fg_gcontext_ = xcb_generate_id(connection_);
  cookie = xcb_create_gc(connection_, fg_gcontext_, screen_->root,
                         XCB_GC_FOREGROUND, (uint32_t[]){fg_});
  error_check = xcb_request_check(connection_, cookie);
  if (error_check != NULL) {
    entis_error("(%u) Failed to generate XCB fg graphics context",
                error_check->error_code);
    xcb_destroy_window(connection_, window_);
    xcb_disconnect(connection_);
    return false;
  }
  free(error_check);
  bg_gcontext_ = xcb_generate_id(connection_);
  cookie = xcb_create_gc(connection_, bg_gcontext_, screen_->root,
                         XCB_GC_FOREGROUND, (uint32_t[]){bg_});
  error_check = xcb_request_check(connection_, cookie);
  if (error_check != NULL) {
    entis_error("(%u) Failed to generate XCB bg graphics context",
                error_check->error_code);
    xcb_destroy_window(connection_, window_);
    xcb_disconnect(connection_);
    return false;
  }
  free(error_check);
  xcb_poly_fill_rectangle(connection_, pixmap_, bg_gcontext_, 1,
                          (xcb_rectangle_t[]){{0, 0, width_, height_}});
  if (strlen(title) != 0) {
    xcb_change_property(connection_, XCB_PROP_MODE_REPLACE, window_,
                        XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(title),
                        title);
  }
  xcb_map_window(connection_, window_);
  xcb_flush(connection_);
  nanosleep(&(struct timespec){0, 3.125e7}, NULL);
  entis_clear_events();
  xcb_ = true;
  return true;
}
bool entis_term_xcb() {
  if (connection_ == NULL) {
    entis_warning("No connection to XCB established");
    return false;
  }
  xcb_destroy_window(connection_, window_);
  xcb_disconnect(connection_);
  xcb_ = false;
  return true;
}

bool entis_init_ft() {
  if (ft_ == true) {
    entis_warning("FreeType already initialized");
    return false;
  }
  FT_Error error;
  error = FT_Init_FreeType(&library_);
  if (error) {
    entis_error("Failed to initlize FreeType2");
    return false;
  }
  ft_ = true;
  return true;
}
bool entis_term_ft() {
  ft_ = false;
  return false;
}

void entis_resize(uint32_t width, uint32_t height) {
  for (uint32_t i = 0; i < height_; ++i) {
    free(frame_buffer_[i]);
  }
  free(frame_buffer_);
  width_ = width;
  height_ = height;
  frame_buffer_ = (uint32_t**)malloc(height_ * sizeof(uint32_t*));
  for (uint32_t i = 0; i < height_; ++i) {
    frame_buffer_[i] = (uint32_t*)malloc(width_ * sizeof(uint32_t));
  }
  entis_xcb_reload_pixmap();
}

void entis_clear() {
  for (uint32_t i = 0; i < height_; ++i) {
    for (uint32_t j = 0; j < width_; ++j) {
      frame_buffer_[i][j] = bg_;
    }
  }
  entis_xcb_clear();
}

bool entis_xcb() {
  if (connection_ != NULL && xcb_ != false) {
    return true;
  }
  return false;
}

void entis_xcb_flush() {
  if (xcb_) {
    xcb_flush(connection_);
  }
}

xcb_connection_t* entis_xcb_connection() { return connection_; }

void entis_xcb_reload_pixmap() {
  xcb_free_pixmap(connection_, pixmap_);
  xcb_void_cookie_t cookie =
      xcb_create_pixmap(connection_, screen_->root_depth, pixmap_,
                        screen_->root, width_, height_);
  xcb_generic_error_t* error_check = xcb_request_check(connection_, cookie);
  if (error_check != NULL) {
    entis_error("(%u) Failed to resize XCB pixmap", error_check->error_code);
  }
  free(error_check);
  xcb_poly_fill_rectangle(connection_, pixmap_, bg_gcontext_, 1,
                          (xcb_rectangle_t[]){{0, 0, width_, height_}});
}
void entis_xcb_copy_pixmap() {
  xcb_void_cookie_t cookie = xcb_copy_area(
      connection_, pixmap_, window_, gcontext_, 0, 0, 0, 0, width_, height_);
  xcb_generic_error_t* error_check = xcb_request_check(connection_, cookie);
  if (error_check != NULL) {
    entis_error("(%u) Failed to copy XCB pixmap to XCB window",
                error_check->error_code);
  }
}
void entis_xcb_update() {
  entis_xcb_copy_pixmap();
  entis_xcb_flush();
}
void entis_xcb_clear() {
  xcb_poly_fill_rectangle(connection_, pixmap_, bg_gcontext_, 1,
                          (xcb_rectangle_t[]){{0, 0, width_, height_}});
}

entis_event entis_wait_event() {
  if (!xcb_) {
    return entis_event_no_event();
  }
  entis_xcb_update();
  while (true) {
    entis_event event = entis_event_wait_event();
    switch (event.type) {
      case ENTIS_EXPOSE:
        entis_xcb_update();
        break;
      case ENTIS_CONFIGURE_NOTIFY:
        if (event.configure.width != width_ ||
            event.configure.height != height_) {
          entis_resize(event.configure.width, event.configure.height);
        }
        break;
      case ENTIS_REPARENT_NOTIFY:
      case ENTIS_MAP_NOTIFY:
      case ENTIS_NO_EXPOSURE:
        break;
      default:
        return event;
    }
  }
}
entis_event entis_poll_event() {
  if (!xcb_) {
    return entis_event_no_event();
  }
  entis_xcb_update();
  while (true) {
    entis_event event = entis_event_poll_event();
    switch (event.type) {
      case ENTIS_EXPOSE:
        entis_xcb_update();
        break;
      case ENTIS_CONFIGURE_NOTIFY:
        if (event.configure.width != width_ ||
            event.configure.height != height_) {
          entis_resize(event.configure.width, event.configure.height);
        }
        break;
      case ENTIS_REPARENT_NOTIFY:
      case ENTIS_MAP_NOTIFY:
      case ENTIS_NO_EXPOSURE:
        break;
      default:
        return event;
    }
  }
}

entis_event entis_wait_event_type(uint32_t type) {
  entis_event event = entis_wait_event();
  while ((event.type & type) == false) {
    event = entis_wait_event();
  }
  return event;
}
entis_event entis_poll_event_type(uint32_t type) {
  entis_event event = entis_poll_event();
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
  entis_event event = entis_poll_event();
  while (event.type != ENTIS_NO_EVENT && event.type) {
    event = entis_poll_event();
  }
}

void entis_color_int(uint32_t color) {
  fg_ = color;
  if (xcb_) {
    xcb_void_cookie_t cookie = xcb_change_gc(
        connection_, fg_gcontext_, XCB_GC_FOREGROUND, (uint32_t[]){fg_});
    xcb_generic_error_t* error_check = xcb_request_check(connection_, cookie);
    if (error_check != NULL) {
      entis_warning("(%u)Failed to change XCB fg graphics context color",
                    error_check->error_code);
    }
    free(error_check);
  }
}
void entis_color_rgb(uint8_t r, uint8_t g, uint8_t b) {
  fg_ = ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
  if (xcb_) {
    xcb_void_cookie_t cookie = xcb_change_gc(
        connection_, fg_gcontext_, XCB_GC_FOREGROUND, (uint32_t[]){fg_});
    xcb_generic_error_t* error_check = xcb_request_check(connection_, cookie);
    if (error_check != NULL) {
      entis_warning("(%u)Failed to change XCB fg graphics context color",
                    error_check->error_code);
    }
    free(error_check);
  }
}
void entis_color_drgb(double r, double g, double b) {
  fg_ = ((uint32_t)(r * 255) << 16) | ((uint32_t)(g * 255) << 8) |
        (uint32_t)(b * 255);
  if (xcb_) {
    xcb_void_cookie_t cookie = xcb_change_gc(
        connection_, fg_gcontext_, XCB_GC_FOREGROUND, (uint32_t[]){fg_});
    xcb_generic_error_t* error_check = xcb_request_check(connection_, cookie);
    if (error_check != NULL) {
      entis_warning("(%u)Failed to change XCB fg graphics context color",
                    error_check->error_code);
    }
    free(error_check);
  }
}
void entis_background_int(uint32_t color) {
  bg_ = color;
  if (xcb_) {
    xcb_void_cookie_t cookie = xcb_change_gc(
        connection_, bg_gcontext_, XCB_GC_FOREGROUND, (uint32_t[]){bg_});
    xcb_generic_error_t* error_check = xcb_request_check(connection_, cookie);
    if (error_check != NULL) {
      entis_warning("(%u)Failed to change XCB bg graphics context color",
                    error_check->error_code);
    }
    free(error_check);
  }
}
void entis_background_rgb(uint8_t r, uint8_t g, uint8_t b) {
  bg_ = ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
  if (xcb_) {
    xcb_void_cookie_t cookie = xcb_change_gc(
        connection_, bg_gcontext_, XCB_GC_FOREGROUND, (uint32_t[]){bg_});
    xcb_generic_error_t* error_check = xcb_request_check(connection_, cookie);
    if (error_check != NULL) {
      entis_warning("(%u)Failed to change XCB bg graphics context color",
                    error_check->error_code);
    }
    free(error_check);
  }
}
void entis_background_drgb(double r, double g, double b) {
  bg_ = ((uint32_t)(r * 255) << 16) | ((uint32_t)(g * 255) << 8) |
        (uint32_t)(b * 255);
  if (xcb_) {
    xcb_void_cookie_t cookie = xcb_change_gc(
        connection_, bg_gcontext_, XCB_GC_FOREGROUND, (uint32_t[]){bg_});
    xcb_generic_error_t* error_check = xcb_request_check(connection_, cookie);
    if (error_check != NULL) {
      entis_warning("(%u)Failed to change XCB bg graphics context color",
                    error_check->error_code);
    }
    free(error_check);
  }
}

bool entis_load_font(const char* font_name) {
  FT_Error error = FT_New_Face(library_, font_name, 0, &face_);
  if (error == FT_Err_Unknown_File_Format) {
    entis_warning("Font \"%s\" is an unsuported format", font_name);
    return false;
  } else if (error) {
    entis_warning("Could not open \"%s\" font", font_name);
    return false;
  }
  error = FT_Set_Char_Size(face_, 0, 12 * 64, 276, 276);
  if (error) {
    fprintf(stderr, "[ERROR FreeType2] Failed to set char size\n");
  }
  return true;
}
void enti_font_size(uint16_t pt, uint32_t dpi) {
  FT_Error error = FT_Set_Char_Size(face_, pt * 64, pt * 64, dpi, dpi);
  if (error) {
    entis_warning("Failed to set character size to %lu with dpi of %lu", pt,
                  dpi);
  }
}
void enti_font_px(uint16_t px) {
  FT_Error error = FT_Set_Pixel_Sizes(face_, 0, 12);
  if (error) {
    entis_warning("Failed to set character size to %lupx", px);
  }
  xcb_connection_t* ensti_xcb_connection();
}

void entis_point(uint32_t x, uint32_t y) {
  if (x < width_ && y < height_) {
    frame_buffer_[y][x] = fg_;
    if (xcb_) {
      xcb_poly_point(connection_, XCB_COORD_MODE_ORIGIN, pixmap_, fg_gcontext_,
                     1, (xcb_point_t[]){{x, y}});
    }
  }
}
void entis_points(uint32_t* x, uint32_t* y, uint32_t n) {
  xcb_point_t* points = (xcb_point_t*)malloc(n * sizeof(xcb_point_t));
  for (uint32_t i = 0; i < n; ++i) {
    if (x[i] < width_ && y[i] < height_) {
      frame_buffer_[y[i]][x[i]] = fg_;
      points[i] = (xcb_point_t){x[i], y[i]};
    }
  }
  if (xcb_) {
    xcb_poly_point(connection_, XCB_COORD_MODE_ORIGIN, pixmap_, fg_gcontext_, 1,
                   points);
  }
}
// TODO: Compleate reasterization of lines
void entis_line(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1){

}

uint32_t entis_get_color(uint32_t x, uint32_t y) {
  if (x >= width_ || y >= height_) {
    return 0;
  }
  return frame_buffer_[y][x];
}
