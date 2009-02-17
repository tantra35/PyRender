#pragma once

#include "PHPDbConnection.h"
#define BUFFMAXSIZE 256

typedef struct
{
	PyObject_HEAD
	PyObject* data;
	CPyDbConnWraper* dbcon;
	char buff[BUFFMAXSIZE];
	int buff_pos;
} Render_RenderObject;
