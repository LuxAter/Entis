#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include <X11/Xlib.h>

#include "entis.h"
#include "event.h"

static Display* connection_;

static int screen_;
static Window root_window_;
static Window window_;
static Pixmap pixmap_;
static GC gcontext_, pixmap_gcontext_, pixmap_bg_gcontext_,
    pixmap_font_gcontext_, pixmap_font_bg_gcontext_;
static bool enable_font_background_;
static char* font_name_;
static XFontStruct* font_;

static uint16_t width_, height_;

static uint16_t pix_width_, pix_height_;

void entis_init(const char* title, unsigned int w, unsigned int h,
                uint32_t value_mask, void* value_list) {
  width_ = w;
  height_ = h;
  connection_ = XOpenDisplay(NULL);
  if (connection_ == NULL) {
    fprintf(stderr, "Failed to connect to X server\n");
    exit(1);
  }
  screen_ = DefaultScreen(connection_);
  root_window_ = DefaultRootWindow(connection_);
  window_ = XCreateSimpleWindow(connection_, root_window_, 0, 0, w, h, 0,
                                BlackPixel(connection_, screen_),
                                WhitePixel(connection_, screen_));
  if (!window_) {
    fprintf(stderr, "Failed to create X11 window\n");
    exit(1);
  }
  Atom delWindow = XInternAtom(connection_, "WM_DELETE_WINDOW", 0);
  XSetWMProtocols(connection_, window_, &delWindow, 1);
  pixmap_ = XCreatePixmap(connection_, root_window_, w, h,
                          DefaultDepth(connection_, screen_));
  if (!pixmap_) {
    fprintf(stderr, "Failed to generate X11 pixmap\n");
    exit(1);
  }
  XMapWindow(connection_, window_);
  XSelectInput(connection_, window_,
               ExposureMask | StructureNotifyMask | PointerMotionMask |
                   ButtonPressMask | KeyPressMask);
  XGCValues vals;
  vals.graphics_exposures = 0;
  gcontext_ = XCreateGC(connection_, window_, GCGraphicsExposures, &vals);
  if (!gcontext_) {
    fprintf(stderr, "Failed to create X11 graphics context\n");
    exit(1);
  }
  pixmap_gcontext_ = XCreateGC(connection_, pixmap_, 0, 0);
  if (!pixmap_gcontext_) {
    fprintf(stderr, "Failed to create X11 graphics context\n");
    exit(1);
  }
  pixmap_bg_gcontext_ = XCreateGC(connection_, pixmap_, 0, 0);
  if (!pixmap_bg_gcontext_) {
    fprintf(stderr, "Failed to create X11 graphics context\n");
    exit(1);
  }
  pixmap_font_gcontext_ = XCreateGC(connection_, pixmap_, 0, 0);
  if (!pixmap_font_gcontext_) {
    fprintf(stderr, "Failed to create X11 graphics context\n");
    exit(1);
  }
  pixmap_font_bg_gcontext_ = XCreateGC(connection_, pixmap_, 0, 0);
  if (!pixmap_font_bg_gcontext_) {
    fprintf(stderr, "Failed to create X11 graphics context\n");
    exit(1);
  }
  enable_font_background_ = false;
  /* font_name_ = "-*-helvetica-*-10-*"; */
  font_name_ = "10x20";
  font_ = XLoadQueryFont(connection_, font_name_);
  if (!font_) {
    fprintf(stderr, "Failed to load X11 font %s\n", font_name_);
    font_ = XLoadQueryFont(connection_, "fixed");
  }
  XSetFont(connection_, pixmap_font_gcontext_, font_->fid);
  XClearArea(connection_, window_, 0, 0, 0, 0, true);
  entis_set_background(0x000000);
  entis_clear();
  entis_update();
  entis_pause(0, 3.125e7);
  entis_clear_events();
  /* entis_set_color(0xFFFFFF); */
}

void entis_term() {
  XDestroyWindow(connection_, window_);
  XFreeGC(connection_, gcontext_);
  XFreeGC(connection_, pixmap_gcontext_);
  XFreeGC(connection_, pixmap_bg_gcontext_);
  XFreePixmap(connection_, pixmap_);
  XCloseDisplay(connection_);
}

void entis_set_font(const char* font_name, uint16_t size) {
  font_name_ = malloc(strlen(font_name) + 1);
  strcpy(font_name_, font_name);
  char* font_str_ = malloc(255);
  sprintf(font_str_, "-*-%s-*-*-*-*-%d-*-*-*-*-*-*-*", font_name, size);
  XFreeFont(connection_, font_);
  font_ = XLoadQueryFont(connection_, font_str_);
  if (!font_) {
    fprintf(stderr, "Failed to load X11 font %s\n", font_name_);
    font_ = XLoadQueryFont(connection_, "fixed");
  }
  XSetFont(connection_, pixmap_font_gcontext_, font_->fid);
}

void entis_set_font_size(uint16_t size) {
  char* font_str_ = malloc(255);
  sprintf(font_str_, "-*-%s-*-*-*-*-%d-*-*-*-*-*-*-*", font_name_, size);
  XFreeFont(connection_, font_);
  font_ = XLoadQueryFont(connection_, font_str_);
  if (!font_) {
    fprintf(stderr, "Failed to load X11 font %s\n", font_name_);
    font_ = XLoadQueryFont(connection_, "fixed");
  }
  XSetFont(connection_, pixmap_font_gcontext_, font_->fid);
}

void entis_flush() { XFlush(connection_); }

Display* entis_get_connection() { return connection_; }
Window entis_get_window() { return window_; }
int entis_get_screen() { return screen_; }
Pixmap entis_get_pixmap() { return pixmap_; }

void entis_set_color(uint32_t color) {
  XSetForeground(connection_, pixmap_gcontext_, color);
}
void entis_set_color_rgb(uint32_t r, uint32_t g, uint32_t b) {
  entis_set_color(0x010000 * r + 0x000100 * g + b);
}
void entis_set_color_drgb(double r, double g, double b) {
  entis_set_color(0x010000 * (uint32_t)(255 * r) +
                  0x000100 * (uint32_t)(256 * g) + (uint32_t)(256 * b));
}

void entis_set_background(uint32_t color) {
  XSetWindowBackground(connection_, window_, color);
  XSetForeground(connection_, pixmap_bg_gcontext_, color);
}
void entis_set_background_rgb(uint32_t r, uint32_t g, uint32_t b) {
  entis_set_background(0x010000 * r + 0x000100 * g + b);
}
void entis_set_background_drgb(double r, double g, double b) {
  entis_set_background(0x010000 * (uint32_t)(255 * r) +
                       0x000100 * (uint32_t)(256 * g) + (uint32_t)(256 * b));
}
void entis_font_color(uint32_t color) {
  XSetForeground(connection_, pixmap_font_gcontext_, color);
}
void entis_font_color_rgb(uint32_t r, uint32_t g, uint32_t b) {
  entis_font_color(0x010000 * r + 0x000100 * g + b);
}
void entis_font_color_drgb(double r, double g, double b) {
  entis_font_color(0x010000 * (uint32_t)(255 * r) +
                   0x000100 * (uint32_t)(256 * g) + (uint32_t)(256 * b));
}
void entis_font_set_background(bool setting) {
  enable_font_background_ = setting;
}
void entis_font_background(uint32_t color) {
  if (enable_font_background_ == false) {
    enable_font_background_ = true;
  }
  XSetForeground(connection_, pixmap_font_bg_gcontext_, color);
}
void entis_font_background_rgb(uint32_t r, uint32_t g, uint32_t b) {
  entis_font_background(0x010000 * r + 0x000100 * g + b);
}
void entis_font_background_drgb(double r, double g, double b) {
  entis_font_background(0x010000 * (uint32_t)(255 * r) +
                        0x000100 * (uint32_t)(256 * g) + (uint32_t)(256 * b));
}

void entis_reload_pixmap(uint32_t w, uint32_t h) {
  XFreePixmap(connection_, pixmap_);
  pixmap_ = XCreatePixmap(connection_, root_window_, w, h,
                          DefaultDepth(connection_, screen_));
  XClearWindow(connection_, window_);
  width_ = w;
  height_ = h;
  entis_clear();
}

void entis_copy_pixmap() {
  XCopyArea(connection_, pixmap_, window_, gcontext_, 0, 0, width_, height_, 0,
            0);
}

void entis_update() {
  entis_copy_pixmap();
  entis_flush();
}

void entis_clear() {
  XFillRectangle(connection_, pixmap_, pixmap_bg_gcontext_, 0, 0, width_,
                 height_);
  entis_flush();
}

XEvent entis_wait_event() {
  entis_update();
  XEvent event = entis_event_wait_event();
  while (true) {
    switch (event.type) {
      case NoExpose:
      case Expose: {
        entis_copy_pixmap();
        entis_flush();
        break;
      }
      case ReparentNotify: {
        break;
      }
      case ConfigureNotify: {
        if (event.xconfigure.width != width_ ||
            event.xconfigure.height != height_) {
          entis_reload_pixmap(event.xconfigure.width, event.xconfigure.height);
        }
        break;
      }
      case MapNotify: {
        break;
      }
      default: { return event; }
    }
    event = entis_event_wait_event();
  }
  return event;
}

XEvent entis_poll_event() {
  entis_update();
  XEvent event = entis_event_poll_event();
  while (true) {
    switch (event.type) {
      case NoExpose:
      case Expose: {
        entis_copy_pixmap();
        entis_flush();
        break;
      }
      case ReparentNotify: {
        break;
      }
      case ConfigureNotify: {
        if (event.xconfigure.width != width_ ||
            event.xconfigure.height != height_) {
          entis_reload_pixmap(event.xconfigure.width, event.xconfigure.height);
        }
        break;
      }
      case MapNotify: {
        break;
      }
      default: { return event; }
    }
    event = entis_event_poll_event();
  }
  return event;
}

XEvent entis_wait_event_type(uint32_t type) {
  XEvent event = entis_wait_event();
  while (event.type != type) {
    event = entis_wait_event();
  }
  return event;
}

XEvent entis_poll_event_type(uint32_t type) {
  XEvent event = entis_poll_event();
  while ((event.type & type) == false && event.type != 0) {
    event = entis_poll_event();
  }
  return event;
}

XKeyEvent entis_wait_key() { return entis_wait_event_type(KeyPress).xkey; }
XKeyEvent entis_poll_key() { return entis_poll_event_type(KeyPress).xkey; }

XButtonEvent entis_wait_button() {
  return entis_wait_event_type(ButtonPress).xbutton;
}
XButtonEvent entis_poll_button() {
  return entis_poll_event_type(ButtonPress).xbutton;
}

void entis_clear_events() {
  XEvent event = entis_poll_event();
  while (event.type != 0) {
    event = entis_poll_event();
  }
}

void entis_point(uint16_t x, uint16_t y) {
  XDrawPoint(connection_, pixmap_, pixmap_gcontext_, x, y);
}
void entis_segment(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
  XDrawLine(connection_, pixmap_, pixmap_gcontext_, x0, y1, x1, y1);
}
void entis_line(uint16_t* x, uint16_t* y, uint16_t n) {
  XPoint points[n];
  for (int i = 0; i < n; i++) {
    points[i] = (XPoint){x[i], y[i]};
  }
  XDrawLines(connection_, pixmap_, pixmap_gcontext_, points, n,
             CoordModeOrigin);
}
void entis_triangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                    uint16_t x2, uint16_t y2) {
  XDrawLines(connection_, pixmap_, pixmap_gcontext_,
             (XPoint[]){{x0, y0}, {x1, y1}, {x2, y2}, {x0, y0}}, 4,
             CoordModeOrigin);
}
void entis_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  XDrawRectangle(connection_, pixmap_, pixmap_gcontext_, x, y, width, height);
}
void entis_arc(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
               uint16_t angle1, uint16_t angle2) {
  XDrawArc(connection_, pixmap_, pixmap_gcontext_, x, y, width, height, angle1,
           angle2);
}
void entis_poly(uint16_t* x, uint16_t* y, uint16_t n) {
  XPoint points[n + 1];
  for (int i = 0; i < n; i++) {
    points[i] = (XPoint){x[i], y[i]};
  }
  points[n] = points[0];
  XDrawLines(connection_, pixmap_, pixmap_gcontext_, points, n + 1,
             CoordModeOrigin);
}

void entis_fill_triangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                         uint16_t x2, uint16_t y2) {
  XFillPolygon(connection_, pixmap_, pixmap_gcontext_,
               (XPoint[]){{x0, y0}, {x1, y1}, {x2, y2}, {x0, y0}}, 4, Convex,
               CoordModeOrigin);
}
void entis_fill_rectangle(uint16_t x, uint16_t y, uint16_t width,
                          uint16_t height) {
  XFillRectangle(connection_, pixmap_, pixmap_gcontext_, x, y, width, height);
}
void entis_fill_arc(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                    uint16_t angle1, uint16_t angle2) {
  XFillArc(connection_, pixmap_, pixmap_gcontext_, x, y, width, height, angle1,
           angle2);
}
void entis_fill_poly(uint16_t* x, uint16_t* y, uint16_t n) {
  XPoint points[n + 1];
  for (int i = 0; i < n; i++) {
    points[i] = (XPoint){x[i], y[i]};
  }
  points[n] = points[0];
  XFillPolygon(connection_, pixmap_, pixmap_gcontext_, points, n + 1, Convex,
               CoordModeOrigin);
}

void entis_reg_poly(uint16_t x, uint16_t y, uint16_t radius_x,
                    uint16_t radius_y, uint16_t n, double offset) {
  uint16_t points_x[n + 1], points_y[n + 1];
  double delta = 2.0 * M_PI / (double)n;
  int index = 0;
  for (double theta = 0 + offset; theta < (2 * M_PI) + offset;
       theta += delta, index++) {
    points_x[index] = radius_x * cos(theta) + x;
    points_y[index] = radius_y * sin(theta) + y;
  }
  points_x[n] = points_x[0];
  points_y[n] = points_y[0];
  entis_poly(points_x, points_y, n + 1);
}
void entis_fill_reg_poly(uint16_t x, uint16_t y, uint16_t radius_x,
                         uint16_t radius_y, uint16_t n, double offset) {
  uint16_t points_x[n + 1], points_y[n + 1];
  double delta = 2.0 * M_PI / (double)n;
  int index = 0;
  for (double theta = 0 + offset; theta < (2 * M_PI) + offset;
       theta += delta, index++) {
    points_x[index] = radius_x * cos(theta) + x;
    points_y[index] = radius_y * sin(theta) + y;
  }
  points_x[n] = points_x[0];
  points_y[n] = points_y[0];
  entis_fill_poly(points_x, points_y, n + 1);
}

void entis_circle(uint16_t x, uint16_t y, uint16_t radius) {
  entis_reg_poly(x, y, radius, radius, radius, 0);
}
void entis_fill_circle(uint16_t x, uint16_t y, uint16_t radius) {
  entis_fill_reg_poly(x, y, radius, radius, radius, 0);
}

void entis_draw_string(uint16_t x, uint16_t y, const char* fmt, ...) {
  char* text = malloc(255);
  va_list args;
  va_start(args, fmt);
  vsprintf(text, fmt, args);
  va_end(args);
  if (enable_font_background_ == true) {
    uint16_t text_width = XTextWidth(font_, text, strlen(text));
    uint16_t text_height = font_->max_bounds.ascent + font_->max_bounds.descent;
    XFillRectangle(connection_, pixmap_, pixmap_font_bg_gcontext_, x,
                   y - text_height, text_width, text_height);
  }
  XDrawString(connection_, pixmap_, pixmap_font_gcontext_, x, y, text,
              strlen(text));
}
void entis_erase_string(uint16_t x, uint16_t y, const char* fmt, ...) {
  char* text = malloc(255);
  va_list args;
  va_start(args, fmt);
  vsprintf(text, fmt, args);
  va_end(args);
  uint16_t text_width = XTextWidth(font_, text, strlen(text));
  uint16_t text_height = font_->max_bounds.ascent + font_->max_bounds.descent;
  XFillRectangle(connection_, pixmap_, pixmap_bg_gcontext_, x, y - text_height,
                 text_width, text_height);
}

uint16_t entis_get_pixel_width() { return (width_ / pix_width_); }
uint16_t entis_get_pixel_height() { return (height_ / pix_height_); };
void entis_set_pixel_size(uint16_t width, uint16_t height) {
  pix_width_ = width;
  pix_height_ = height;
}
void entis_set_pixel(uint16_t x, uint16_t y) {
  if (pix_width_ * x < width_ && pix_height_ * y < height_) {
    entis_fill_rectangle(x * pix_width_, y * pix_height_, pix_width_,
                         pix_height_);
  }
}

void entis_pixel_set_pixel(uint16_t x, uint16_t y) {
  if (x < width_ && y < height_) {
    x -= (x % pix_width_);
    y -= (y % pix_height_);
    entis_fill_rectangle(x, y, pix_width_, pix_height_);
  }
}

void entis_pause(uint64_t seconds, uint64_t nanoseconds) {
  nanosleep(&(struct timespec){seconds, nanoseconds}, NULL);
}
