#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "entis.h"

int main(int argc, char* argv[]) {
  entis_init("Entis", 500, 500, 0, NULL);
  entis_set_background(ENTIS_BLACK);
  entis_set_color(ENTIS_DARK_CYAN);
  entis_set_pixel_size(10, 10);
  entis_clear();
  for (int i = 0; i < entis_get_pixel_width(); i++) {
    for (int j = 0; j < entis_get_pixel_height(); j++) {
      if (i % 2 == 0 && j % 2 == 1) {
        entis_set_pixel(i, j);
      } else if (i % 2 == 1 && j % 2 == 0) {
        entis_set_pixel(i, j);
      }
    }
  }
  entis_set_color(ENTIS_MAGENTA);
  entis_event event = entis_wait_event();
  while (event.key.keycode != KEY_ESCAPE) {
    event = entis_wait_event();
    while (event.type != ENTIS_NO_EVENT) {
      if (event.type == ENTIS_KEY_RELEASE) {
        if(event.key.keycode== KEY_ESCAPE){
          break;
        }else{
          printf("%c\n", event.key.keysym);
        }
      } else if (event.type == ENTIS_BUTTON_RELEASE) {
        entis_pixel_set_pixel(event.button.x, event.button.y);
      }
    event = entis_poll_event();
    }
    entis_clear_events();
    entis_sleep(0.1);
  }
  entis_term();
  return 0;
}
