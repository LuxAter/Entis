#ifndef ENTIS_ENTIS_H_
#define ENTIS_ENTIS_H_

#ifdef cplusplus__
extern "C" {
#endif

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef max
#define max(X, Y) (((X) > (Y)) ? (X) : (Y))
#endif
#ifndef min
#define min(X, Y) (((X) < (Y)) ? (X) : (Y))
#endif

enum EntisInitFlags {
  ENTIS_FB = 1 << 0,
  ENTIS_XCB = 1 << 1,
  ENTIS_TTF = 1 << 2,
  ENTIS_PNG = 1 << 3,
  ENTIS_ALL = 1 << 0 | 1 << 1 | 1 << 2 | 1 << 3,
};
enum EntisColors {
  ENTIS_BLACK = 0x000000,
  ENTIS_RED = 0xff0000,
  ENTIS_GREEN = 0x00ff00,
  ENTIS_BLUE = 0x0000ff,
  ENTIS_YELLOW = 0xffff00,
  ENTIS_MAGENTA = 0xff00ff,
  ENTIS_CYAN = 0x00ffff,
  ENTIS_WHITE = 0xffffff,
  ENTIS_DARK_RED = 0x800000,
  ENTIS_DARK_GREEN = 0x008000,
  ENTIS_DARK_BLUE = 0x000080,
  ENTIS_DARK_YELLOW = 0x808000,
  ENTIS_DARK_MAGENTA = 0x800080,
  ENTIS_DARK_CYAN = 0x008080,
  ENTIS_DARK_WHITE = 0x808080
};

bool entis_init(unsigned width, unsigned height, unsigned flags);
bool entis_term();
void entis_set_flag(unsigned flag);
bool entis_get_flag(unsigned flag);

bool entis_resize(unsigned width, unsigned height);
unsigned entis_width();
unsigned entis_height();
void entis_size(unsigned* width, unsigned* height);

void entis_sleep(double s);

void entis_color_int(uint32_t color);
void entis_color_rgb(uint8_t r, uint8_t g, uint8_t b);
void entis_color_drgb(double r, double g, double b);
void entis_background_int(uint32_t color);
void entis_background_rgb(uint8_t r, uint8_t g, uint8_t b);
void entis_background_drgb(double r, double g, double b);

bool entis_write_bmp(const char* file_name);

#ifdef cplusplus__
}
#endif

#ifdef cplusplus__
namespace entis {
enum InitFlags {
  FB = ENTIS_FB,
  XCB = ENTIS_XCB,
  TTF = ENTIS_TTF,
  PNG = ENTIS_PNG,
  ALL = ENTIS_ALL,
};
enum Colors {
  BLACK = 0x000000,
  RED = 0xff0000,
  GREEN = 0x00ff00,
  BLUE = 0x0000ff,
  YELLOW = 0xffff00,
  MAGENTA = 0xff00ff,
  CYAN = 0x00ffff,
  WHITE = 0xffffff,
  DARK_RED = 0x800000,
  DARK_GREEN = 0x008000,
  DARK_BLUE = 0x000080,
  DARK_YELLOW = 0x808000,
  DARK_MAGENTA = 0x800080,
  DARK_CYAN = 0x008080,
  DARK_WHITE = 0x808080
};
inline bool init(unsigned width, unsigned height, unsigned flags = ALL) {
  return entis_init(width, height, flags);
}
inline bool term() { return entis_term(); }
inline void color(uint32_t color) { entis_color_int(color); }
inline void color(uint8_t r, uint8_t g, uint8_t b) { entis_color_rgb(r, g, b); }
inline void color(double r, double g, double b) { entis_color_drgb(r, g, b); }
inline void background(uint32_t color) { entis_background_int(color); }
inline void background(uint8_t r, uint8_t g, uint8_t b) {
  entis_background_rgb(r, g, b);
}
inline void background(double r, double g, double b) {
  entis_background_drgb(r, g, b);
}
}  // namespace entis
#endif

#endif  // ENTIS_ENTIS_H_
