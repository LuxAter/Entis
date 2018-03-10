#ifndef ENTIS_ENTIS_H_
#define ENTIS_ENTIS_H_

#include <X11/Xlib.h>
#include <inttypes.h>
#include <stdbool.h>

#include "event.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define D30 0.52359877559829881565
#define D45 0.78539816339744827899
#define D60 1.04719755119659763131
#define D90 1.57079632679489655799

#define ENTIS_BLACK 0x000000
#define ENTIS_RED 0xFF0000
#define ENTIS_GREEN 0x00FF00
#define ENTIS_BLUE 0x0000FF
#define ENTIS_YELLOW 0xFFFF00
#define ENTIS_MAGENTA 0xFF00FF
#define ENTIS_CYAN 0x00FFFF
#define ENTIS_WHITE 0xFFFFFF

typedef struct Button {
  uint16_t x, y, width, height;
  const char* text;
  uint32_t fg_normal, fg_hover;
  uint32_t bg_normal, bg_hover;
  bool is_hover;
} Button;

void entis_init(const char* title, unsigned int w, unsigned int h,
                uint32_t value_mask, void* value_list);
void entis_term();

void entis_set_font(const char* font_name, uint16_t size);
void entis_set_font_size(uint16_t size);
void entis_flush();

Display* entis_get_connection();
Window entis_get_window();
Pixmap entis_get_pixmap();
int entis_get_screen();
bool entis_font_check_background();
uint32_t entis_get_color(uint16_t index);

void entis_set_color(uint32_t color);
void entis_set_color_rgb(uint32_t r, uint32_t g, uint32_t b);
void entis_set_color_drgb(double r, double g, double b);
void entis_set_background(uint32_t color);
void entis_set_background_rgb(uint32_t r, uint32_t g, uint32_t b);
void entis_set_background_drgb(double r, double g, double b);
void entis_font_color(uint32_t color);
void entis_font_color_rgb(uint32_t r, uint32_t g, uint32_t b);
void entis_font_color_drgb(double r, double g, double b);
void entis_font_set_background(bool setting);
void entis_font_background(uint32_t color);
void entis_font_background_rgb(uint32_t r, uint32_t g, uint32_t b);
void entis_font_background_drgb(double r, double g, double b);

void entis_reload_pixmap(uint32_t w, uint32_t h);

void entis_copy_pixmap();
void entis_update();
void entis_clear();

EntisEvent entis_wait_event();
EntisEvent entis_poll_event();

EntisEvent entis_wait_event_type(uint32_t type);
EntisEvent entis_poll_event_type(uint32_t type);

entis_key_event entis_wait_key();
entis_key_event entis_poll_key();

entis_button_event entis_wait_button();
entis_button_event entis_poll_button();

void entis_clear_events();

void entis_erase(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void entis_point(uint16_t x, uint16_t y);
void entis_segment(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void entis_line(uint16_t* x, uint16_t* y, uint16_t n);
void entis_triangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                    uint16_t x2, uint16_t y2);
void entis_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void entis_arc(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
               uint16_t angle1, uint16_t angle2);
void entis_poly(uint16_t* x, uint16_t* y, uint16_t n);

void entis_fill_triangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                         uint16_t x2, uint16_t y2);
void entis_fill_rectangle(uint16_t x, uint16_t y, uint16_t width,
                          uint16_t height);
void entis_fill_arc(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                    uint16_t angle1, uint16_t angle2);
void entis_fill_poly(uint16_t* x, uint16_t* y, uint16_t n);

void entis_reg_poly(uint16_t x, uint16_t y, uint16_t radius_x,
                    uint16_t radius_y, uint16_t n, double offset);
void entis_fill_reg_poly(uint16_t x, uint16_t y, uint16_t radius_x,
                         uint16_t radius_y, uint16_t n, double offset);

void entis_circle(uint16_t x, uint16_t y, uint16_t radius);
void entis_fill_circle(uint16_t x, uint16_t y, uint16_t radius);

void entis_draw_string(uint16_t x, uint16_t y, const char* fmt, ...);
void entis_erase_string(uint16_t x, uint16_t y, const char* fmt, ...);
uint16_t entis_string_width(const char* fmt, ...);
uint16_t entis_string_height(const char* fmt, ...);

uint16_t entis_get_pixel_width();
uint16_t entis_get_pixel_height();
void entis_set_pixel_size(uint16_t width, uint16_t height);
void entis_set_pixel(uint16_t x, uint16_t y);
void entis_pixel_set_pixel(uint16_t x, uint16_t y);

void entis_pause(uint64_t seconds, uint64_t nanoseconds);

Button entis_button(char* text, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void entis_button_fg(Button* button, uint32_t normal, uint32_t hover);
void entis_button_bg(Button* button, uint32_t normal, uint32_t hover);
bool entis_handle_button(Button* button, EntisEvent event);
void entis_draw_button(Button button);
void entis_erase_button(Button button);

#endif  // ENTIS_ENTIS_H_
