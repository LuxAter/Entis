#ifndef ENTIS_FRAME_BUFFER_H_
#define ENTIS_FRAME_BUFFER_H_

#ifdef cplusplus__
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

bool entis_fb_init(unsigned width, unsigned height);
bool entis_fb_term();

bool entis_fb_resize(unsigned width, unsigned height);
unsigned entis_fb_width();
unsigned entis_fb_height();
void entis_fb_size(unsigned* width, unsigned* height);

void entis_fb_color(uint32_t color);
void entis_fb_background(uint32_t color);

void entis_fb_pixel(unsigned x, unsigned y);
void entis_fb_ipixel(int x, int y);
void entis_fb_dpixel(double x, double y);

void entis_fb_line_low(unsigned x0, unsigned y0, unsigned x1, unsigned y1);
void entis_fb_line_high(unsigned x0, unsigned y0, unsigned x1, unsigned y1);
void entis_fb_line(unsigned x0, unsigned y0, unsigned x1, unsigned y1);
void entis_fb_iline(int x0, int y0, int x1, int y1);
void entis_fb_dline(double x0, double y0, double x1, double y1);

void entis_fb_tri(unsigned x0, unsigned y0, unsigned x1, unsigned y1,
                  unsigned x2, unsigned y2);
void entis_fb_itri(int x0, int y0, int x1, int y1, int x2, int y2);
void entis_fb_dtri(double x0, double y0, double x1, double y1, double x2,
                   double y2);

void entis_fb_rect(unsigned x, unsigned y, unsigned w, unsigned h);
void entis_fb_irect(int x, int y, int w, int h);
void entis_fb_drect(double x, double y, double w, double h);

void entis_fb_polygon(unsigned cx, unsigned cy, unsigned sides);
void entis_fb_ipolygon(int cx, int cy, int sides);
void entis_fb_dpolygon(double cx, double cy, double sides);

void entis_fb_circle(unsigned cx, unsigned cy, unsigned r);
void entis_fb_icircle(int cx, int cy, int r);
void entis_fb_dcircle(double cx, double cy, double r);

void entis_fb_poly(unsigned* x, unsigned* y, unsigned n);
void entis_fb_ipoly(int* x, int* y, unsigned n);
void entis_fb_dpoly(double* x, double* y, unsigned n);

bool entis_fb_write_bmp(const char* file_name);

#ifdef cplusplus__
}
#endif

#endif  // ENTIS_FRAME_BUFFER_H_
