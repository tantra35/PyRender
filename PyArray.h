#pragma once

#include "Interface.h"

class CPyArray : public IArray
{
private:
	PyObject* m_pyarr;
	int m_pos;

private:
	CPyArray(void);

public:
	CPyArray(PyObject* pyarr);
	~CPyArray(void);
	
public:
	void my_free();

	int get_count();
	bool get_item(int i, CSimpleTypeVar* var);

	int get_key();
	bool get_value(CSimpleTypeVar* var);
	bool get_valid();

	void reset();
	bool next(CSimpleTypeVar* var);
};
