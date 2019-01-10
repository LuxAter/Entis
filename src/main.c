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
  entis_color_int(ENTIS_WHITE);
  entis_background_int(ENTIS_BLACK);
  entis_clear();
  bool draw = false;
  while(entis_xcb_window_open()){
    entis_event ev = entis_poll_event();
    while(ev.type != ENTIS_NO_EVENT){
      if(ev.type == ENTIS_MOTION_NOTIFY){
        if(draw) entis_circle_fill(ev.motion.x, ev.motion.y, 5);
      }else if(ev.type == ENTIS_BUTTON_PRESS){
        draw = true;
        entis_circle_fill(ev.motion.x, ev.motion.y, 5);
      }else if(ev.type == ENTIS_BUTTON_RELEASE){
        draw = false;
        entis_circle_fill(ev.motion.x, ev.motion.y, 5);
      }else if(ev.type == ENTIS_KEY_PRESS && ev.key.keycode == KEY_S){
        entis_write_png("save.png");
      }else if(ev.type == ENTIS_KEY_PRESS && ev.key.keycode == KEY_L){
        entis_read_png("test.png");
      }else if(ev.type == ENTIS_KEY_PRESS && ev.key.keycode == KEY_Q){
        entis_xcb_close_window();
      }else if(ev.type == ENTIS_KEY_PRESS && ev.key.keycode == KEY_R){
        entis_color_rgb(rand() % 256, rand() % 256, rand() % 256);
      }
      ev = entis_poll_event();
    }
    entis_sleep(0.05);
  }
  entis_term();
}
