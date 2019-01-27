#include <stdio.h>
#include <time.h>

#include "../entis.h"
#include "loop.h"

void process_event(entis_event ev) {
  switch (ev.type) {
    case ENTIS_CONFIGURE_NOTIFY:
      width = ev.configure.width;
      height = ev.configure.height;
      if (resize) {
        resize();
      }
      break;
    case ENTIS_MOTION_NOTIFY:
      mouseX = ev.motion.x;
      mouseY = ev.motion.y;
      break;
    case ENTIS_KEY_PRESS:
      if (key_press) {
        key_press(ev.key.keysym);
      }
      if (ev.key.keycode == KEY_ESCAPE) {
        entis_xcb_close_window();
      }
      break;
    case ENTIS_BUTTON_PRESS:
      if (button_press) {
        button_press(ev.button.button);
      }
      break;
    case ENTIS_ENTER_NOTIFY:
      focused = true;
      break;
    case ENTIS_LEAVE_NOTIFY:
      focused = false;
      break;
    default:
      break;
  }
}

double clock_to_ms(clock_t ticks) {
  return (ticks / (double)CLOCKS_PER_SEC) * 1000.0;
}

int main(int argc, char* argv[]) {
  srand(time(NULL));
  if (create) {
    create();
  }
  entis_init(width, height, ENTIS_XCB | ENTIS_TTF);
  entis_color_int(ENTIS_WHITE);
  entis_background_int(ENTIS_BLACK);
  entis_clear();
  if (setup) {
    setup();
  }
  uint32_t frames = 0;
  double fps = 60.0;
  clock_t delta_time = 0;
  while (entis_xcb_window_open()) {
    clock_t start = clock();
    if (loop_ && draw) {
      draw();
    }
    entis_event ev = entis_poll_event();
    while (ev.type != ENTIS_NO_EVENT) {
      process_event(ev);
      ev = entis_poll_event();
    }
    entis_sleep(sleep_time_);
    delta_time += clock() - start;
    frames++;
    if (clock_to_ms(delta_time) > 1000.0) {
      fps = (double)frames * 0.5 + fps * 0.5;
      frames = 0;
      delta_time -= CLOCKS_PER_SEC;
      if (fps <= 30) {
        entis_warning("LOW FPS: %f", fps);
      }
    }
  }
  if (destroy) {
    destroy();
  }
  entis_term();
  if (terminate) {
    terminate();
  }
  return 0;
}
