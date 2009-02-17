
#include "stdafx.h"
#include "PHPCore.h"

extern "C"
{
#include <php.h>
#include <php_main.h>
#include <php_variables.h>
#include <TSRM.h>
#include <SAPI.h>
#include <zend_compile.h>
};

extern zend_class_entry* CPHPPyContext_class_entry;
extern zend_module_entry PHPPyRender_module;

static int
php_apache_sapi_ub_write(const char *str, uint str_length TSRMLS_DC)
{
	IPyCountext* ctx = (IPyCountext*)SG(server_context);
	return ctx->write(str, str_length);
};

static void
php_apache_sapi_flush(void *server_context)
{
	IPyCountext* ctx = (IPyCountext*)server_context;
	ctx->flush();

	//php_struct *ctx;
	//request_rec *r;
	//TSRMLS_FETCH();

	//ctx = server_context;

	///* If we haven't registered a server_context yet,
	//* then don't bother flushing. */
	//if (!server_context) {
	//	return;
	//}

	//r = ctx->r;

	//sapi_send_headers(TSRMLS_C);

	//r->status = SG(sapi_headers).http_response_code;
	//SG(headers_sent) = 1;

	//if (ap_rflush(r) < 0 || r->connection->aborted) {
	//	php_handle_aborted_connection();
	//}
};

static struct stat*
php_apache_sapi_get_stat(TSRMLS_D)
{
	/*
	php_struct *ctx = SG(server_context);

	ctx->finfo.st_uid = ctx->r->finfo.user;
	ctx->finfo.st_gid = ctx->r->finfo.group;
	ctx->finfo.st_dev = ctx->r->finfo.device;
	ctx->finfo.st_ino = ctx->r->finfo.inode;
	ctx->finfo.st_atime = ctx->r->finfo.atime/1000000;
	ctx->finfo.st_mtime = ctx->r->finfo.mtime/1000000;
	ctx->finfo.st_ctime = ctx->r->finfo.ctime/1000000;

	ctx->finfo.st_size = ctx->r->finfo.size;
	ctx->finfo.st_nlink = ctx->r->finfo.nlink;

	return &ctx->finfo;
	*/
	return NULL;
};

static char *
php_apache_sapi_getenv(char *name, size_t name_len TSRMLS_DC)
{
	/*
	php_struct *ctx = SG(server_context);
	const char *env_var;

	env_var = apr_table_get(ctx->r->subprocess_env, name);

	return (char *) env_var;
	*/

	return NULL;
};

static int
php_apache_sapi_header_handler(sapi_header_struct *sapi_header,sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	/*
	php_struct *ctx;
	char *val;

	ctx = SG(server_context);

	val = strchr(sapi_header->header, ':');

	if (!val) {
	sapi_free_header(sapi_header);
	return 0;
	}

	*val = '\0';

	do {
	val++;
	} while (*val == ' ');

	if (!strcasecmp(sapi_header->header, "content-type")) {
	val = apr_pstrdup(ctx->r->pool, val);
	ap_set_content_type(ctx->r, val);
	} else if (sapi_header->replace) {
	apr_table_set(ctx->r->headers_out, sapi_header->header, val);
	} else {
	apr_table_add(ctx->r->headers_out, sapi_header->header, val);
	}

	sapi_free_header(sapi_header);
	*/
	return 0;
};

static int
php_apache_sapi_send_headers(sapi_headers_struct *sapi_headers TSRMLS_DC)
{
	//php_struct *ctx = SG(server_context);
	//const char *sline = SG(sapi_headers).http_status_line;

	//ctx->r->status = SG(sapi_headers).http_response_code;

	///* httpd requires that r->status_line is set to the first digit of
	//* the status-code: */
	//if (sline && strlen(sline) > 12 && strncmp(sline, "HTTP/1.", 7) == 0 
	//	&& sline[8] == ' ') {
	//		ctx->r->status_line = apr_pstrdup(ctx->r->pool, sline + 9);
	//	}

	return SAPI_HEADER_SENT_SUCCESSFULLY;
};

static int
php_apache_sapi_read_post(char *buf, uint count_bytes TSRMLS_DC)
{
	//apr_size_t len, tlen=0;
	//php_struct *ctx = SG(server_context);
	//request_rec *r;
	//apr_bucket_brigade *brigade;

	//r = ctx->r;
	//brigade = ctx->brigade;
	//len = count_bytes;

	///*
	//* This loop is needed because ap_get_brigade() can return us partial data
	//* which would cause premature termination of request read. Therefor we
	//* need to make sure that if data is available we fill the buffer completely.
	//*/

	//while (ap_get_brigade(r->input_filters, brigade, AP_MODE_READBYTES, APR_BLOCK_READ, len) == APR_SUCCESS) {
	//	apr_brigade_flatten(brigade, buf, &len);
	//	apr_brigade_cleanup(brigade);
	//	tlen += len;
	//	if (tlen == count_bytes || !len) {
	//		break;
	//	}
	//	buf += len;
	//	len = count_bytes - tlen;
	//}

	//return tlen;
	return 0;
};

static char *
php_apache_sapi_read_cookies(TSRMLS_D)
{
	//php_struct *ctx = SG(server_context);
	//const char *http_cookie;

	//http_cookie = apr_table_get(ctx->r->headers_in, "cookie");

	///* The SAPI interface should use 'const char *' */
	//return (char *) http_cookie;

	return NULL;
};

static void
php_apache_sapi_register_variables(zval *track_vars_array TSRMLS_DC)
{
	zval* PHPPyContext;
	MAKE_STD_ZVAL(PHPPyContext);
	object_init_ex(PHPPyContext, CPHPPyContext_class_entry); 

	php_register_variable_ex("PyContext", PHPPyContext, track_vars_array TSRMLS_CC);
};

static void php_apache_sapi_log_message(char *msg)
{
	//php_struct *ctx;
	//TSRMLS_FETCH();

	//ctx = SG(server_context);

	//if (ctx == NULL) { /* we haven't initialized our ctx yet, oh well */
	//	ap_log_error(APLOG_MARK, APLOG_ERR | APLOG_STARTUP, 0, NULL, "%s", msg);
	//} else {
	//	ap_log_rerror(APLOG_MARK, APLOG_ERR, 0, ctx->r, "%s", msg);
	//}
}  

static sapi_module_struct apache2_sapi_module =
{
	"apache2handler",
	"Apache 2.0 Handler",

		NULL,				/* startup */
		NULL,			/* shutdown */

		NULL,						/* activate */
		NULL,						/* deactivate */

		php_apache_sapi_ub_write,			/* unbuffered write */
		php_apache_sapi_flush,				/* flush */
		php_apache_sapi_get_stat,			/* get uid */
		php_apache_sapi_getenv,				/* getenv */

		php_error,					/* error handler */

		php_apache_sapi_header_handler,			/* header handler */
		php_apache_sapi_send_headers,			/* send headers handler */
		NULL,						/* send header handler */

		php_apache_sapi_read_post,			/* read POST data */
		php_apache_sapi_read_cookies,			/* read Cookies */

		php_apache_sapi_register_variables,
		php_apache_sapi_log_message,			/* Log message */
		NULL,
		"D:/Data/src/EmbedPhp/Debug/",

		STANDARD_SAPI_MODULE_PROPERTIES
};

bool Exec_PHP(IPyCountext* context, char* php_filename)
{
	TSRMLS_FETCH();	
	SG(server_context) = context;

	zval *local_retval=NULL;
	zend_file_handle zfd = {0};

	zfd.type = ZEND_HANDLE_FILENAME;
	zfd.filename = php_filename;
	zfd.free_filename = 0;
	zfd.opened_path = NULL;
	
	zend_first_try
	{
		php_request_startup(TSRMLS_C); 

		php_execute_script(&zfd, TSRMLS_C);
		sapi_flush(TSRMLS_C); 

		php_request_shutdown(NULL);
	}
	zend_end_try();

	return true;
};

bool Init_PHP()
{
#ifdef ZTS
	tsrm_startup(1, 1, 0, NULL);
#endif 
	sapi_startup(&apache2_sapi_module); 
	php_module_startup(&apache2_sapi_module, &PHPPyRender_module, 1);

	return true;
};
