#include "cdbc.h"
#include <string.h>

#define Cso(opt, val)   curl_easy_setopt((cd)->con, (opt), (val))
static size_t collect(void *ptr, size_t size, size_t nmemb, void *stream);
static size_t sender(void *ptr, size_t size, size_t nmemb, void *userdata);
static size_t curl_hdr(void *ptr, size_t size, size_t nmemb, void *stream);
static void cdbc_addheader(CDBC *cd, char *header);

static int curl_global;

static int debug_callback(CURL *con, int infotype, char *s, size_t len, void *v)
{
        switch (infotype) {
                case CURLINFO_DATA_IN:
                        fprintf(stderr, "<<----\n"); break;
                case CURLINFO_DATA_OUT:
                        fprintf(stderr, "---->>\n"); break;
        }
        // write(2, s, len);
        return (0);
}

CDBC *cdbc_new(char *baseurl)
{
	CDBC *cd;

	if (!curl_global) {
		curl_global = 1;
		curl_global_init(CURL_GLOBAL_DEFAULT);
	}

	if ((cd = (CDBC *)calloc(1, sizeof (struct _cdbc))) == NULL)
		return (NULL);

	cd->dbname	= NULL;

	cd->baseurl	= strdup(baseurl);

	/*
	 * Ensure base URL has no trailing slash; I'll be adding that
	 * later on, and CouchDB doesn't like the double-slash.
	 */

	if (cd->baseurl[strlen(cd->baseurl) - 1] == '/')
		cd->baseurl[strlen(cd->baseurl) - 1] = '\0';

	dsinit(&cd->buf);
	dsinit(&cd->url);

	if ((cd->con = curl_easy_init()) == NULL) {
		cdbc_free(cd);
		return (NULL);
	}

	cdbc_addheader(cd, "Content-type: application/json");
	cdbc_addheader(cd, "Y-JP: true");
	cdbc_addheader(cd, "Expect:");	// Turn it off

	Cso(CURLOPT_HTTPHEADER, cd->hlist);

	/*
	 * don't fail on error. Instead, I'll handle the error in the
	 * header callback, returning -1 and a prepared XML fault
	 */

	Cso(CURLOPT_FAILONERROR, 0);
	// Cso( CURLOPT_DEBUGFUNCTION, debug_callback);
	Cso(CURLOPT_FILE, stdout );
	Cso(CURLOPT_WRITEDATA, &cd->buf );		// `ptr' in collect()
	Cso(CURLOPT_WRITEFUNCTION, collect );

	Cso(CURLOPT_USERAGENT, "CDBC");

	// Cso( CURLOPT_DEBUGFUNCTION, debug_callback);
	Cso(CURLOPT_VERBOSE, 0);
	

	Cso(CURLOPT_ENCODING, "deflate");
	Cso(CURLOPT_ENCODING, "gzip");


	return (cd);
}

int cdbc_request(CDBC *cd, char *resource)
{
	if (!cd)
		return CDBC_ERROR;

	dsprep(&cd->buf);

	dsprep(&cd->url);

	dscat(&cd->url, cd->baseurl);

	if (resource && *resource) {
		if (*resource != '/') 
			dscat(&cd->url, "/");
		dscat(&cd->url, resource);
	}

	Cso( CURLOPT_URL, dsstring(&cd->url) );

	cd->status = curl_easy_perform(cd->con);
	if (cd->status != 0) {
		return CDBC_STATUS;
	}

	curl_easy_getinfo(cd->con, CURLINFO_RESPONSE_CODE, &cd->code);
	if (cd->code != 200)
	{
		if (cd->code == 404)
			return CDBC_NOTFOUND;
		return CDBC_SERVER;
			
	}

	return (CDBC_OK);

}

size_t cdbc_body_length(CDBC *cd)
{
	return (cd ? dslen(&cd->buf) : CDBC_ERROR);
}

char *cdbc_body(CDBC *cd)
{
	return (cd ? dsstring(&cd->buf) : NULL);
}

int cdbc_body_tofile(CDBC *cd, FILE *fp)
{
	if (!cd)
		return CDBC_ERROR;

	return fwrite(dsstring(&cd->buf), sizeof (char), dslen(&cd->buf), fp);
}

int cdbc_usedb(CDBC *cd, char *dbname)
{
	if (!cd)
		return CDBC_ERROR;

	if (cd->dbname) {
		free(cd->dbname);
	}

	cd->dbname = strdup(dbname);
}

int cdbc_get(CDBC *cd, char *docid)
{
	str uri;

	if (!cd)
		return CDBC_ERROR;

	if (!cd->dbname)
		return CDBC_NODBNAME;

	dsinit(&uri);

	dscat(&uri, cd->dbname);
	dsadd(&uri, '/');
	dscat(&uri, docid);

	return cdbc_request(cd, dsstring(&uri));

}

int cdbc_create(CDBC *cd, char *docid, char *json)
{
	if (!cd)
		return CDBC_ERROR;

	if (!cd->dbname)
		return CDBC_NODBNAME;

	dsprep(&cd->url);

	dscat(&cd->url, cd->baseurl);
	dsadd(&cd->url, '/');
	dscat(&cd->url, cd->dbname);
	if (docid && *docid) {
		dsadd(&cd->url, '/');
		dscat(&cd->url, docid);

		Cso( CURLOPT_UPLOAD, TRUE );	// a.k.a. _PUT
		Cso( CURLOPT_READFUNCTION, sender );
		Cso( CURLOPT_READDATA, json );
		Cso( CURLOPT_INFILESIZE, (long)strlen(json) );
	} else {
		Cso( CURLOPT_POST, TRUE );
		Cso( CURLOPT_POSTFIELDS, json );
		Cso( CURLOPT_POSTFIELDSIZE, strlen(json) );
	}

	Cso( CURLOPT_URL, dsstring(&cd->url) );


	cd->status = curl_easy_perform(cd->con);
	if (cd->status != 0) {
		return CDBC_STATUS;
	}

	curl_easy_getinfo(cd->con, CURLINFO_RESPONSE_CODE, &cd->code);
	if (cd->code != 200)
	{
		if (cd->code == 404)
			return CDBC_NOTFOUND;
		return CDBC_SERVER;
			
	}

	return (CDBC_OK);
}

static void cdbc_addheader(CDBC *cd, char *header)
{
	if (cd && header && *header) {
		cd->hlist = curl_slist_append(cd->hlist, header);
	}
}

void cdbc_free(CDBC *cd)
{
	if (cd) {
		free(cd->baseurl);

		if (cd->dbname)
			free(cd->dbname);

		dsfree(&cd->url);
		dsfree(&cd->buf);

		free(cd);
		cd = NULL;
	}
}



static size_t collect(void *ptr, size_t size, size_t nmemb, void *stream)
{
	str *st = (str *)stream;
	size_t nbytes, n;
	char *bp = (char *)ptr;

	nbytes = size * nmemb;

	/*
	fprintf(stdout, "[[[->");
	fwrite(ptr, size, nmemb, stdout);
	fprintf(stdout, "<-]]]");
	*/

	for (n = 0; n < nbytes; n++) {
		dsadd(st, *bp++);
	}

	return (nbytes);		// Indicate success
}

static size_t sender(void *ptr, size_t size, size_t nmemb, void *userdata)
{
	size_t len;
	char *json = (char *)userdata;


	size_t nbytes = size * nmemb;

	// printf("sender len: %ld\n", nbytes);

	len = strlen(json);

	memcpy(ptr, userdata, len);

	return len;
}
