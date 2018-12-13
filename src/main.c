#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "entis.h"

void append(char* s, char c) {
        int len = strlen(s);
        s[len] = c;
        s[len+1] = '\0';
}

int main(int argc, char* argv[]) {
  entis_init("Entis", 500, 500, 0, NULL);
  entis_set_background(ENTIS_BLACK);
  entis_load_font("-*-fixed-bold-*-*-*-18-*-*-*-*-*-*-*");
  entis_clear();
  entis_event event = entis_poll_event();
  uint16_t x = 0, y = 0;
  char str[255];
  while (event.key.keycode != KEY_ESCAPE) {
    event = entis_wait_event();
    while (event.type != ENTIS_NO_EVENT) {
      if (event.type == ENTIS_KEY_RELEASE) {
        if (event.key.keycode != KEY_ESCAPE) {
          append(str, event.key.keysym);
          entis_draw_text(x, y, str);
        }else if(event.key.keycode== KEY_ESCAPE){
          break;
        }
      } else if (event.type == ENTIS_BUTTON_RELEASE) {
        x = event.button.x;
        y = event.button.y;
        str[0] = 0;
      }
      event = entis_poll_event();
    }
    entis_clear_events();
    entis_sleep(0.1);
  }
  entis_term();
  return 0;
}
