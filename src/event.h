/* Copyright (C)
 * 2019 - Arden Rasmussen
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */
#ifndef ENTIS_EVENT_H_
#define ENTIS_EVENT_H_

/**
 * @file event.h
 * @brief Core of entis event handling
 * @author Arden Rasmussen
 * @version 2.0
 * @date 2019-01-09
 *
 * This file contains all of the entis event handling. All of these functions
 * requires that the XCB modules has been initialized. None of the functions in
 * this file should be called by the user, they should only be called
 * internally.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <xcb/xcb.h>

enum EventType {
  ENTIS_NO_EVENT = (1lu << 0),
  ENTIS_KEY_PRESS = (1lu << 1),
  ENTIS_KEY_RELEASE = (1lu << 2),
  ENTIS_BUTTON_PRESS = (1lu << 3),
  ENTIS_BUTTON_RELEASE = (1lu << 4),
  ENTIS_MOTION_NOTIFY = (1lu << 5),
  ENTIS_ENTER_NOTIFY = (1lu << 6),
  ENTIS_LEAVE_NOTIFY = (1lu << 7),
  ENTIS_FOCUS_IN = (1lu << 8),
  ENTIS_FOCUS_OUT = (1lu << 9),
  ENTIS_KEYMAP_NOTIFY = (1lu << 10),
  ENTIS_EXPOSE = (1lu << 11),
  ENTIS_GRAPHICS_EXPOSURE = (1lu << 12),
  ENTIS_NO_EXPOSURE = (1lu << 13),
  ENTIS_VISIBILITY_NOTIFY = (1lu << 14),
  ENTIS_CREATE_NOTIFY = (1lu << 15),
  ENTIS_DESTROY_NOTIFY = (1lu << 16),
  ENTIS_UNMAP_NOTIFY = (1lu << 17),
  ENTIS_MAP_NOTIFY = (1lu << 18),
  ENTIS_MAP_REQUEST = (1lu << 19),
  ENTIS_REPARENT_NOTIFY = (1lu << 20),
  ENTIS_CONFIGURE_NOTIFY = (1lu << 21),
  ENTIS_CONFIGURE_REQUEST = (1lu << 22),
  ENTIS_GRAVITY_NOTIFY = (1lu << 23),
  ENTIS_RESIZE_REQUEST = (1lu << 24),
  ENTIS_PROPERTY_NOTIFY = (1lu << 25),
  ENTIS_SELECTION_CLEAR = (1lu << 26),
  ENTIS_SELECTION_REQUEST = (1lu << 27),
  ENTIS_SELECTION_NOTIFY = (1lu << 28),
  ENTIS_MAPPING_NOTIFY = (1lu << 29),
  ENTIS_GE_GENERIC = (1lu << 30)
};
/**
 * @brief Key based event.
 *
 * This event is used to represent key presses, and key releases.
 */
typedef struct entis_key_event {
  enum EventType type;  //!< Type of event.
  uint32_t time;        //!< Time the event occurred.
  int16_t x, y;
  int16_t root_x, root_y;
  uint16_t state;    //!< State of the key (PRESSED/RELEASED)
  uint16_t keycode;  //!< Keycode of the key that is pressed. This is unique to
                     //!< key, not to character (e.g q=Q).
  uint16_t keysym;   //!< Keysym of the key that is pressed. This is unique for
                     //!< each character (e.g q!=Q).
} entis_key_event;
/**
 * @brief Mouse button based event.
 *
 * This event is used for mouse button presses and releases.
 */
typedef struct entis_button_event {
  enum EventType type;  //!< Type of event.
  uint32_t time;        //!< Time the event occurred.
  int16_t x,            //!< X position of the mouse.
      y;                //!< Y position of the mouse.
  int16_t root_x, root_y;
  uint16_t state;  //!< State that the button (PRESSED/RELEASED).
  uint8_t button;  //!< The button that was pressed.
} entis_button_event;
/**
 * @brief Mouse movement event.
 *
 * This event is used to declare an event where the mouse moved.
 */
typedef struct entis_motion_event {
  enum EventType type;  //!< Type of event.
  uint32_t time;        //!< Time the event occurred.
  int16_t x,            //!< X position of the mouse.
      y;                //!< Y position of the mouse.
  int16_t root_x, root_y;
  uint16_t state;
  uint8_t detail;
} entis_motion_event;
/**
 * @brief Mouse crossing event.
 *
 * This event is disabled in this current version, and is not important.
 */
typedef struct entis_crossing_event {
  enum EventType type;  //!< Type of event.
  uint32_t time;        //!< Time the event occurred.
  int16_t x, y;
  int16_t root_x, root_y;
  uint16_t state;
  uint8_t button;
} entis_crossing_event;
/**
 * @brief Window focus event.
 *
 * This event is when the user focuses on the window, the window losses focus.
 */
typedef struct entis_focus_event {
  enum EventType type;  //!< Type of event.
  uint8_t mode;         //!< If the window gains or loses focus.
  uint8_t detail;
} entis_focus_event;
/**
 * @brief Rendering event.
 *
 * This is an internal event that the user will not have access to.
 */
typedef struct entis_expose_event {
  enum EventType type;  //!< Type of event.
  uint16_t x, y;
  uint16_t width, height;
  uint16_t count;
} entis_expose_event;
/**
 * @brief Rendering event.
 *
 * This is an internal event that the user will not have access to.
 */
typedef struct entis_graphics_expose_event {
  enum EventType type;  //!< Type of event.
  uint16_t x, y;
  uint16_t width, height;
  uint16_t major, minor;
  uint16_t count;
} entis_graphics_expose_event;
/**
 * @brief Rendering event.
 *
 * This is an internal event that the user will not have access to.
 */
typedef struct entis_no_expose_event {
  enum EventType type;  //!< Type of event.
  uint16_t major, minor;
} entis_no_expose_event;
/**
 * @brief Window visibility.
 *
 * This event is used when the visibility of a window changes. For example being
 * minimized.
 */
typedef struct entis_visibility_event {
  enum EventType type;  //!< Type of event.
  uint8_t state;        //!< State of the visibility.
} entis_visibility_event;
/**
 * @brief Window creation event.
 *
 * This event is handled internally and will only be created once when the XCB
 * window is created.
 */
typedef struct entis_create_event {
  enum EventType type;  //!< Type of event.
  uint16_t x, y;
  uint16_t width, height;
  uint16_t border_width;
} entis_create_event;
/**
 * @brief Window destruction event.
 *
 * This event is handled internally and will only be created once when the XCB
 * window is destroyed.
 */
typedef struct entis_destroy_event {
  enum EventType type;  //!< Type of event.
  xcb_window_t window;
} entis_destroy_event;
/**
 * @brief Unmapping event
 *
 * This event is handled internally and is not provided to the user.
 */
typedef struct entis_unmap_event {
  enum EventType type;  //!< Type of event.
  xcb_window_t window;
} entis_unmap_event;
/**
 * @brief Mapping event
 *
 * This event is handled internally and is not provided to the user.
 */
typedef struct entis_map_event {
  enum EventType type;  //!< Type of event.
  xcb_window_t window;
} entis_map_event;
/**
 * @brief Request event
 *
 * This event is handled internally and is not provided to the user.
 */
typedef struct entis_map_request_event {
  enum EventType type;  //!< Type of event.
  xcb_window_t parent;
} entis_map_request_event;
/**
 * @brief Reparent event
 *
 * This event is handled internally and is not provided to the user.
 */
typedef struct entis_reparent_event {
  enum EventType type;  //!< Type of event.
  xcb_window_t window, parent;
  uint16_t x, y;
} entis_reparent_event;
/**
 * @brief Window configure event.
 *
 * This event is when the window is modified. If the window is moved, or
 * resized. This event is used.
 */
typedef struct entis_configure_event {
  enum EventType type;    //!< Type of event.
  xcb_window_t window;    //!< Window that the event is associated with.
  uint16_t x,             //!< X position of the window.
      y;                  //!< Y position of the window.
  uint16_t width,         //!< Width of the window
      height;             //!< Height of the window.
  uint16_t border_width;  //!< Width of the windows border.
} entis_configure_event;
/**
 * @brief Window gravity event
 *
 * @todo Figure out what this event is.
 */
typedef struct entis_gravity_event {
  enum EventType type;  //!< Type of event.
  xcb_window_t window;
  int16_t x, y;
} entis_gravity_event;
/**
 * @brief Window resize event.
 *
 * This event is used when the current is resized.
 */
typedef struct entis_resize_event {
  enum EventType type;  //!< Type of event.
  xcb_window_t window;  //!< Window that the event is associated with.
  uint16_t width,       //!< Width of the window
      height;           //!< Height of the window.
} entis_resize_event;
/**
 * @brief Window configure request event.
 *
 * This event is used internally to request changes to window properties.
 */
typedef struct entis_configure_request_event {
  enum EventType type;  //!< Type of event.
  xcb_window_t window, sibling;
  uint16_t x,             //!< X position of the window.
      y;                  //!< Y position of the window.
  uint16_t width,         //!< Width of the window
      height;             //!< Height of the window.
  uint16_t border_width;  //!< Width of the windows border.
  uint16_t value_mask;
} entis_configure_request_event;
/**
 * @brief Window Circulate event
 *
 * @todo Figure out what this event is.
 */
typedef struct entis_circulate_event {
  enum EventType type;  //!< Type of event.
  uint8_t place;
} entis_circulate_event;
/**
 * @brief Window circulate request event
 *
 * @todo Figure out what this event is.
 */
typedef struct entis_circulate_request_event {
  enum EventType type;  //!< Type of event.
  uint8_t place;
} entis_circulate_request_event;
/**
 * @brief Window property event
 *
 * @todo Figure out what this event is.
 */
typedef struct entis_property_event {
  enum EventType type;  //!< Type of event.
  uint32_t time;
  uint32_t atom;
  uint8_t state;
} entis_property_event;
/**
 * @brief Selection clear event.
 *
 * This is when the user clears a selection.
 *
 * @todo Figure out what the values indicate.
 */
typedef struct entis_selection_clear_event {
  enum EventType type;  //!< Type of event.
  uint32_t selection;
  uint32_t time;
} entis_selection_clear_event;
/**
 * @brief Selection request event.
 *
 * @todo Figures out what this event is for.
 */
typedef struct entis_selection_request_event {
  enum EventType type;  //!< Type of event.
  xcb_window_t owner, requestor;
  uint32_t selection, target, property;
  uint32_t time;
} entis_selection_request_event;
/**
 * @brief Selection event.
 *
 * @todo Figures out what this event is for.
 */
typedef struct entis_selection_event {
  enum EventType type;  //!< Type of event.
  xcb_window_t requestor;
  uint32_t selection, target, property;
  uint32_t time;
} entis_selection_event;
/**
 * @brief Key mapping event
 *
 * This event is used when the user remaps the keyboard. For example when
 * changing languages. This event is currently not handled internally in the key
 * event processing.
 */
typedef struct entis_mapping_event {
  enum EventType type;  //!< Type of event.
  uint8_t request, first_keycode, count;
} entis_mapping_event;

/**
 * @brief Core event
 *
 * This is the core event, it consists of a type specifier, and a union of all
 * the possible event types.
 */
typedef struct entis_event {
  enum EventType type;  //!< Type of the provided event.
  union {
    entis_key_event key;
    entis_button_event button;
    entis_motion_event motion;
    entis_crossing_event crossing;
    entis_focus_event focus;
    entis_expose_event expose;
    entis_graphics_expose_event graphics_expose;
    entis_no_expose_event no_expose;
    entis_visibility_event visibility;
    entis_create_event create;
    entis_destroy_event destroy;
    entis_unmap_event unmap;
    entis_map_event map;
    entis_map_request_event map_request;
    entis_reparent_event reparent;
    entis_configure_event configure;
    entis_gravity_event gravity;
    entis_resize_event resize;
    entis_configure_request_event configure_request;
    entis_circulate_event circulate;
    entis_circulate_request_event circulate_request;
    entis_property_event property;
    entis_selection_event selection;
    entis_selection_clear_event selection_clear;
    entis_selection_request_event selection_request;
    entis_mapping_event mapping;
  };
} entis_event;

entis_event entis_event_wait_event();
entis_event entis_event_poll_event();
enum EventType entis_event_parse_type(xcb_generic_event_t* event);
entis_key_event entis_event_parse_key(xcb_generic_event_t* event,
                                      enum EventType type);
entis_button_event entis_event_parse_button(xcb_generic_event_t* event,
                                            enum EventType type);
entis_motion_event entis_event_parse_motion(xcb_generic_event_t* event,
                                            enum EventType type);
entis_crossing_event entis_event_parse_crossing(xcb_generic_event_t* event,
                                                enum EventType type);
entis_focus_event entis_event_parse_focus(xcb_generic_event_t* event,
                                          enum EventType type);
entis_expose_event entis_event_parse_expose(xcb_generic_event_t* event,
                                            enum EventType type);
entis_graphics_expose_event entis_event_parse_graphics_expose(
    xcb_generic_event_t* event, enum EventType type);
entis_no_expose_event entis_event_parse_no_expose(xcb_generic_event_t* event,
                                                  enum EventType type);
entis_visibility_event entis_event_parse_visibility(xcb_generic_event_t* event,
                                                    enum EventType type);
entis_create_event entis_event_parse_create(xcb_generic_event_t* event,
                                            enum EventType type);
entis_destroy_event entis_event_parse_destroy(xcb_generic_event_t* event,
                                              enum EventType type);
entis_unmap_event entis_event_parse_unmap(xcb_generic_event_t* event,
                                          enum EventType type);
entis_map_event entis_event_parse_map(xcb_generic_event_t* event,
                                      enum EventType type);
entis_map_request_event entis_event_parse_map_request(
    xcb_generic_event_t* event, enum EventType type);
entis_reparent_event entis_event_parse_reparent(xcb_generic_event_t* event,
                                                enum EventType type);
entis_configure_event entis_event_parse_configure(xcb_generic_event_t* event,
                                                  enum EventType type);
entis_gravity_event entis_event_parse_gravity(xcb_generic_event_t* event,
                                              enum EventType type);
entis_resize_event entis_event_parse_resize(xcb_generic_event_t* event,
                                            enum EventType type);
entis_configure_request_event entis_event_parse_configure_request(
    xcb_generic_event_t* event, enum EventType type);
entis_property_event entis_event_parse_property(xcb_generic_event_t* event,
                                                enum EventType type);
entis_selection_event entis_event_parse_selection(xcb_generic_event_t* event,
                                                  enum EventType type);
entis_selection_clear_event entis_event_parse_selection_clear(
    xcb_generic_event_t* event, enum EventType type);
entis_selection_request_event entis_event_parse_selection_request(
    xcb_generic_event_t* event, enum EventType type);
entis_mapping_event entis_event_parse_mapping(xcb_generic_event_t* event,
                                              enum EventType type);
entis_event entis_event_parse_event(xcb_generic_event_t* event);
entis_event entis_event_no_event();
bool entis_event_key_state(uint16_t keysym);

#ifdef __cplusplus
}
#endif

#endif  // ENTIS_EVENT_H_
