#include    <slutil.h>
#include    <io.h>
#include    <time.h>


void utuninam (char * pname, char * pext) {
    long    l;

    do {
        time (&l);  l %= 100000000L;
        utl2crdx (l, pname, -8, 10);
        if (pext != NULL) utchext (pname, pext);
    } while (access (pname, 0) == 0);
    return;
}

