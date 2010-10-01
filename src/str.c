#include "str.h"

void dscat(str *st, char *s)
{
	while (s && *s) {
		dsadd(st, *s++);
	}
}
