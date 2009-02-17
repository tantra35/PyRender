#pragma once

#include "Interface.h"
#include "PyArray.h"
#include "PyDict.h"

inline void wrap_python_var(PyObject *item, CSimpleTypeVar* var)
{
	if(var)
	{
		if(PyInt_Check(item))
		{
			var->type = CSimpleTypeVar::VAR_LONG;
			var->lval = PyInt_AsLong(item);		
		}
		else if(PyLong_Check(item))
		{
			var->type = CSimpleTypeVar::VAR_LONG;
			var->lval = PyLong_AsLong(item);
		}
		else if(PyFloat_Check(item))
		{
			var->type = CSimpleTypeVar::VAR_FLOAT;			
			var->fval = PyFloat_AS_DOUBLE(item);		
		}
		else if(PyString_Check(item))
		{
			var->type = CSimpleTypeVar::VAR_STRING;
			PyString_AsStringAndSize(item, &(var->sval.str), (Py_ssize_t*)&(var->sval.str_l));
		}
		else if(PyDict_Check(item))
		{
			var->type = CSimpleTypeVar::VAR_DICT;
			var->dval = new CPyDict(item);
		}
		else if(PySequence_Check(item))
		{
			var->type = CSimpleTypeVar::VAR_ARRAY;
			var->aval = new CPyArray(item);
		}
		else
		{
			var->type = CSimpleTypeVar::VAR_NULL;
		};
	};
};