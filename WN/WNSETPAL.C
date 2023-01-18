/***********************************************************************
    wnsetpal - Define entire palette of EGA colors

void wnsetpal (char *ptable);

char *ptable;   Pointer to table of seventeen color values

Description This function defines the physical colors to be
            displayed by attribute values and color dot values.
            The current video device must be an Enhanced Graphics
            Adapter (EGA).

            The first sixteen table values correspond to the sixteen
            possible attribute values and color dot values.  The
            seventeenth table value prescribes the border
            ("overscan") color.  (The Enhanced Color Display
            supports a border color only in 200-line modes.)

            Bits 0-5 correspond to the individual color signals,
            except that bit 4 corresponds to the intensity bit when
            used with the IBM Color Display.  Bits 6 and 7 of the
            color values are ignored.

            As an example, the EGA ROM BIOS defines the following
            color values when the adapter is reset:

            Entry  Value  R'    G'/I  B'    R     G     B  Color
                          5     4     3     2     1     0
            -----  -----  -------------------------------  --------
            0       0x00  0     0     0     0     0     0  black
            1       0x01  0     0     0     0     0     1  blue
            2       0x02  0     0     0     0     1     0  green
            3       0x03  0     0     0     0     1     1  cyan
            4       0x04  0     0     0     1     0     0  red
            5       0x05  0     0     0     1     0     1  magenta
            6       0x14  0     1     0     1     0     0  brown
            7       0x07  0     0     0     1     1     1  white
            8       0x38  1     1     1     0     0     0  dark gray
            9       0x39  1     1     1     0     0     1  light blue
            10      0x3a  1     1     1     0     1     0  light green
            11      0x3b  1     1     1     0     1     1  light cyan
            12      0x3c  1     1     1     1     0     0  light red
            13      0x3d  1     1     1     1     0     1  light magenta
            14      0x3e  1     1     1     1     0     1  yellow
            15      0x3f  1     1     1     1     1     1  intense white
            16 brd  0x00  0     0     0     0     0     0  black

*****************************************************************************/

#include "language.h"
#include <dos.h>
#include <slwind.h>

void wnsetpal (char *ptable) {
    union  REGS   inregs,outregs;
    struct SREGS  segregs;

    if (wnishigh()) {
        inregs.x.ax = 0x1002;
        inregs.x.dx = utoff ((ptable == NULL) ? Pal_Stnd : ptable);
        segregs.es  = utseg ((ptable == NULL) ? Pal_Stnd : ptable);
        int86x(0x10, &inregs, &outregs, &segregs);
    }
}

