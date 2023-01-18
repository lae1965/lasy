#include <dos.h>
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

static  char corr[] = {
                KB_S_N_END, KB_S_N_DOWN, KB_S_N_PGDN,
                KB_S_N_LEFT,KB_S_S_PAD5, KB_S_N_RIGHT,
                KB_S_N_HOME,KB_S_N_UP,   KB_S_N_PGUP,
                KB_S_N_INS,

                KB_S_N_DEL, KB_S_C_LEFT, KB_S_C_RIGHT,
                KB_S_C_DOWN,KB_S_C_INS,  KB_S_C_DEL,
                KB_S_C_HOME,KB_S_C_END,  KB_S_C_PGUP,
                KB_S_C_PGDN,KB_S_C_UP,   0
};

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

static void    kbcorrect (void);

int  kbgetkey (void (*fn1)(void)) {
    void      (*func)(void);
    HOTKEY     *hk;
    union REGS  regs;
    int far    *p;
    bool        done = NO, found;

    oldstat = 256;
    Kb_redraw = YES;
    p = uttofar (KB_DATASEG, KB_HEADLOC, int);
    while (!done) {
        do {
            if (Kb_fun != NULL) (* Kb_fun) ();
            kbgetsta ();
            if (oldstat != curstat) {
                if (fn1 != NULL) (* fn1) ();
                oldstat = curstat;
            }
            if (Kb_2_head != Kb_2_tail) {
                Key_Buf_Flag = NO;
                Kb_sym = * Kb_2_tail++;
                if (Kb_2_tail == Kb_2_buf + 16) Kb_2_tail = Kb_2_buf;
                return (Kb_sym);
            }
        } while (*p == *(p+1));

        Key_Buf_Flag = YES;

        regs.x.ax = 0;
        regs.h.ah = kb101key () ? 0x10 : 0x00;    /* 101-клавишная kb ? */

        int86 (KB_BIOS_INT, &regs, &regs);         /* Забрали символ из kbbuf*/

        reg_ah = regs.h.ah;
        reg_al = regs.h.al;

        kbcorrect ();

        Kb_sc  =    (int) reg_ah;
        Kb_ch  =    (int) reg_al;
        cursym.h.scan =   reg_ah;
        Kb_sym  = cursym.x;

        found = NO;
        if (Key_Hot_Flag) {
            for (hk = Hot_Keys; hk != NULL; hk = hk->next) {
                if (hk->sym == cursym.x  &&  hk->fn !=  NULL) {
                    func = hk->fn;   (*func)();   found = YES;
                    break;
                }
            }
        }
        done = !found;
    }
    return (Kb_sym);
}

static void    kbcorrect (void) {
    char       *pcorr;

    if (Kb_status.num_state == 1 && cursym.h.status == 0) {
            if (reg_al == '/')      { reg_ah = 53; return; }
            else if (reg_al == '.') { reg_ah = 52; return; }
            else if (reg_al == '*') { reg_ah = 9;  return; }
            else if (reg_al == '-') { reg_ah = 12; return; }
            else if (reg_al == '+') { reg_ah = 13; return; }
            else if (reg_al == 13)  { reg_ah = 28; return; }
    }

    if (reg_ah != 0 && (pcorr = strchr (corr,reg_ah)) != NULL) {
        if (reg_al==0xE0 || (cursym.b.shift==1 && Kb_status.num_state == 0))
            { reg_al = 0;}
        else if (Kb_status.num_state == 1 && cursym.b.shift == 0) {
            if ((oldstat = (int) (pcorr - corr)) < 10)
                reg_ah = (byte) oldstat + 2;
        }
    }
    return;
}

void (* _Cdecl kbhotkey (int sym, void (*fn)(void)))() {
    HOTKEY  *hk, *hk_last;
    void    (*fr)() = NULL;

    for (hk = Hot_Keys; hk != NULL; hk = hk->next) {
        hk_last = hk;
        if (hk->sym == sym) { fr = hk->fn; hk->fn = fn; return (fr); }
    }
    if (fn == NULL)                         return (NULL);
    if ((hk = utalloc (HOTKEY)) == NULL)    return (NULL);
    if (Hot_Keys == NULL)  Hot_Keys = hk;
    else                   hk_last->next = hk;
    hk->sym = sym;  hk->fn = fn;    hk->next = NULL;
    return (NULL);
}

bool kb101key (void) {
    return ((*(char *)uttofaru (KB_DATASEG, KB_101KEYS) & 0x10) != 0);
}

bool kbready (void) {
    union REGS regs;
    int far *p;

    p = uttofar (KB_DATASEG, KB_HEADLOC, int);
    if (* p == * (p + 1))  return (NO);
    else {
        regs.x.ax = utpeekw (uttofaru (KB_DATASEG, * p));
        reg_ah = regs.h.ah;
        reg_al = regs.h.al;
        kbgetsta ();
        kbcorrect ();
        Kb_sc = (int) reg_ah;
        Kb_ch = (int) reg_al;
        cursym.h.scan =   reg_ah;
        Kb_sym  = cursym.x;
        return (YES);
    }
}

void    kbgetsta (void) {
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

void kbsetsta (KBSTAT *pkeybd) {
    utpokew (KB_SHIFTADDR, utpeekw (pkeybd));
    kbready ();
}

