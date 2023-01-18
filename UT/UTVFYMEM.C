#include    <alloc.h>
#include    <slutil.h>

static long     oldmem, newmem;

bool    utvfymem (long remain) {
    bool    retcode = YES;

    newmem = coreleft ();
    if (oldmem < remain && newmem < oldmem) retcode = NO;
    oldmem = newmem;
    return retcode;
}

