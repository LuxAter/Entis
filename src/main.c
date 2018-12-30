#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "entis.h"

void setup(){
  entis_background_int(ENTIS_DARK_WHITE);
  entis_clear();
}
void draw(){

}
void destroy(){

}

int main(int argc, char* argv[]) {
  entis_init(500, 500, ENTIS_XCB | ENTIS_TTF);
  while(entis_xcb_window_open()){
    entis_event ev = entis_poll_event();
    while(ev.type != ENTIS_NO_EVENT){
      if(ev.type == ENTIS_KEY_PRESS){
        entis_debug("DOWN %u", ev.key.keycode);
      }else if(ev.type == ENTIS_KEY_RELEASE){
        entis_debug("UP   %u", ev.key.keycode);
      }
      ev = entis_poll_event();
    }
    if(entis_key_state(KEY_G)){
      entis_note("G IS HELD DOWN");
    }
  }
  entis_term();
}
