#include <dos.h>
#include <stdio.h>
#include <slkeyb.h>
#include <slchsc.def>
#include <slutil.h>

typedef struct hotkey {
    int             sym;
    void          (*fn)(void);
    struct hotkey  *next;
}   HOTKEY;

typedef union {
    struct {
        unsigned    sccode  :8;
        unsigned    shift   :1;
        unsigned    ctrl    :1;
        unsigned    alt     :1;
        unsigned    dummy   :5;
    } b;
    struct  {
        byte    scan;
        byte    status;
    } h;
    int     x;
} SYMB;

static  SYMB        cursym;
static  int         oldstat, curstat;
static  HOTKEY *    Hot_Keys = NULL;
static  byte        reg_al, reg_ah;

void                (* Kb_fun)  (void) = NULL;
bool                Key_Hot_Flag = YES, Key_Buf_Flag;
int                 Kb_ch, Kb_sc;
KBSTAT              Kb_status;
int                 Kb_sym;
bool                Kb_redraw;

static bool kb101 (void);
static void    getsta (void);

void main (void) {
    void      (*func)(void);
    HOTKEY     *hk;
    union REGS  regs;
    int far    *p;
    bool        done = NO, found;

    oldstat = 256;
    Kb_redraw = YES;

    while (1) {
        p = uttofar (KB_DATASEG, KB_HEADLOC, int);

        kbgetsta ();

        Key_Buf_Flag = YES;

        regs.x.ax = 0;
        regs.h.ah = kb101 () ? 0x10 : 0x00;    /* 101-клавишная kb ? */

        int86 (KB_BIOS_INT, &regs, &regs);         /* Забрали символ из kbbuf*/

        reg_ah = regs.h.ah;
        reg_al = regs.h.al;

        Kb_sc  =    (int) reg_ah;
        Kb_ch  =    (int) reg_al;
        cursym.h.scan =   reg_ah;
        Kb_sym  = cursym.x;

        printf ("Scan = %i\tChar = %i \tSym = % i\n", Kb_sc, Kb_ch, Kb_sym);
    }
    return;
}

static bool kb101 (void) {
    return ((*(char *)uttofaru (KB_DATASEG, KB_101KEYS) & 0x10) != 0);
}

static void    getsta (void) {
    cursym.x = 0;
    curstat = utpeekw (KB_SHIFTADDR);
    memcpy (&Kb_status, &curstat, sizeof (int));

    if (Kb_status.right_shift == 1 || Kb_status.left_shift == 1)
        cursym.b.shift = 1;
    cursym.b.ctrl = Kb_status.ctrl_shift;
    cursym.b.alt  = Kb_status.alt_shift;
    Kb_sym  = cursym.x;
    return;
}

