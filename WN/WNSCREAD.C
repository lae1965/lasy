/***************************************************************************
    wnscread - Return the displayed character and attribute

char wnscread (int *pfore, int *pback);

int *pfore;     Foreground attribute
int *pback;     Background attribute

Description This function returns the character at the current
            cursor position on the current display page.
            The display attribute byte at the current position is
            returned as the foreground and background display
            attributes.  However, these values are undefined if the
            screen is in a graphics mode.

Returns     Character read
            *pfore      Foreground attribute
            *pback      Background attribute
*************************************************************************/

#include <dos.h>
#include <slwind.h>

char wnscread (int *pfore, int *pback) {
    union REGS inregs,outregs;

    inregs.h.ah = 8;
    inregs.h.bh = (unsigned char) n_w_envr.n_v_page;
    int86 (WN_V_INT, &inregs, &outregs);

    *pfore = utlonyb(outregs.h.ah);
    *pback = uthinyb(outregs.h.ah);
    return (outregs.h.al);
}

