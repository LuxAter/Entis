#ifndef ENTIS_ENTIS_H_
#define ENTIS_ENTIS_H_

#include <xcb/xcb.h>
#include <stdbool.h>

#include "event.h"

#define ENTIS_WHITE 0xFFFFFF
#define ENTIS_BLACK 0x000000

void entis_init(const char* title, unsigned int w, unsigned int h, uint32_t value_mask, void* value_list);
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

void entis_point(uint16_t x, uint16_t y);
void entis_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

EntisEvent entis_wait_event();
EntisEvent entis_poll_event();

EntisEvent entis_wait_event_type(uint32_t type);
EntisEvent entis_poll_event_type(uint32_t type);

entis_key_event entis_wait_key();
entis_key_event entis_poll_key();

entis_button_event entis_wait_button();
entis_button_event entis_poll_button();


void entis_clear_events();

#endif  // ENTIS_ENTIS_H_

