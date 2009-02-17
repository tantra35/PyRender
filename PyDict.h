#pragma once

#include "Interface.h"

class CPyDict : public IDict
{
private:
	PyObject* m_pydict;
	int m_pos;

private:
	CPyDict(void);

public:
	CPyDict(PyObject* pydict);
	~CPyDict(void);

public:
	void my_free();

	int get_count();
	bool get_item(char* name, CSimpleTypeVar* var);

	char* get_key();
	bool get_value(CSimpleTypeVar* var);
	bool get_valid();

	void reset();
	bool next(char** name, CSimpleTypeVar* var);
};
