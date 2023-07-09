// Definitions and documentation on heapq.

#include <Python.h>
#include "structmember.h"
#include "heapq.h"


// define our subslot for heapq sequence methods
static PySequenceMethods foo_heapq_sq_methods = {
    (lenfunc)foo_heapq_sq_length,               /*sq_length*/
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


// define our subslot for heapq public methods
static PyMethodDef foo_heapq_tp_methods[] = {
    {"push", _PyCFunction_CAST(foo_heapq_tp_method_push), METH_FASTCALL, "Push an item onto the heap."},
    {"pop", _PyCFunction_CAST(foo_heapq_tp_method_pop), METH_FASTCALL, "Pop an item from the heap and return it."},
    {NULL, NULL, 0, NULL}
};

// define our members
static PyMemberDef foo_heapq_tp_members[] = {
    {"_opid", T_INT, offsetof(foo_heapq, opid), READONLY, ""},
    {"_heap", T_OBJECT_EX, offsetof(foo_heapq, heap), READONLY, ""},
    {"_keyfunc", T_OBJECT_EX, offsetof(foo_heapq, keyfunc), READONLY, ""},
    {NULL, 0, 0, 0, NULL}
};


// define our heapq type object
static PyTypeObject HeapqType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "heapq",                                    /*tp_name*/
    sizeof(foo_heapq),                          /*tp_basicsize*/
    0,                                          /*tp_itemsize*/
    (destructor)foo_heapq_tp_dealloc,           /*tp_dealloc*/
    0,                                          /*tp_print*/
    0,                                          /*tp_getattr*/
    0,                                          /*tp_setattr*/
    0,                                          /*tp_compare*/
    foo_heapq_tp_repr,                          /*tp_repr*/
    0,                                          /*tp_as_number*/
    &foo_heapq_sq_methods,                      /*tp_as_sequence*/
    0,                                          /*tp_as_mapping*/
    0,                                          /*tp_hash */
    0,                                          /*tp_call*/
    foo_heapq_tp_str,                           /*tp_str*/
    PyObject_GenericGetAttr,                    /*tp_getattro*/
    PyObject_GenericSetAttr,                    /*tp_setattro*/
    0,                                          /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,   /*tp_flags*/
    0,                                          /*tp_doc*/
    0,                                          /*tp_traverse*/
    (inquiry)foo_heapq_tp_clear,                /*tp_clear*/
    0,                                          /*tp_richcompare*/
    0,                                          /*tp_weaklistoffset*/
    0,                                          /*tp_iter*/
    0,                                          /*tp_iternext*/
    foo_heapq_tp_methods,                       /*tp_methods*/
    foo_heapq_tp_members,                       /*tp_members*/
    0,                                          /*tp_getsets*/
    0,                                          /*tp_base*/
    0,                                          /*tp_dict*/
    0,                                          /*tp_descr_get*/
    0,                                          /*tp_descr_set*/
    0,                                          /*tp_dictoffset*/
    (initproc)foo_heapq_tp_init,                /*tp_init*/
    0,                                          /*tp_alloc*/
    foo_heapq_tp_new,                           /*tp_new*/
};

// begin constructor/destructor methods
static PyObject *foo_heapq_tp_new(PyTypeObject *subtype, PyObject *args, PyObject *kwargs) {

    foo_heapq *self;

    self = (foo_heapq *)subtype->tp_alloc(subtype, 0);

    return (PyObject *)self;

}

static int foo_heapq_tp_init(foo_heapq *self, PyObject *args, PyObject *kwargs) {

    PyObject *heap, *keyfunc;
    int opid;

    static char *kwlist[] = {"key", "first_out", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "Oi", kwlist, &keyfunc, &opid)) {
        return -1;
    }

    Py_INCREF(keyfunc);

    if (opid != LOWEST && opid != HIGHEST) {
        Py_INCREF(PyExc_ValueError);
        PyErr_SetString(PyExc_ValueError, "first_out must be equal to five_one_one_heapq.LOWEST or five_one_one_heapq.HIGHEST");
        return -1;
    }

    if (opid == LOWEST) {
        self->opid = Py_LT;
    } else {
        self->opid = Py_GT;
    }

    heap = PyList_New(0);
    if (heap == NULL) {
        return -1;
    }

    self->heap = heap;
    self->keyfunc = keyfunc;

    Py_INCREF(keyfunc);

    return 0;

}

static void foo_heapq_tp_clear(foo_heapq *self) {

    Py_XDECREF(self->heap);
    Py_XDECREF(self->keyfunc);
    Py_XDECREF(self->logger);

}

static void foo_heapq_tp_dealloc(foo_heapq *self) {

    foo_heapq_tp_clear(self);
    Py_TYPE(self)->tp_free((PyObject *)self);

}

// begin sequence methods
static Py_ssize_t foo_heapq_sq_length(PyObject *self) {

    PyObject *heap = ((foo_heapq *)self)->heap;
    if (heap == NULL) {
        return 0;
    }

    Py_ssize_t res = PyList_GET_SIZE(heap);

    return res;

}

static PyObject *foo_heapq_tp_repr(PyObject *self) {

    PyObject *formatstr = PyUnicode_FromString("<five_one_one_q.c.heapq %s>");
    if (formatstr == NULL) {
        return NULL;
    }
    PyObject *selfstr = foo_heapq_tp_str(self);
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

static PyObject *foo_heapq_tp_str(PyObject *self) {

    PyObject *heap = ((foo_heapq *)self)->heap;
    if (heap == NULL) {
        return NULL;
    }

    PyObject *res = PyObject_Str(heap);
    if (res == NULL) {
        return NULL;
    }

    return res;

}

// begin public methods
static PyObject *foo_heapq_tp_method_push(PyObject *self, PyObject *const *args, Py_ssize_t nargs) {

    PyObject *heap, *keyfunc, *item;

    if (nargs != 1) {
        Py_INCREF(PyExc_TypeError);
        PyErr_SetString(PyExc_TypeError, "push expects exactly 1 argument.");
        return NULL;
    }

    item = args[0];

    heap = ((foo_heapq *)self)->heap;
    Py_INCREF(heap);
    keyfunc = ((foo_heapq *)self)->keyfunc;
    Py_INCREF(keyfunc);

    if (PyList_Append(heap, item) != 0) {
        Py_DECREF(heap);
        Py_DECREF(keyfunc);
        return NULL;
    }

    if (foo_heapq_siftdown(self, (PyListObject *)heap, 0, PyList_GET_SIZE(heap)-1, keyfunc, ((foo_heapq *)self)->opid) != 0) {
        Py_DECREF(heap);
        Py_DECREF(keyfunc);
        return NULL;
    }

    Py_DECREF(heap);
    Py_DECREF(keyfunc);

    #if _DEBUG >= 2
    if (foo_heapq_log_debug_healthcheck(self) != 0) {
        return NULL;
    }
    #endif

    Py_RETURN_NONE;

}

static PyObject *foo_heapq_tp_method_pop(PyObject *self, PyObject *const *args, Py_ssize_t nargs) {

    PyObject *heap, *keyfunc;
    int size;

    if (nargs != 0) {
        Py_INCREF(PyExc_TypeError);
        PyErr_SetString(PyExc_TypeError, "pop expects exactly 0 argument.");
        return NULL;
    }

    heap = ((foo_heapq *)self)->heap;
    Py_INCREF(heap);
    keyfunc = ((foo_heapq *)self)->keyfunc;
    Py_INCREF(keyfunc);

    size = PyList_GET_SIZE(heap);
    if (size == 0) {
        Py_DECREF(heap);
        Py_DECREF(keyfunc);
        Py_INCREF(PyExc_IndexError);
        PyErr_SetString(PyExc_IndexError, "cannot pop from an empty heap");
        return NULL;
    }

    PyObject *last = PyList_GET_ITEM(heap, size - 1);
    // we want to own a reference to last, for the time being
    Py_INCREF(last);

    if (PyList_SetSlice(heap, size - 1, size, NULL) != 0) {
        Py_DECREF(heap);
        Py_DECREF(keyfunc);
        Py_DECREF(last);
        return NULL;
    }

    size--;
    if (size == 0) {
        Py_DECREF(heap);
        Py_DECREF(keyfunc);
        return last;
    }

    PyObject *res = PyList_GET_ITEM(heap, 0);
    Py_INCREF(res);
    PyList_SET_ITEM(heap, 0, last);

    if (foo_heapq_siftup(self, (PyListObject *)heap, 0, keyfunc, ((foo_heapq *)self)->opid) != 0) {
        Py_DECREF(last);
        Py_DECREF(res);
        Py_DECREF(heap);
        Py_DECREF(keyfunc);
        return NULL;
    }

    Py_DECREF(last);
    Py_DECREF(heap);
    Py_DECREF(keyfunc);

    #if _DEBUG >= 2
    if (foo_heapq_log_debug_healthcheck(self) != 0) {
        return NULL;
    }
    #endif

    return res;

}

// begin helper methods
static int foo_heapq_siftdown(PyObject *self, PyListObject *heap, Py_ssize_t lo, Py_ssize_t pos, PyObject *keyfunc, int opid) {

    PyObject *item, *parent;
    PyObject *item_key = NULL, *parent_key = NULL;
    Py_ssize_t parentpos, size;
    int cmp;

    assert(PyList_Check(heap));
    size = PyList_GET_SIZE(heap);
    if (pos >= size) {
        PyErr_SetString(PyExc_IndexError, "index out of range");
        return -1;
    }

    // move parents down (to the right) until we find a place the item fits.
    item = PyList_GET_ITEM(heap, pos);

    item_key = PyObject_CallOneArg(keyfunc, item);
    if (item_key == NULL) {
        return -1;
    }

    while (pos > lo) {
        parentpos = (pos - 1) >> 1;
        parent = PyList_GET_ITEM(heap, parentpos);
        parent_key = PyObject_CallOneArg(keyfunc, parent);
        if (parent_key == NULL) {
            Py_DECREF(item_key);
            return -1;
        }
        Py_INCREF(item_key);
        Py_INCREF(parent_key);
        cmp = PyObject_RichCompareBool(item_key, parent_key, opid);
        Py_DECREF(parent_key);
        Py_DECREF(item_key);
        if (cmp < 0) {
            Py_DECREF(parent_key);
            Py_DECREF(item_key);
            return -1;
        }
        if (size != PyList_GET_SIZE(heap)) {
            Py_DECREF(parent_key);
            Py_DECREF(item_key);
            PyErr_SetString(PyExc_RuntimeError,
                            "list changed size during iteration");
            return -1;
        }
        if (cmp == 0) {
            break;
        }
        Py_DECREF(parent_key);
        parent = PyList_GET_ITEM(heap, parentpos);
        item = PyList_GET_ITEM(heap, pos);
        // set item steals a reference to our objects
        Py_INCREF(parent);
        Py_INCREF(item);
        PyList_SET_ITEM(heap, parentpos, item);
        PyList_SET_ITEM(heap, pos, parent);
        pos = parentpos;
    }

    Py_DECREF(item_key);
    if (parent_key != NULL) {
        Py_DECREF(parent_key);
    }

    return 0;

}

static int foo_heapq_siftup(PyObject *self, PyListObject *heap, Py_ssize_t pos, PyObject *keyfunc, int opid) {

    Py_ssize_t startpos, size, childpos, limit;
    PyObject *tmp1, *tmp2;
    int cmp;

    assert(PyList_Check(heap));
    size = PyList_GET_SIZE(heap);
    startpos = pos;
    if (pos >= size) {
        PyErr_SetString(PyExc_IndexError, "index out of range");
        return -1;
    }

    // children bubble up (to the left) until encountering a leaf.
    // leftmost position that has no children
    limit = size >> 1;
    while (pos < limit) {
        // childpos is index of index of the left child of `pos`.
        childpos = 2*pos + 1;
        if (childpos + 1 < size) {
            PyObject *a = PyList_GET_ITEM(heap, childpos);
            PyObject *b = PyList_GET_ITEM(heap, childpos + 1);
            PyObject *a_key = PyObject_CallOneArg(keyfunc, a);
            if (a_key == NULL) {
                return -1;
            }
            PyObject *b_key = PyObject_CallOneArg(keyfunc, b);
            if (b_key == NULL) {
                Py_DECREF(a_key);
                return -1;
            }
            Py_INCREF(a_key);
            Py_INCREF(b_key);
            cmp = PyObject_RichCompareBool(a_key, b_key, opid);
            Py_DECREF(a_key);
            Py_DECREF(b_key);
            if (cmp < 0) {
                Py_DECREF(a_key);
                Py_DECREF(b_key);
                return -1;
            }
            // if cmp is 0, increment childpos
            childpos += ((unsigned)cmp ^ 1);
            if (size != PyList_GET_SIZE(heap)) {
                Py_DECREF(a_key);
                Py_DECREF(b_key);
                PyErr_SetString(PyExc_RuntimeError, "list changed size during iteration");
                return -1;
            }
            Py_DECREF(a_key);
            Py_DECREF(b_key);
        }
        // smaller child swaps places with parent
        tmp1 = PyList_GET_ITEM(heap, childpos);
        tmp2 = PyList_GET_ITEM(heap, pos);
        // set item steals a reference to the objects
        Py_INCREF(tmp1);
        Py_INCREF(tmp2);
        PyList_SET_ITEM(heap, childpos, tmp2);
        PyList_SET_ITEM(heap, pos, tmp1);
        pos = childpos;
    }

    // Parents will now be sifted down (to the right)
    return foo_heapq_siftdown(self, heap, startpos, pos, keyfunc, opid);

}

static int foo_heapq_ensure_logger(PyObject *self) {

    if (((foo_heapq *)self)->logger == NULL) {

        PyObject *logging = PyImport_ImportModule("logging");
        if (logging == NULL) {
            return -1;
        }
        PyObject *logger = PyObject_CallMethod(logging, "getLogger", "(s)", "five_one_one_q.c");
        if (logger == NULL) {
            Py_DECREF(logging);
            return -1;
        }
        ((foo_heapq *)self)->logger = logger;
        Py_DECREF(logging);

    }

    return 0;

}

static int foo_heapq_log_debug(PyObject *self, PyObject *args) {

    if (foo_heapq_ensure_logger(self) != 0) {
        return -1;
    }

    PyObject *logger = ((foo_heapq *)self)->logger;
    PyObject *debug_callable = PyObject_GetAttrString(logger, "debug");
    if (debug_callable == NULL) {
        return -1;
    }

    PyObject *res = PyObject_Call(debug_callable, args, NULL);
    if (res == NULL) {
        Py_DECREF(debug_callable);
        return -1;
    }

    Py_DECREF(debug_callable);
    Py_DECREF(res);
    // note that we're stealing a reference to args
    Py_DECREF(args);

    return 0;

}

static int foo_heapq_log_debug_refcnt(PyObject *self, const char *label, PyObject *obj) {

    if (obj == NULL) {
        return -1;
    }

    PyObject *debug_args = Py_BuildValue("(ssi)", "refcount of %s: %s", label, obj->ob_refcnt);
    if (debug_args == NULL) {
        return -1;
    }

    return foo_heapq_log_debug(self, debug_args);

}

static int foo_heapq_log_debug_healthcheck(PyObject *self) {

    PyObject *heap = ((foo_heapq *)self)->heap;
    Py_INCREF(heap);

    for (int i = 0; i < PyList_GET_SIZE(heap); i++) {

        PyObject *debug_args = Py_BuildValue("(sii)", "refcount of heap[%s]: %s", i, PyList_GET_ITEM(heap, i)->ob_refcnt);
        if (debug_args == NULL) {
            Py_DECREF(heap);
            return -1;
        }

        if (foo_heapq_log_debug(self, debug_args) != 0) {
            Py_DECREF(heap);
            return -1;
        }

    }

    Py_DECREF(heap);
    return 0;

}


// define our module methods
// currently no module methods
static PyMethodDef foo_heapq_method_def[] = {
    {NULL, NULL, 0, NULL}
};

// define our module
static struct PyModuleDef foo_heapq_module_def = {
    PyModuleDef_HEAD_INIT,
    "c",
    "b-plus module implemented in C.",
    -1,
    foo_heapq_method_def,
};

// register our module and add the heapq type to it
PyMODINIT_FUNC PyInit_c(void) {

    PyObject *heapq_module = PyModule_Create(&foo_heapq_module_def);

    PyModule_AddType(heapq_module, &HeapqType);
    PyModule_AddIntConstant(heapq_module, "LOWEST", LOWEST);
    PyModule_AddIntConstant(heapq_module, "HIGHEST", HIGHEST);

    return heapq_module;

}
