#include "stdafx.h"
#undef _DEBUG
#include <Python.h>
#define _DEBUG

#include "PHPDbConnectionCursor.h"
#include "PHPDbConnection.h"

CPyDbConnWraper::CPyDbConnWraper(PyObject* dbh)
{
	this->dbh = dbh;
	Py_INCREF(this->dbh);
};

CPyDbConnWraper::~CPyDbConnWraper()
{
	Py_DECREF(this->dbh);
};

bool CPyDbConnWraper::commit()
{
	PyObject *result;
	bool retVal = true;
	PyObject *Method;

	Method = PyObject_GetAttrString(this->dbh, "commit");
	result = PyEval_CallObject(Method, NULL);

	//метод окончился провалом и было брошено Pythonic Исключение 
	if(result == NULL)
	{
		retVal = false;
		PyErr_Clear();
	}
	else
	{
		//декременируем счетчик ссылок - нам он не особо нужен результат
		Py_DECREF(result);
	};

	Py_DECREF(Method);		

	return retVal;
};

bool CPyDbConnWraper::rollback()
{
	PyObject *result;
	bool retVal = true;
	PyObject *Method;

	Method = PyObject_GetAttrString(this->dbh, "rollback");
	result = PyEval_CallObject(Method, NULL);

	//метод окончился провалом и было брошено Pythonic Исключение 
	if(result == NULL)
	{
		retVal = false;
		PyErr_Clear();
	}
	else
	{
		//декременируем счетчик ссылок - нам он не особо нужен результат
		Py_DECREF(result);
	};

	Py_DECREF(Method);

	return retVal;
};

IDbConnectionCursor* CPyDbConnWraper::cursor()
{
	PyObject *result;
	CPyDbConnCursorWraper* retVal = NULL;

	result = PyObject_CallMethod(this->dbh, "cursor", NULL);

	//метод окончился провалом и было брошено Pythonic Исключение 
	if(result != NULL)
	{
		retVal = new CPyDbConnCursorWraper(result);
		Py_DECREF(result);
	}
	else
	{
		PyErr_Clear();
	};

	return retVal;
};
