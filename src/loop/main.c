#include <stdio.h>

#include "../entis.h"
#include "loop.h"

int main(int argc, char* argv[]) {
  entis_init(500, 500, ENTIS_XCB | ENTIS_TTF);
  entis_color_int(ENTIS_WHITE);
  entis_background_int(ENTIS_BLACK);
  entis_clear();
  if (setup) {
    setup();
  }
  while (entis_xcb_window_open()) {
    if(draw){
      draw();
    }
    entis_clear_events();
    /* entis_sleep(0.01); */
  }
  if(destroy){
    destroy();
  }
  entis_term();
  return 0;
}
