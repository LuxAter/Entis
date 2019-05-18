#include <Python.h>

#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <xcb/xcb.h>

#include "event.h"

/* int connection_ = 0; */
static xcb_connection_t* connection_;
static xcb_screen_t* screen_;

static xcb_window_t window_;
static xcb_pixmap_t pixmap_;
static xcb_gcontext_t gcontext_, pixmap_gcontext_, pixmap_bg_gcontext_;

static uint16_t width_, height_;
static uint16_t pix_width_, pix_height_;

static PyObject* EntisError;
static PyObject* XCBError;

// ENTIS.CORE
// ============================================================================
static PyObject* entis_init(PyObject* self, PyObject* args) {
  const char* title;
  uint16_t width, height;

  if (!PyArg_ParseTuple(args, "sII", &title, &width, &height)) return NULL;

  if (connection_ != NULL) {
    PyErr_SetString(EntisError, "XCB connection already established");
    return NULL;
  }
  connection_ = xcb_connect(NULL, NULL);
  if (connection_ == NULL) {
    PyErr_SetString(XCBError, "Failed to connecton to X server");
    return NULL;
  } else if (xcb_connection_has_error(connection_)) {
    PyErr_SetString(XCBError, "Error while attempting to connect to X server");
    xcb_disconnect(connection_);
    return NULL;
  }
  screen_ = xcb_setup_roots_iterator(xcb_get_setup(connection_)).data;
  window_ = xcb_generate_id(connection_);
  xcb_void_cookie_t cookie;
  uint32_t internal_value[1] = {
      XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS |
      XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_ENTER_WINDOW |
      XCB_EVENT_MASK_LEAVE_WINDOW | XCB_EVENT_MASK_KEY_PRESS |
      XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_STRUCTURE_NOTIFY};
  cookie = xcb_create_window(
      connection_, XCB_COPY_FROM_PARENT, window_, screen_->root, 0, 0, width,
      height, 0, XCB_WINDOW_CLASS_INPUT_OUTPUT, screen_->root_visual,
      XCB_CW_EVENT_MASK, internal_value);
  xcb_generic_error_t* error_check = xcb_request_check(connection_, cookie);
  if (error_check != NULL) {
    PyErr_SetString(XCBError, "Failed to generate xcb window");
    xcb_destroy_window(connection_, window_);
    xcb_disconnect(connection_);
    return NULL;
  }
  free(error_check);
  pixmap_ = xcb_generate_id(connection_);
  cookie = xcb_create_pixmap(connection_, screen_->root_depth, pixmap_,
                             screen_->root, width, height);
  error_check = xcb_request_check(connection_, cookie);
  if (error_check != NULL) {
    PyErr_SetString(XCBError, "Failed to generate xcb pixmap");
    xcb_destroy_window(connection_, window_);
    xcb_disconnect(connection_);
    return NULL;
  }
  free(error_check);
  pixmap_gcontext_ = xcb_generate_id(connection_);
  cookie = xcb_create_gc(connection_, pixmap_gcontext_, screen_->root, 0, NULL);
  error_check = xcb_request_check(connection_, cookie);
  if (error_check != NULL) {
    PyErr_SetString(XCBError, "Failed to generate xcb pixmap graphics context");
    xcb_destroy_window(connection_, window_);
    xcb_disconnect(connection_);
    return NULL;
  }
  free(error_check);
  pixmap_bg_gcontext_ = xcb_generate_id(connection_);
  cookie = xcb_create_gc(connection_, pixmap_bg_gcontext_, screen_->root,
                         XCB_GC_FOREGROUND, (uint32_t[]){0x000000});
  error_check = xcb_request_check(connection_, cookie);
  if (error_check != NULL) {
    PyErr_SetString(XCBError, "Failed to generate xcb pixmap graphics context");
    xcb_destroy_window(connection_, window_);
    xcb_disconnect(connection_);
    return NULL;
  }
  free(error_check);
  xcb_poly_fill_rectangle(connection_, pixmap_, pixmap_bg_gcontext_, 1,
                          (xcb_rectangle_t[]){{0, 0, width, height}});
  gcontext_ = xcb_generate_id(connection_);
  cookie = xcb_create_gc(connection_, gcontext_, screen_->root, 0, NULL);
  error_check = xcb_request_check(connection_, cookie);
  if (error_check != NULL) {
    PyErr_SetString(XCBError, "Failed to generate xcb graphics context");
    xcb_destroy_window(connection_, window_);
    xcb_disconnect(connection_);
    return NULL;
  }
  free(error_check);
  if (strlen(title) != 0) {
    xcb_change_property(connection_, XCB_PROP_MODE_REPLACE, window_,
                        XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, strlen(title),
                        title);
  }
  width_ = width;
  height_ = height;
  xcb_map_window(connection_, window_);
  xcb_flush(connection_);
  nanosleep(&(struct timespec){0, 3.125e7}, NULL);
  Py_RETURN_TRUE;
}

static PyObject* entis_term(PyObject* self) {
  if (connection_ == NULL) {
    PyErr_SetString(XCBError, "No connecton to X server");
    return NULL;
  }
  xcb_destroy_window(connection_, window_);
  xcb_disconnect(connection_);
  connection_ = NULL;
  Py_RETURN_NONE;
}

static PyObject* entis_connection_valid(PyObject* self) {
  if (connection_ == NULL) {
    Py_RETURN_FALSE;
  }
  Py_RETURN_TRUE;
}
static PyObject* entis_flush(PyObject* self) {
  xcb_flush(connection_);
  Py_RETURN_NONE;
}

static PyObject* entis_set_color(PyObject* self, PyObject* args) {
  uint32_t color;
  if (!PyArg_ParseTuple(args, "I", &color)) return NULL;

  uint32_t values[1] = {color};
  xcb_void_cookie_t cookie =
      xcb_change_gc(connection_, pixmap_gcontext_, XCB_GC_FOREGROUND, values);
  xcb_generic_error_t* error_check = xcb_request_check(connection_, cookie);
  if (error_check != NULL) {
    PyErr_SetString(XCBError, "Failed to change xcb graphics context color");
    return NULL;
  }
  free(error_check);
  Py_RETURN_NONE;
}

static PyObject* entis_set_background(PyObject* self, PyObject* args) {
  uint32_t color;
  if (!PyArg_ParseTuple(args, "I", &color)) return NULL;
  uint32_t values[1] = {color};
  xcb_void_cookie_t cookie = xcb_change_window_attributes(
      connection_, window_, XCB_CW_BACK_PIXEL, values);
  xcb_generic_error_t* error_check = xcb_request_check(connection_, cookie);
  if (error_check != NULL) {
    PyErr_SetString(XCBError, "Failed to change xcb window background color");
    return NULL;
  }
  free(error_check);
  cookie = xcb_change_gc(connection_, pixmap_bg_gcontext_, XCB_GC_FOREGROUND,
                         values);
  error_check = xcb_request_check(connection_, cookie);
  if (error_check != NULL) {
    PyErr_SetString(XCBError, "Failed to change xcb graphics context color");
    return NULL;
  }
  free(error_check);
  Py_RETURN_NONE;
}

static PyObject* entis_reload_pixmap(PyObject* self, PyObject* args) {
  uint16_t width, height;
  if (!PyArg_ParseTuple(args, "II", &width, &height)) return NULL;
  xcb_free_pixmap(connection_, pixmap_);
  xcb_void_cookie_t cookie = xcb_create_pixmap(
      connection_, screen_->root_depth, pixmap_, screen_->root, width, height);
  xcb_generic_error_t* error_check = xcb_request_check(connection_, cookie);
  if (error_check != NULL) {
    PyErr_SetString(XCBError, "Failed to resize xcb pixmap");
    return NULL;
  }
  free(error_check);
  width_ = width;
  height_ = height;
  entis_clear();
}

static PyObject* entis_copy_pixmap(PyObject* self) {
  xcb_void_cookie_t cookie = xcb_copy_area(
      connection_, pixmap_, window_, gcontext_, 0, 0, 0, 0, width_, height_);
  xcb_generic_error_t* error_check = xcb_request_check(connection_, cookie);
  if (error_check != NULL) {
    PyErr_SetString(XCBError, "Failed to copy pixmap to window");
    return NULL;
  }
  Py_RETURN_NONE;
}

static PyObject* entis_update(PyObject* self) {
  entis_copy_pixmap();
  entis_flush();
  Py_RETURN_NONE;
}

static PyObject* entis_clear(PyObject* self) {
  xcb_poly_fill_rectangle(connection_, pixmap_, pixmap_bg_gcontext_, 1,
                          (xcb_rectangle_t[]){{0, 0, width_, height_}});
  Py_RETURN_NONE;
}

static PyObject* entis_event(PyObject* self){
  Py_RETURN_NONE;
}

static PyMethodDef EntisMethods[] = {
    {"init", entis_init, METH_VARARGS, "Constructs xcb window."},
    {"term", entis_term, METH_VARARGS, "Destroys connection to xcb."},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef entismodule = {PyModuleDef_HEAD_INIT, "entis", NULL,
                                         -1, EntisMethods};

PyMODINIT_FUNC PyInit_entis(void) {
  PyObject* m = PyModule_Create(&entismodule);
  if (m == NULL) {
    return NULL;
  }
  EntisError = PyErr_NewException("entis.error", NULL, NULL);
  Py_INCREF(EntisError);
  PyModule_AddObject(m, "error", EntisError);
  XCBError = PyErr_NewException("XCB.error", NULL, NULL);
  Py_INCREF(XCBError);
  PyModule_AddObject(m, "error", XCBError);
  return m;
}
