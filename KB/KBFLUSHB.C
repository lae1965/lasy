#include <dos.h>
#include <slkeyb.h>

void kbflushb (void) {
    union REGS  regs;
    int far   * p;
    byte        cod;

    cod = kb101key() ? 0x10 : 0x00;

    p = uttofar (KB_DATASEG, KB_HEADLOC, int);

    while (* p != * (p + 1)) {
        regs.h.ah = cod;
        int86 (KB_BIOS_INT, &regs, &regs);
    }
    return;
}

bool kbvfysym (int sym) {
    int far   * p;

    p = uttofar (KB_DATASEG, KB_HEADLOC, int);
    while (* p != * (p + 1)) {
        kbgetkey (NULL);
        if (Kb_sym == sym) {
            kbflushb ();
            return YES;
        }
    }
    return NO;
}

