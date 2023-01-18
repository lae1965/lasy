#include   <slutil.h>



void    utfree (void * p) {
    char ** q = (char **) p;

    free (* q); * q = NULL;
}

