#ifndef ENTIS_ENTIS_H_
#define ENTIS_ENTIS_H_

#include <xcb/xcb.h>
#include <stdbool.h>

void entis_init();
void entis_term();

bool entis_connection_valid();

xcb_connection_t* entis_get_connection();
xcb_screen_t* entis_get_screen();

void entis_flush();

// struct entis_window entis_create_window(int x, int y, int width, int height);

#endif  // ENTIS_ENTIS_H_

