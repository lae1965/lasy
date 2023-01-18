/*****************************************************************************
    wniniwin - Определяет начальные установки для функций "оконного" интерфейса.

int wninit (int mode, int rows);

int     mode;       Видео режим
int     rows;       Число строк для дисплея

Выход:  Число строк на экране или 0 в случае неудачи

*****************************************************************************/

#include <time.h>
#include <stdlib.h>
#include <dos.h>
#include <slmenu.h>
#include <slmous.h>
#include <slkeyb.h>

void far    wnhandms     (int ev, int bs, int xp, int yp, int xd, int yd);
static bool wnsetadp     (VIDEO_STATE *mvs);
static int  hard_handler (int errval, int ax, int bp, int si);

/************************************************************************/
/*                     Global variables                                 */
/************************************************************************/

int         Kb_2_buf [17], * Kb_2_head, * Kb_2_tail;
int         Err_Val;
int         Err_Disk;
int         Err_Hard = NO;
VIDEO_STATE N_V_STATE;              /* Current characteristics of adapter   */
CURNT_STATE n_w_envr;               /* Current window environment           */
WINDOW    * Win_List = NULL;        /* Pointer to WINDOW structures list    */
WINDOW    * Cur_Wind = NULL;        /* Window selected for I/O or NULL      */
WINDOW    * Lst_Wind = NULL;        /* Window witch last selected for I/O   */
MENU      * Cur_Menu = NULL;        /* Pointer to a current menu            */
int         Cmd[13] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
char Pal_Stnd [17] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x00
};
char Pal_Cur [17];

ATTRIB      Att_User   = {
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY),       /* TEXT Trade Mark  */
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY),       /* BORD             */
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY),       /* TITL             */
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY),       /* HIGH             */
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY),       /* PROT             */
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY),       /* BOLD             */
    wnscattr (SL_BLUE,  SL_WHITE|SL_INTENSITY),      /* ERR  Background  */
    wnscattr (SL_BLACK, SL_CYAN),                 /* SHDW Shadows     */
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY),       /* BLCK             */
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY),       /* AFLD             */
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY),       /* NFLD             */
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY) };     /* NICE             */

ATTRIB Att_Wind = {
    wnscattr (SL_WHITE,    SL_BLACK),                /* TEXT */
    wnscattr (SL_WHITE,    SL_RED),                  /* BORD */
    wnscattr (SL_WHITE,    SL_RED),                  /* TITL */
    wnscattr (SL_BLACK,    SL_WHITE | SL_INTENSITY), /* HIGH */
    wnscattr (SL_BLACK,    SL_WHITE | SL_INTENSITY), /* PROT */
    wnscattr (SL_CYAN,     SL_BROWN | SL_INTENSITY), /* BOLD */
    wnscattr (SL_CYAN,     SL_BROWN | SL_INTENSITY), /* ERR  */
    wnscattr (SL_CYAN,     SL_BROWN | SL_INTENSITY), /* SHDW */
    wnscattr (SL_BLUE,     SL_CYAN  | SL_INTENSITY), /* BLCK */
    wnscattr (SL_BLUE,     SL_CYAN  | SL_INTENSITY), /* AFLD */
    wnscattr (SL_BLUE,     SL_CYAN  | SL_INTENSITY), /* NFLD */
    wnscattr (SL_BLUE,     SL_CYAN  | SL_INTENSITY) };/* NICE */

ATTRIB Att_Mnu1 = {
    wnscattr (SL_CYAN,     SL_WHITE | SL_INTENSITY), /* TEXT */
    wnscattr (SL_CYAN,     SL_WHITE | SL_INTENSITY), /* BORD */
    wnscattr (SL_CYAN,     SL_WHITE | SL_INTENSITY), /* TITL */
    wnscattr (SL_BLACK,    SL_WHITE | SL_INTENSITY), /* HIGH */
    wnscattr (SL_BLACK,    SL_WHITE | SL_INTENSITY), /* PROT */
    wnscattr (SL_CYAN,     SL_BROWN | SL_INTENSITY), /* BOLD */
    wnscattr (SL_CYAN,     SL_BROWN | SL_INTENSITY), /* ERR  */
    wnscattr (SL_CYAN,     SL_BROWN | SL_INTENSITY), /* SHDW */
    wnscattr (SL_BLUE,     SL_CYAN  | SL_INTENSITY), /* BLCK */
    wnscattr (SL_BLUE,     SL_CYAN  | SL_INTENSITY), /* AFLD */
    wnscattr (SL_BLUE,     SL_CYAN  | SL_INTENSITY), /* NFLD */
    wnscattr (SL_BLUE,     SL_CYAN  | SL_INTENSITY) };/* NICE */

ATTRIB Att_Mnu2 = {
    wnscattr (SL_CYAN,     SL_WHITE | SL_INTENSITY), /* TEXT */
    wnscattr (SL_CYAN,     SL_WHITE | SL_INTENSITY), /* BORD */
    wnscattr (SL_CYAN,     SL_WHITE | SL_INTENSITY), /* TITL */
    wnscattr (SL_BLACK,    SL_WHITE | SL_INTENSITY), /* HIGH */
    wnscattr (SL_BLACK,    SL_WHITE | SL_INTENSITY), /* PROT */
    wnscattr (SL_CYAN,     SL_BROWN | SL_INTENSITY), /* BOLD */
    wnscattr (SL_CYAN,     SL_BROWN | SL_INTENSITY), /* ERR  */
    wnscattr (SL_CYAN,     SL_BROWN | SL_INTENSITY), /* SHDW */
    wnscattr (SL_BLUE,     SL_CYAN  | SL_INTENSITY), /* BLCK */
    wnscattr (SL_BLUE,     SL_CYAN  | SL_INTENSITY), /* AFLD */
    wnscattr (SL_BLUE,     SL_CYAN  | SL_INTENSITY), /* NFLD */
    wnscattr (SL_BLUE,     SL_CYAN  | SL_INTENSITY) };/* NICE */

ATTRIB Att_Chlp = {
    wnscattr (SL_MAGENTA,  SL_CYAN | SL_INTENSITY),  /* TEXT */
    wnscattr (SL_MAGENTA,  SL_CYAN | SL_INTENSITY),  /* BORD */
    wnscattr (SL_MAGENTA,  SL_WHITE| SL_INTENSITY),  /* TITL */
    wnscattr (SL_CYAN,     SL_WHITE| SL_INTENSITY),  /* HIGH */
    wnscattr (SL_CYAN,     SL_WHITE| SL_INTENSITY),  /* PROT */
    wnscattr (SL_MAGENTA,  SL_WHITE| SL_INTENSITY),  /* BOLD */
    wnscattr (SL_CYAN,     SL_BROWN | SL_INTENSITY), /* ERR  */
    wnscattr (SL_CYAN,     SL_BROWN | SL_INTENSITY), /* SHDW */
    wnscattr (SL_BLUE,     SL_CYAN  | SL_INTENSITY), /* BLCK */
    wnscattr (SL_BLUE,     SL_CYAN  | SL_INTENSITY), /* AFLD */
    wnscattr (SL_BLUE,     SL_CYAN  | SL_INTENSITY), /* NFLD */
    wnscattr (SL_BLUE,     SL_CYAN  | SL_INTENSITY) };/* NICE */

ATTRIB Att_Khlp = {
    wnscattr (SL_CYAN,     SL_BLACK),                /* TEXT */
    wnscattr (SL_MAGENTA,  SL_CYAN | SL_INTENSITY),  /* BORD */
    wnscattr (SL_MAGENTA,  SL_WHITE| SL_INTENSITY),  /* TITL */
    wnscattr (SL_CYAN,     SL_WHITE| SL_INTENSITY),  /* HIGH */
    wnscattr (SL_CYAN,     SL_WHITE| SL_INTENSITY),  /* PROT */
    wnscattr (SL_BLACK,    SL_WHITE),                /* BOLD */
    wnscattr (SL_CYAN,     SL_BROWN | SL_INTENSITY), /* ERR  */
    wnscattr (SL_CYAN,     SL_BROWN | SL_INTENSITY), /* SHDW */
    wnscattr (SL_BLUE,     SL_CYAN  | SL_INTENSITY), /* BLCK */
    wnscattr (SL_BLUE,     SL_CYAN  | SL_INTENSITY), /* AFLD */
    wnscattr (SL_BLUE,     SL_CYAN  | SL_INTENSITY), /* NFLD */
    wnscattr (SL_BLUE,     SL_CYAN  | SL_INTENSITY)  /* NICE */
};

ATTRIB Att_Disk = {
    wnscattr (SL_WHITE,   SL_BLACK),                 /* TEXT */
    wnscattr (SL_WHITE,   SL_BLACK),                 /* BORD */
    wnscattr (SL_WHITE,   SL_BLACK),                 /* TITL */
    wnscattr (SL_CYAN ,   SL_BLACK),                 /* HIGH */
    0,                                               /* PROT */
    wnscattr (SL_WHITE,   SL_BROWN | SL_INTENSITY)   /* BOLD */
};

ATTRIB Att_Roll = {
    wnscattr (SL_CYAN,     SL_WHITE | SL_INTENSITY), /* TEXT */
    wnscattr (SL_CYAN,     SL_WHITE | SL_INTENSITY), /* BORD */
    wnscattr (SL_CYAN,     SL_WHITE | SL_INTENSITY), /* TITL */
    wnscattr (SL_BLACK,    SL_WHITE | SL_INTENSITY), /* HIGH */
    wnscattr (SL_CYAN,     SL_BLUE),                 /* PROT */
    wnscattr (SL_CYAN,     SL_BROWN | SL_INTENSITY)  /* BOLD */
};

ATTRIB Att_Work = {
    wnscattr (SL_WHITE,   SL_BLACK),                 /* TEXT */
    wnscattr (SL_WHITE,   SL_BLACK),                 /* BORD */
    0,                                               /* TITL */
    wnscattr (SL_BLACK,   SL_WHITE),                 /* HIGH */
    0,                                               /* PROT */
    wnscattr (SL_WHITE,   SL_WHITE|SL_INTENSITY)     /* BOLD */
};

ATTRIB Att_Warn = {
    wnscattr (SL_RED,     SL_WHITE | SL_INTENSITY),  /* TEXT */
    wnscattr (SL_RED,     SL_WHITE | SL_INTENSITY),  /* BORD */
    wnscattr (SL_RED,     SL_WHITE | SL_INTENSITY),  /* TITL */
    wnscattr (SL_WHITE,   SL_BLACK),                 /* HIGH */
    0,                                               /* PROT */
    wnscattr (SL_RED,     SL_BROWN | SL_INTENSITY)   /* BOLD */
};
ATTRIB      Att_Edit   = {
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY),       /* TEXT Trade Mark  */
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY),       /* BORD             */
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY),       /* TITL             */
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY),       /* HIGH             */
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY),       /* PROT             */
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY),       /* BOLD             */
    wnscattr (SL_BLUE,  SL_WHITE|SL_INTENSITY),      /* ERR  Background  */
    wnscattr (SL_BLACK, SL_CYAN),                    /* SHDW Shadows     */
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY),       /* BLCK             */
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY),       /* AFLD             */
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY),       /* NFLD             */
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY) };     /* NICE             */

ATTRIB      Att_Tree   = {
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY),       /* TEXT Trade Mark  */
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY),       /* BORD             */
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY),       /* TITL             */
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY),       /* HIGH             */
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY),       /* PROT             */
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY),       /* BOLD             */
    wnscattr (SL_BLUE,  SL_WHITE|SL_INTENSITY),      /* ERR  Background  */
    wnscattr (SL_BLACK, SL_CYAN),                    /* SHDW Shadows     */
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY),       /* BLCK             */
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY),       /* AFLD             */
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY),       /* NFLD             */
    wnscattr (SL_BLUE,  SL_CYAN|SL_INTENSITY) };     /* NICE             */


/************************************************************************/
/*                        WNINIWIN                                      */
/************************************************************************/

static VIDEO_STATE state;           /* Previos characteristics adapter  */
static char init_with_clear = 0;    /* Flag for wnfinwin() :            */
                                    /* 0 - restore previos cursor       */
                                    /* 1 - restore previos mode, cursor */
static  LOCATE  cur_loc;            /* Relative location of own cursor  */
static  CURTYP  cur_typ;            /* Cursor type.                     */
static  char  * wnprvimg = "WNPRVIMG.▓▓▓";

int  cdecl wniniwin (int mode, int rows) {
    union REGS reg;                     /* registers definition         */
    char  sc_line;                      /* scan lines on the screen VGA */
                                        /* 0-200; 1-350; 2-400          */
	char  font;                         /* 11h-8x14; 12h-8x8; 14h-8x16  */
    int   eq_list, i;                   /* for equipment list           */
    char far * pscreen, * pdata;
    int   hand;

    if (!n_w_envr.known) {
        if (wnsetadp (&state) == NO) return (0);
        N_V_STATE = state;
        n_w_envr.n_a_page = n_w_envr.n_v_page = utpeekb (uttofaru (0x40, 0x62));

        reg.h.ah = 3;
		reg.h.bh = (char)n_w_envr.n_a_page;
        int86 (WN_V_INT, &reg, &reg);
        cur_loc.row  = reg.h.dh;
        cur_loc.col  = reg.h.dl;
        cur_typ.high = reg.h.ch;
        cur_typ.low  = reg.h.cl;
		n_w_envr.known = 1;
        if (wnishigh ())
            for (i = 0; i < 13; i++)      Cmd[i] |= 0x8000;

        msinst ();
        mshide ();
        msptsmsk (SL_WHITE, SL_WHITE, 0, MS_XOR);

        pdata = malloc (NUM_ROWS * NUM_COLS * 2);  /* БП */
        hand = utcreate (wnprvimg);
        pscreen = wnviptrl (0, 0);
        wnvicopy (&pscreen,&pdata,NUM_ROWS,NUM_COLS,NUM_COLS*2,0,Cmd[6]);
        utwrite (hand, pdata, NUM_ROWS * NUM_COLS * 2);
        free (pdata);
        utclose (hand);

        randomize ();
        Kb_2_head = Kb_2_tail = Kb_2_buf;
    }

    if (mode != _PREVIOS) {
        rows = (rows == _PREVIOS) ? state.numtextrows : rows;
        if (!utrange (mode, _TEXTBW40, _TEXTC80) || mode == _TEXTMONO) {
            if (state.adapter == _VGA || state.adapter == _EGA) {
                switch (rows) {
                    case  _MIDROWS:
                        sc_line = 1;    /* 350 scan lines               */
                        font = 0x12;    /* 8x8  font                    */
                        break;
                    case  _MAXROWS:
                        sc_line = 2;    /* 400 scan lines               */
                        font = 0x12;    /* 8x8  font                    */
                        break;
                    case  _MINROWS:
                    default:
                        sc_line = 2;    /* 400 scan lines               */
                        font =
                          (N_V_STATE.adapter == _VGA) ? (char)0x14 : (char)0x11;
                                                        /* 8x16 or 8x14 font */
                        break;
                }
                if (state.adapter == _VGA) {
                    reg.h.bl = 0x30;    /* Select scan lines VGA    */
                    reg.h.al = sc_line; /* 0 - 200; 1 - 350; 2 - 400*/
                    reg.h.ah = 0x12;    /* Alternate select         */
                    int86 (WN_V_INT, &reg, &reg);

                    eq_list = utpeekw (uttofaru (0x40, 0x10));
                    if (mode == 7 && (eq_list & 0x30) !=  0x30)
                        utpokew (uttofaru (0x40, 0x10), (eq_list | 0x30));
                    else if ((eq_list & 0x30) == 0x30)
                        utpokew (uttofaru(0x40, 0x10), (eq_list & ~0x30));
                } else  mode = (mode == _TEXTMONO) ? _TEXTC80 : mode;

                reg.h.al = (char)mode;      /* mode number          */
                reg.h.ah = 0;               /* Set mode regim       */
                int86 (WN_V_INT, &reg, &reg);

                reg.h.al = font;
                reg.h.bl = 0;               /* Block to load        */
                reg.h.ah = 0x11;            /* Character Generator  */
                int86 (WN_V_INT, &reg, &reg);

       /************************************ Set PrintScreen routine
                if (rows != _MINROWS) {
                    reg.h.bl = 0x20;
                    reg.h.ah = 0x12;
                    int86 (WN_V_INT, &reg, &reg);
                }
       *********************************************************************/
            } else {
                reg.h.al = (char)mode;      /* mode number          */
                reg.h.ah = 0;               /* Set mode regim       */
                int86 (WN_V_INT, &reg, &reg);
            }
            init_with_clear = 1;
        }  else  return (0);
    }

    if (!wnsetadp (&N_V_STATE)) return (0);
    n_w_envr.n_a_page = n_w_envr.n_v_page = utpeekb (uttofaru (0x40, 0x62));
    reg.h.ah = 1;
    reg.x.cx = 0x2000;
    int86 (WN_V_INT, &reg, &reg);

    if (N_V_STATE.mode > 3 && N_V_STATE.mode != 7) return 0;


    msbound (0, 0, NUM_ROWS - 1, NUM_COLS - 1);


    harderr (hard_handler);

    return (NUM_ROWS);
}

/*
    wnfinwin - Восстанавливает состояние адаптера и курсора, как до winiwin()

void wnfinwin (void);

*/

void wnfinwin (void) {
    union REGS reg;
    char far * pscreen, * pdata;
    int   hand;

    if (init_with_clear)
        wniniwin (state.mode, state.numtextrows);

    pdata = malloc (NUM_ROWS * NUM_COLS * 2); /* Б П */

    if ((hand = utopen (wnprvimg, UT_R)) != -1) {
        pscreen = wnviptrl (0, 0);
        utread (hand, pdata, NUM_ROWS * NUM_COLS * 2);
        wnvicopy (&pdata, &pscreen, NUM_ROWS, NUM_COLS, NUM_COLS*2, 0,
                            1 + ((wnishigh ()) ? 0x8000 : 0));
        utclose (hand);
        utremove (wnprvimg);
    }
    free (pdata);



    reg.h.dh =  cur_loc.row;
    reg.h.dl =  cur_loc.col;
	reg.h.bh =  (char)n_w_envr.n_a_page;
    reg.h.ah =  2;
    int86 (WN_V_INT, &reg, &reg);

    reg.h.ch =  cur_typ.high;
    reg.h.cl =  cur_typ.low;
    reg.h.ah = 1;
    int86 (WN_V_INT, &reg, &reg);

    reg.x.ax = 0;           /* Reset mouse driver. */
    int86 (0x33, &reg, &reg);
    wnsetpal (NULL);
}

/*
    wnsetadp - Explore  givens adapter

bool wnsetadp (VIDEO_STATE *state);

VIDEO_STATE *state;    Adapter and Monitor characteristics

Description Know types of adapter and it's characteristics.

Returns     YES         if success
            NO          if not
            state       Info returning structure.
*/

static bool cdecl wnsetadp (VIDEO_STATE * mvs) {

	union   REGS    inpreg, outreg;     /* for int86()                  */
    struct  SREGS   segreg;             /* for int86x()                 */
    char    Buf1    [0x40];             /* Temporary buffer for  0x1B   */
                                        /* video function               */
    int     adapter;                    /* Adapter type                 */

    /* Check for VGA presents */
    segreg.es   = utseg (Buf1);
    inpreg.x.di = utoff (Buf1);
    inpreg.x.bx = 0;                /* 16k        */
    inpreg.x.ax = 0x1B00;           /* MOV AH,1Bh */
    int86x (WN_V_INT, &inpreg, &outreg, &segreg);

    if (outreg.h.al == 0x1B) adapter = Buf1 [0x25]; /* function supported */
    else {                      /* Check for _EGA                      */
        inpreg.h.bl = 0x10;     /* Return EGA information              */
        inpreg.h.ah = 0x12;
        int86 (WN_V_INT, &inpreg, &outreg);

        if ((outreg.h.bl & 0xFC) == 0) adapter = _EGA; /* function supported */
        else                           adapter = _CGA;
    }
    mvs->adapter = adapter;

    /******************************************************
     * Put to structure other parameters:
     * numtextcols;
     * numtextrows;
     * mode;
     * bytes_char;
     */
    inpreg.h.ah = 0x0F;     /* Get Current video mode, text cols
                             * and video page */
    int86 (WN_V_INT, &inpreg, &outreg);

    mvs->numtextcols = (short) outreg.h.ah;
    mvs->mode        = (short) outreg.h.al;

    if ((mvs->byte_char  = (short) utpeekw (uttofaru (0x40,0x85))) == 0)
            mvs->byte_char = (mvs->mode == _TEXTMONO) ? 14 : 8;

    switch (adapter) {
        case    _CGA:
            mvs->numtextrows = 25;
            break;
        case    _EGA:               /* Define number of available rows  */
            inpreg.x.ax = 0x1130;   /* Get EGA information              */
            inpreg.h.bh = 0;        /* Return 1Fh pointer not hear used */
            int86 (WN_V_INT, &inpreg, &outreg);
            mvs->numtextrows = (short) outreg.h.dl + 1;
            break;
        case    _VGA:               /* Define number of available rows  */
            mvs->numtextrows = (short) Buf1 [0x22];
            break;
    }
    return YES;
}

#pragma warn -par
static int hard_handler (int errval, int ax, int bp, int si) {
    Err_Val = _DI;
    Err_Disk = ax;
    Err_Hard = YES;
    return (3);       /* RET_FAIL */
}
#pragma warn +par


