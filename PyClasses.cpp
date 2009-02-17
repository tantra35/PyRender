#include "stdafx.h"
extern "C"
{
#include <php.h>
#include <zend.h>
#include <SAPI.h>
#include <zend_list.h>
#include <zend_exceptions.h>
#include <zend_interfaces.h>
#include <ext\spl\spl_iterators.h>
};

#include "Interface.h"
#include "PHPArray.h"

//-----------------------------------------------------------------------------
zend_class_entry* CPHPPyDbConnection_class_entry;
zend_class_entry* CPHPPyDbConnectionCursor_class_entry;
zend_class_entry* CPHPPyContext_class_entry;
zend_class_entry* CPHPPyContextException_class_entry;

zend_class_entry* g_CPHPPyArray_class_entry;
zend_class_entry* g_CPHPPyDict_class_entry;

static int g_dbconnrtype;
static int g_dbconncursorrtype;
static int g_pyarrayrtype;
static int g_pydictrtype;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
inline void get_item_helper(zval* zrval, CSimpleTypeVar* var, TSRMLS_D)
{
	switch(var->type)
	{
		case CSimpleTypeVar::VAR_STRING:
			ZVAL_STRINGL(zrval, var->sval.str, var->sval.str_l, 1);
		break;

		case CSimpleTypeVar::VAR_LONG:
			ZVAL_LONG(zrval, var->lval);
		break;
		
		case CSimpleTypeVar::VAR_FLOAT:
			ZVAL_DOUBLE(zrval, var->fval);
		break;

		case CSimpleTypeVar::VAR_ARRAY:
		{
			zval* zpyarrayres;
			MAKE_STD_ZVAL(zpyarrayres);

			object_init_ex(zrval, g_CPHPPyArray_class_entry);
			ZEND_REGISTER_RESOURCE(zpyarrayres, var->aval, g_pyarrayrtype); 
			add_property_zval(zrval, "pyarray", zpyarrayres);
		};
		break;

		case CSimpleTypeVar::VAR_DICT:
		{
			zval* zpydictres;
			MAKE_STD_ZVAL(zpydictres);

			object_init_ex(zrval, g_CPHPPyDict_class_entry);
			ZEND_REGISTER_RESOURCE(zpydictres, var->dval, g_pydictrtype); 
			add_property_zval(zrval, "pydict", zpydictres);
		};
		break;

		default:
			ZVAL_NULL(zrval);
	};
};

//-----------------------------------------------------------------------------
ZEND_FUNCTION(CPHPPyContext_get_connection)
{
	zval* dbconnres;
	MAKE_STD_ZVAL(dbconnres);

	IPyCountext* server_context = (IPyCountext*)SG(server_context);
	IDbConnection* dbconn   = server_context->get_connection();
	object_init_ex(return_value, CPHPPyDbConnection_class_entry); 

	ZEND_REGISTER_RESOURCE(dbconnres, dbconn, g_dbconnrtype); 
	add_property_zval(return_value, "dbconn", dbconnres);
};

ZEND_FUNCTION(CPHPPyContext_get_data)
{
	IPyCountext* server_context = (IPyCountext*)SG(server_context);
	CSimpleTypeVar var;

	server_context->get_data(&var);
	get_item_helper(return_value, &var, TSRMLS_C);	
};

static zend_function_entry CPHPPyContext_methods[] =
{
	ZEND_FALIAS(get_connection, CPHPPyContext_get_connection, NULL)
	ZEND_FALIAS(get_data, CPHPPyContext_get_data, NULL)
	{NULL, NULL, NULL}
};

//-----------------------------------------------------------------------------
ZEND_FUNCTION(CPHPPyDbConnection_commit)
{
	IDbConnection* dbconn;
	zval** dbconn_rid;

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "dbconn", 7, (void**)&dbconn_rid) == SUCCESS)
	{
		ZEND_FETCH_RESOURCE(dbconn, IDbConnection*, dbconn_rid, -1, NULL, g_dbconnrtype);

		if(dbconn)
		{
			bool retval = dbconn->commit();
			RETURN_BOOL(retval);
		}
		else
		{
			zend_error(E_ERROR, "Can't get dbconn from resource");	
		};
	}
	else
	{
		zend_error(E_ERROR, "Can't get dbconn property");	
	};
};

ZEND_FUNCTION(CPHPPyDbConnection_rollback)
{
	IDbConnection* dbconn;
	zval** dbconn_rid;
	
	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "dbconn", 7, (void**)&dbconn_rid) == SUCCESS)
	{
		ZEND_FETCH_RESOURCE(dbconn, IDbConnection*, dbconn_rid, -1, NULL, g_dbconnrtype);
		
		if(dbconn)
		{
			bool retval = dbconn->rollback();
			RETURN_BOOL(retval);
		}
		else
		{
			zend_error(E_ERROR, "Can't get dbconn from resource");	
		};
	}
	else
	{
		zend_error(E_ERROR, "Can't get dbconn property");	
	};
};

ZEND_FUNCTION(CPHPPyDbConnection_cursor)
{
	IDbConnection* dbconn;
	zval** dbconn_rid;

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "dbconn", 7, (void**)&dbconn_rid) == SUCCESS)
	{
		ZEND_FETCH_RESOURCE(dbconn, IDbConnection*, dbconn_rid, -1, NULL, g_dbconnrtype);

		if(dbconn)
		{
			zval *cursorres;
			MAKE_STD_ZVAL(cursorres);

			IDbConnectionCursor* cursor = dbconn->cursor();
			object_init_ex(return_value, CPHPPyDbConnectionCursor_class_entry); 

			ZEND_REGISTER_RESOURCE(cursorres, cursor, g_dbconncursorrtype); 
			add_property_zval(return_value, "dbconncursor", cursorres);
		}
		else
		{
			zend_error(E_ERROR, "Can't get dbconn from resource");	
		};
	}
	else
	{
		zend_error(E_ERROR, "Can't get dbconn property");	
	};
};


static zend_function_entry CPHPPyDbConnection_methods[] =
{
	ZEND_FALIAS(commit, CPHPPyDbConnection_commit, NULL)
	ZEND_FALIAS(rollback, CPHPPyDbConnection_rollback, NULL)
	ZEND_FALIAS(cursor, CPHPPyDbConnection_cursor, NULL)
	{NULL, NULL, NULL}
};

//-----------------------------------------------------------------------------
ZEND_FUNCTION(CPHPPyDbConnectionCursor_getprop)
{
	IDbConnectionCursor* dbconncur;
	zval** dbconncursor_rid;

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "dbconncursor", 13, (void**)&dbconncursor_rid) == SUCCESS)
	{
		char* propname;
		size_t propname_l;
		ZEND_FETCH_RESOURCE(dbconncur, IDbConnectionCursor*, dbconncursor_rid, -1, NULL, g_dbconncursorrtype);

		if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &propname, &propname_l) == SUCCESS)
		{
			if(strcmp(propname, "rowcount") == 0)
			{
				RETURN_LONG(dbconncur->get_rowcount());
			}
			else if(strcmp(propname, "description") == 0)
			{
				IArray* descr = dbconncur->get_description();

				if(descr)
				{
					zval* zpyarrayres;
					MAKE_STD_ZVAL(zpyarrayres);
					ZEND_REGISTER_RESOURCE(zpyarrayres, descr, g_pyarrayrtype);

					object_init_ex(return_value, g_CPHPPyArray_class_entry);
					add_property_zval(return_value, "pyarray", zpyarrayres);
				}
				else
				{
					RETURN_NULL();
				};									
			}
			else
			{
				zend_throw_exception(CPHPPyContextException_class_entry, "Unk cursor propery", 4 TSRMLS_CC); 
			};
		};
	}
	else
	{
		zend_throw_exception(CPHPPyContextException_class_entry, "Can't get dbconn property", 2 TSRMLS_CC); 
	};
};

ZEND_FUNCTION(CPHPPyDbConnectionCursor_close)
{
	IDbConnectionCursor* dbconncur;
	zval** dbconncursor_rid;

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "dbconncursor", 13, (void**)&dbconncursor_rid) == SUCCESS)
	{
		ZEND_FETCH_RESOURCE(dbconncur, IDbConnectionCursor*, dbconncursor_rid, -1, NULL, g_dbconncursorrtype);
		dbconncur->close();
	}
	else
	{
		zend_error(E_ERROR, "Can't get dbconn property");	
	};
};

ZEND_FUNCTION(CPHPPyDbConnectionCursor_execute)
{
	IDbConnectionCursor* dbconncur;
	zval** dbconncursor_rid;

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "dbconncursor", 13, (void**)&dbconncursor_rid) == SUCCESS)
	{
		ZEND_FETCH_RESOURCE(dbconncur, IDbConnectionCursor*, dbconncursor_rid, -1, NULL, g_dbconncursorrtype);

		zval* zqueryparams = NULL;
		char* qerytempl;
		size_t qerytempl_len;

		if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|a", &qerytempl, &qerytempl_len, &zqueryparams) == SUCCESS)
		{
			IArray* queryparams = zqueryparams?(new CPHPArray(zqueryparams)):NULL;
			RETURN_LONG(dbconncur->execute(qerytempl, queryparams));
		}
		else
		{
			RETURN_FALSE;
		};
	}
	else
	{
		zend_error(E_ERROR, "Can't get dbconn property");	
	};
};

ZEND_FUNCTION(CPHPPyDbConnectionCursor_executemany)
{
	IDbConnectionCursor* dbconncur;
	zval** dbconncursor_rid;

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "dbconncursor", 13, (void**)&dbconncursor_rid) == SUCCESS)
	{
		zval* zqueryparams = NULL;
		char* qerytempl;
		size_t qerytempl_len;
		ZEND_FETCH_RESOURCE(dbconncur, IDbConnectionCursor*, dbconncursor_rid, -1, NULL, g_dbconncursorrtype);

		if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sa", &qerytempl, &qerytempl_len, &zqueryparams) == SUCCESS)
		{
			IArray* queryparams = NULL;
			int rowscount;

			if(zqueryparams)
			{
				queryparams = new CPHPArray(zqueryparams);
			};

			rowscount = dbconncur->executemany(qerytempl, queryparams);								
			RETURN_LONG(rowscount);
		}
		else
		{
			RETURN_FALSE;
		};
	}
	else
	{
		zend_error(E_ERROR, "Can't get dbconn property");	
	};
};

ZEND_FUNCTION(CPHPPyDbConnectionCursor_fetchone)
{
	IDbConnectionCursor* dbconncur;
	zval** dbconncursor_rid;

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "dbconncursor", 13, (void**)&dbconncursor_rid) == SUCCESS)
	{
		ZEND_FETCH_RESOURCE(dbconncur, IDbConnectionCursor*, dbconncursor_rid, -1, NULL, g_dbconncursorrtype);
		IArray* row = dbconncur->fetchone();

		if(row)
		{
			zval* zpyarrayres;
			MAKE_STD_ZVAL(zpyarrayres);
			ZEND_REGISTER_RESOURCE(zpyarrayres, row, g_pyarrayrtype);

			object_init_ex(return_value, g_CPHPPyArray_class_entry);
			add_property_zval(return_value, "pyarray", zpyarrayres);			
		}
		else
		{
			RETURN_FALSE;
		};
	}
	else
	{
		zend_error(E_ERROR, "Can't get dbconn property");	
	};
};

ZEND_FUNCTION(CPHPPyDbConnectionCursor_fetchmany)
{
	IDbConnectionCursor* dbconncur;
	zval** dbconncursor_rid;

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "dbconncursor", 13, (void**)&dbconncursor_rid) == SUCCESS)
	{
		long arraysize = -1;
		ZEND_FETCH_RESOURCE(dbconncur, IDbConnectionCursor*, dbconncursor_rid, -1, NULL, g_dbconncursorrtype);

		if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &arraysize) == SUCCESS)
		{
			IArray* rows = dbconncur->fetchmany(arraysize);

			if(rows)
			{
				zval* zpyarrayres;
				MAKE_STD_ZVAL(zpyarrayres);
				ZEND_REGISTER_RESOURCE(zpyarrayres, rows, g_pyarrayrtype);

				object_init_ex(return_value, g_CPHPPyArray_class_entry);
				add_property_zval(return_value, "pyarray", zpyarrayres);
			}
			else
			{
				RETURN_FALSE;
			};
		}
		else
		{
			RETURN_FALSE;
		};
	}
	else
	{
		zend_error(E_ERROR, "Can't get dbconn property");	
	};
};

ZEND_FUNCTION(CPHPPyDbConnectionCursor_fetchall)
{
	IDbConnectionCursor* dbconncur;
	zval** dbconncursor_rid;

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "dbconncursor", 13, (void**)&dbconncursor_rid) == SUCCESS)
	{
		ZEND_FETCH_RESOURCE(dbconncur, IDbConnectionCursor*, dbconncursor_rid, -1, NULL, g_dbconncursorrtype);
		IArray* rows = dbconncur->fetchall();

		if(rows)
		{
			zval* zpyarrayres;
			MAKE_STD_ZVAL(zpyarrayres);
			ZEND_REGISTER_RESOURCE(zpyarrayres, rows, g_pyarrayrtype);

			object_init_ex(return_value, g_CPHPPyArray_class_entry);
			add_property_zval(return_value, "pyarray", zpyarrayres);		
		}
		else
		{
			RETURN_FALSE;
		};
	}
	else
	{
		zend_error(E_ERROR, "Can't get dbconn property");	
	};
};

static
ZEND_BEGIN_ARG_INFO_EX(CPHPPyDbConnectionCursor_getprop___get, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO() 

static zend_function_entry CPHPPyDbConnectionCursor_methods[] = 
{
	ZEND_FALIAS(__get, CPHPPyDbConnectionCursor_getprop,	CPHPPyDbConnectionCursor_getprop___get) 
	ZEND_FALIAS(close, CPHPPyDbConnectionCursor_close,	NULL) 
	ZEND_FALIAS(execute, CPHPPyDbConnectionCursor_execute,	NULL) 
	ZEND_FALIAS(executemany, CPHPPyDbConnectionCursor_executemany,	NULL) 
	ZEND_FALIAS(fetchone, CPHPPyDbConnectionCursor_fetchone,	NULL)
	ZEND_FALIAS(fetchmany, CPHPPyDbConnectionCursor_fetchmany,	NULL)	
	ZEND_FALIAS(fetchall, CPHPPyDbConnectionCursor_fetchall,	NULL)	
	{NULL, NULL, NULL}
};

static void CPHPPyDbConnectionCursor_destructor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	IDbConnectionCursor* pcursor = (IDbConnectionCursor*)rsrc->ptr;
	pcursor->my_free();
};

//-----------------------------------------------------------------------------
ZEND_FUNCTION(CPHPPyArray_offsetExists)
{
	IArray* pyarray;
	zval** pyarray_rid;

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "pyarray", 8, (void**)&pyarray_rid) == SUCCESS)
	{
		long index;
		ZEND_FETCH_RESOURCE(pyarray, IArray*, pyarray_rid, -1, NULL, g_pyarrayrtype);

		if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == SUCCESS)
		{
			RETURN_BOOL(pyarray->get_item(index, NULL));
		};
	}
	else
	{
		zend_error(E_ERROR, "Can't get pyarray property");	
	};
};

ZEND_FUNCTION(CPHPPyArray_offsetGet)
{
	IArray* pyarray;
	zval** pyarray_rid;

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "pyarray", 8, (void**)&pyarray_rid) == SUCCESS)
	{
		long index;
		CSimpleTypeVar var;	

		ZEND_FETCH_RESOURCE(pyarray, IArray*, pyarray_rid, -1, NULL, g_pyarrayrtype);

		if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index) == SUCCESS)
		{
			pyarray->get_item(index, &var);								
			get_item_helper(return_value, &var, TSRMLS_C);
		};
	}
	else
	{
		zend_error(E_ERROR, "Can't get pyarray property");	
	};
};

ZEND_FUNCTION(CPHPPyArray_offsetSet)
{
};

ZEND_FUNCTION(CPHPPyArray_offsetUnset)
{
};

ZEND_FUNCTION(CPHPPyArray_rewind)
{
	IArray* pyarray;
	zval** pyarray_rid;

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "pyarray", 8, (void**)&pyarray_rid) == SUCCESS)
	{
		ZEND_FETCH_RESOURCE(pyarray, IArray*, pyarray_rid, -1, NULL, g_pyarrayrtype);
		pyarray->reset();
	}
	else
	{
		zend_error(E_ERROR, "Can't get pyarray property");	
	};
};

ZEND_FUNCTION(CPHPPyArray_valid)
{
	IArray* pyarray;
	zval** pyarray_rid;

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "pyarray", 8, (void**)&pyarray_rid) == SUCCESS)
	{
		ZEND_FETCH_RESOURCE(pyarray, IArray*, pyarray_rid, -1, NULL, g_pyarrayrtype);
		RETURN_BOOL(pyarray->get_valid());
	}
	else
	{
		zend_error(E_ERROR, "Can't get pyarray property");	
	};
};

ZEND_FUNCTION(CPHPPyArray_key)
{
	IArray* pyarray;
	zval** pyarray_rid;

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "pyarray", 8, (void**)&pyarray_rid) == SUCCESS)
	{
		ZEND_FETCH_RESOURCE(pyarray, IArray*, pyarray_rid, -1, NULL, g_pyarrayrtype);
		RETURN_LONG(pyarray->get_key());
	}
	else
	{
		zend_error(E_ERROR, "Can't get pyarray property");	
	};
};

ZEND_FUNCTION(CPHPPyArray_current)
{
	IArray* pyarray;
	zval** pyarray_rid;

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "pyarray", 8, (void**)&pyarray_rid) == SUCCESS)
	{
		CSimpleTypeVar var;
		ZEND_FETCH_RESOURCE(pyarray, IArray*, pyarray_rid, -1, NULL, g_pyarrayrtype);		

		if(pyarray->get_value(&var))
		{			
			get_item_helper(return_value, &var, TSRMLS_C);			
		}
		else
		{
			RETURN_NULL();
		};
	}
	else
	{
		zend_error(E_ERROR, "Can't get pyarray property");	
	};
};

ZEND_FUNCTION(CPHPPyArray_next)
{
	IArray* pyarray;
	zval** pyarray_rid;

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "pyarray", 8, (void**)&pyarray_rid) == SUCCESS)
	{
		ZEND_FETCH_RESOURCE(pyarray, IArray*, pyarray_rid, -1, NULL, g_pyarrayrtype);
		pyarray->next(NULL);
	}
	else
	{
		zend_error(E_ERROR, "Can't get pyarray property");	
	};
};

ZEND_FUNCTION(CPHPPyArray_count)
{
	IArray* pyarray;
	zval** pyarray_rid;

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "pyarray", 8, (void**)&pyarray_rid) == SUCCESS)
	{
		ZEND_FETCH_RESOURCE(pyarray, IArray*, pyarray_rid, -1, NULL, g_pyarrayrtype);
		RETURN_LONG(pyarray->get_count());
	}
	else
	{
		zend_error(E_ERROR, "Can't get pyarray property");	
	};
};

static
ZEND_BEGIN_ARG_INFO_EX(CPHPPyArray_offsetGet_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO() 

static
ZEND_BEGIN_ARG_INFO_EX(CPHPPyArray_offsetSet_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, newval)
ZEND_END_ARG_INFO()

static zend_function_entry g_CPHPPyArray_methods[] = 
{
	ZEND_FALIAS(offsetExists, CPHPPyArray_offsetExists, CPHPPyArray_offsetGet_arginfo)
	ZEND_FALIAS(offsetGet, CPHPPyArray_offsetGet, CPHPPyArray_offsetGet_arginfo)
	ZEND_FALIAS(offsetSet, CPHPPyArray_offsetSet, CPHPPyArray_offsetSet_arginfo)
	ZEND_FALIAS(offsetUnset, CPHPPyArray_offsetUnset, CPHPPyArray_offsetGet_arginfo)

	ZEND_FALIAS(rewind, CPHPPyArray_rewind, NULL)
	ZEND_FALIAS(valid, CPHPPyArray_valid, NULL)
	ZEND_FALIAS(key, CPHPPyArray_key, NULL)
	ZEND_FALIAS(current, CPHPPyArray_current, NULL)
	ZEND_FALIAS(next, CPHPPyArray_next, NULL)

	ZEND_FALIAS(count, CPHPPyArray_count, NULL)
	{NULL, NULL, NULL}
};

static void CPHPPyArray_destructor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	IArray* pyarray = (IArray*)rsrc->ptr;
	pyarray->my_free();
};

//-----------------------------------------------------------------------------
ZEND_FUNCTION(CPHPPyDict_offsetExists)
{
	IDict* pydict;
	zval** pydict_rid;

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "pydict", 7, (void**)&pydict_rid) == SUCCESS)
	{
		ZEND_FETCH_RESOURCE(pydict, IDict*, pydict_rid, -1, NULL, g_pydictrtype);

		char* index;
		int index_l;

		if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &index, &index_l) == SUCCESS)
		{
			RETURN_BOOL(pydict->get_item(index, NULL));
		};
	}
	else
	{
		zend_error(E_ERROR, "Can't get pyarray property");	
	};
};

ZEND_FUNCTION(CPHPPyDict_offsetGet)
{
	IDict* pydict;
	zval** pydict_rid;

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "pydict", 7, (void**)&pydict_rid) == SUCCESS)
	{
		ZEND_FETCH_RESOURCE(pydict, IDict*, pydict_rid, -1, NULL, g_pydictrtype);

		char* index;
		int index_l;

		if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &index, &index_l) == SUCCESS)
		{
			CSimpleTypeVar var;

			pydict->get_item(index, &var);								
			get_item_helper(return_value, &var, TSRMLS_C);
		};
	}
	else
	{
		zend_error(E_ERROR, "Can't get pydict property");	
	};
};

ZEND_FUNCTION(CPHPPyDict_offsetSet)
{
};

ZEND_FUNCTION(CPHPPyDict_offsetUnset)
{
};

ZEND_FUNCTION(CPHPPyDict_rewind)
{
	IDict* pydict;
	zval** pydict_rid;

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "pydict", 7, (void**)&pydict_rid) == SUCCESS)
	{
		ZEND_FETCH_RESOURCE(pydict, IDict*, pydict_rid, -1, NULL, g_pydictrtype);
		pydict->reset();
	}
	else
	{
		zend_error(E_ERROR, "Can't get pydict property");	
	};
};

ZEND_FUNCTION(CPHPPyDict_valid)
{
	IDict* pydict;
	zval** pydict_rid;

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "pydict", 7, (void**)&pydict_rid) == SUCCESS)
	{
		ZEND_FETCH_RESOURCE(pydict, IDict*, pydict_rid, -1, NULL, g_pydictrtype);
		RETURN_BOOL(pydict->get_valid());
	}
	else
	{
		zend_error(E_ERROR, "Can't get pydict property");	
	};
};

ZEND_FUNCTION(CPHPPyDict_key)
{
	IDict* pydict;
	zval** pydict_rid;

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "pydict", 7, (void**)&pydict_rid) == SUCCESS)
	{
		ZEND_FETCH_RESOURCE(pydict, IDict*, pydict_rid, -1, NULL, g_pydictrtype);
		RETURN_STRING(pydict->get_key(), 1);
	}
	else
	{
		zend_error(E_ERROR, "Can't get pydict property");	
	};
};

ZEND_FUNCTION(CPHPPyDict_current)
{
	IDict* pydict;
	zval** pydict_rid;

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "pydict", 7, (void**)&pydict_rid) == SUCCESS)
	{
		ZEND_FETCH_RESOURCE(pydict, IDict*, pydict_rid, -1, NULL, g_pydictrtype);
		CSimpleTypeVar var;

		if(pydict->get_value(&var))
		{
			get_item_helper(return_value, &var, TSRMLS_C);			
		}
		else
		{
			RETURN_NULL();
		};
	}
	else
	{
		zend_error(E_ERROR, "Can't get pydict property");	
	};
};

ZEND_FUNCTION(CPHPPyDict_next)
{
	IDict* pydict;
	zval** pydict_rid;

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "pydict", 7, (void**)&pydict_rid) == SUCCESS)
	{
		ZEND_FETCH_RESOURCE(pydict, IDict*, pydict_rid, -1, NULL, g_pydictrtype);
		pydict->next(NULL, NULL);
	}
	else
	{
		zend_error(E_ERROR, "Can't get pydic property");
	};
};

ZEND_FUNCTION(CPHPPyDict_count)
{
	IDict* pydict;
	zval** pydict_rid;

	if(zend_hash_find(Z_OBJPROP_P(this_ptr), "pydict", 7, (void**)&pydict_rid) == SUCCESS)
	{
		ZEND_FETCH_RESOURCE(pydict, IDict*, pydict_rid, -1, NULL, g_pydictrtype);
		RETURN_LONG(pydict->get_count());
	}
	else
	{
		zend_error(E_ERROR, "Can't get pydict property");	
	};
};

static
ZEND_BEGIN_ARG_INFO_EX(CPHPPyDict_offsetGet_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, index)
ZEND_END_ARG_INFO() 

static
ZEND_BEGIN_ARG_INFO_EX(CPHPPyDict_offsetSet_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, index)
	ZEND_ARG_INFO(0, newval)
ZEND_END_ARG_INFO()

static zend_function_entry g_CPHPPyDict_methods[] = 
{
	ZEND_FALIAS(offsetExists, CPHPPyDict_offsetExists, CPHPPyDict_offsetGet_arginfo)
	ZEND_FALIAS(offsetGet, CPHPPyDict_offsetGet, CPHPPyDict_offsetGet_arginfo)
	ZEND_FALIAS(offsetSet, CPHPPyDict_offsetSet, CPHPPyDict_offsetSet_arginfo)
	ZEND_FALIAS(offsetUnset, CPHPPyDict_offsetUnset, CPHPPyDict_offsetGet_arginfo)

	ZEND_FALIAS(rewind, CPHPPyDict_rewind, NULL)
	ZEND_FALIAS(valid, CPHPPyDict_valid, NULL)
	ZEND_FALIAS(key, CPHPPyDict_key, NULL)
	ZEND_FALIAS(current, CPHPPyDict_current, NULL)
	ZEND_FALIAS(next, CPHPPyDict_next, NULL)

	ZEND_FALIAS(count, CPHPPyDict_count, NULL)
	{NULL, NULL, NULL}
};

static void CPHPPyDict_destructor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	IDict* pydict = (IDict*)rsrc->ptr;
	pydict->my_free();
};

//-----------------------------------------------------------------------------
static ZEND_MODULE_STARTUP_D(apache)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "PHPPyContext", CPHPPyContext_methods);
	CPHPPyContext_class_entry = zend_register_internal_class(&ce, TSRMLS_C);

	INIT_CLASS_ENTRY(ce, "PHPPyContextException", NULL);
	CPHPPyContextException_class_entry = zend_register_internal_class_ex(&ce, NULL, NULL, TSRMLS_C);
	
	g_dbconnrtype = zend_register_list_destructors_ex(NULL, NULL, "PHPPyDbConnectionRes", module_number);	
	INIT_OVERLOADED_CLASS_ENTRY(ce, "PHPPyDbConnection", CPHPPyDbConnection_methods, NULL, NULL, NULL);
	CPHPPyDbConnection_class_entry = zend_register_internal_class_ex(&ce, NULL, NULL, TSRMLS_C);

	g_dbconncursorrtype = zend_register_list_destructors_ex(CPHPPyDbConnectionCursor_destructor, NULL, "PHPPyDbConnectionCursorRes", module_number);
	INIT_OVERLOADED_CLASS_ENTRY(ce, "PHPPyDbConnectionCursor", CPHPPyDbConnectionCursor_methods, NULL, NULL, NULL);
	CPHPPyDbConnectionCursor_class_entry = zend_register_internal_class_ex(&ce, NULL, NULL, TSRMLS_C);
	CPHPPyDbConnectionCursor_class_entry->ce_flags |= ZEND_ACC_FINAL; 

	g_pyarrayrtype = zend_register_list_destructors_ex(CPHPPyArray_destructor, NULL, "PHPPyArrayRes", module_number);	
	INIT_CLASS_ENTRY(ce, "PHPPyArray", g_CPHPPyArray_methods);
	g_CPHPPyArray_class_entry = zend_register_internal_class(&ce, TSRMLS_C);
	zend_class_implements(g_CPHPPyArray_class_entry, TSRMLS_C, 3, spl_ce_ArrayAccess, spl_ce_Iterator, spl_ce_Countable);
	g_CPHPPyArray_class_entry->ce_flags |= ZEND_ACC_FINAL; 

	g_pydictrtype = zend_register_list_destructors_ex(CPHPPyDict_destructor, NULL, "PHPPyDictRes", module_number);
	INIT_CLASS_ENTRY(ce, "PHPPyDict", g_CPHPPyDict_methods);
	g_CPHPPyDict_class_entry = zend_register_internal_class(&ce, TSRMLS_C);
	zend_class_implements(g_CPHPPyDict_class_entry, TSRMLS_C, 3, spl_ce_ArrayAccess, spl_ce_Iterator, spl_ce_Countable);
	g_CPHPPyDict_class_entry->ce_flags |= ZEND_ACC_FINAL;

	return SUCCESS; 
}

static ZEND_MODULE_SHUTDOWN_D(apache)
{
	return SUCCESS;
} 

zend_module_entry PHPPyRender_module = {
	STANDARD_MODULE_HEADER,
		"apache2handler",
		NULL,
		PHP_MINIT(apache),
		PHP_MSHUTDOWN(apache),
		NULL,
		NULL, 
		NULL,
		NULL,
		STANDARD_MODULE_PROPERTIES
}; 