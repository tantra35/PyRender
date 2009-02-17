#pragma once

#include "Interface.h"
#include "PyRender.h"

class CPyContext : public IPyCountext
{
private:
	Render_RenderObject* m_PyRenderObject;
	PyObject* m_retval;
	bool is_callable;

private:
	CPyContext(void);

public:	
	CPyContext(Render_RenderObject* PyRenderObject, PyObject* retval);
	~CPyContext(void);

	int write(const char* buff, int length);
	void flush();

	IDbConnection* get_connection();
	bool get_data(CSimpleTypeVar* var);	
};
