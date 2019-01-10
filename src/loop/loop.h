#ifndef ENTIS_LOOP_CORE_H_
#define ENTIS_LOOP_CORE_H_

#include "../entis.h"

#define PI 3.14159265358979323846

void setup() __attribute((weak));
void draw() __attribute((weak));
void destroy() __attribute((weak));

extern uint32_t width, height;

void size(uint32_t w, uint32_t h);

void fill(uint32_t c);
void background(uint32_t c);

void circle(uint32_t w, uint32_t h, int32_t r);

#endif  // ENTIS_LOOP_CORE_H_
