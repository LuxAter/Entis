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
  entis_set_font("clean", 30);
  entis_set_pixel_size(50, 50);
  entis_clear();
  Button button = entis_button("Hello World", 50, 50, 300, 100);
  entis_button_bg(&button, 0xFF00FF, 0x00FFFF);
  entis_button_fg(&button, 0x0000FF, 0xFF0000);
    entis_draw_button(button);
  while (true) {
    EntisEvent event = entis_wait_event();
    if(entis_handle_button(&button, event)){
      entis_erase_button(button);
    }
    if (event.type == ENTIS_KEY_PRESS && event.key.keysym == (int)'q') {
      break;
    }
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
