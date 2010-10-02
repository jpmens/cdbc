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

### int cdbc_view_walk(CDBC *cd, int (*func)(CDBC *, json_t *obj), char *ddoc, char *view, char *arg, ...);

### char *cdbc_lasturl(CDBC *cd);

