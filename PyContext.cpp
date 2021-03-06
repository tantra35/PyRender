#include "StdAfx.h"

#include "PyContext.h"
#include "Wraper.h"

CPyContext::CPyContext(void)
{
};

CPyContext::CPyContext(Render_RenderObject* PyRenderObject, PyObject* retval)
{
	m_retval = retval;
	Py_INCREF(m_retval);

	PyRenderObject->buff_pos = 0;
	is_callable = PyCallable_Check(m_retval);

	m_PyRenderObject = PyRenderObject;
	Py_INCREF(m_PyRenderObject);
};

CPyContext::~CPyContext(void)
{
	Py_DECREF(m_PyRenderObject);
	Py_DECREF(m_retval);
}

IDbConnection* CPyContext::get_connection()
{
	return m_PyRenderObject->dbcon;
};

bool CPyContext::get_data(CSimpleTypeVar* var)
{
	wrap_python_var(m_PyRenderObject->data, var);
	return true;
};

int CPyContext::write(const char* buff, int length)
{	
	if(buff)
	{
		if(is_callable)
		{
			int tail = BUFFMAXSIZE - m_PyRenderObject->buff_pos;

			if(tail >= length)
			{
				memcpy(&(m_PyRenderObject->buff[m_PyRenderObject->buff_pos]), buff, length);
				m_PyRenderObject->buff_pos += length;
			}
			else
			{
				memcpy(&(m_PyRenderObject->buff[m_PyRenderObject->buff_pos]), buff, tail);
				int datatail = length - tail;

				PyObject* result = PyEval_CallFunction(m_retval, "(s#)", m_PyRenderObject->buff, BUFFMAXSIZE);
				Py_XDECREF(result);

				m_PyRenderObject->buff_pos = 0;

				if((length - tail) > BUFFMAXSIZE)
				{
					PyObject* result = PyEval_CallFunction(m_retval, "(s#)", &buff[tail], datatail);
					Py_XDECREF(result);
				}
				else	
				{
					memcpy(&(m_PyRenderObject->buff[0]), &buff[tail], datatail);
					m_PyRenderObject->buff_pos = datatail;
				};
			};
		}
		else
		{
			PyObject* pystr	= PyString_FromStringAndSize(buff, length);
			PyList_Append(m_retval, pystr);

			Py_DECREF(pystr);
		};
	};

	return length;
};

void CPyContext::flush()
{
	if(is_callable)
	{
		PyObject* result = PyEval_CallFunction(m_retval, "(s#)", m_PyRenderObject->buff, m_PyRenderObject->buff_pos);
		Py_XDECREF(result);

		m_PyRenderObject->buff_pos = 0;
	};
};
