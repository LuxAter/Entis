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
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <wchar.h>

#include <ft2build.h>
#include <xcb/xcb.h>
#include FT_FREETYPE_H

#include "epng.h"
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
 * connection. This includes constructing a window, initializing a double buffer
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
 * @brief Initializes TTF module
 *
 * This function initializes the FreeType2 library that is necessary for text
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

/**
 * @brief Resizes the entis window
 *
 * Resizes the internal framebuffer and if XCB is enabled, it will also resize
 * the XCB window. This function is called if the window is automatically
 * resized.
 *
 * @param [in] width New width of the framebuffer/window.
 * @param [in] height New height of the framebuffer/window.
 */
void entis_resize(uint32_t width, uint32_t height);
/**
 * @brief Gets the width of the framebuffer/window.
 *
 * @return The width of the framebuffer.
 */
uint32_t entis_width();
/**
 * @brief Gets the height of the framebuffer/window.
 *
 * @return The height of the framebuffer.
 */
uint32_t entis_height();
/**
 * @brief Gets the size of the current framebuffer/window.
 *
 * @param [out] width Stores the width of the framebuffer.
 * @param [out] height Stores the height of the framebuffer.
 */
void entis_size(uint32_t* width, uint32_t* height);
/**
 * @brief Clears the framebuffer/window.
 *
 * Clears the internal framebuffer and XCB window, setting every pixel to the
 * background color.
 */
void entis_clear();

/**
 * @brief Retrieves the internal framebuffer.
 *
 * This returns the internal framebuffer used by libentis.
 *
 * @warning This should not be used by the user. It is only intended to be used
 * for reading and writing images. Avoid use of this function.
 *
 * @return pointer to pixel data of the internal frame buffer.
 */
uint32_t** entis_get_frame_buffer();

/**
 * @brief Pauses program for given duration
 *
 * Pauses the program the given duration of seconds, the accuracy is up to
 * nanoseconds.
 *
 * @param [in] s Seconds to sleep for.
 */
void entis_sleep(double s);

/**  @} */

/**
 * @defgroup XCB
 * XCB connection, and window interface.
 * @{ */

/**
 * @brief Checks if XCB module has been enabled.
 *
 * @return `true` if XCB has been initialized, `false` otherwise.
 */
bool entis_xcb();
/**
 * @brief XCB flush
 *
 * Flushes all XCB connection calls to the X server. This must be done to render
 * everything. In most cases this is automatically done when events are handled.
 */
void entis_xcb_flush();
/**
 * @brief Retrieves XCB connection
 *
 * This retrieves the internal XCB connection.
 *
 * @warning This should not be used by the user. It is only intended to be used
 * for user input prompts. Avoid use of this function.
 *
 * @return XCB connection pointer.
 */
xcb_connection_t* entis_xcb_connection();

/**
 * @brief Resizes the XCB window
 *
 * This function resizes the XCB window to the desired size, this should only be
 * called by `entis_resize`, as that will also resize the framebuffer.
 */
void entis_xcb_resize_window();

/**
 * @brief Toggles XCB window fullscreen.
 *
 * This function toggles the XCB window from being fullscreen, to normalized.
 * This will automattiacly resize the window and framebuffer accordingly.
 */
void entis_xcb_toggle_fullscreen();
/**
 * @brief Resizes the XCB pixmap
 *
 * This function should only be called after the window has been resized. It
 * changes the internal XCB pixmap to match the current size of the window.
 */
void entis_xcb_reload_pixmap();
/**
 * @brief Copies XCB pixmap to XCB window
 *
 * This function must be called to render the contents of the internal pixmap to
 * the visual window.
 */
void entis_xcb_copy_pixmap();
/**
 * @brief Flushes XCB requests, and copies the internal pixmap.
 *
 * This function just called `entis_xcb_flush` followed by
 * `entis_xcb_copy_pixmap`.
 */
void entis_xcb_update();
/**
 * @brief Clears XCB window
 *
 * Fills the XCB pixmap and window with the current background color.
 */
void entis_xcb_clear();
/**
 * @brief Checks if XCB window is still open.
 *
 * @return `true` if the XCB window is open, `false` otherwise.
 */
bool entis_xcb_window_open();
/**
 * @brief Sets the XCB window to close.
 *
 * This does not actually close the XCB window, just changes the state such that
 * `entis_xcb_window_open` will return `false`.
 */
void entis_xcb_close_window();

/**  @} */

/**
 * @defgroup Event
 * Event handling functions
 * @{ */

/**
 * @brief Waits for user input event
 *
 * This waits for any user input event.
 *
 * @return `entis_event` containing the corresponding user event that occurred.
 */
entis_event entis_wait_event();
/**
 * @brief Returns queued user event if it exists.
 *
 * This checks for any events that are waiting in the event queue, and returns
 * the oldest event, if it exists. Otherwise no event is returned.
 *
 * @return `entis_event` containing user event data.
 */
entis_event entis_poll_event();
/**
 * @brief Waits for user event of specified type.
 *
 * Waits for a user event of a given type. This function ignores all other user
 * events until one of the provided type is given.
 *
 * @param type Type of event to wait for.
 *
 * @return `entis_event` containing user event data.
 */
entis_event entis_wait_event_type(uint32_t type);
/**
 * @brief Returns queued user event of given type.
 *
 * Checks if an event of the provided type has been queued, ignoring all
 * previous queued events. If an event of that type exists, then it is returned.
 * Otherwise no event is returned.
 *
 * @param type Type of event to look for.
 *
 * @return `entis_event` containing the user event data.
 */
entis_event entis_poll_event_type(uint32_t type);

/**
 * @brief Waits for a key event to occurs.
 *
 * Waits specifically for a key event to occurs, then returns that event.
 *
 * @return `entis_key_event` containing key event data.
 */
entis_key_event entis_wait_key();
/**
 * @brief Checks queued events for a key event.
 *
 * Polls for a key event.
 *
 * @return `entis_key_event` containing key event data.
 */
entis_key_event entis_poll_key();

/**
 * @brief Checks if a key is pressed.
 *
 * Checks is a provided key is currently pressed down. This is useful as the XCB
 * key press events will only occurs after the system settings for key repeat
 * delay. This method allows for users to access the state of any key at any
 * time.
 *
 * @param keysym Keysym to retrieve the state of.
 *
 * @return `true` if the key is currently pressed, `false` otherwise.
 */
bool entis_key_state(uint16_t keysym);

/**
 * @brief Waits for a button event to occur.
 *
 * Waits specifically for a button event to occur, then returns that event.
 *
 * @return `entis_button_event` containing button event data.
 */
entis_button_event entis_wait_button();
/**
 * @brief Checks queued events for a button event.
 *
 * Polls for a button event.
 *
 * @return `entis_button_event` containing button event data.
 */
entis_button_event entis_poll_button();

/**
 * @brief Clears event queue.
 *
 * Clears the internal event queue, processing the necessary internal events.
 */
void entis_clear_events();

/**  @} */

/**
 * @defgroup TTF
 * True type font loading, pixel sizing
 * @{ */

/**
 * @brief Loads a `.ttf` font
 *
 * Loads a true type font. This requires the TTF modules to be initialized. This
 * is also required for any later text rendering.
 *
 * @param font_name File path of true type file.
 *
 * @todo Implement smart font finding, searching the systems default font
 * directories.
 *
 * @return `true` if the font was loaded and `false` otherwise.
 */
bool entis_load_font(const char* font_name);
/**
 * @brief Sets the font size
 *
 * Sets the font size in `pt`. Note that the size in pixels from the `pt` is
 * dependent on the dots per inch of the screen.
 *
 * @param pt Font size.
 * @param dpi Dots per inch of display (`0` uses default of `75`).
 */
void entis_font_size(uint16_t pt, uint32_t dpi);
/**
 * @brief Sets the font size
 *
 * Sets the font size in terms of pixels. This sets the height of the glyphs.
 *
 * @param px Pixels for the Y axis of the glyphs.
 */
void entis_font_px(uint16_t px);
/**
 * @brief Determines the base line of font.
 *
 * This is an internal function that is called whenever the settings of a font
 * have been changed. This determine at what `y` value is the baseline of the
 * font. Thus allowing for the characters to be properly aligned when printing.
 */
void entis_font_offset();

/**  @} */

/**
 * @defgroup Rendering
 * Basic color control, and primitive rendering
 * @{ */

/**
 * @brief Sets the current color.
 *
 * Sets the current drawing color given a 32 bit(000000-FFFFFF) unsigned
 * integer.
 *
 * @param color Color to set to.
 */
void entis_color_int(uint32_t color);
/**
 * @brief Sets the current color.
 *
 * Sets the current drawing color given three 8 bit(0-255)(0-FF) unsigned
 * integer values for red, green, and blue.
 *
 * @param r Red component of the color.
 * @param g Green component of the color.
 * @param b Blue component of the color.
 */
void entis_color_rgb(uint8_t r, uint8_t g, uint8_t b);
/**
 * @brief Sets the current color.
 *
 * Sets the current drawing color given three double(0.0-1.0) values for red,
 * green, and blue.
 *
 * @param r Red component of the color.
 * @param g Green component of the color.
 * @param b Blue component of the color.
 */
void entis_color_drgb(double r, double g, double b);
/**
 * @brief Sets the background color.
 *
 * Sets the current background color given a 32 bit(000000-FFFFFF) unsigned
 * integer.
 *
 * @param color Color to set to.
 */
void entis_background_int(uint32_t color);
/**
 * @brief Sets the background color.
 *
 * Sets the current background color given three 8 bit(0-255)(0-FF) unsigned
 * integer values for red, green, and blue.
 *
 * @param r Red component of the color.
 * @param g Green component of the color.
 * @param b Blue component of the color.
 */
void entis_background_rgb(uint8_t r, uint8_t g, uint8_t b);
/**
 * @brief Sets the background color.
 *
 * Sets the current background color given three double(0.0-1.0) values for red,
 * green, and blue.
 *
 * @param r Red component of the color.
 * @param g Green component of the color.
 * @param b Blue component of the color.
 */
void entis_background_drgb(double r, double g, double b);

/**
 * @brief Retrieve the color of a given pixel from the internal buffer.
 *
 * Retrieves the color of the internal buffer at a provided point in the grid.
 *
 * @param x X location of the pixel to sample.
 * @param y Y location of the pixel to sample.
 *
 * @return 32 bit(000000-FFFFFF) value representing the color at the requested
 * pixel.
 */
uint32_t entis_get_color(uint32_t x, uint32_t y);

/**
 * @brief Draws a single pixel.
 *
 * Sets a pixel to the current color.
 *
 * @param x X coordinate of the pixel.
 * @param y Y coordinate of the pixel.
 */
void entis_point(uint32_t x, uint32_t y);
/**
 * @brief Draws a set of pixels.
 *
 * Sets a given list of `(x,y)` values to the current color.
 *
 * @param x X coordinates of the pixels.
 * @param y Y coordinates of the pixels.
 * @param n Number of pixels in the list.
 */
void entis_points(uint32_t* x, uint32_t* y, uint32_t n);
/**
 * @brief Draws a line between two points.
 *
 * Draws a line between two provided points.
 *
 * @param x0 X coordinate of the starting point.
 * @param y0 Y coordinate of the starting point.
 * @param x1 X coordinate of the ending point.
 * @param y1 Y coordinate of the ending point.
 */
void entis_line(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1);
/**
 * @brief Draws a horizontal line between two x values.
 *
 * Draws a horizontal line between two X values at a provided Y position.
 *
 * @param x0 X coordinate of starting point.
 * @param x1 X coordinate of entding point.
 * @param y Y coordinate of both ponts.
 */
void entis_horizontal_line(int32_t x0, int32_t x1, int32_t y);
/**
 * @brief Draws a triangle between three points.
 *
 * Draws a triangle between three provided points. This is the same as drawing
 * three lines between the points.
 *
 * @param x0 X of first point.
 * @param y0 Y of first point.
 * @param x1 X of second point.
 * @param y1 Y of second point.
 * @param x2 X of third point.
 * @param y2 Y of third point.
 */
void entis_triangle(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1,
                    uint32_t x2, uint32_t y2);
/**
 * @brief Draws a rectangle.
 *
 * Draws a rectangle from the provided point, with given width and height.
 *
 * @param x X position of the top left corner of the rectangle.
 * @param y Y position of the top left corner of the rectangle.
 * @param width Width of the rectangle.
 * @param height Height of the rectangle.
 */
void entis_rectangle(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
/**
 * @brief Draws a circle.
 *
 * Draws a circle centered at a provided point, with given radius in pixels.
 *
 * @param cx X position of the center of the circle.
 * @param cy Y position of the center of the circle.
 * @param r Radius of the circle in pixels.
 */
void entis_circle(uint32_t cx, uint32_t cy, uint32_t r);
/**
 * @brief Draws an arbitrary polygon
 *
 * Draws an arbitrary closed polygon, given coordinates of points and a number
 * of points.
 *
 * @param x X positions of points.
 * @param y Y positions of points.
 * @param n Number of points in polygon.
 */
void entis_poly(uint32_t* x, uint32_t* y, uint32_t n);

/**
 * @brief Draws a filled triangle between three points.
 *
 * Draws a filled triangle between three provided points. This is the same as
 * drawing three lines between the points.
 *
 * @param x0 X of first point.
 * @param y0 Y of first point.
 * @param x1 X of second point.
 * @param y1 Y of second point.
 * @param x2 X of third point.
 * @param y2 Y of third point.
 */
void entis_triangle_fill(uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1,
                         uint32_t x2, uint32_t y2);
/**
 * @brief Draws a filled rectangle.
 *
 * Draws a filled rectangle from the provided point, with given width and
 * height.
 *
 * @param x X position of the top left corner of the rectangle.
 * @param y Y position of the top left corner of the rectangle.
 * @param width Width of the rectangle.
 * @param height Height of the rectangle.
 */
void entis_rectangle_fill(uint32_t x, uint32_t y, uint32_t width,
                          uint32_t height);
/**
 * @brief Draws a filled circle.
 *
 * Draws a filled circle centered at a provided point, with given radius in
 * pixels.
 *
 * @param cx X position of the center of the circle.
 * @param cy Y position of the center of the circle.
 * @param r Radius of the circle in pixels.
 */
void entis_circle_fill(uint32_t cx, uint32_t cy, uint32_t r);
/**
 * @brief Draws a filled arbitrary polygon
 *
 * Draws a filled arbitrary closed polygon, given coordinates of points and a
 * number of points.
 *
 * @param x X positions of points.
 * @param y Y positions of points.
 * @param n Number of points in polygon.
 */
void entis_poly_fill(uint32_t* x, uint32_t* y, uint32_t n);

/**  @} */

/**
 * @defgroup Text
 * Text rendering and sizing of glyphs.
 * @{ */

/**
 * @brief Renders text
 *
 * Renders text to the internal buffer. Note the TTF modules must be
 * initialized.
 *
 * @param x X position of the left of the text.
 * @param y Y position of the top of the text.
 * @param str String to render.
 */
void entis_text(uint32_t x, uint32_t y, const char* str);
/**
 * @brief Renders text
 *
 * Renders text to the internal buffer. Note the TTF modules must be
 * initialized.
 *
 * This alternative enables rendering of Unicode characters by the
 * use of a wide character.
 *
 * @param x X position of the left of the text.
 * @param y Y position of the top of the text.
 * @param str String to render.
 */
void entis_wtext(uint32_t x, uint32_t y, const wchar_t* str);
/**
 * @brief Renders text
 *
 * Renders text to the internal buffer. Note the TTF modules must be
 * initialized.
 *
 * @param x X position of the left of the text.
 * @param y Y position of the baseline of the text.
 * @param str String to render.
 */
void entis_btext(uint32_t x, uint32_t y, const char* str);
/**
 * @brief Renders text
 *
 * Renders text to the internal buffer. Note the TTF modules must be
 * initialized.
 *
 * This alternative enables rendering of Unicode characters by the
 * use of a wide character.
 *
 * @param x X position of the left of the text.
 * @param y Y position of the baseline of the text.
 * @param str String to render.
 */
void entis_wbtext(uint32_t x, uint32_t y, const wchar_t* str);
/**
 * @brief Renders text
 *
 * Renders text to the internal buffer. Note the TTF modules must be
 * initialized.
 *
 * This variant renders mono colored text. This is slightly faster,
 * but produces less desirable results.
 *
 * @param x X position of the left of the text.
 * @param y Y position of the top of the text.
 * @param str String to render.
 */
void entis_mtext(uint32_t x, uint32_t y, const char* str);
/**
 * @brief Renders text
 *
 * Renders text to the internal buffer. Note the TTF modules must be
 * initialized.
 *
 * This variant renders mono colored text. This is slightly faster,
 * but produces less desirable results.
 *
 * This alternative enables rendering of Unicode characters by the
 * use of a wide character.
 *
 * @param x X position of the left of the text.
 * @param y Y position of the top of the text.
 * @param str String to render.
 */
void entis_wmtext(uint32_t x, uint32_t y, const wchar_t* str);
/**
 * @brief Renders text
 *
 * Renders text to the internal buffer. Note the TTF modules must be
 * initialized.
 *
 * This variant renders mono colored text. This is slightly faster,
 * but produces less desirable results.
 *
 * @param x X position of the left of the text.
 * @param y Y position of the baseline of the text.
 * @param str String to render.
 */
void entis_bmtext(uint32_t x, uint32_t y, const char* str);
/**
 * @brief Renders text
 *
 * Renders text to the internal buffer. Note the TTF modules must be
 * initialized.
 *
 * This variant renders mono colored text. This is slightly faster,
 * but produces less desirable results.
 *
 * This alternative enables rendering of Unicode characters by the
 * use of a wide character.
 *
 * @param x X position of the left of the text.
 * @param y Y position of the baseline of the text.
 * @param str String to render.
 */
void entis_wbmtext(uint32_t x, uint32_t y, const wchar_t* str);

/**
 * @brief Determines size of text provided string.
 *
 * Determines the size that would be required to render a provided string of
 * characters.
 *
 * @param str String to measure.
 * @param [out] width Resulting width required for text.
 * @param [out] height Resulting height required for text.
 */
void entis_text_size(const char* str, uint32_t* width, uint32_t* height);
/**
 * @brief Determines the width required to render text.
 *
 * Determines the width that would be required to render the provided string of
 * characters.
 *
 * @param str String to measure.
 *
 * @return Width in pixels of rendered string.
 */
uint32_t entis_text_width(const char* str);
/**
 * @brief Determines the height required to render text.
 *
 * Determines the height that would be required to render the provided string of
 * characters.
 *
 * @param str String to measure.
 *
 * @return Height in pixels of rendered string.
 */
uint32_t entis_text_height(const char* str);
/**
 * @brief Determines the size of a character
 *
 * Determines the size of a provided character.
 *
 * @param ch Character to measure
 * @param [out] width Width of that character.
 * @param [out] height Height of that character.
 */
void entis_char_size(char ch, uint32_t* width, uint32_t* height);
/**
 * @brief Determines the width of a character.
 *
 * Determines the width of a provided character.
 *
 * @param ch Character to measure.
 *
 * @return Width of that character.
 */
uint32_t entis_char_width(char ch);
/**
 * @brief Determines the height of a character.
 *
 * Determines the height of a provided character.
 *
 * @param ch Character to measure.
 *
 * @return Height of that character.
 */
uint32_t entis_char_height(char ch);
/**
 * @brief Determines the size of a glyph.
 *
 * Determines the size of glyph with the current settings. Note that this is
 * constant until either the font is changed, or the size of the font is
 * changed.
 *
 * @param [out] width Width of the glyph.
 * @param [out] height Height of the glyph.
 */
void entis_glyph_size(uint32_t* width, uint32_t* height);
/**
 * @brief Determines the width of a glyph.
 *
 * Determines the width of glyph with the current settings. Note that this is
 * constant until either the font is changed, or the size of the font is
 * changed.
 *
 * @return Width of a glyph.
 */
uint32_t entis_glyph_width();
/**
 * @brief Determines the height of a glyph.
 *
 * Determines the height of glyph with the current settings. Note that this is
 * constant until either the font is changed, or the size of the font is
 * changed.
 *
 * @return Height of a glyph.
 */
uint32_t entis_glyph_height();

/**  @} */

/**
 * @defgroup Image
 * Image reading and writing
 * @{ */

/**
 * @brief Saves internal buffer to PNG file
 *
 * Writes the current state of the internal buffer to a PNG file, using libpng.a
 *
 * @param file_name Path of output file.
 */
void entis_write_png(const char* file_name);
/**
 * @brief Reads PNG file to internal buffer.
 *
 * Reads a valid PNG file to the internal buffer, and if XCB has been enabled to
 * the XCB window as well. The internal buffer and window are resized to the
 * size of the input PNG image.
 *
 * @note It takes a second or two for the XCB window to update, as the process
 * of copying the pixel data to the XCB window is very slow.
 *
 * @todo Improve the method currently used for copying to XCB window.
 *
 * @param file_name PNG file to read.
 */
void entis_read_png(const char* file_name);

/**  @} */

#ifdef __cplusplus
}
#endif

#endif  // ENTIS_ENTIS_H_
