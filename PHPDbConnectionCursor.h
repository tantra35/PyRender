#pragma once

#include "Interface.h"

class CPyDbConnCursorWraper : public IDbConnectionCursor
{
private:
	PyObject* PyCursor;

	CPyDbConnCursorWraper(void){}

public:
	CPyDbConnCursorWraper(PyObject*);
	~CPyDbConnCursorWraper(void);

	void my_free();

	IArray* get_description();
	long get_rowcount();	

	void close();
	int execute(char* querytempl, IArray* params);
	int executemany(char* querytempl, IArray* params);	
	IArray* fetchone();
	IArray* fetchmany(long arraysize = -1);
	IArray* fetchall();
};
