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
  /* entis_clear_events(); */
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

void entis_color_int(uint32_t color) { fg_ = color; }
void entis_color_rgb(uint8_t r, uint8_t g, uint8_t b) {
  fg_ = ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}
void entis_color_drgb(double r, double g, double b) {
  fg_ = ((uint32_t)(r * 255) << 16) | ((uint32_t)(g * 255) << 8) |
        (uint32_t)(b * 255);
}
void entis_background_int(uint32_t color) { bg_ = color; }
void entis_background_rgb(uint8_t r, uint8_t g, uint8_t b) {
  fg_ = ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;
}
void entis_background_drgb(double r, double g, double b) {
  fg_ = ((uint32_t)(r * 255) << 16) | ((uint32_t)(g * 255) << 8) |
        (uint32_t)(b * 255);
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
}
