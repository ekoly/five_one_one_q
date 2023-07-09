// Declarations of public methods of bucketq.
// See bucketq.c for documentation and implementation details.

#ifndef _FOO_BUCKETQ_H
#define _FOO_BUCKETQ_H

#include <Python.h>

#define _DEBUG 1

const int LOWEST = 1, HIGHEST = 2;
PyObject *_collections = NULL;
PyObject *_deque_append_str = NULL;
PyObject *_deque_popleft_str = NULL;

// define our python type
typedef struct foo_bucketq {
    PyObject_HEAD
    int opid;
    PyObject *buckets;
    PyObject *keyfunc;
    PyObject *logger;
} foo_bucketq;

// allocation method declarations
static PyObject *foo_bucketq_tp_new(PyTypeObject *subtype, PyObject *args, PyObject *kwargs);
static void foo_bucketq_tp_clear(foo_bucketq *self);
static void foo_bucketq_tp_dealloc(foo_bucketq *self);
static int foo_bucketq_tp_init(foo_bucketq *self, PyObject *args, PyObject *kwargs);

// method declarations for the equivalents of "dunder" methods
static Py_ssize_t foo_bucketq_sq_length(PyObject *self);
static PyObject *foo_bucketq_tp_repr(PyObject *self);
static PyObject *foo_bucketq_tp_str(PyObject *self);

// public method declarations
static PyObject *foo_bucketq_tp_method_push(PyObject *self, PyObject *const *args, Py_ssize_t nargs);
static PyObject *foo_bucketq_tp_method_pop(PyObject *self, PyObject *const *args, Py_ssize_t nargs);

// helper methods, not intended for outside use
static int ensure_collections();
static PyObject *foo_bucketq_new_deque();
static Py_ssize_t foo_bucketq_bisect(PyObject *self, PyObject *buckets, PyObject *x_key, int opid);
static int foo_bucketq_push_imp(PyObject *self, PyObject *buckets, PyObject *item, PyObject *keyfunc, int opid);
static PyObject *foo_bucketq_pop_imp(PyObject *self, PyObject *buckets);

// logging methods, intended to only be used if _DEBUG > 0
static int foo_bucketq_ensure_logger(PyObject *self);
static int foo_bucketq_log_debug(PyObject *self, PyObject *args);
static int foo_bucketq_log_debug_refcnt(PyObject *self, const char *label, PyObject *obj);
static int foo_bucketq_log_debug_healthcheck(PyObject *self);

#endif
