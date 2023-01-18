#include <slwind.h>


void    wntogcur (bool insert, bool visi) {
    register  scl = N_V_STATE.byte_char - 1;

    wnsizcur (visi, (insert) ? ((scl < 10) ? scl - 1 : scl - 2) :
                              ((scl < 10) ? scl-3 : scl-5), scl, WN_CUR_ADJ);
    return;
}

