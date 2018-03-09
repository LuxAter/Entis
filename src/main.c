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
  entis_set_color(ENTIS_RED);
  entis_clear();
  for(int i = 0; i < 500; i += 10){
    entis_circle(i, i, i);
  }
  /* entis_reg_poly(200, 200, 50, 50, 100, 0); */
  entis_button_event event = entis_wait_event_type(ENTIS_BUTTON_RELEASE).button;
  while (event.x > 100 || event.y > 100) {
    entis_point(event.x, event.y);
    event = entis_wait_event_type(ENTIS_BUTTON_RELEASE).button;
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
