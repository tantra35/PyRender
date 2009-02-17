#include "StdAfx.h"
#undef _DEBUG
#include <Python.h>
#define _DEBUG

#include "PyArray.h"
#include "PyDict.h"
#include "Wraper.h"

CPyDict::CPyDict(void)
{
};

CPyDict::CPyDict(PyObject* pydict)
{
	m_pos    = 0;
	m_pydict = pydict;

	Py_INCREF(m_pydict);
};

CPyDict::~CPyDict(void)
{
	Py_DECREF(m_pydict);
};

void CPyDict::my_free()
{
	delete this;
};

int CPyDict::get_count()
{
	return PyDict_Size(m_pydict);
};

bool CPyDict::get_item(char* name, CSimpleTypeVar* var)
{
	PyObject* item = PyDict_GetItemString(m_pydict, name);
	bool retval = false;

	if(item)
	{
		retval = true;
		wrap_python_var(item, var);
	};

	return retval;
};


char* CPyDict::get_key()
{
	int pos  = m_pos;
	PyObject* pykey;

	bool retval = PyDict_Next(m_pydict, &pos, &pykey, NULL);

	return PyString_AS_STRING(pykey);
};

bool CPyDict::get_value(CSimpleTypeVar* var)
{
	int pos  = m_pos;
	PyObject* pyval;

	bool retval = PyDict_Next(m_pydict, &pos, NULL, &pyval);
	wrap_python_var(pyval, var);

	return retval;
};

bool CPyDict::get_valid()
{
	int pos  = m_pos;
	return PyDict_Next(m_pydict, &pos, NULL, NULL);	
};

void CPyDict::reset()
{
	m_pos = 0;
};

bool CPyDict::next(char** name, CSimpleTypeVar* var)
{
	PyObject* pykey;
	PyObject* pyval;
	bool retval = PyDict_Next(m_pydict, &m_pos, &pykey, &pyval);

	if(retval)
	{
		wrap_python_var(pyval, var);

		if(name)
		{
			*name = PyString_AS_STRING(pykey);
		};
	};

	return retval;
};
