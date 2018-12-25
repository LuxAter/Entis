#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "entis.h"
#include "error.h"

void append(char* s, char c) {
  int len = strlen(s);
  s[len] = c;
  s[len + 1] = '\0';
}

int main(int argc, char* argv[]) {
  entis_init(500, 500, ENTIS_XCB | ENTIS_TTF);
  entis_clear();
  entis_color_int(0xff0000);
  entis_rectangle_fill(0,0,100,100);
  entis_color_int(0x00ff00);
  entis_rectangle_fill(0,100,100,100);
  uint32_t x[255];
  uint32_t y[255];
  uint32_t n = 0;
  while (true) {
    entis_button_event ev = entis_wait_button();
    if (ev.x <= 100 && ev.y <= 100) {
      break;
    } else if (ev.x <= 100 && ev.y <= 200) {
      printf("HI!\n");
      entis_poly_fill(x, y, n);
    } else {
      /* entis_color_int(rand() % 0xffffff); */
      entis_circle(ev.x, ev.y, 30);
      entis_point(ev.x, ev.y);
      x[n] = ev.x;
      y[n] = ev.y;
      n++;
    }
  }
  entis_term();
  return 0;
  /* entis_init("Entis", 500, 500, 0, NULL); */
  /* entis_set_background(ENTIS_BLACK); */
  /* entis_load_font("ps2p.ttf"); */
  /* entis_set_font_size(16, 0); */
  /* entis_clear(); */
  /* entis_set_font_color(ENTIS_WHITE); */
  /* entis_event event = entis_poll_event(); */
  /* uint16_t x = 0, y = 18; */
  /* char str[255]; */
  /* while (event.key.keycode != KEY_ESCAPE) { */
  /*   event = entis_wait_event(); */
  /*   while (event.type != ENTIS_NO_EVENT) { */
  /*     if (event.type == ENTIS_KEY_RELEASE) { */
  /*       if (event.key.keycode != KEY_ESCAPE) { */
  /*         append(str, event.key.keysym); */
  /*         entis_draw_text(x, y, str); */
  /*       } else { */
  /*         break; */
  /*       } */
  /*     } else if (event.type == ENTIS_BUTTON_RELEASE) { */
  /*       x = event.button.x; */
  /*       y = event.button.y; */
  /*       entis_set_font_size(20 * y / 500.0, 0); */
  /*       printf("(%dpt)\n", (int)(20 * y / 500.0)); */
  /*       entis_segment(x, y, x + 200, y); */
  /*       str[0] = 0; */
  /*     } */
  /*     event = entis_poll_event(); */
  /*   } */
  /*   entis_clear_events(); */
  /*   entis_sleep(0.1); */
  /* } */
  /* entis_term(); */
  /* return 0; */
}
