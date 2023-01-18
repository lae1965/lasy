#include <dos.h>
#include <slwind.h>

void wnblinks (int option) {
    union REGS      inregs, outregs;

    if (!wnishigh()) return;
    inregs.x.ax = 0x1003;
    inregs.h.bl = (unsigned char) option;
    int86 (0x10, &inregs, &outregs);
    return;
}

