#include <slutil.h>
#include <stdlib.h>

void utalarm (void) {

    utspeakr (20  + random   (20));
    utsleept (4);
    utspeakr (0);
    return;
}

