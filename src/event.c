#include "event.h"

#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <stdio.h>
#include <stdlib.h>

#include "entis.h"

EntisEvent entis_event_wait_event() {
  XEvent event;
  XNextEvent(entis_get_connection(), &event);
  return entis_event_parse_event(event);
}
EntisEvent entis_event_poll_event() {
  if (XPending(entis_get_connection()) == 0) {
    return (EntisEvent){ENTIS_NO_EVENT};
  }
  XEvent event;
  XNextEvent(entis_get_connection(), &event);
  return entis_event_parse_event(event);
}

KeySym KeyCodeToKeySym(Display* display, KeyCode keycode,
                       unsigned int event_mask) {
  KeySym keysym = NoSymbol;

  // Get the map
  XkbDescPtr keyboard_map =
      XkbGetMap(display, XkbAllClientInfoMask, XkbUseCoreKbd);
  if (keyboard_map) {
    // What is diff between XkbKeyGroupInfo and XkbKeyNumGroups?
    unsigned char info = XkbKeyGroupInfo(keyboard_map, keycode);
    unsigned int num_groups = XkbKeyNumGroups(keyboard_map, keycode);

    // Get the group
    unsigned int group = 0x00;
    switch (XkbOutOfRangeGroupAction(info)) {
      case XkbRedirectIntoRange:
        /* If the RedirectIntoRange flag is set, the four least significant
         * bits of the groups wrap control specify the index of a group to
         * which all illegal groups correspond. If the specified group is
         * also out of range, all illegal groups map to Group1.
         */
        group = XkbOutOfRangeGroupInfo(info);
        if (group >= num_groups) {
          group = 0;
        }
        break;

      case XkbClampIntoRange:
        /* If the ClampIntoRange flag is set, out-of-range groups correspond
         * to the nearest legal group. Effective groups larger than the
         * highest supported group are mapped to the highest supported group;
         * effective groups less than Group1 are mapped to Group1 . For
         * example, a key with two groups of symbols uses Group2 type and
         * symbols if the global effective group is either Group3 or Group4.
         */
        group = num_groups - 1;
        break;

      case XkbWrapIntoRange:
        /* If neither flag is set, group is wrapped into range using integer
         * modulus. For example, a key with two groups of symbols for which
         * groups wrap uses Group1 symbols if the global effective group is
         * Group3 or Group2 symbols if the global effective group is Group4.
         */
      default:
        if (num_groups != 0) {
          group %= num_groups;
        }
        break;
    }

    XkbKeyTypePtr key_type = XkbKeyKeyType(keyboard_map, keycode, group);
    unsigned int active_mods = event_mask & key_type->mods.mask;

    int i, level = 0;
    for (i = 0; i < key_type->map_count; i++) {
      if (key_type->map[i].active &&
          key_type->map[i].mods.mask == active_mods) {
        level = key_type->map[i].level;
      }
    }

    keysym = XkbKeySymEntry(keyboard_map, keycode, level, group);
    XkbFreeClientMap(keyboard_map, XkbAllClientInfoMask, true);
  }

  return keysym;
}

enum EventType entis_event_parse_type(XEvent event) {
  switch (event.type) {
    case KeyPress:
      return ENTIS_KEY_PRESS;
    case KeyRelease:
      return ENTIS_KEY_RELEASE;
    case ButtonPress:
      return ENTIS_BUTTON_PRESS;
    case ButtonRelease:
      return ENTIS_BUTTON_RELEASE;
    case MotionNotify:
      return ENTIS_MOTION_NOTIFY;
    case EnterNotify:
      return ENTIS_ENTER_NOTIFY;
    case LeaveNotify:
      return ENTIS_LEAVE_NOTIFY;
    case FocusIn:
      return ENTIS_FOCUS_IN;
    case FocusOut:
      return ENTIS_FOCUS_OUT;
    case KeymapNotify:
      return ENTIS_KEYMAP_NOTIFY;
    case Expose:
      return ENTIS_EXPOSE;
    case GraphicsExpose:
      return ENTIS_GRAPHICS_EXPOSURE;
    case NoExpose:
      return ENTIS_NO_EXPOSURE;
    case VisibilityNotify:
      return ENTIS_VISIBILITY_NOTIFY;
    case CreateNotify:
      return ENTIS_CREATE_NOTIFY;
    case DestroyNotify:
      return ENTIS_DESTROY_NOTIFY;
    case UnmapNotify:
      return ENTIS_UNMAP_NOTIFY;
    case MapNotify:
      return ENTIS_MAP_NOTIFY;
    case MapRequest:
      return ENTIS_MAP_REQUEST;
    case ReparentNotify:
      return ENTIS_REPARENT_NOTIFY;
    case ConfigureNotify:
      return ENTIS_CONFIGURE_NOTIFY;
    case ConfigureRequest:
      return ENTIS_CONFIGURE_REQUEST;
    case GravityNotify:
      return ENTIS_GRAVITY_NOTIFY;
    case ResizeRequest:
      return ENTIS_RESIZE_REQUEST;
    case CirculateNotify:
      return ENTIS_CIRCULATE_NOTIFY;
    case CirculateRequest:
      return ENTIS_CIRCULATE_REQUEST;
    case PropertyNotify:
      return ENTIS_PROPERTY_NOTIFY;
    case SelectionClear:
      return ENTIS_SELECTION_CLEAR;
    case SelectionRequest:
      return ENTIS_SELECTION_REQUEST;
    case SelectionNotify:
      return ENTIS_SELECTION_NOTIFY;
    case ClientMessage:
      return ENTIS_CLIENT_MESSAGE;
    case MappingNotify:
      return ENTIS_MAPPING_NOTIFY;
    default:
      return ENTIS_NO_EVENT;
  }
  return ENTIS_NO_EVENT;
}

entis_key_event entis_event_parse_key(XEvent event, enum EventType type) {
  if (type == ENTIS_KEY_PRESS || type == ENTIS_KEY_RELEASE) {
    XKeyEvent ev = event.xkey;
    return (entis_key_event){
        type,
        ev.time,
        ev.x,
        ev.y,
        ev.x_root,
        ev.y_root,
        ev.state,
        ev.keycode,
        KeyCodeToKeySym(entis_get_connection(), ev.keycode, ev.state)};
  } else {
    return (entis_key_event){ENTIS_NO_EVENT, 0, 0, 0, 0, 0, 0, 0};
  }
}
entis_button_event entis_event_parse_button(XEvent event, enum EventType type) {
  if (type == ENTIS_BUTTON_PRESS || type == ENTIS_BUTTON_RELEASE) {
    XButtonEvent ev = event.xbutton;
    return (entis_button_event){type,      ev.time,   ev.x,     ev.y,
                                ev.x_root, ev.y_root, ev.state, ev.button};
  } else {
    return (entis_button_event){ENTIS_NO_EVENT, 0, 0, 0, 0, 0, 0, 0};
  }
}
entis_motion_event entis_event_parse_motion(XEvent event, enum EventType type) {
  if (type == ENTIS_MOTION_NOTIFY) {
    XMotionEvent ev = event.xmotion;
    return (entis_motion_event){type,      ev.time,   ev.x,     ev.y,
                                ev.x_root, ev.y_root, ev.state, ev.is_hint};
  } else {
    return (entis_motion_event){ENTIS_NO_EVENT, 0, 0, 0, 0, 0, 0, 0};
  }
}
entis_crossing_event entis_event_parse_crossing(XEvent event,
                                                enum EventType type) {
  if (type == ENTIS_ENTER_NOTIFY || type == ENTIS_LEAVE_NOTIFY) {
    /* xcb_enter_notify_event_t* ev = (xcb_enter_notify_event_t*)event; */
    XCrossingEvent ev = event.xcrossing;
    return (entis_crossing_event){type,      ev.time,   ev.x,     ev.y,
                                  ev.x_root, ev.y_root, ev.state, ev.detail};
  } else {
    return (entis_crossing_event){ENTIS_NO_EVENT, 0, 0, 0, 0, 0, 0, 0};
  }
}
entis_focus_event entis_event_parse_focus(XEvent event, enum EventType type) {
  if (type == ENTIS_FOCUS_IN || type == ENTIS_FOCUS_OUT) {
    XFocusChangeEvent ev = event.xfocus;
    return (entis_focus_event){type, ev.mode, ev.detail};
  } else {
    return (entis_focus_event){ENTIS_NO_EVENT, 0, 0};
  }
}
entis_expose_event entis_event_parse_expose(XEvent event, enum EventType type) {
  if (type == ENTIS_EXPOSE) {
    XExposeEvent ev = event.xexpose;
    return (entis_expose_event){type,     ev.x,      ev.y,
                                ev.width, ev.height, ev.count};
  } else {
    return (entis_expose_event){ENTIS_NO_EVENT, 0, 0, 0, 0, 0};
  }
}
entis_graphics_expose_event entis_event_parse_graphics_expose(
    XEvent event, enum EventType type) {
  if (type == ENTIS_GRAPHICS_EXPOSURE) {
    XGraphicsExposeEvent ev = event.xgraphicsexpose;
    return (entis_graphics_expose_event){
        type,      ev.x,          ev.y,          ev.width,
        ev.height, ev.major_code, ev.minor_code, ev.count};
  } else {
    return (entis_graphics_expose_event){ENTIS_NO_EVENT, 0, 0, 0, 0, 0, 0, 0};
  }
}
entis_no_expose_event entis_event_parse_no_expose(XEvent event,
                                                  enum EventType type) {
  if (type == ENTIS_NO_EXPOSURE) {
    XNoExposeEvent ev = event.xnoexpose;
    return (entis_no_expose_event){type, ev.major_code, ev.minor_code};
  } else {
    return (entis_no_expose_event){ENTIS_NO_EVENT, 0, 0};
  }
}
entis_visibility_event entis_event_parse_visibility(XEvent event,
                                                    enum EventType type) {
  if (type == ENTIS_VISIBILITY_NOTIFY) {
    XVisibilityEvent ev = event.xvisibility;
    return (entis_visibility_event){type, ev.state};
  } else {
    return (entis_visibility_event){ENTIS_NO_EVENT, 0};
  }
}
entis_create_event entis_event_parse_create(XEvent event, enum EventType type) {
  if (type == ENTIS_CREATE_NOTIFY) {
    XCreateWindowEvent ev = event.xcreatewindow;
    return (entis_create_event){type,     ev.x,      ev.y,
                                ev.width, ev.height, ev.border_width};
  } else {
    return (entis_create_event){ENTIS_NO_EVENT, 0, 0, 0, 0, 0};
  }
}
entis_destroy_event entis_event_parse_destroy(XEvent event,
                                              enum EventType type) {
  if (type == ENTIS_DESTROY_NOTIFY) {
    XDestroyWindowEvent ev = event.xdestroywindow;
    return (entis_destroy_event){type, ev.event};
  } else {
    return (entis_destroy_event){ENTIS_NO_EVENT, 0};
  }
}
entis_unmap_event entis_event_parse_unmap(XEvent event, enum EventType type) {
  if (type == ENTIS_UNMAP_NOTIFY) {
    XUnmapEvent ev = event.xunmap;
    return (entis_unmap_event){type, ev.event};
  } else {
    return (entis_unmap_event){ENTIS_NO_EVENT, 0};
  }
}
entis_map_event entis_event_parse_map(XEvent event, enum EventType type) {
  if (type == ENTIS_MAP_NOTIFY) {
    XMapEvent ev = event.xmap;
    return (entis_map_event){type, ev.event};
  } else {
    return (entis_map_event){ENTIS_NO_EVENT, 0};
  }
}
entis_map_request_event entis_event_parse_map_request(XEvent event,
                                                      enum EventType type) {
  if (type == ENTIS_MAP_REQUEST) {
    XMapRequestEvent ev = event.xmaprequest;
    return (entis_map_request_event){type, ev.parent};
  } else {
    return (entis_map_request_event){ENTIS_NO_EVENT, 0};
  }
}
entis_reparent_event entis_event_parse_reparent(XEvent event,
                                                enum EventType type) {
  if (type == ENTIS_REPARENT_NOTIFY) {
    XReparentEvent ev = event.xreparent;
    return (entis_reparent_event){type, ev.event, ev.parent, ev.x, ev.y};
  } else {
    return (entis_reparent_event){ENTIS_NO_EVENT, 0, 0, 0, 0};
  }
}
entis_configure_event entis_event_parse_configure(XEvent event,
                                                  enum EventType type) {
  if (type == ENTIS_CONFIGURE_NOTIFY) {
    XConfigureEvent ev = event.xconfigure;
    return (entis_configure_event){type,     ev.event,  ev.x,           ev.y,
                                   ev.width, ev.height, ev.border_width};
  } else {
    return (entis_configure_event){ENTIS_NO_EVENT, 0, 0, 0, 0, 0, 0};
  }
}
entis_gravity_event entis_event_parse_gravity(XEvent event,
                                              enum EventType type) {
  if (type == ENTIS_GRAVITY_NOTIFY) {
    XGravityEvent ev = event.xgravity;
    return (entis_gravity_event){type, ev.event, ev.x, ev.y};
  } else {
    return (entis_gravity_event){ENTIS_NO_EVENT, 0, 0, 0};
  }
}
entis_resize_event entis_event_parse_resize(XEvent event, enum EventType type) {
  if (type == ENTIS_RESIZE_REQUEST) {
    XResizeRequestEvent ev = event.xresizerequest;
    return (entis_resize_event){type, ev.window, ev.width, ev.height};
  } else {
    return (entis_resize_event){ENTIS_NO_EVENT, 0, 0, 0};
  }
}
entis_configure_request_event entis_event_parse_configure_request(
    XEvent event, enum EventType type) {
  if (type == ENTIS_CONFIGURE_REQUEST) {
    XConfigureRequestEvent ev = event.xconfigurerequest;
    return (entis_configure_request_event){
        type,      ev.window,       ev.x,         ev.y, ev.width,
        ev.height, ev.border_width, ev.value_mask};
  } else {
    return (entis_configure_request_event){ENTIS_NO_EVENT, 0, 0, 0, 0, 0, 0, 0};
  }
}
entis_circulate_event entis_event_parse_circulate(XEvent event,
                                                  enum EventType type) {
  if (type == ENTIS_CIRCULATE_NOTIFY) {
    XCirculateEvent ev = event.xcirculate;
    return (entis_circulate_event){type, ev.place};
  } else {
    return (entis_circulate_event){ENTIS_NO_EVENT, 0};
  }
}
entis_circulate_request_event entis_event_parse_circulate_request(
    XEvent event, enum EventType type) {
  if (type == ENTIS_CIRCULATE_REQUEST) {
    XCirculateRequestEvent ev = event.xcirculaterequest;
    return (entis_circulate_request_event){type, ev.place};
  } else {
    return (entis_circulate_request_event){ENTIS_NO_EVENT, 0};
  }
}
entis_property_event entis_event_parse_property(XEvent event,
                                                enum EventType type) {
  if (type == ENTIS_PROPERTY_NOTIFY) {
    XPropertyEvent ev = event.xproperty;
    return (entis_property_event){type, ev.time, ev.atom, ev.state};
  } else {
    return (entis_property_event){ENTIS_NO_EVENT, 0, 0, 0};
  }
}
entis_selection_event entis_event_parse_selection(XEvent event,
                                                  enum EventType type) {
  if (type == ENTIS_SELECTION_NOTIFY) {
    XSelectionEvent ev = event.xselection;
    return (entis_selection_event){type,      ev.requestor, ev.selection,
                                   ev.target, ev.property,  ev.time};
  } else {
    return (entis_selection_event){ENTIS_NO_EVENT, 0, 0, 0, 0, 0};
  }
}
entis_selection_clear_event entis_event_parse_selection_clear(
    XEvent event, enum EventType type) {
  if (type == ENTIS_SELECTION_CLEAR) {
    XSelectionClearEvent ev = event.xselectionclear;
    return (entis_selection_clear_event){type, ev.selection, ev.time};
  } else {
    return (entis_selection_clear_event){ENTIS_NO_EVENT, 0, 0};
  }
}
entis_selection_request_event entis_event_parse_selection_request(
    XEvent event, enum EventType type) {
  if (type == ENTIS_SELECTION_REQUEST) {
    XSelectionRequestEvent ev = event.xselectionrequest;
    return (entis_selection_request_event){
        type,      ev.owner,    ev.requestor, ev.selection,
        ev.target, ev.property, ev.time};
  } else {
    return (entis_selection_request_event){ENTIS_NO_EVENT, 0, 0, 0, 0, 0, 0};
  }
}
entis_mapping_event entis_event_parse_mapping(XEvent event,
                                              enum EventType type) {
  if (type == ENTIS_MAPPING_NOTIFY) {
    XMappingEvent ev = event.xmapping;
    return (entis_mapping_event){type, ev.request, ev.first_keycode, ev.count};
  } else {
    return (entis_mapping_event){ENTIS_NO_EVENT, 0, 0, 0};
  }
}

EntisEvent entis_event_parse_event(XEvent event) {
  EntisEvent ret;
  ret.type = entis_event_parse_type(event);
  switch (entis_event_parse_type(event)) {
    case ENTIS_KEY_PRESS:
      ret.key = entis_event_parse_key(event, ENTIS_KEY_PRESS);
      break;
    case ENTIS_KEY_RELEASE:
      ret.key = entis_event_parse_key(event, ENTIS_KEY_RELEASE);
      break;
    case ENTIS_BUTTON_PRESS:
      ret.button = entis_event_parse_button(event, ENTIS_BUTTON_PRESS);
      break;
    case ENTIS_BUTTON_RELEASE:
      ret.button = entis_event_parse_button(event, ENTIS_BUTTON_RELEASE);
      break;
    case ENTIS_MOTION_NOTIFY:
      ret.motion = entis_event_parse_motion(event, ENTIS_MOTION_NOTIFY);
      break;
    case ENTIS_ENTER_NOTIFY:
      ret.crossing = entis_event_parse_crossing(event, ENTIS_ENTER_NOTIFY);
      break;
    case ENTIS_LEAVE_NOTIFY:
      ret.crossing = entis_event_parse_crossing(event, ENTIS_LEAVE_NOTIFY);
      break;
    case ENTIS_FOCUS_IN:
      ret.focus = entis_event_parse_focus(event, ENTIS_FOCUS_IN);
      break;
    case ENTIS_FOCUS_OUT:
      ret.focus = entis_event_parse_focus(event, ENTIS_FOCUS_OUT);
      break;
    case ENTIS_KEYMAP_NOTIFY:
      /* ret.keymap = entis_event_parse_keymap(event, ENTIS_KEYMAP_NOTIFY); */
      break;
    case ENTIS_EXPOSE:
      ret.expose = entis_event_parse_expose(event, ENTIS_EXPOSE);
      break;
    case ENTIS_GRAPHICS_EXPOSURE:
      ret.graphics_expose =
          entis_event_parse_graphics_expose(event, ENTIS_GRAPHICS_EXPOSURE);
      break;
    case ENTIS_NO_EXPOSURE:
      ret.no_expose = entis_event_parse_no_expose(event, ENTIS_NO_EXPOSURE);
      break;
    case ENTIS_VISIBILITY_NOTIFY:
      ret.visibility =
          entis_event_parse_visibility(event, ENTIS_VISIBILITY_NOTIFY);
      break;
    case ENTIS_CREATE_NOTIFY:
      ret.create = entis_event_parse_create(event, ENTIS_CREATE_NOTIFY);
      break;
    case ENTIS_DESTROY_NOTIFY:
      ret.destroy = entis_event_parse_destroy(event, ENTIS_DESTROY_NOTIFY);
      break;
    case ENTIS_UNMAP_NOTIFY:
      ret.unmap = entis_event_parse_unmap(event, ENTIS_UNMAP_NOTIFY);
      break;
    case ENTIS_MAP_NOTIFY:
      ret.map = entis_event_parse_map(event, ENTIS_MAP_NOTIFY);
      break;
    case ENTIS_MAP_REQUEST:
      ret.map_request = entis_event_parse_map_request(event, ENTIS_MAP_REQUEST);
      break;
    case ENTIS_REPARENT_NOTIFY:
      ret.reparent = entis_event_parse_reparent(event, ENTIS_REPARENT_NOTIFY);
      break;
    case ENTIS_CONFIGURE_NOTIFY:
      ret.configure =
          entis_event_parse_configure(event, ENTIS_CONFIGURE_NOTIFY);
      break;
    case ENTIS_CONFIGURE_REQUEST:
      ret.configure_request =
          entis_event_parse_configure_request(event, ENTIS_CONFIGURE_REQUEST);
      break;
    case ENTIS_GRAVITY_NOTIFY:
      ret.gravity = entis_event_parse_gravity(event, ENTIS_GRAVITY_NOTIFY);
      break;
    case ENTIS_RESIZE_REQUEST:
      ret.resize = entis_event_parse_resize(event, ENTIS_RESIZE_REQUEST);
      break;
    case ENTIS_CIRCULATE_NOTIFY:
      ret.circulate =
          entis_event_parse_circulate(event, ENTIS_CIRCULATE_NOTIFY);
      break;
    case ENTIS_CIRCULATE_REQUEST:
      ret.circulate_request =
          entis_event_parse_circulate_request(event, ENTIS_CIRCULATE_REQUEST);
      break;
    case ENTIS_PROPERTY_NOTIFY:
      ret.property = entis_event_parse_property(event, ENTIS_PROPERTY_NOTIFY);
      break;
    case ENTIS_SELECTION_CLEAR:
      ret.selection_clear =
          entis_event_parse_selection_clear(event, ENTIS_SELECTION_CLEAR);
      break;
    case ENTIS_SELECTION_REQUEST:
      ret.selection_request =
          entis_event_parse_selection_request(event, ENTIS_SELECTION_REQUEST);
      break;
    case ENTIS_SELECTION_NOTIFY:
      ret.selection =
          entis_event_parse_selection(event, ENTIS_SELECTION_NOTIFY);
      break;
    case ENTIS_MAPPING_NOTIFY:
      ret.mapping = entis_event_parse_mapping(event, ENTIS_MAPPING_NOTIFY);
      break;
    default:
      ret.type = ENTIS_NO_EVENT;
      break;
  }
  return ret;
}
