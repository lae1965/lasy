#include 	<alloc.h>
#include    <slutil.h>


char    * utallocb (word * len) {
    word    size = *len;
    char    *p;

    while (size > 1000) {
        if ((p = calloc (size, 1)) != NULL) break;
		size -= 100;
	}

    *len = size;
    return  p;
}

