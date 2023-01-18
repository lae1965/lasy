#include <slkeyb.h>


void kbsetsym (int sym) {

    * Kb_2_head++ = sym;
    if (Kb_2_head == Kb_2_tail) * Kb_2_head++ = sym;
    if (Kb_2_head == Kb_2_buf + 16) Kb_2_head = Kb_2_buf;
    return;
}

