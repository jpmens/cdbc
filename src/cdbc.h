#ifndef __CDBC_H
# define __CDBC_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <jansson.h>
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include "str.h"

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

typedef struct _cdbc {
	char *baseurl;
	CURL *con;
	CURLcode status;
	long code;
	str	buf;				// dynstring buffer
	str	url;				// the real uri
	int	debuglevel;

	char *dbname;				// Current CouchDB database

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

#endif /* __CDBC_H */
