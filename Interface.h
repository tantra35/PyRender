#pragma once

struct IAlloc
{
	virtual void my_free() = 0;
};

typedef struct _CSimpleTypeVar CSimpleTypeVar;

typedef struct _IArray : public IAlloc
{
	virtual int get_count() = 0;
	virtual bool get_item(int i, CSimpleTypeVar* var) = 0;

	virtual int get_key() = 0;
	virtual bool get_value(CSimpleTypeVar* var) = 0;
	virtual bool get_valid() = 0;

	virtual void reset() = 0;
	virtual bool next(CSimpleTypeVar* var) = 0;
} IArray;

typedef struct _IDict : public IAlloc
{
	virtual int get_count() = 0;
	virtual bool get_item(char* name, CSimpleTypeVar* var) = 0;

	virtual char* get_key() = 0;
	virtual bool get_value(CSimpleTypeVar* var) = 0;
	virtual bool get_valid() = 0;

	virtual void reset() = 0;
	virtual bool next(char** name, CSimpleTypeVar* var) = 0;
} IDict;

typedef struct _CSimpleTypeVar
{
	typedef enum {VAR_LONG, VAR_FLOAT, VAR_STRING, VAR_NULL, VAR_ARRAY, VAR_DICT} VarType;
	VarType type;
	union
	{
		long lval;
		double fval;
		IArray* aval;
		IDict* dval;
		struct
		{
			char* str;
			size_t str_l;
		} sval;
	};
} CSimpleTypeVar;

struct IDbConnectionCursor : public IAlloc
{
	virtual IArray* get_description() = 0;
	virtual long get_rowcount() = 0;	

	virtual void close() = 0;
	virtual int execute(char* querytempl, IArray* params) = 0;
	virtual int executemany(char* querytempl, IArray* params) = 0;
	virtual IArray* fetchone() = 0;
	virtual IArray* fetchmany(long arraysize = -1) = 0;
	virtual IArray* fetchall() = 0;
};

struct IDbConnection
{
	virtual bool rollback() = 0;
	virtual bool commit() = 0;
	virtual IDbConnectionCursor* cursor() = 0;
};

struct IPyCountext
{
	virtual int write(const char* buff, int length) = 0;
	virtual void flush() = 0;

	virtual IDbConnection* get_connection() = 0;
	virtual bool get_data(CSimpleTypeVar* var) = 0;
};
