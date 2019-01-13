#ifndef ENTIS_LOOP_CORE_H_
#define ENTIS_LOOP_CORE_H_

#include "../entis.h"

#define PI 3.14159265358979323846
#define TWO_PI 6.28318530718

void setup() __attribute__((weak));
void create() __attribute__((weak));
void resize() __attribute__((weak));
void draw() __attribute__((weak));
void destroy() __attribute__((weak));
void terminate() __attribute__((weak));

void key_press(uint16_t ch) __attribute__((weak));
void button_press(uint8_t button) __attribute__((weak));

extern uint32_t width, height;
extern uint32_t mouseX, mouseY;

void size(uint32_t w, uint32_t h);

void fill(uint32_t c);
void background(uint32_t c);

void circle(uint32_t w, uint32_t h, int32_t r);

#endif  // ENTIS_LOOP_CORE_H_
