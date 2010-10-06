# CDBC: CouchDB C API

Warning: this is a work in progress, and the API will change!

There are dozens of API implementations for [CouchDB](http://couchdb.apache.org/) in all sorts of languages, but I've yet to find a decent C implementation, which is why I'm implementing one myself. I know of some attempts, but they haven't wow'ed me:

* [Getting started with C](http://wiki.apache.org/couchdb/Getting_started_with_C) on the CouchDB Wiki.
* [Pillowtalk](http://www.sevenforge.com/pillowtalk/)

The _C_ouch_DB_ _C_ library (which has nothing at all to do with ODBC -- I noticed joke only later), is a wrapper which uses Daniel Stenberg's [libCurl](http://curl.haxx.se/libcurl/) for the HTTP transport, and Petri Lehtinen's [Jansson](http://www.digip.org/jansson/), a C library for encoding, decoding and manipulating [JSON](http://www.json.org/) data.

I chose [Jansson](http://www.digip.org/jansson/) over other JSON C implementations because I particulary liked its interface, and it worked out of the box for me.




## Function reference

### CDBC *cdbc_new(char *url);

`cdbc_new` creates a new CDBC object and allocates memory for it. The specified `url` is associated with this connection object and should include _scheme_, _host_ and, optionally a  _port_ separated by a colon. A single trailing slash in the URI is allowed.

	CDBC *cd;

	cd = cdbc_new("http://example.org:5984");
	if (cd == NULL) {
		// ... error
	}

### void cdbc_free(CDBC *);
### int cdbc_request(CDBC *, char *);

### char *cdbc_body(CDBC *, size_t *length);
### size_t cdbc_body_length(CDBC *cd);
### int cdbc_body_tofile(CDBC *, FILE *);

### int cdbc_usedb(CDBC *, char *);

### int cdbc_get(CDBC *cd, char *docid);
### int cdbc_create(CDBC *cd, char *docid, char *json);
### int cdbc_create_js(CDBC *cd, char *docid, json_t *js);

### json_t *cdbc_get_js(CDBC *cd, char *docid);

### char *cdbc_get_js_string(CDBC *cd, char *field);
### int cdbc_get_js_integer(CDBC *cd, char *field);

### int cdbc_view_walk(CDBC *cd, int (*func)(CDBC *, json_t *rowobj), char *ddoc, char *view, char *arg, ...);

### char *cdbc_lasturl(CDBC *cd);

