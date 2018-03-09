#ifndef ENTIS_ENTIS_H_
#define ENTIS_ENTIS_H_

#include <stdbool.h>
#include <xcb/xcb.h>

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

void entis_init(const char* title, unsigned int w, unsigned int h,
                uint32_t value_mask, void* value_list);
void entis_term();

bool entis_connection_valid();
void entis_flush();

xcb_connection_t* entis_get_connection();
xcb_screen_t* entis_get_screen();
xcb_window_t entis_get_window();
xcb_pixmap_t entis_get_pixmap();
xcb_gcontext_t entis_get_gcontext();

void entis_set_color(uint32_t color);
void entis_set_color_rgb(uint32_t r, uint32_t g, uint32_t b);
void entis_set_color_drgb(double r, double g, double b);
void entis_set_background(uint32_t color);
void entis_set_background_rgb(uint32_t r, uint32_t g, uint32_t b);
void entis_set_background_drgb(double r, double g, double b);

void entis_reload_pixmap(uint32_t w, uint32_t h);

void entis_copy_pixmap();
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

#endif  // ENTIS_ENTIS_H_
