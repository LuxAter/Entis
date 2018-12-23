#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "entis.h"

void append(char* s, char c) {
  int len = strlen(s);
  s[len] = c;
  s[len + 1] = '\0';
}

int main(int argc, char* argv[]) {
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
