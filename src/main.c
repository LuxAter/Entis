#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "loop/loop.h"

float diameter; 
float angle = 0;

void setup() {
  size(640, 360);
  diameter = 360- 10;
  /* noStroke(); */
  fill(0xFFFA00);
}

void draw() {
  
  background(0);

  float d1 = 10 + (sin(angle) * diameter/2) + diameter/2;
  float d2 = 10 + (sin(angle + PI/2) * diameter/2) + diameter/2;
  float d3 = 10 + (sin(angle + PI) * diameter/2) + diameter/2;
  
  circle(0, height/2, d1);
  circle(width/2, height/2, d2);
  circle(width, height/2, d3);
  
  angle += 0.02;
}
/*  */
/* int main(int argc, char* argv[]) { */
/*   entis_init(500, 500, ENTIS_XCB | ENTIS_TTF); */
/*   entis_color_int(ENTIS_WHITE); */
/*   entis_background_int(ENTIS_BLACK); */
/*   entis_clear(); */
/*   bool draw = false; */
/*   while(entis_xcb_window_open()){ */
/*     entis_event ev = entis_poll_event(); */
/*     while(ev.type != ENTIS_NO_EVENT){ */
/*       if(ev.type == ENTIS_MOTION_NOTIFY){ */
/*         if(draw) entis_circle_fill(ev.motion.x, ev.motion.y, 5); */
/*       }else if(ev.type == ENTIS_BUTTON_PRESS){ */
/*         draw = true; */
/*         entis_circle_fill(ev.motion.x, ev.motion.y, 5); */
/*       }else if(ev.type == ENTIS_BUTTON_RELEASE){ */
/*         draw = false; */
/*         entis_circle_fill(ev.motion.x, ev.motion.y, 5); */
/*       }else if(ev.type == ENTIS_KEY_PRESS && ev.key.keycode == KEY_S){ */
/*         entis_write_png("save.png"); */
/*       }else if(ev.type == ENTIS_KEY_PRESS && ev.key.keycode == KEY_L){ */
/*         entis_read_png("test.png"); */
/*       }else if(ev.type == ENTIS_KEY_PRESS && ev.key.keycode == KEY_Q){ */
/*         entis_xcb_close_window(); */
/*       }else if(ev.type == ENTIS_KEY_PRESS && ev.key.keycode == KEY_R){ */
/*         entis_color_rgb(rand() % 256, rand() % 256, rand() % 256); */
/*       } */
/*       ev = entis_poll_event(); */
/*     } */
/*     entis_sleep(0.05); */
/*   } */
/*   entis_term(); */
/* } */
