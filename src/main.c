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
  EntisEvent event = entis_event_wait_event();
  while (event.type != ENTIS_BUTTON_RELEASE) {
    printf("TYPE: %d\n", event.type);
    event = entis_event_wait_event();
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
