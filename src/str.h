#ifndef _DSTR_H_
# define _DSTR_H_
/*
 * Jan-Piet Mens <jpm@gin.pfm-mainz.de>
 * Create and manipulate dynamic strings. These routines
 * are implemented as * macros for increased performance.
 * Note, that all routines are passed a pointer to a dyn-
 * string.
 */

#include <stdlib.h>
#ifndef STR_STRUCT_DEFINED
typedef unsigned char uchar;

typedef struct {
	uchar *s;		/* Memory */
	unsigned long i;	/* Index to current pos. */
	unsigned long m;	/* Maximum allocated so-far */
 } str;
#endif /* STR_STRUCT_DEFINED */

#ifndef STRCHUNK
# define STRCHUNK	(64)	/* Minimum allocation size */
#endif

/* Initialize a dynamic string. Use if str is auto */
#define dsinit(st)  (st)->s = (uchar *)malloc(STRCHUNK), \
		    (st)->i = 0, (st)->m = STRCHUNK

/*
 * The next may only be used if the string has been declared
 * `static'. This will initialize the string if it is the
 * first call, and otherwise, it will re-start the string at
 * zero.
 *
 *	static str st;
 *
 *	dsprep(&st);
 */

#define dsprep(st)   { if ((st)->m < STRCHUNK) \
			dsinit((st)); \
		       else  (st)->i = 0; }

/*
 * Add the character 'c' to the string 'st'. If 'st' is full,
 * re-allocate to accomodate another STRCHUNK bytes. Always
 * NULL-terminate the target string 'st'. 
 */

#define dsadd(st, c)	do { \
			  if (((st)->i) >= ((st)->m - 1))   {\
				  (st)->s = (uchar *)realloc(\
				   (char *)(st)->s,(unsigned)((st)->m\
				    + STRCHUNK));\
				(st)->m += STRCHUNK;\
			  } \
			  (st)->s[(st)->i++] = (uchar)(c), \
			  (st)->s[(st)->i] = '\0' ; \
			} while (0)

/* Access the actual string */
#define dsstring(st)	((st)->s)

/* Destroy string */
#define dsfree(st)	if ((st)->s) (void)free((char *)(st)->s)

/* How long is the dynamic string ? */
#define dslen(st)	((long)( (st)->i ))

// void dscat(str *, char *);

#endif /* _DSTR_H_ */
