#include "StdAfx.h"

#include <zend.h>
#include <zend_list.h>
#include <zend_exceptions.h>
#include <zend_interfaces.h>

#include "PHPArray.h"

bool get_next_num_item(zval* arr, HashPosition* pos)
{
	int key_type = zend_hash_get_current_key_type_ex(Z_ARRVAL_P(arr), pos);

	while(key_type != HASH_KEY_IS_LONG && key_type != HASH_KEY_NON_EXISTANT)
	{
		zend_hash_move_forward_ex(Z_ARRVAL_P(arr), pos);
		key_type = zend_hash_get_current_key_type_ex(Z_ARRVAL_P(arr), pos);
	};

	return key_type == HASH_KEY_IS_LONG;
};

CPHPArray::CPHPArray(void)
{
}

CPHPArray::CPHPArray(zval* arr)
{
	m_arr = arr;
	ZVAL_ADDREF(m_arr);

	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(m_arr), &m_pos);
	get_next_num_item(m_arr, &m_pos);
};

CPHPArray::~CPHPArray(void)
{
	ZVAL_DELREF(m_arr);
};

void CPHPArray::my_free()
{
	delete this;
};

int CPHPArray::get_count()
{
	return zend_hash_num_elements(Z_ARRVAL_P(m_arr)); 
};

inline void get_item_helper(zval* zvar, CSimpleTypeVar* var)
{
	if(var)
	{
		switch(Z_TYPE_P(zvar))
		{
			case IS_STRING:
				var->type       = CSimpleTypeVar::VAR_STRING;
				var->sval.str   = Z_STRVAL_P(zvar);
				var->sval.str_l = Z_STRLEN_P(zvar);
			break;
			
			case IS_DOUBLE:
				var->type = CSimpleTypeVar::VAR_FLOAT;
				var->fval = Z_DVAL_P(zvar);
			break;

			case IS_LONG:
				var->type = CSimpleTypeVar::VAR_LONG;
				var->lval = Z_LVAL_P(zvar);
			break;

			case IS_ARRAY:
				var->type = CSimpleTypeVar::VAR_ARRAY;
				var->aval = new CPHPArray(zvar);
			break;

			default:
				var->type = CSimpleTypeVar::VAR_NULL;
		};
	};
};

bool CPHPArray::get_item(int i, CSimpleTypeVar* var)
{
	zval **tmp; 
	bool retval = false;

	if(zend_hash_index_find(Z_ARRVAL_P(m_arr), i, (void**)&tmp) == SUCCESS)
	{
		retval = true;
		get_item_helper(*tmp, var);
	};

	return retval;
};

int CPHPArray::get_key()
{
	ulong retval;
	zend_hash_get_current_key_ex(Z_ARRVAL_P(m_arr), NULL, NULL, &retval, 0, &m_pos);

	return retval;
};

bool CPHPArray::get_value(CSimpleTypeVar* var)
{
	zval **tmp; 
	bool retval = false;

	if(zend_hash_get_current_data_ex(Z_ARRVAL_P(m_arr),  (void**) &tmp, &m_pos) == SUCCESS)
	{
		get_item_helper(*tmp, var);
		retval = true;
	};

	return retval;
};

bool CPHPArray::get_valid()
{
	zval **tmp;
	return (zend_hash_get_current_data_ex(Z_ARRVAL_P(m_arr),  (void **) &tmp, &m_pos) == SUCCESS);
};

void CPHPArray::reset()
{
	zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(m_arr), &m_pos);
	get_next_num_item(m_arr, &m_pos);
};

bool CPHPArray::next(CSimpleTypeVar* var)
{
	zval **tmp; 
	bool retval = false;

	if(zend_hash_get_current_data_ex(Z_ARRVAL_P(m_arr),  (void **) &tmp, &m_pos) == SUCCESS)
	{
		retval = true;
		get_item_helper(*tmp, var);
		
		zend_hash_move_forward_ex(Z_ARRVAL_P(m_arr), &m_pos);
		get_next_num_item(m_arr, &m_pos);
	};

	return retval;
};
