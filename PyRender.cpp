// PyRender.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#undef _DEBUG
#include <Python.h>
#define _DEBUG

#include <stdio.h>
#include "PHPCore.h"
#include "PyContext.h"
#include "PyRender.h"

static void
Render_RenderObject_dealloc(Render_RenderObject* self)
{
	delete self->dbcon;
	Py_DECREF(self->data);

	//удляем объект
	self->ob_type->tp_free((PyObject*)self); 
};

static int
Render_RenderObject_init(Render_RenderObject *self, PyObject *args, PyObject *kwds)
{
	PyObject* data;
	PyObject* PyBbcon;

	if(PyArg_ParseTuple(args, "OO", &PyBbcon, &data))
	{

		self->dbcon = new CPyDbConnWraper(PyBbcon);

		self->data  = data;
		Py_INCREF(self->data);
	};

	return 0;
};

static PyObject *
Render_RenderObject_render(Render_RenderObject* self, PyObject *args)
{
	char* templ;
	PyObject* retval = PyList_New(0); 
	PyObject* write = NULL;

	if(PyArg_ParseTuple(args, "s|O", &templ, &write))
	{
		CPyContext context(self, write?write:retval);
		Exec_PHP((IPyCountext*)&context, templ);
	};

	return retval;
};

static PyMethodDef Render_methods[] =
{
	{"render", (PyCFunction)Render_RenderObject_render, METH_VARARGS, "Return the name, combining the first and last name"},
	{NULL}
};

static PyTypeObject Render_RenderType = {
	PyObject_HEAD_INIT(NULL)
	0,                         /*ob_size*/
	"Render.Render",             /*tp_name*/
	sizeof(Render_RenderObject), /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	(destructor)Render_RenderObject_dealloc,                         /*tp_dealloc*/
	0,                         /*tp_print*/
	0,                         /*tp_getattr*/
	0,                         /*tp_setattr*/
	0,                         /*tp_compare*/
	0,                         /*tp_repr*/
	0,                         /*tp_as_number*/
	0,                         /*tp_as_sequence*/
	0,                         /*tp_as_mapping*/
	0,                         /*tp_hash */
	0,                         /*tp_call*/
	0,                         /*tp_str*/
	0,                         /*tp_getattro*/
	0,                         /*tp_setattro*/
	0,                         /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT,        /*tp_flags*/
	"Render objects",           /* tp_doc */
	0,		               /* tp_traverse */
	0,		               /* tp_clear */
	0,		               /* tp_richcompare */
	0,		               /* tp_weaklistoffset */
	0,		               /* tp_iter */
	0,		               /* tp_iternext */
	Render_methods,             /* tp_methods */
	NULL,             /* tp_members */
	0,                         /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)Render_RenderObject_init,      /* tp_init */
	NULL,                         /* tp_alloc */
	PyType_GenericNew
};

PyMODINIT_FUNC
initRender(void)
{	
	if (PyType_Ready(&Render_RenderType) >= 0)
	{
		if(Init_PHP())
		{
			PyObject* m;
			m = Py_InitModule("Render", NULL);

			Py_INCREF(&Render_RenderType);
			PyModule_AddObject(m, "Render", (PyObject *)&Render_RenderType);
		};
	};
};
