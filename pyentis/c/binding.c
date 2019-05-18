#include <Python.h>
#include <structmember.h>

#include "entis.h"

static PyObject* EntisError;
static PyObject* XCBError;

typedef struct {
  PyObject_HEAD
  uint16_t keycode;
  uint16_t keysym;
  uint16_t state;
} PyKeyEvent;

static PyMemberDef PyKeyEventMembers[] =
    {{"keycode", T_INT, offsetof(PyKeyEvent, keycode), 0, "keycode"},
     {"keysym", T_INT, offsetof(PyKeyEvent, keysym), 0, "keysym"},
     {"state", T_INT, offsetof(PyKeyEvent, state), 0, "state"},
     {NULL}};

static PyTypeObject PyKeyEventType = {
    PyVarObject_HEAD_INIT(NULL, 0).tp_name = "entis.KeyEvent",
    .tp_doc = "KeyEvent object",
    .tp_basicsize = sizeof(PyKeyEvent),
    .tp_itemsize = 0,
    .tp_new = PyType_GenericNew,
    .tp_members = PyKeyEventMembers,
};

static PyObject* py_entis_init(PyObject* self, PyObject* args) {
  const char* title;
  uint16_t width, height;

  if (!PyArg_ParseTuple(args, "sII", &title, &width, &height)) return NULL;
  entis_init(title, width, height, 0, NULL);
  if (entis_connection_valid()) {
    Py_RETURN_TRUE;
  }
  Py_RETURN_FALSE;
}

static PyObject* py_entis_term(PyObject* self) {
  entis_term();
  Py_RETURN_NONE;
}

static PyObject* py_entis_connection_valid(PyObject* self) {
  if (entis_connection_valid()) {
    Py_RETURN_TRUE;
  }
  Py_RETURN_FALSE;
}

static PyObject* py_entis_flush(PyObject* self) {
  entis_flush();
  Py_RETURN_NONE;
}

static PyObject* py_entis_set_color(PyObject* self, PyObject* args) {
  double r, g = -1, b = -1;
  if (!PyArg_ParseTuple(args, "d|dd", &r, &g, &b)) return NULL;
  if (g != -1 && b != -1) {
    if (r > 1.0 || g > 1.0 || b > 1.0) {
      entis_set_color_drgb(r, g, b);
    } else {
      entis_set_color_rgb((uint32_t)(r), (uint32_t)(g), (uint32_t)(b));
    }
  } else if (r >= 255) {
    entis_set_color((uint32_t)(r));
  }
  Py_RETURN_NONE;
}

static PyObject* py_entis_set_background(PyObject* self, PyObject* args) {
  double r, g = -1, b = -1;
  if (!PyArg_ParseTuple(args, "d|dd", &r, &g, &b)) return NULL;
  if (g != -1 && b != -1) {
    if (r > 1.0 || g > 1.0 || b > 1.0) {
      entis_set_background_drgb(r, g, b);
    } else {
      entis_set_background_rgb((uint32_t)(r), (uint32_t)(g), (uint32_t)(b));
    }
  } else if (r >= 255) {
    entis_set_background((uint32_t)(r));
  }
  Py_RETURN_NONE;
}

static PyObject* py_entis_reload_pixmap(PyObject* self, PyObject* args) {
  uint32_t width, height;
  if (!PyArg_ParseTuple(args, "II", &width, &height)) return NULL;
  entis_reload_pixmap(width, height);
  Py_RETURN_NONE;
}

static PyObject* py_entis_copy_pixmap(PyObject* self) {
  entis_copy_pixmap();
  Py_RETURN_NONE;
}
static PyObject* py_entis_update(PyObject* self) {
  entis_update();
  Py_RETURN_NONE;
}
static PyObject* py_entis_clear(PyObject* self) {
  entis_clear();
  Py_RETURN_NONE;
}

static PyObject* py_entis_wait_key(PyObject* self) {
  entis_key_event ev = entis_wait_key();
  return Py_BuildValue("III", ev.keycode, ev.keysym, ev.state);
  /* PyObject* PyObject_New(&PyKeyEventType); */
  /* PyObject* argList = Py_BuildValue("III", ev.keycode, ev.keysym, ev.state); */
  /* PyObject* obj = PyObject_CallObject((PyObject*)&PyKeyEventType, argList); */
  /* Py_DECREF(argList); */
  /* return obj; */
}

static PyObject* py_entis_wait_button(PyObject* self) {
  entis_wait_key();
  Py_RETURN_FALSE;
}

static PyObject* py_entis_clear_events(PyObject* self) {
  entis_clear_events();
  Py_RETURN_NONE;
}

static PyObject* py_entis_point(PyObject* self, PyObject* args){
  uint16_t x, y;
  if (!PyArg_ParseTuple(args, "II", &x, &y)) return NULL;
  entis_point(x,y);
  Py_RETURN_NONE;
}

static PyObject* py_entis_fill_circle(PyObject* self, PyObject* args){
  uint16_t x, y, r;
  if (!PyArg_ParseTuple(args, "III", &x, &y, &r)) return NULL;
  entis_fill_circle(x,y,r);
  Py_RETURN_NONE;
}

static PyMethodDef EntisMethods[] = {
    {"init", py_entis_init, METH_VARARGS, "Initialize Entis library"},
    {"term", py_entis_term, METH_VARARGS, "Terminates Entis library"},
    {"set_color", py_entis_set_color, METH_VARARGS, "Sets the current color"},
    {"set_background", py_entis_set_color, METH_VARARGS,
     "Sets the current color"},
    {"reload_pixmap", py_entis_reload_pixmap, METH_VARARGS,
     "Resize the pixmap"},
    {"copy_pixmap", py_entis_copy_pixmap, METH_VARARGS, "Render the pixmap"},
    {"update", py_entis_update, METH_VARARGS, "Updates the window"},
    {"clear", py_entis_clear, METH_VARARGS,
     "Clears the window to background color"},
    {"wait_key", py_entis_wait_key, METH_VARARGS,
     "Waits for some user key event"},
    {"wait_button", py_entis_wait_key, METH_VARARGS,
     "Waits for some user button event"},
    {"clear_events", py_entis_clear_events, METH_VARARGS,
     "Clears buffered events"},
    {"point", py_entis_point, METH_VARARGS, "Draws point"},
    {"fill_circle", py_entis_fill_circle, METH_VARARGS, "Draws filled circle"},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef entismodule = {PyModuleDef_HEAD_INIT, "pyentis", NULL,
                                         -1, EntisMethods};

PyMODINIT_FUNC PyInit_pyentis(void) {
  if (PyType_Ready(&PyKeyEventType) < 0) {
    return NULL;
  }
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
  Py_INCREF(&PyKeyEventType);
  PyModule_AddObject(m, "KeyEvent", (PyObject*)&PyKeyEventType);
  return m;
}
