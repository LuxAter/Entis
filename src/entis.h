#ifndef ENTIS_ENTIS_H_
#define ENTIS_ENTIS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <xcb/xcb.h>
#include <stdbool.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void entis_init(const char* title, uint32_t w, uint32_t h);
void entis_term();

bool entis_connection_valid();
void entis_flush();

void entis_color_int(uint32_t color);
void entis_color_rgb(uint8_t r, uint8_t g, uint8_t b);
void entis_color_frgb(double r, double g, double b);
void entis_background_int(uint32_t color);
void entis_background_rgb(uint8_t r, uint8_t g, uint8_t b);
void entis_background_frgb(double r, double g, double b);

#ifdef __cplusplus
}
#endif

#endif  // ENTIS_ENTIS_H_
