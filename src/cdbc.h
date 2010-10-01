#ifndef __CDBC_H
# define __CDBC_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <jansson.h>
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>

#define CDBC_ERROR		-1	
#define CDBC_OK			0
#define CDBC_NOTFOUND		1
#define CDBC_SERVER		2
#define CDBC_STATUS		3	// check curl status
#define CDBC_NODBNAME		4	// Don't have db name

#ifndef TRUE
# define TRUE (1)
# define FALSE (0)
#endif

#define STR_STRUCT_DEFINED 1
typedef unsigned char uchar;

typedef struct {
	uchar *s;		/* Memory */
	unsigned long i;	/* Index to current pos. */
	unsigned long m;	/* Maximum allocated so-far */
 } str;

#define STRCHUNK	(256)	/* Minimum allocation size */

#include "str.h"

typedef struct _cdbc {
	char *baseurl;
	CURL *con;
	CURLcode status;
	long code;
	str	buf;				// dynstring buffer
	str	url;				// the real uri
	int	debuglevel;

	char *dbname;				// Current CouchDB database
	json_t *js;
	json_error_t jserr;

	// xp_debug_func debugfunc;
	struct curl_slist *hlist;		// Headerlist
} CDBC;


#define cdbc_curl_verbose(cdbc, verbose) \
	curl_easy_setopt(cdbc->con, CURLOPT_VERBOSE, verbose)
#define cdbc_curl_debuglevel(cdbc, lev) \
		(cdbc)->debuglevel = (lev)


CDBC *cdbc_new(char *);
void cdbc_free(CDBC *);
int cdbc_request(CDBC *, char *);

char *cdbc_body(CDBC *);
size_t cdbc_body_length(CDBC *cd);
int cdbc_body_tofile(CDBC *, FILE *);

int cdbc_usedb(CDBC *, char *);

int cdbc_get(CDBC *cd, char *docid);
int cdbc_create(CDBC *cd, char *docid, char *json);
int cdbc_create_js(CDBC *cd, char *docid, json_t *js);

json_t *cdbc_get_js(CDBC *cd, char *docid);

char *cdbc_get_js_string(CDBC *cd, char *field);
int cdbc_get_js_integer(CDBC *cd, char *field);

int cdbc_view_walk(CDBC *cd, char *ddoc, char *view, char *args, int (*func)(CDBC *, json_t *obj));

#endif /* __CDBC_H */
