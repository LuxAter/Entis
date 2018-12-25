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
void entis_line_low(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1) {
  int dx = x1 - x0;
  int dy = y1 - y0;
  int yi = 1;
  if (dy < 0) {
    yi = -1;
    dy = -dy;
  }
  int d = 2 * dy - dx;
  int y = y0;
  for (int x = x0; x <= x1; ++x) {
    entis_point(x, y);
    if (d > 0) {
      y += yi;
      d -= (2 * dx);
    }
    d += (2 * dy);
  }
}
void entis_line_high(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1) {
  int dx = x1 - x0;
  int dy = y1 - y0;
  int xi = 1;
  if (dx < 0) {
    xi = -1;
    dx = -dx;
  }
  int d = 2 * dx - dy;
  int x = x0;
  for (int y = y0; y <= y1; ++y) {
    entis_point(x, y);
    if (d > 0) {
      x += xi;
      d -= (2 * dy);
    }
    d += (2 * dx);
  }
}
void entis_line(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1) {
  if (labs((int64_t)y1 - (int64_t)y0) < labs((int64_t)x1 - (int64_t)x0)) {
    if (x0 > x1) {
      entis_line_low(x1, y1, x0, y0);
    } else {
      entis_line_low(x0, y0, x1, y1);
    }
  } else {
    if (y0 > y1) {
      entis_line_high(x1, y1, x0, y0);
    } else {
      entis_line_high(x0, y0, x1, y1);
    }
  }
}

void entis_triangle(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1,
                    uint32_t x2, uint32_t y2) {
  entis_line(x0, y0, x1, y1);
  entis_line(x1, y1, x2, y2);
  entis_line(x2, y2, x0, y0);
}

void entis_rectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
  entis_line(x, y, x + width, y);
  entis_line(x, y, x, y + height);
  entis_line(x + width, y, x + width, y + height);
  entis_line(x, y + height, x + width, y + height);
}

void entis_circle(uint32_t cx, uint32_t cy, uint32_t r) {
  uint32_t x = r - 1;
  uint32_t y = 0;
  int32_t err = (x * x) - (r * r) + r;
  while (x >= y) {
    entis_point(cx + x, cy + y);
    entis_point(cx + y, cy + x);
    entis_point(cx - y, cy + x);
    entis_point(cx - x, cy + y);
    entis_point(cx - x, cy - y);
    entis_point(cx - y, cy - x);
    entis_point(cx + y, cy - x);
    entis_point(cx + x, cy - y);
    if (err <= 0) {
      y++;
      err += 2 * y + 1;
    } else {
      x--;
      err -= x * 2 + 1;
    }
  }
}

void entis_poly(uint32_t* x, uint32_t* y, uint32_t n) {
  for (uint32_t i = 1; i < n; ++i) {
    entis_line(x[i - 1], y[i - 1], x[i], y[i]);
  }
  entis_line(x[0], y[0], x[n - 1], y[n - 1]);
}

void entis_triangle_fill(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1,
                         uint32_t x2, uint32_t y2) {
  entis_poly_fill((uint32_t[]){x0, x1, x2}, (uint32_t[]){y0, y1, y2}, 3);
}

void entis_rectangle_fill(uint32_t x, uint32_t y, uint32_t width,
                          uint32_t height) {
  for (uint32_t i = y; i < y + height; ++i) {
    for (uint32_t j = x; j < x + width; ++j) {
      entis_point(j, i);
    }
  }
}

void entis_circle_fill(uint32_t cx, uint32_t cy, uint32_t r) {
  uint32_t rsq = r * r;
  for (int32_t y = -r; y <= r; ++y) {
    uint32_t ysq = y * y;
    for (uint32_t x = -r; x <= r; ++x) {
      if (x * x + ysq < rsq) {
        entis_point(cx + x, cy + y);
      }
    }
  }
}
void entis_poly_fill(uint32_t *x, uint32_t *y, uint32_t n) {
  uint32_t min_y = y[0];
  uint32_t max_y = y[0];
  for (uint32_t i = 1; i < n; ++i) {
    min_y = max(min_y, y[i]);
    max_y = max(max_y, y[i]);
  }
  for (uint32_t i = min_y; i <= max_y; ++i) {
    uint32_t inter_mem = 8, inter_size = 0;
    uint32_t* inter = (uint32_t*)malloc(8 * sizeof(uint32_t));
    for (uint32_t j = 0; j < n; ++j) {
      uint32_t k = (j + 1) % n;
      if (max(y[j], y[k]) > i && min(y[j], y[k]) <= i) {
        uint32_t inter_pos = 0;
        if (x[j] == x[k]) {
          inter_pos = x[j];
        } else {
          double m =
              ((double)y[k] - (double)y[j]) / ((double)x[k] - (double)x[j]);
          inter_pos = (uint32_t)((i - y[j] + (m * x[j])) / m);
        }
        if (inter_size == inter_mem) {
          uint32_t* temp_inter =
              (uint32_t*)realloc(inter, inter_mem * 2 * sizeof(uint32_t));
          inter = temp_inter;
        }
        for (uint32_t l = 0; l < inter_size; ++l) {
          if (inter_pos < inter[l]) {
            for (uint32_t m = l + 1; m < inter_size + 1; ++m) {
              inter[m] = inter[m - 1];
            }
            inter[l] = inter_pos;
            break;
          }
        }
        inter_size++;
        // INSERT INTERSECTION
      }
    }
    for (uint32_t j = 0; j < inter_size; j += 2) {
      entis_line(inter[j], i, inter[(j + 1) % inter_size], i);
    }
    free(inter);
  }
}

uint32_t entis_get_color(uint32_t x, uint32_t y) {
  if (x >= width_ || y >= height_) {
    return 0;
  }
  return frame_buffer_[y][x];
}
