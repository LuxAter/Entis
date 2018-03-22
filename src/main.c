#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "entis.h"
/* #include "window.h" */
/* #include "color.h" */
/* #include "primitives.h" */

int main(int argc, char* argv[]) {
  entis_init("Entis", 500, 500, 0, NULL);
  entis_set_background(ENTIS_BLACK);
  entis_set_color(ENTIS_DARK_CYAN);
  entis_set_pixel_size(50, 50);
  entis_clear();
  for (int i = 0; i < entis_get_pixel_width(); i++) {
    for (int j = 0; j < entis_get_pixel_height(); j++) {
      /* printf("(%d/%d, %d/%d)\n", i, entis_get_pixel_width(), j,
       * entis_get_pixel_height()); */
      if (i % 2 == 0 && j % 2 == 1) {
        entis_set_pixel(i, j);
      } else if (i % 2 == 1 && j % 2 == 0) {
        entis_set_pixel(i, j);
      }
    }
  }
  entis_set_color(ENTIS_MAGENTA);
  /* entis_reg_poly(200, 200, 50, 50, 100, 0); */
  entis_key_event event = entis_wait_key();
  while (event.keycode != KEY_ESCAPE) {
    printf("KEYCODE: %d SYM: %c\n", event.keycode, event.keysym);
    /* entis_point(event.x, event.y); */
    /* entis_pixel_set_pixel(event.x, event.y); */
    /* entis_update(); */
    event = entis_wait_key();
  }
  /* xcb_generic_event_t* event; */
  /* while(true){ */
  /*   event = entis_wait_event(); */
  /*   if((event->response_type & ~0x80) != 3){ */
  /*     break; */
  /*   } */
  /*   free(event); */
  /* } */
  entis_term();
  return 0;
}
