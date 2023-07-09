// Declarations of public methods of heapq.
// See heapq.c for documentation and implementation details.

#ifndef _FOO_HEAPQ_H
#define _FOO_HEAPQ_H

#include <Python.h>

#define _DEBUG 2

const int LOWEST = 1, HIGHEST = 2;

// define our python type
typedef struct foo_heapq {
    PyObject_HEAD
    int opid;
    PyObject *heap;
    PyObject *keyfunc;
    PyObject *logger;
} foo_heapq;

// allocation method declarations
static PyObject *foo_heapq_tp_new(PyTypeObject *subtype, PyObject *args, PyObject *kwargs);
static void foo_heapq_tp_clear(foo_heapq *self);
static void foo_heapq_tp_dealloc(foo_heapq *self);
static int foo_heapq_tp_init(foo_heapq *self, PyObject *args, PyObject *kwargs);

// method declarations for the equivalents of "dunder" methods
static Py_ssize_t foo_heapq_sq_length(PyObject *self);
static PyObject *foo_heapq_tp_repr(PyObject *self);
static PyObject *foo_heapq_tp_str(PyObject *self);

// public method declarations
static PyObject *foo_heapq_tp_method_push(PyObject *self, PyObject *const *args, Py_ssize_t nargs);
static PyObject *foo_heapq_tp_method_pop(PyObject *self, PyObject *const *args, Py_ssize_t nargs);

// helper methods, not intended for outside use
static int foo_heapq_siftdown(PyObject *self, PyListObject *heap, Py_ssize_t lo, Py_ssize_t pos, PyObject *keyfunc, int opid);
static int foo_heapq_siftup(PyObject *self, PyListObject *heap, Py_ssize_t pos, PyObject *keyfunc, int opid);

static int foo_heapq_ensure_logger(PyObject *self);
static int foo_heapq_log_debug(PyObject *self, PyObject *args);
static int foo_heapq_log_debug_refcnt(PyObject *self, const char *label, PyObject *obj);
static int foo_heapq_log_debug_healthcheck(PyObject *self);

#endif
