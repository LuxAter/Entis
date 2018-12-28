#ifndef ENTIS_ENTIS_H_
#define ENTIS_ENTIS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>

#include <ft2build.h>
#include <xcb/xcb.h>
#include FT_FREETYPE_H

#include "error.h"
#include "event.h"
#include "key_codes.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define max(X, Y) (((X) > (Y)) ? (X) : (Y))
#define min(X, Y) (((X) < (Y)) ? (X) : (Y))
#ifndef max
#define max(a, b)           \
  ({                        \
    __typeof__(a) _a = (a); \
    __typeof__(b) _b = (b); \
    _a > _b ? _a : _b;      \
  })
#endif
#ifndef min
#define min(a, b)           \
  ({                        \
    __typeof__(a) _a = (a); \
    __typeof__(b) _b = (b); \
    _a < _b ? _a : _b;      \
  })
#endif
#define ENTIS_D30 0.52359877559829881565
#define ENTIS_D45 0.78539816339744827899
#define ENTIS_D60 1.04719755119659763131
#define ENTIS_D90 1.57079632679489655799

#define ENTIS_BLACK 0x000000
#define ENTIS_RED 0xFF0000
#define ENTIS_GREEN 0x00FF00
#define ENTIS_BLUE 0x0000FF
#define ENTIS_YELLOW 0xFFFF00
#define ENTIS_MAGENTA 0xFF00FF
#define ENTIS_CYAN 0x00FFFF
#define ENTIS_WHITE 0xFFFFFF
#define ENTIS_DARK_RED 0x800000
#define ENTIS_DARK_GREEN 0x008000
#define ENTIS_DARK_BLUE 0x000080
#define ENTIS_DARK_YELLOW 0x808000
#define ENTIS_DARK_MAGENTA 0x800080
#define ENTIS_DARK_CYAN 0x008080
#define ENTIS_DARK_WHITE 0x808080

/**
 * @defgroup Core
 * Core initialization and termination of entis and the repsective submodules.
 * @{ */

enum EntisInitFlags { ENTIS_XCB = 1 << 0, ENTIS_TTF = 1 << 1 };

bool entis_init(uint32_t width, uint32_t height, uint32_t flags);
bool entis_term();

bool entis_init_xcb(const char* title);
bool entis_term_xcb();

bool entis_init_ft();
bool entis_term_ft();

void entis_resize(uint32_t width, uint32_t height);
uint32_t entis_width();
uint32_t entis_height();
void entis_size(uint32_t* width, uint32_t* height);
void entis_clear();

/**  @} */

bool entis_xcb();
void entis_xcb_flush();
xcb_connection_t* entis_xcb_connection();

void entis_xcb_reload_pixmap();
void entis_xcb_copy_pixmap();
void entis_xcb_update();
void entis_xcb_clear();

entis_event entis_wait_event();
entis_event entis_poll_event();
entis_event entis_wait_event_type(uint32_t type);
entis_event entis_poll_event_type(uint32_t type);

entis_key_event entis_wait_key();
entis_key_event entis_poll_key();

entis_button_event entis_wait_button();
entis_button_event entis_poll_button();

void entis_clear_events();

void entis_color_int(uint32_t color);
void entis_color_rgb(uint8_t r, uint8_t g, uint8_t b);
void entis_color_drgb(double r, double g, double b);
void entis_background_int(uint32_t color);
void entis_background_rgb(uint8_t r, uint8_t g, uint8_t b);
void entis_background_drgb(double r, double g, double b);

// TODO: Add searching current directory, and then search font diretorys, with a waring?
bool entis_load_font(const char* font_name);
void entis_font_size(uint16_t pt, uint32_t dpi);
void entis_font_px(uint16_t px);
void entis_font_offset();

void entis_point(uint32_t x, uint32_t y);
void entis_points(uint32_t* x, uint32_t* y, uint32_t n);
void entis_line(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1);
void entis_triangle(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1,
                    uint32_t x2, uint32_t y2);
void entis_rectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
void entis_circle(uint32_t cx, uint32_t cy, uint32_t r);
void entis_poly(uint32_t* x, uint32_t* y, uint32_t n);

void entis_triangle_fill(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1,
                         uint32_t x2, uint32_t y2);
void entis_rectangle_fill(uint32_t x, uint32_t y, uint32_t width,
                          uint32_t height);
void entis_circle_fill(uint32_t cx, uint32_t cy, int32_t r);
void entis_poly_fill(uint32_t* x, uint32_t* y, uint32_t n);

// TODO: Figure out the proper shifting to align baseline for all letters! I can do it!!
void entis_text(uint32_t x, uint32_t y, const char* str);
void entis_wtext(uint32_t x, uint32_t y, const wchar_t* str);
void entis_btext(uint32_t x, uint32_t y, const char* str);
void entis_wbtext(uint32_t x, uint32_t y, const wchar_t* str);
void entis_mtext(uint32_t x, uint32_t y, const char* str);
void entis_wmtext(uint32_t x, uint32_t y, const wchar_t* str);
void entis_bmtext(uint32_t x, uint32_t y, const char* str);
void entis_wbmtext(uint32_t x, uint32_t y, const wchar_t* str);

void entis_text_size(const char* str, uint32_t* width, uint32_t* height);
uint32_t entis_text_width(const char* str);
uint32_t entis_text_height(const char* str);
void entis_char_size(char ch, uint32_t* width, uint32_t* height);
uint32_t entis_char_width(char ch);
uint32_t entis_char_height(char ch);
void entis_glyph_size(uint32_t* width, uint32_t* height);
uint32_t entis_glyph_width();
uint32_t entis_glyph_height();

uint32_t entis_get_color(uint32_t x, uint32_t y);

#ifdef __cplusplus
}
#endif

#endif  // ENTIS_ENTIS_H_
