#include "StdAfx.h"
#undef _DEBUG
#include <Python.h>
#define _DEBUG

#include "PyArray.h"
#include "PyDict.h"
#include "Wraper.h"

CPyArray::CPyArray(void)
{
}

CPyArray::CPyArray(PyObject* pyarr)
{
	m_pos = 0;
	m_pyarr = pyarr;
	Py_INCREF(m_pyarr);
};

CPyArray::~CPyArray(void)
{
	Py_DECREF(m_pyarr);
};

void CPyArray::my_free()
{
	delete this;
};

int CPyArray::get_count()
{
	return PySequence_Size(m_pyarr);
};

bool CPyArray::get_item(int i, CSimpleTypeVar* var)
{
	PyObject* item = PySequence_GetItem(m_pyarr, i);
	bool retval = false;

	if(item)
	{
		retval = true;
		wrap_python_var(item, var);
		Py_DECREF(item); 
	}
	else
	{
		PyErr_Clear();
	};

	return retval;
};

void CPyArray::reset()
{
	m_pos = 0;
};

int CPyArray::get_key()
{
	return m_pos;
};

bool CPyArray::get_value(CSimpleTypeVar* var)
{
	PyObject *item = PySequence_GetItem(m_pyarr, m_pos);
	bool retval = false;

	if(item)
	{		
		retval = true;
		wrap_python_var(item, var);				
		Py_DECREF(item); 
	}
	else
	{
		PyErr_Clear();
	};

	return retval;
};

bool CPyArray::get_valid()
{
	return m_pos < PySequence_Size(m_pyarr);
};

bool CPyArray::next(CSimpleTypeVar* var)
{
	PyObject *item = PySequence_GetItem(m_pyarr, m_pos); // возвращает borrow ссылку поэтому декремент счетчика ссылок после использовни€ не требуетьс€
	bool retval = false;
	m_pos += 1;
	
	if(item)
	{	
		retval = true;
		wrap_python_var(item, var);
		Py_DECREF(item);
	}
	else
	{
		PyErr_Clear();
	};
	
	return retval;
};
