#include <alloc.h>
#include <sldbas.h>


int dbopen (char * pfilnam, int doctype, bool db_global, int mode) {
    char        * path;
    register      hand;

    path = calloc (80, 1);
    if (db_global) strcpy (path, Db_Env);
    strcat (path, pfilnam);
    utchext (path, Db_ext [doctype]);

    hand = utopen (path, mode);

    free (path);
    return hand;
}

