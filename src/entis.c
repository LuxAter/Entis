#include <xcb/xcb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "entis.h"

static xcb_connection_t* connection_;
static xcb_screen_t* screen_;

void entis_init(){
  if (connection_ != NULL){
    fprintf(stderr, "Connection to X server already established!\n");
  }
  connection_ = xcb_connect(NULL, NULL);
  if (connection_ == NULL){
    fprintf(stderr, "Failed to connect to X server\n");
    exit(1);
  }
  else if(xcb_connection_has_error(connection_)){
    fprintf(stderr, "Error while attempting to connect to X server\n");
    xcb_disconnect(connection_);
    exit(1);
  }
  screen_ = xcb_setup_roots_iterator(xcb_get_setup(connection_)).data;
}

void entis_term(){
  if (connection_ == NULL){
    fprintf(stderr, "No connection to X server\n");
  }
  xcb_disconnect(connection_);
}

bool entis_connection_valid(){
  if (connection_ == NULL){
    return false;
  }
  return true;
}

xcb_connection_t* entis_get_connection(){
  return connection_;
}

xcb_screen_t* entis_get_screen(){
  return screen_;
}

void entis_flush(){
  xcb_flush(connection_);
}
