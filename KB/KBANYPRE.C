#include <slkeyb.h>

bool kbanypre (bool flush) {

    if (kbready ()) {
        if (flush)  kbflushb ();
        return (YES);
    }
    return (NO);
}

bool kbescpre (bool flush) {

    if (kbready ()) {
        if (Kb_ch == 27 && Kb_sc == 1) { kbflushb (); return (YES); }
        if (flush) kbflushb ();
    }
    return (NO);
}
