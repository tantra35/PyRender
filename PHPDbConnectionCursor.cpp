#include "StdAfx.h"
#undef _DEBUG
#include <Python.h>
#define _DEBUG

#include "phpdbconnectioncursor.h"
#include "PyArray.h"

CPyDbConnCursorWraper::CPyDbConnCursorWraper(PyObject* PyCursor)
{
	this->PyCursor = PyCursor;
	Py_INCREF(this->PyCursor);

};

CPyDbConnCursorWraper::~CPyDbConnCursorWraper(void)
{
	Py_DECREF(this->PyCursor);
};

void CPyDbConnCursorWraper::my_free()
{
	delete this;
};

IArray* CPyDbConnCursorWraper::get_description()
{
	PyObject* pydescr = PyObject_GetAttrString(this->PyCursor, "description");
	IArray* retval = NULL;

	if(pydescr != Py_None)
	{
		retval = new CPyArray(pydescr);
	};

	Py_DECREF(pydescr);

	return retval;
};

long CPyDbConnCursorWraper::get_rowcount()
{
	PyObject* pyrowcount = PyObject_GetAttrString(this->PyCursor, "rowcount");
	long retVal = PyLong_AsLong(pyrowcount);

	Py_DECREF(pyrowcount);

	return retVal;
};

void CPyDbConnCursorWraper::close()
{
	PyObject* result = PyObject_CallMethod(this->PyCursor, "close", NULL);	

	if(result != NULL)
	{
		Py_DECREF(result);
	}
	else
	{
		PyErr_Clear();
	};
};

inline void execute_helper(PyObject* queryparams, IArray* params)
{
	CSimpleTypeVar var;
	params->reset();

	for(int i=0; params->next(&var); ++i)
	{
		PyObject* py_param;

		switch(var.type)
		{	
			case CSimpleTypeVar::VAR_STRING:
				py_param = PyString_FromStringAndSize(var.sval.str, var.sval.str_l);
			break;

			case CSimpleTypeVar::VAR_LONG:
				py_param = PyLong_FromLong(var.lval);
			break;

			default:
				py_param = Py_None;
				Py_INCREF(py_param);
		};

		PyList_Append(queryparams, py_param);
		Py_DECREF(py_param);
	};
};

int CPyDbConnCursorWraper::execute(char * operation, IArray* params)
{
	int retval = 0;
	PyObject* queryparams = PyList_New(0);
	
	if(params)
	{
		execute_helper(queryparams, params);
		params->my_free();
	};

	PyObject* result = PyObject_CallMethod(this->PyCursor, "execute", "sO", operation, queryparams);	
	Py_DECREF(queryparams);

	if(result == NULL)
	{
		PyErr_Clear();
	}
	else	
	{
		retval = PyInt_AS_LONG(result);
		Py_DECREF(result);
	};	

	return retval;
};

int CPyDbConnCursorWraper::executemany(char * operation, IArray* paramsarr)
{
	int retval = 0;
	PyObject* queryparamsarr = PyList_New(0);

	CSimpleTypeVar var;
	paramsarr->reset();

	for(int i=0; paramsarr->next(&var); ++i)
	{
		PyObject* queryparams = Py_None;

		if(var.type == CSimpleTypeVar::VAR_ARRAY)
		{
			queryparams = PyList_New(0);
			IArray* params = var.aval;

			execute_helper(queryparams, params);	
			params->my_free();
		};

		PyList_Append(queryparamsarr, queryparams);
		Py_DECREF(queryparams);
	};

	PyObject* result = PyObject_CallMethod(this->PyCursor, "executemany", "sO", operation, queryparamsarr);
	Py_DECREF(queryparamsarr);
	paramsarr->my_free();	

	if(result != NULL)
	{
		retval = PyInt_AS_LONG(result);
		Py_DECREF(result);			
	}
	else	
	{
		PyErr_Clear();
	};	

	return retval;	
};

IArray* CPyDbConnCursorWraper::fetchone()
{
	PyObject* result = PyObject_CallMethod(this->PyCursor, "fetchone", NULL);
	IArray* retval = NULL;

	if(result != NULL)
	{
		if(result != Py_None)
		{
			retval = new CPyArray(result);
		};

		Py_DECREF(result);
	}
	else
	{
		PyErr_Clear();
	};	

	return retval;
};

IArray* CPyDbConnCursorWraper::fetchmany(long arraysize)
{
	PyObject* arglist = (arraysize != -1)?Py_BuildValue("(l)", arraysize):NULL;
	PyObject* Method  = PyObject_GetAttrString(this->PyCursor, "fetchmany");
	PyObject* result  = PyEval_CallObject(Method, arglist);

	Py_XDECREF(arglist);
	Py_DECREF(Method);

	IArray* retval = NULL;

	if(result != NULL)
	{
		if(result != Py_None)
		{	
			retval = new CPyArray(result);
		};

		Py_DECREF(result);
	}
	else
	{
		PyErr_Clear();
	};	

	return retval;
};

IArray* CPyDbConnCursorWraper::fetchall()
{
	PyObject* result = PyObject_CallMethod(this->PyCursor, "fetchall", NULL);
	IArray* retval = NULL;

	if(result != NULL)
	{
		if(result != Py_None)
		{		
			retval = new CPyArray(result);
		};

		Py_DECREF(result);
	}
	else
	{
		PyErr_Clear();
	};	

	return retval;
};
