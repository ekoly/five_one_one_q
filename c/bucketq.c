// Definitions and documentation on bucketq.

#include <Python.h>
#include <string.h>
#include "structmember.h"
#include "bucketq.h"


// define our subslot for bucketq sequence methods
static PySequenceMethods foo_bucketq_sq_methods = {
    (lenfunc)foo_bucketq_sq_length,             /*sq_length*/
    0,                                          /*sq_concat*/
    0,                                          /*sq_repeat*/
    0,                                          /*sq_item*/
    0,                                          /*was_sq_slice*/
    0,                                          /*sq_ass_item (???)*/
    0,                                          /*was_sq_ass_slice (???)*/
    0,                                          /*sq_contains*/
    0,                                          /*sq_inplace_concat*/
    0,                                          /*sq_inplace_repeat*/
};


// define our subslot for bucketq public methods
static PyMethodDef foo_bucketq_tp_methods[] = {
    {"push", _PyCFunction_CAST(foo_bucketq_tp_method_push), METH_FASTCALL, "Push an item onto the buckets."},
    {"pop", _PyCFunction_CAST(foo_bucketq_tp_method_pop), METH_FASTCALL, "Pop an item from the buckets and return it."},
    {"empty", _PyCFunction_CAST(foo_bucketq_tp_method_empty), METH_FASTCALL, "Return True if the queue is empty, and False otherwise."},
    {NULL, NULL, 0, NULL}
};

// define our members
static PyMemberDef foo_bucketq_tp_members[] = {
    {"first_out", T_INT, offsetof(foo_bucketq, first_out), READONLY, ""},
    {"_opid", T_INT, offsetof(foo_bucketq, opid), READONLY, ""},
    {"_buckets", T_OBJECT_EX, offsetof(foo_bucketq, buckets), READONLY, ""},
    {"_keyfunc", T_OBJECT_EX, offsetof(foo_bucketq, keyfunc), READONLY, ""},
    {NULL, 0, 0, 0, NULL}
};


// define our bucketq type object
static PyTypeObject HeapqType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "bucketq",                                  /*tp_name*/
    sizeof(foo_bucketq),                        /*tp_basicsize*/
    0,                                          /*tp_itemsize*/
    (destructor)foo_bucketq_tp_dealloc,         /*tp_dealloc*/
    0,                                          /*tp_print*/
    0,                                          /*tp_getattr*/
    0,                                          /*tp_setattr*/
    0,                                          /*tp_compare*/
    foo_bucketq_tp_repr,                        /*tp_repr*/
    0,                                          /*tp_as_number*/
    &foo_bucketq_sq_methods,                    /*tp_as_sequence*/
    0,                                          /*tp_as_mapping*/
    0,                                          /*tp_hash */
    0,                                          /*tp_call*/
    foo_bucketq_tp_str,                         /*tp_str*/
    PyObject_GenericGetAttr,                    /*tp_getattro*/
    PyObject_GenericSetAttr,                    /*tp_setattro*/
    0,                                          /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   /*tp_flags*/
    0,                                          /*tp_doc*/
    0,                                          /*tp_traverse*/
    (inquiry)foo_bucketq_tp_clear,              /*tp_clear*/
    0,                                          /*tp_richcompare*/
    0,                                          /*tp_weaklistoffset*/
    0,                                          /*tp_iter*/
    0,                                          /*tp_iternext*/
    foo_bucketq_tp_methods,                     /*tp_methods*/
    foo_bucketq_tp_members,                     /*tp_members*/
    0,                                          /*tp_getsets*/
    0,                                          /*tp_base*/
    0,                                          /*tp_dict*/
    0,                                          /*tp_descr_get*/
    0,                                          /*tp_descr_set*/
    0,                                          /*tp_dictoffset*/
    (initproc)foo_bucketq_tp_init,              /*tp_init*/
    0,                                          /*tp_alloc*/
    foo_bucketq_tp_new,                         /*tp_new*/
};

// begin constructor/destructor methods
static PyObject *foo_bucketq_tp_new(PyTypeObject *subtype, PyObject *args, PyObject *kwargs) {

    foo_bucketq *self;

    self = (foo_bucketq *)subtype->tp_alloc(subtype, 0);

    return (PyObject *)self;

}

static int foo_bucketq_tp_init(foo_bucketq *self, PyObject *args, PyObject *kwargs) {

    // buckets will be a list of 2-tuples of (priority, deque)
    PyObject *buckets, *keyfunc;
    int first_out;

    static char *kwlist[] = {"key", "first_out", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "Oi", kwlist, &keyfunc, &first_out)) {
        return -1;
    }

    Py_INCREF(keyfunc);

    if (first_out == LOWEST) {
        self->opid = Py_LT;
    } else if (first_out == HIGHEST) {
        self->opid = Py_GT;
    } else {
        Py_DECREF(keyfunc);
        PyErr_SetString(PyExc_ValueError, "first_out must be equal to five_one_one_bucketq.LOWEST or five_one_one_bucketq.HIGHEST");
        return -1;
    }

    self->first_out = first_out;

    buckets = PyList_New(0);
    if (buckets == NULL) {
        Py_DECREF(keyfunc);
        return -1;
    }

    self->buckets = buckets;
    self->keyfunc = keyfunc;

    return 0;

}

static void foo_bucketq_tp_clear(foo_bucketq *self) {

    Py_XDECREF(self->buckets);
    Py_XDECREF(self->keyfunc);

}

static void foo_bucketq_tp_dealloc(foo_bucketq *self) {

    foo_bucketq_tp_clear(self);
    Py_TYPE(self)->tp_free((PyObject *)self);

}

// begin sequence methods
static Py_ssize_t foo_bucketq_sq_length(PyObject *self) {

    PyObject *bucket, *deq;
    Py_ssize_t result, deqsize;

    PyObject *buckets = ((foo_bucketq *)self)->buckets;
    if (buckets == NULL) {
        return 0;
    }

    Py_INCREF(buckets);

    result = 0;

    for (Py_ssize_t ix = 0; ix < PyList_GET_SIZE(buckets); ix++) {
        bucket = PyList_GET_ITEM(buckets, ix);
        Py_INCREF(bucket);
        deq = PyTuple_GET_ITEM(bucket, 1);
        Py_DECREF(bucket);
        Py_INCREF(deq);
        deqsize = PyObject_Length(deq);
        Py_DECREF(deq);
        if (deqsize < 0) {
            Py_DECREF(buckets);
            return 0;
        }
        result += deqsize;
    }

    Py_DECREF(buckets);

    return result;

}

// begin repr and str
static PyObject *foo_bucketq_tp_repr(PyObject *self) {

    PyObject *formatstr = PyUnicode_FromString("<five_one_one_q.c.bucketq %s>");
    if (formatstr == NULL) {
        return NULL;
    }
    PyObject *selfstr = foo_bucketq_tp_str(self);
    if (selfstr == NULL) {
        Py_DECREF(formatstr);
        return NULL;
    }

    PyObject *formatargs = Py_BuildValue("(O)", selfstr);
    if (formatargs == NULL) {
        Py_DECREF(formatstr);
        Py_DECREF(selfstr);
        return NULL;
    }

    PyObject *res = PyUnicode_Format(formatstr, formatargs);
    if (res == NULL) {
        Py_DECREF(formatstr);
        Py_DECREF(selfstr);
        Py_DECREF(formatargs);
        return NULL;
    }

    Py_DECREF(formatstr);
    Py_DECREF(selfstr);
    Py_DECREF(formatargs);

    return res;

}

static PyObject *foo_bucketq_tp_str(PyObject *self) {

    PyObject *buckets = ((foo_bucketq *)self)->buckets;
    if (buckets == NULL) {
        return NULL;
    }

    PyObject *res = PyObject_Str(buckets);
    if (res == NULL) {
        return NULL;
    }

    return res;

}

// begin public methods
static PyObject *foo_bucketq_tp_method_push(PyObject *self, PyObject *const *args, Py_ssize_t nargs) {

    PyObject *buckets, *keyfunc, *item;
    int res;

    if (nargs != 1) {
        PyErr_SetString(PyExc_TypeError, "push expects exactly 1 argument.");
        return NULL;
    }

    item = args[0];
    Py_INCREF(item);

    buckets = ((foo_bucketq *)self)->buckets;
    Py_INCREF(buckets);
    keyfunc = ((foo_bucketq *)self)->keyfunc;
    Py_INCREF(keyfunc);

    res = foo_bucketq_push_imp(self, buckets, item, keyfunc, ((foo_bucketq *)self)->opid);
    Py_DECREF(item);
    Py_DECREF(buckets);
    Py_DECREF(keyfunc);
    if (res != 0) {
        return NULL;
    }

    #if _DEBUG >= 2
    if (foo_bucketq_log_debug_healthcheck(self) != 0) {
        return NULL;
    }
    #endif

    Py_RETURN_NONE;

}

static PyObject *foo_bucketq_tp_method_pop(PyObject *self, PyObject *const *args, Py_ssize_t nargs) {

    PyObject *buckets, *keyfunc, *res;

    if (nargs != 0) {
        PyErr_SetString(PyExc_TypeError, "pop expects exactly 0 argument.");
        return NULL;
    }

    // buckets will be a list of 2-tuples of (priority, deque)
    buckets = ((foo_bucketq *)self)->buckets;
    Py_INCREF(buckets);
    keyfunc = ((foo_bucketq *)self)->keyfunc;
    Py_INCREF(keyfunc);

    res = foo_bucketq_pop_imp(self, buckets);

    Py_DECREF(buckets);
    Py_DECREF(keyfunc);

    if (res == NULL) {
        return NULL;
    }

    // this will count empty buckets and if the number has crossed a threshold,
    // clears empty buckets, and changes cleaning strategy to cleaning immediately
    // after a bucket becomes empty
    if (foo_bucketq_calleverypop(self) != 0) {
        return NULL;
    }

    #if _DEBUG >= 2
    if (foo_bucketq_log_debug_healthcheck(self) != 0) {
        return NULL;
    }
    #endif

    return res;

}

static PyObject *foo_bucketq_tp_method_empty(PyObject *self, PyObject *const *args, Py_ssize_t nargs) {

    PyObject *buckets, *bucket, *deq;
    Py_ssize_t deqsize;

    if (nargs != 0) {
        PyErr_SetString(PyExc_TypeError, "pop expects exactly 0 argument.");
        return NULL;
    }

    buckets = ((foo_bucketq *)self)->buckets;
    Py_INCREF(buckets);

    for (Py_ssize_t ix = 0; ix < PyList_GET_SIZE(buckets); ix++) {
        bucket = PyList_GET_ITEM(buckets, ix);
        Py_INCREF(bucket);
        deq = PyTuple_GET_ITEM(bucket, 1);
        Py_DECREF(bucket);
        Py_INCREF(deq);
        deqsize = PyObject_Length(deq);
        Py_DECREF(deq);
        if (deqsize > 0) {
            Py_DECREF(buckets);
            Py_RETURN_FALSE;
        }
    }

    Py_DECREF(buckets);
    Py_RETURN_TRUE;

}

// private helper methods
static int ensure_collections() {

    if (_collections == NULL) {
        _collections = PyImport_ImportModule("collections");
        if (_collections == NULL) {
            return -1;
        }
        _deque_append_str = PyUnicode_FromString("append");
        if (_deque_append_str == NULL) {
            _collections = NULL;
            return -1;
        }
        _deque_popleft_str = PyUnicode_FromString("popleft");
        if (_deque_popleft_str == NULL) {
            _collections = NULL;
            return -1;
        }
    }

    return 0;

}

static PyObject *foo_bucketq_new_deque() {

    if (ensure_collections() != 0) {
        return NULL;
    }

    PyObject *deque_class = PyObject_GetAttrString(_collections, "deque");
    if (deque_class == NULL) {
        return NULL;
    }

    return PyObject_CallNoArgs(deque_class);

}

static Py_ssize_t foo_bucketq_bisect(PyObject *self, PyObject *buckets, PyObject *x_key, int opid) {

    Py_ssize_t lo, hi, mid;
    int cmp;
    PyObject *mid_bucket, *mid_bucket_key;

    lo = 0;
    hi = PyList_GET_SIZE(buckets);

    while (lo < hi) {
        mid = (lo + hi) / 2;
        mid_bucket = PyList_GET_ITEM(buckets, mid);
        mid_bucket_key = PyTuple_GET_ITEM(mid_bucket, 0);
        cmp = PyObject_RichCompareBool(mid_bucket_key, x_key, opid); 
        if (cmp < 0) {
            return -1;
        }
        if (cmp > 0) {
            lo = mid + 1;
        } else {
            hi = mid;
        }
    }

    return lo;

}

static int foo_bucketq_push_imp(PyObject *self, PyObject *buckets, PyObject *item, PyObject *keyfunc, int opid) {

    PyObject *item_key, *bucket, *deq, *res;
    Py_ssize_t pos, size;
    int cmp;

    size = PyList_GET_SIZE(buckets);

    item_key = PyObject_CallOneArg(keyfunc, item);
    pos = foo_bucketq_bisect(self, buckets, item_key, opid);
    if (pos < 0) {
        Py_DECREF(item_key);
        return -1;
    }

    if (pos >= size) {
        // special case where new bucket needs to be appended
        deq = foo_bucketq_new_deque();
        bucket = Py_BuildValue("(OO)", item_key, deq);
        // if either of the above calls returned NULL, will be caught here
        if (PyList_Append(buckets, bucket) != 0) {
            Py_DECREF(item_key);
            Py_DECREF(bucket);
            return -1;
        }
    } else {
        // need to determine if the bucket matches our item's key
        bucket = PyList_GET_ITEM(buckets, pos);
        cmp = PyObject_RichCompareBool(PyTuple_GET_ITEM(bucket, 0), item_key, Py_EQ);
        if (cmp < 0) {
            Py_DECREF(item_key);
            return -1;
        }
        if (cmp > 0) {
            // no changes needed, we don't need the item key ref any more
            Py_DECREF(item_key);
            deq = PyTuple_GET_ITEM(bucket, 1);
        } else {
            // need new bucket
            deq = foo_bucketq_new_deque();
            bucket = Py_BuildValue("(OO)", item_key, deq);
            // if either of the above calls returned NULL, will be caught here
            if (PyList_Insert(buckets, pos, bucket) != 0) {
                Py_DECREF(item_key);
                Py_DECREF(bucket);
                return -1;
            }
        }
    }

    res = PyObject_CallMethodObjArgs(deq, _deque_append_str, item, NULL);
    if (res == NULL) {
        return -1;
    }
    Py_DECREF(res);

    return 0;

}

static PyObject *foo_bucketq_pop_imp(PyObject *self, PyObject *buckets) {

    PyObject *bucket, *deq, *res;
    Py_ssize_t size, ix;

    size = PyList_GET_SIZE(buckets);
    
    if (size == 0) {
        PyErr_SetString(PyExc_IndexError, "tried to pop from an empty deque");
        return NULL;
    }

    for (ix = 0; ix < size; ix++) {
        bucket = PyList_GET_ITEM(buckets, ix);
        Py_INCREF(bucket);
        deq = PyTuple_GET_ITEM(bucket, 1);
        Py_DECREF(bucket);
        Py_INCREF(deq);
        // We prefer to call `popleft()` first and then do error handling
        // in opposed to checking length every iteration
        res = PyObject_CallMethodNoArgs(deq, _deque_popleft_str);
        Py_DECREF(deq);
        if (res != NULL) {
            break;
        }
        PyErr_Clear();
    }

    if (res == NULL) {
        PyErr_SetString(PyExc_IndexError, "tried to pop from an empty queue");
        return NULL;
    }

    // if ix == max_empty_buckets then the min number of empty buckets is
    // max_empty_buckets + 1
    if (ix >= MAX_EMPTY_BUCKETS) {
        // force an empty bucket check if we are accumulating a lot of empty buckets
        if (foo_bucketq_periodic_check(self) != 0) {
            return NULL;
        }
    } else if (((foo_bucketq *)self)->cleaning_strategy == CLEAN_ON_EMPTY) {
        // if we get here, it is this function's responsibility to clean up
        // empty deqs
        size = PyObject_Length(deq);
        if (size == 0) {
            if (PyList_SetSlice(buckets, ix, ix+1, NULL) != 0) {
                Py_DECREF(bucket);
                return NULL;
            }
        }
    }

    return res;

}

static int foo_bucketq_calleverypop(PyObject *self) {
    // Keep count of operations and perform certain tasks every X number.
    // For very large numbers of operations, pop_counter may roll over,
    // which is ok for our purposes
    unsigned counter = ((foo_bucketq *)self)->pop_counter++;

    if ((((foo_bucketq *)self)->cleaning_strategy == CLEAN_PERIODICALLY) && (counter % CHECK_EVERY == 0)) {
        if (foo_bucketq_periodic_check(self) != 0) {
            return -1;
        }
    }

    return 0;

}

static int foo_bucketq_periodic_check(PyObject *self) {

    int res;
    PyObject *buckets = ((foo_bucketq *)self)->buckets;
    Py_INCREF(buckets);

    // count empty buckets
    res = foo_bucketq_count_empty_buckets(self, buckets);
    // error check
    if (res < 0) {
        Py_DECREF(buckets);
        return -1;
    }
    // if we've exceeded the max number of empty buckets, change cleaning strategy to
    // clearing buckets immediately after they become empty instead of periodically
    if (res > MAX_EMPTY_BUCKETS) {
        ((foo_bucketq *)self)->cleaning_strategy = CLEAN_ON_EMPTY;
        res = foo_bucketq_clean_empty_buckets(self, buckets);
        if (res < 0) {
            Py_DECREF(buckets);
            return -1;
        }
        PyObject *debug_args = Py_BuildValue(
            "(sii)",
            "Found %s empty buckets which exceeds the limit of %s, buckets will now be cleaned as soon as they become empty.",
            res,
            MAX_EMPTY_BUCKETS
        );
        if (foo_bucketq_log_debug(debug_args) != 0) {
            Py_DECREF(buckets);
            return -1;
        }
    }

    Py_DECREF(buckets);

    return 0;

}

static int foo_bucketq_count_empty_buckets(PyObject *self, PyObject *buckets) {

    PyObject *bucket, *deq;
    Py_ssize_t deqsize, ix;
    int num_empty = 0;

    for (ix = PyList_GET_SIZE(buckets)-1; ix >= 0; ix--) {
        bucket = PyList_GET_ITEM(buckets, ix);
        Py_INCREF(bucket);
        deq = PyTuple_GET_ITEM(bucket, 1);
        Py_DECREF(bucket);
        Py_INCREF(deq);
        deqsize = PyObject_Length(deq);
        Py_DECREF(deq);
        if (deqsize < 0) {
            return -1;
        }
        if (deqsize == 0) {
            num_empty++;
        }
    }

    return num_empty;

}

static int foo_bucketq_clean_empty_buckets(PyObject *self, PyObject *buckets) {

    PyObject *bucket, *deq;
    Py_ssize_t deqsize, ix;
    int num_empty = 0;

    for (ix = PyList_GET_SIZE(buckets)-1; ix >= 0; ix--) {
        bucket = PyList_GET_ITEM(buckets, ix);
        Py_INCREF(bucket);
        deq = PyTuple_GET_ITEM(bucket, 1);
        Py_DECREF(bucket);
        Py_INCREF(deq);
        deqsize = PyObject_Length(deq);
        Py_DECREF(deq);
        if (deqsize < 0) {
            return -1;
        }
        if (deqsize == 0) {
            if (PyList_SetSlice(buckets, ix, ix+1, NULL) != 0) {
                return -1;
            }
            num_empty++;
        }
    }

    return num_empty;

}

// logging methods
static int ensure_logging() {

    if (_logger == NULL) {

        _logging = PyImport_ImportModule("logging");
        if (_logging == NULL) {
            return -1;
        }
        _logger = PyObject_CallMethod(_logging, "getLogger", "(s)", "five_one_one_q.c");
        if (_logger == NULL) {
            Py_DECREF(_logging);
            _logging = NULL;
            return -1;
        }
        _logger_debug_str = PyUnicode_FromString("debug");
        if (_logger_debug_str == NULL) {
            Py_DECREF(_logging);
            _logging = NULL;
            Py_DECREF(_logger);
            _logger = NULL;
            return -1;
        }
    }

    return 0;

}

static int foo_bucketq_log_debug(PyObject *args) {
    // This function assumes `args` is not going to be used further by the caller
    // and thus it "steals" the reference

    if (args == NULL) {
        return -1;
    }

    if (ensure_logging() != 0) {
        Py_DECREF(args);
        return -1;
    }

    PyObject *debug_callable = PyObject_GetAttr(_logger, _logger_debug_str);
    if (debug_callable == NULL) {
        Py_DECREF(args);
        return -1;
    }

    PyObject *res = PyObject_Call(debug_callable, args, NULL);
    if (res == NULL) {
        Py_DECREF(args);
        Py_DECREF(debug_callable);
        return -1;
    }

    Py_DECREF(args);
    Py_DECREF(debug_callable);
    Py_DECREF(res);

    return 0;

}

static int foo_bucketq_log_debug_refcnt(PyObject *self, const char *label, PyObject *obj) {

    if (obj == NULL) {
        return -1;
    }

    PyObject *debug_args = Py_BuildValue("(ssi)", "refcount of %s: %s", label, obj->ob_refcnt);
    if (debug_args == NULL) {
        return -1;
    }

    return foo_bucketq_log_debug(debug_args);

}

static int foo_bucketq_log_debug_healthcheck(PyObject *self) {

    PyObject *buckets = ((foo_bucketq *)self)->buckets;
    Py_INCREF(buckets);

    for (int i = 0; i < PyList_GET_SIZE(buckets); i++) {

        PyObject *debug_args = Py_BuildValue("(sii)", "refcount of buckets[%s]: %s", i, PyList_GET_ITEM(buckets, i)->ob_refcnt);
        if (debug_args == NULL) {
            Py_DECREF(buckets);
            return -1;
        }

        if (foo_bucketq_log_debug(debug_args) != 0) {
            Py_DECREF(buckets);
            return -1;
        }

    }

    Py_DECREF(buckets);
    return 0;

}


// define our module methods
// currently no module methods
static PyMethodDef foo_bucketq_method_def[] = {
    {NULL, NULL, 0, NULL}
};

// define our module
static struct PyModuleDef foo_bucketq_module_def = {
    PyModuleDef_HEAD_INIT,
    "c",
    "Bucket queue module implemented in C.",
    -1,
    foo_bucketq_method_def,
};

// register our module and add the bucketq type to it
PyMODINIT_FUNC PyInit_c(void) {

    PyObject *bucketq_module = PyModule_Create(&foo_bucketq_module_def);

    PyModule_AddType(bucketq_module, &HeapqType);
    PyModule_AddIntConstant(bucketq_module, "LOWEST", LOWEST);
    PyModule_AddIntConstant(bucketq_module, "HIGHEST", HIGHEST);

    return bucketq_module;

}
