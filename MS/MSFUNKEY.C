#include <slmous.h>
#include <slwind.h>
#include <slkeyb.h>
#include <slchsc.def>

#define FUNCTION_CHAR KB_C_N_F1
#define SHIFT_ADD	  KB_S_S_F1-KB_S_N_F1
#define CONTROL_ADD   KB_S_C_F1-KB_S_N_F1
#define ALT_ADD       KB_S_A_F1-KB_S_N_F1

static	struct {
	int x1, x2;
} KeyLimits[10] = {
    { 0, 6}, { 7,14}, {15,22}, {23,30}, {31,38},
    {39,46}, {47,54}, {55,62}, {63,70}, {71,79}
};

bool msfunkey (int xp, int yp)
{
    register i;
    KBSTAT   status;
    int      scan ;

    if (yp == NUM_ROWS - 1) {
        for (i = 0; i < 10; i++) {
            if ( !utrange (xp, KeyLimits[i].x1, KeyLimits[i].x2)) {
                kbstatus (&status) ;
                scan = KB_S_N_F1 + i;
                if (status.right_shift || status.left_shift)
                    scan += SHIFT_ADD;
                else if (status.ctrl_shift)
                    scan += CONTROL_ADD;
                else if (status.alt_shift)
                    scan += ALT_ADD;
                kbplace (KB_TAIL, FUNCTION_CHAR, scan);
                return (YES);
            }
        }
    }
    return (NO);
}

/*
 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
1xxxxxx 2xxxxxx 3xxxxxx 4xxxxxx 5xxxxxx 6xxxxxx 7xxxxxx 8xxxxxx 9xxxxxx 10xxxxxx
*/
