#pragma once

#undef _DEBUG
#include <Python.h>
#define _DEBUG
#include "Interface.h"

class CPyDbConnWraper : public IDbConnection
{
private:
	PyObject* dbh;
	
	CPyDbConnWraper(){};

public:
	CPyDbConnWraper(PyObject*);
	~CPyDbConnWraper();
	
public:
	bool commit();
	bool rollback();
	IDbConnectionCursor* cursor();
};
