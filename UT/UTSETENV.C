#include <stdlib.h>
#include <slutil.h>

char * Point_Env;
char * Data_Env;


void utsetenv (void) {


    putenv ("POINT=XXXXXXXX");
    putenv ("DATA=XXXX");

    Point_Env = getenv ("POINT");
    Data_Env  = getenv ("DATA");
    return;
}

