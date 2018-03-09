#ifndef ENTIS_EVENT_H_
#define ENTIS_EVENT_H_

#include <X11/Xlib.h>

XEvent entis_event_wait_event();
XEvent entis_event_poll_event();

#endif  // ENTIS_EVENT_H_
