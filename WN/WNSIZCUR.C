/***************************************************************************
    wnsizcur - Set the cursor template on current display page

int wnsizcur (int on, int high, int low, int adjust);

int on;         Cursor off indicator (0 = off)
int high;       The cursor upper scan line
int low;        The cursor lower scan line
int adjust;     WN_CUR_ADJ if cursor scan lines are to be adjusted
                WN_CUR_NAD if not.

Description This function sets the size and on/off state of the
            cursor on the current display page.  No action will be
            taken unless this page is active (displayed).

            The cursor size is determined by the upper and lower
            scan lines, whose values can be between 0 and 13 for the
            Monochrome Adapter, and 0 and 7 for the Color/Graphics
            Adapter.  If off is nonzero, the cursor is turned off
            (no display); otherwise it is on.

Returns     Value of "on" parameter (1 = on)

***************************************************************************/

#include "language.h"
#include <dos.h>
#include <slwind.h>

          /* Address of BIOS INFO bytes/character.        */
#define  BYTES_PER_CHAR (uttofaru(0x0040,0x0085))
#define  COMP_LOC  (uttofar(0x0040,0x0087,unsigned char))
#define  GET_COMP()  (utpeekb(COMP_LOC) & 1)
#define  SET_COMP()  (utpokeb(COMP_LOC, utpeekb(COMP_LOC) | 1) )
#define  RESET_COMP()  (utpokeb(COMP_LOC, utpeekb(COMP_LOC) & 0xFE) )

int wnsizcur (int on, int high, int low, int adjust) {
    union REGS      reg;
    word            b_p_c;

/*    if (!n_w_envr.known) return (0); */

    if (!on) {
        if (N_V_STATE.adapter == _EGA && GET_COMP ()) RESET_COMP ();   
                                                /* clear compensation bit */

        reg.h.ch = 0x30;        /* cursor off */
        reg.h.ah = 0x01;        /* Set cusor type */
        int86 (WN_V_INT, &reg, &reg);
        return (0);
    }
    if (adjust == WN_CUR_ADJ) {
        b_p_c = (unsigned) N_V_STATE.byte_char - 1;
        utbound (low,  0, b_p_c);
        utbound (high, 0, b_p_c);
        reg.h.ch = (char) min (low, high);
        reg.h.cl = (char) max (low, high);
        if (N_V_STATE.adapter == _EGA && !GET_COMP ()) SET_COMP ();   
                                                    /* set compensation bit */
    } else 
        { reg.h.ch = (char) low; reg.h.cl = (char) high; }
    reg.h.ah = 0x01;        /* Set cusor type */
    int86 (WN_V_INT, &reg, &reg);
    return (1);
}

