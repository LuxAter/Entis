#include "event.h"

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>

#include "entis.h"

XEvent entis_event_wait_event() {
  XEvent event;
  XNextEvent(entis_get_connection(), &event);
  return event;
}
XEvent entis_event_poll_event() {
  if (XPending(entis_get_connection()) == 0){
    return (XEvent){0};
  }
  XEvent event;
  XNextEvent(entis_get_connection(), &event);
  return event;
}

