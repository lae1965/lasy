#include <slutil.h>

extern char * Point_Env;
extern char * Data_Env;


void utp2env (void * p) {

    utl2crdx ((long) p,           Point_Env, 0, 16);
    utl2crdx ((long) utget_ds (), Data_Env,  0, 16);
    return;
}

