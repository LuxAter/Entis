#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "entis.h"

/* int main() { */
/*   entis_init("Entis", 500, 500, 0, NULL); */
/*  */
/*   entis_set_color(ENTIS_BLUE); */
/*   entis_fill_circle(400, 400, 200); */
/*   entis_clear_events(); */
/*   #<{(| entis_pause(0, 1e5); |)}># */
/*   entis_pause(1, 0); */
/*   #<{(| entis_wait_key(); |)}># */
/*   #<{(| entis_fill_circle(400, 400, 200); |)}># */
/*   #<{(| entis_wait_key(); |)}># */
/*  */
/*   entis_term(); */
/*  */
/*   return 0; */
/* } */

int main(int argc, char* argv[]) {
  entis_init("Entis", 500, 500, 0, NULL);
  entis_set_background(ENTIS_BLACK);
  entis_set_color(ENTIS_WHITE);
  entis_font_color(ENTIS_WHITE);
  entis_set_font("clean", 20);
  /* entis_set_font_size(50); */
  entis_set_pixel_size(50, 50);
  entis_clear();
  for (int i = 0; i < entis_get_pixel_width(); i++) {
    for (int j = 0; j < entis_get_pixel_height(); j++) {
      /* printf("(%d/%d, %d/%d)\n", i, entis_get_pixel_width(), j,
       * entis_get_pixel_height()); */
      if (i % 2 == 0 && j % 2 == 1) {
        /* entis_set_pixel(i, j); */
      } else if (i % 2 == 1 && j % 2 == 0) {
        /* entis_set_pixel(i, j); */
      }
    }
  }
  entis_draw_string(50, 50, "Clicks: %d", 0);
  entis_set_color(ENTIS_MAGENTA);
  XButtonEvent event = entis_wait_button();
  int count = 0;
  while (event.x > 100 || event.y > 100) {
    entis_erase_string(50, 50, "Clicks: %d", count);
    /* printf("DATA: %d STATE: %d\n", event.keycode, event.state); */
    /* entis_point(event.x, event.y); */
    entis_pixel_set_pixel(event.x, event.y);
    count++;
    entis_draw_string(50, 50, "Clicks: %d", count);
    /* entis_update(); */
    event = entis_wait_button();
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
