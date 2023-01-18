#include     <process.h>
#include     <slutil.h>


int  utspawn (char * tasknam, void * point) {
    char  pointbuf [9], databuf [5];

    utl2crdx ((long) point,       pointbuf, 0, 16);
    utl2crdx ((long) utget_ds (), databuf,  0, 16);

    return (spawnl (P_WAIT, tasknam, tasknam, pointbuf, databuf, NULL));
}

