/* Copyright (C)
 * 2018 - Arden Rasmussen
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */
#ifndef ENTIS_ENTIS_H_
#define ENTIS_ENTIS_H_

/**
 * @file entis.h
 * @brief Core of entis graphics library
 * @author Arden Rasmussen
 * @version 2.0
 * @date 2018-12-28
 *
 * This file contains the core of the entis graphics library. All user calls
 * should be to one of these functions.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <dirent.h>
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
 * Core initialization and termination of entis and the respective sub modules.
 * @{ */

/**
 * @brief Initialization flags
 *
 * These flags determine which modules should be initialized.
 */
enum EntisInitFlags {
  ENTIS_XCB = 1 << 0,  //!< Initializes XCB interface
  ENTIS_TTF = 1 << 1   //!< Initializes FreeType2 Library
};

/**
 * @brief Initializes Entis Library
 *
 * This function constructs the internal framebuffer and initializes other
 * sub modules such as XCB, and FreeType2 as requested. This function must be
 * called before any other entis library function calls.
 *
 * @param [in] width The width of the framebuffer/window to construct.
 * @param [in] height The height of the framebuffer/window to construct.
 * @param [in] flags Additional initialization flags as declared by
 * `EntisInitFlags`.
 *
 * @see EntisInitFlags
 * @see entis_init_xcb
 * @see entis_init_ft
 *
 * @return `true` if initialization of the core module and all desired sub
 * modules is successful, `false` otherwise.
 */
bool entis_init(uint32_t width, uint32_t height, uint32_t flags);
/**
 * @brief Terminates Entis Library
 *
 * Terminates the entis library and all sub modules which have been initialized.
 * This frees the memory of the framebuffer, and so no entis function can be
 * called after the termination is complete.
 *
 * @see entis_term_xcb
 * @see entis_term_ft
 *
 * @return `true` if termination is successful, `false` otherwise.
 */
bool entis_term();

/**
 * @brief Initalize XCB module
 *
 * This function initializes the XCB module, and all requirements of the XCB
 * conneciton. This includes constructing a window, initializing a double buffer
 * for rendering, and generating graphics contexts for rendering the pixmap.
 * This function must be called before any input function, as all input function
 * are reliant on the XCB connection that is established.
 *
 * @param [in] title The title of the window to construct.
 *
 * @return `true` if XCB connection and window construction were successful,
 * `false` otherwise.
 */
bool entis_init_xcb(const char* title);
/**
 * @brief Terminates XCB module
 *
 * This function terminates the XCB connection, and destroys the constructed
 * window, pixmap, and graphics contexts.
 *
 * @return `true` if the XCB connection is successfully terminated, `false`
 * otherwise.
 */
bool entis_term_xcb();

/**
 * @brief Initalizes TTF module
 *
 * This function initializes the FreeType2 library that is neccesary for text
 * rendering.
 *
 * @return `true` if initialization is successful, `false` otherwise.
 */
bool entis_init_ft();
/**
 * @brief Terminates TTF module
 *
 * This function frees the memory stored in the font, and the FreeType2 library.
 *
 * @return `true`
 */
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
