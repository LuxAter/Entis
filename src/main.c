#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "entis.h"
/* #include "window.h" */
/* #include "color.h" */
/* #include "primitives.h" */

int main(int argc, char* argv[]) {
  entis_init("Entis", 500, 500, 0, NULL);
  entis_set_background(0xFF00FF);
  entis_clear();
  entis_set_color(ENTIS_WHITE);
  entis_flush();
  entis_point(100, 100);
  entis_clear_events();
  entis_button_event event = entis_wait_button();
  while (event.x > 100 || event.y > 100) {
    printf("BUTTON:  x: %u, y: %u\n", event.x, event.y);
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
