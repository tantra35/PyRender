#pragma once

#include "Interface.h"

class CPHPArray : public IArray
{
private:
	zval* m_arr;
	HashPosition m_pos; 

private:
	CPHPArray(void);

public:
	CPHPArray(zval* arr);
	~CPHPArray(void);
	
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
