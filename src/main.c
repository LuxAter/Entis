#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "entis.h"
#include "window.h"
#include "color.h"

int main(int argc, char *argv[])
{
  entis_init();
  uint32_t colors[16] = {
    0x263238,
    0xf44336,
    0x4caf50,
    0xffeb3b,
    0x2196f3,
    0x9c27b0,
    0x00bcd4,
    0x90a4ae,
    0x546e7a,
    0xff8961,
    0x80e27e,
    0xffff72,
    0x6ec6ff,
    0xd05ce3,
    0x62efff,
    0xeceff1
  };
  /* entis_palette palette = entis_create_default_palette(); */
  entis_palette palette = entis_create_palette_int(colors);
  uint32_t values[1];
  values[0] = entis_get_color_value(entis_palette_get_color(palette, 0));
  entis_window win = entis_create_window("Hello World", 0, 0, 500, 500, XCB_CW_BACK_PIXEL, values);
  entis_flush();
  sleep(1);
  entis_destroy_window(win);
  entis_term();
  return 0;
}
