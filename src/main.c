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
  entis_load_font("scpnfc.ttf");
  char *str = "\uf303";
  /* str[1] = 0; */
  entis_clear();
  /* entis_font_size(24, 0); */
  entis_font_px(400);
  while (true) {
    entis_key_event ev = entis_wait_key();
    if (ev.keycode == KEY_ESCAPE) {
      break;
    } else {
      /* str[0] = ev.keysym; */
      /* str[0] = 0xf303; */
      entis_clear();
      entis_mtext(50, 50, str);
      uint32_t cw = 0, ch = 0;
      uint32_t gw = 0, gh = 0;
      entis_char_size(str[0], &cw, &ch);
      entis_glyph_size(&gw, &gh);
      entis_color_int(ENTIS_BLUE);
      entis_rectangle(50, 50, gw, gh);
      entis_color_int(ENTIS_GREEN);
      entis_rectangle(50,50, cw, ch);
      entis_color_int(ENTIS_WHITE);
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
