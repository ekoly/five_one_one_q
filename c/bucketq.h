// Declarations of public methods of bucketq.
// See bucketq.c for documentation and implementation details.

#ifndef _FOO_BUCKETQ_H
#define _FOO_BUCKETQ_H

#include <Python.h>

#define _DEBUG 1

// options for first_out=(LOWEST|HIGHEST)
const int LOWEST = 1;
const int HIGHEST = 2;
// cleaning strategy will change if a clean is performed and this many empty
// buckets are found
const int MAX_EMPTY_BUCKETS = 20;
// cleaning will be performed every this many operations
const int CHECK_EVERY = 200;
// cleaning strategies
const int CLEAN_PERIODICALLY = 0;
const int CLEAN_ON_EMPTY = 1;

PyObject *_collections = NULL;
PyObject *_deque_append_str = NULL;
PyObject *_deque_popleft_str = NULL;
PyObject *_logging = NULL;
PyObject *_logger = NULL;
PyObject *_logger_debug_str = NULL;

// define our python type
typedef struct foo_bucketq {
    PyObject_HEAD
    int opid;
    int first_out;
    PyObject *buckets;
    PyObject *keyfunc;
    int cleaning_strategy;
    unsigned pop_counter;
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
static PyObject *foo_bucketq_tp_method_empty(PyObject *self, PyObject *const *args, Py_ssize_t nargs);

// helper methods, not intended for outside use
static int ensure_collections();
static PyObject *foo_bucketq_new_deque();
static Py_ssize_t foo_bucketq_bisect(PyObject *self, PyObject *buckets, PyObject *x_key, int opid);
static int foo_bucketq_push_imp(PyObject *self, PyObject *buckets, PyObject *item, PyObject *keyfunc, int opid);
static PyObject *foo_bucketq_pop_imp(PyObject *self, PyObject *buckets);
static int foo_bucketq_calleverypop(PyObject *self);
static int foo_bucketq_periodic_check(PyObject *self);
static int foo_bucketq_count_empty_buckets(PyObject *self, PyObject *buckets);
static int foo_bucketq_clean_empty_buckets(PyObject *self, PyObject *buckets);

// logging methods, intended to only be used if _DEBUG > 0
static int ensure_logging();
static int foo_bucketq_log_debug(PyObject *args);
static int foo_bucketq_log_debug_refcnt(PyObject *self, const char *label, PyObject *obj);
static int foo_bucketq_log_debug_healthcheck(PyObject *self);

#endif
