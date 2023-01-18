/********************************************************************/
/*                                                                  */
/*  SLUTIL.H     Header file for Turbo C TOOLS Utility functions    */
/*                                                                  */
/********************************************************************/

#ifndef DEF_SLUTIL
#define DEF_SLUTIL  1

#if __STDC__
#define _Cdecl
#else
#define _Cdecl  cdecl
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/********************************************************************/
/* Definitions of data types                                        */
/********************************************************************/

typedef unsigned char byte;
typedef unsigned int  word;
typedef unsigned long dword;
typedef enum { NO = 0, YES = 1 } bool;

typedef struct {                /* HALFREGS:  two halves of 16b reg */
    byte     l;                 /* Lower 8 bits.                    */
    byte     h;                 /* Upper 8 bits.                    */
}   HALFREGS;

typedef union {                 /* DOUBLREG: Two ways to express reg*/
    HALFREGS hl;                /* Two separated halves.            */
    word     x;                 /* 16-bit unsigned quantity.        */
}   DOUBLREG;

typedef struct {                /* ALLREG:  complete CPU state      */
    DOUBLREG ax,bx,cx,dx;       /* General registers.               */
    word     si,di;             /* Index   registers.               */
    word     ds,es,ss,cs;       /* Segment registers.               */
    word     flags,bp,sp,ip;    /* Other   registers.               */
}   ALLREG;

typedef struct segads {         /* ADS:  Physical address           */
    word    r;                  /* Offset.                          */
    word    s;                  /* Segment.                         */
}   ADS;

struct address {                /* Physical address.                */
    word    r;                  /* Offset.                          */
    word    s;                  /* Segment.                         */
};

typedef struct {            /* MEMCTRL:  DOS memory control block   */
    char     _dummy;        /* Dummy to force word alignment of     */
                            /* unsigned members, so no waste        */
                            /* space appears within structure.      */
                            /* Actual 16-byte copy of control block */
                            /* starts here:                         */
    char     ident;         /* Identifying signature.               */
    word     owner_psp;     /* Segment address of own process's PSP */
    word     size;          /* Size of memory block in paragraphs.  */
    char     _reserved[11];
}   MEMCTRL;

/********************************************************************/
/* Структура для utmony2c ()                                        */
/********************************************************************/

typedef struct {
    char   for_1 [20];          /* Наименование для единицы                 */
    char   for_2 [20];          /* Наименование для двойки-четверки         */
    char   for_other [20];      /* Наименование для остальных цифр          */
    bool   sex;                 /* Пол наименования (0 - жен., 1 - муж.)    */
} VALUTA;
/********************************************************************/
/* Flag bits for use with ALLREG structure.                         */
/********************************************************************/

#define CF_FLAG     0x0001      /* Carry flag.                      */
#define PF_FLAG     0x0002      /* Parity flag (1 means even).      */
#define AF_FLAG     0x0010      /* Auxiliary carry flag.            */
#define ZF_FLAG     0x0040      /* Zero flag (1 means zero).        */
#define SF_FLAG     0x0080      /* Sign flag (1 means negative).    */
#define TF_FLAG     0x0100      /* Trap flag (1 means single step). */
#define IF_FLAG     0x0200      /* Interrupt enable flag.           */
#define DF_FLAG     0x0400      /* Direction flag (0 means up).     */
#define OF_FLAG     0x0800      /* Overflow flag (1 means overflow).*/

#define DEF_FLAGS   (IF_FLAG)   /* Commonest value for flags:       */
                                /*   interrupts enabled,            */
                                /*   single-stepping off,           */
                                /*   direction up.                  */
/********************************************************************/
/* Handy system constants and addresses.                            */
/********************************************************************/

#define UTDOSINT 0x21                           /* DOS interrupt    */
#define UTMODELADDR (uttofaru (0xffff, 0xe))    /* Address of model */
#define UTCLOCKADDR (uttofaru (0x40, 0x6c))     /* Address clock cnt*/
#define UTROLLADDR  (uttofaru (0x40, 0x70))     /* Address clock flg*/

#define IBM_PC  ((char) 0xff)   /* PC.                              */
#define IBM_XT  ((char) 0xfe)   /* PC-XT or Portable PC.            */
#define IBM_JR  ((char) 0xfd)   /* IBM PC-Jr.                       */
#define IBM_AT  ((char) 0xfc)   /* IBM (or compat.) PC-AT.          */
#define IBM_X2  ((char) 0xfb)   /* PC-XT with 256/640 motherboard.  */
#define IBM_CV  ((char) 0xf9)   /* PC Convertible.                  */
#define IBM_30  ((char) 0xfa)   /* PS/2 model 30.                   */
#define IBM_50  ((char) 0xfc)   /* PS/2 models 50, 60.              */
#define IBM_80  ((char) 0xf8)   /* PS/2 model 80.                   */

/********************************************************************/
/* Constant values for utintflg(), utmovabs(), and utctlblk()       */
/********************************************************************/

#define UT_INTON        1
#define UT_INTOFF       0
#define UT_MV_FORW      0
#define UT_MV_BACK      1

#define CBRK_SET        1
#define CBRK_GET        0
#define CBRK_ON         1
#define CBRK_OFF        0

/********************************************************************/
/* Constant values for utctrcvt() and utstrjst()                    */
/********************************************************************/

#define ST_RWHITE       0x01    /* Remove white space.              */
#define ST_RLWHITE      0x02    /* Remove leading white space.      */
#define ST_RTWHITE      0x04    /* Remove trailing white space.     */
#define ST_REDUCE       0x08    /* Reduce white space to 1 blank.   */
#define ST_NOQUOTE      0x10    /* Quoted substrings not altered.   */
#define ST_TOUP         0x20    /* Convert to upper case.           */
#define ST_TOLOW        0x40    /* Convert to lower case.           */
#define ST_RCONTROL     0x80    /* Remove all control characters.   */

#define UT_LEFT         0
#define UT_RIGHT        1
#define UT_CENTER       2
#define UT_FORMAT       3
#define UT_SGN_BEG      4

/********************************************************************/
/* Constant values for utdbl2ce ()                                  */
/********************************************************************/

#define CV_RANGE1           0x01   /* Мантисса от 0 до 9.999999999      */
#define CV_BLANK_BP         0x02   /* Не выводить 0 перед точкой        */
#define CV_M_PLUS           0x04   /* Выводить '+' в мантиссе           */
#define CV_M_PLUS_BLANK     0x08   /* Выравнивать под знак в мантиссе   */
#define CV_E_PLUS           0x10   /* Выводить '+' в экспоненте         */
#define CV_E_PLUS_BLANK     0x20   /* Выравнивать под знак в экспоненте */
#define CV_JST              0x40   /* Выравнивать вправо (экспоненту)   */
#define CV_ZERO             0x80   /* Выводить ведущие '0'              */
#define CV_FLOAT            0x100  /* Тип переменной - float            */

#define CV_CENTURE          0x01   /* Выводить век                      */
#define CV_STRMON           0x02   /* Выводить месяц строкой            */
#define CV_LAT              0x04   /* Выводить месяц по английски       */
#define CV_FULL             0x08   /* Выводить месяц полностью          */
#define CV_TODAY            0x10   /* Инициализировать текущей датой    */

/********************************************************************/
/* Constant values for file functions                               */
/********************************************************************/

#define UT_BEG              0
#define UT_CUR              1
#define UT_END              2

#define UT_R                0
#define UT_W                1
#define UT_RW               2
#define UT_WR               UT_RW

/********************************************************************/
/* Global variables maintained by UT functions and macros.          */
/********************************************************************/

extern byte far *Adr_Crit;      /* Address of critical section flag */

/********************************************************************/
/* Macros to combine and extract words, bytes, and nybbles.         */
/********************************************************************/

#define utclrbit(a,b) ((a)-((a)&(b)))       /* Clear b bits of word a */
#define uthiword(a)   (((a)>>16)&0xffffL)   /* High word of long a  */
#define utloword(a)   ((a)&0xffffL)         /* Low  word of long a  */
#define uthibyte(a)   (((a)>>8)&0x00ff)     /* High byte of word a  */
#define utlobyte(a)   ((a)&0x00ff)          /* Low  byte of word a  */
#define uthinyb(a)    (((a)>>4)&0x000f)     /* High nybble of byte a*/
#define utlonyb(a)    ((a)&0x000f)          /* Low nybble of byte a */

#define utwdlong(a,b) ((((0xffffL&(long)(a)))<<16)|(0xffffL&(long)(b)))
#define utbyword(a,b) ((((a)&0x00ff)<<8)|((b)&0x00ff))
#define utnybbyt(a,b) ((((a)&0x000f)<<4)|((b)&0x000f))

/********************************************************************/
/* Macros for pointer manipulation.                                 */
/********************************************************************/

#define uttofaru(seg,off) ((void far *)                              \
                           ((((unsigned long) (unsigned int)         \
                             (seg)) << 16L) |                        \
                           ((unsigned long) (unsigned int) (off))))

#define utoff(p)        ((unsigned int) (p))
#define utseg(p)        ((unsigned int)                              \
                        (((unsigned long) (void far *) (p)) >> 16L))

#define uttofar(seg,off,type)   ((type far *) uttofaru((seg),(off)))
#define utnorm(p,type)      (uttofar (utseg (p) + (utoff (p) >> 4),  \
                                     (utoff (p) & 0x000f), type))

#define utplong(p) ((((unsigned long) utseg (p)) << 4L) +            \
                    ((unsigned long) utoff (p)) & 0xfffffL)

/********************************************************************/
/* Macros for low-level memory manipulation.                        */
/********************************************************************/

#define utpeekb(p)          (*((byte far *) (p)))
#define utpeekw(p)          (*((unsigned int far *) (p)))
#define utpokeb(p,bval)     (*((byte far *) (p)) = (bval))
#define utpokew(p,uval)     (*((unsigned int far *) (p)) = (uval))
#define utpeekn(pf,pn,clen) (utmovmem ((char far *) (pn),(pf),(clen)))
#define utpoken(pn,pf,clen) (utmovmem ((pf),(char far *) (pn),(clen)))

/********************************************************************/
/* Miscellaneous macros.                                            */
/********************************************************************/

#define EOS             '\0'
#define BLANK           ' '
#define NEWLINE         '\n'
#define TAB             '\t'

#define NIL             ((void *) 0)
#define FARNIL          ((void far *) 0)

#define issign(c)       ((c)=='-' || (c)=='+')
#define isblank(c)      ((c)==' ' || (c)=='\t' || (c)=='\n')
#define utsign(a)       ((a) > 0 ? 1 : ((a) == 0 ? 0 : -1))

#define utalloc(type)           ((type *) calloc (1, sizeof (type)))
#define utmcopy(to,from,type)   ((type *) memmove((to),(from),sizeof (type)))

#define utdosmajor      ((byte) uthibyte (utdosver))    /* DOS vers */
#define utdosminor      ((byte) utlobyte (utdosver))    /* DOS vers */
#define utdosver        ((unsigned int) utbyword (_osmajor, _osminor))

#define utchknil()  ((void) 0)
#define utpspseg    ((unsigned) (_psp))
#define utdosrdy()  ((Adr_Crit != FARNIL) ? (utpeekb (Adr_Crit) == 0) : (0))
#define utmodel()   (utpeekb(UTMODELADDR))
#define utinton()   (utintflg (UT_INTON))
#define utintoff()  (utintflg (UT_INTOFF))

#define utspkoff()          (utspeakr (0))
#define utspkon(freq)       (utspeakr (freq))
#define utsound(freq,dura)  (utspkon(freq), utsleept(dura), utspkoff())

/********************************************************************/
/* Macros to perform range checking and limiting.                   */
/********************************************************************/

#define utmax(a,b)  ((a) > (b) ? (a) : (b))
#ifndef max
#define max(a,b)    utmax(a,b)
#endif

#define utmin(a,b)  ((a) <= (b) ? (a) : (b))
#ifndef min
#define min(a,b)    utmin(a,b)
#endif

#define utabs(x)    ((x)<0 ? -(x) : (x))

#define utrange(a,l,h)      (((a) < (l)) || ((a) > (h)))
#define utoutrng(a,l,h)     (utrange(a,l,h))

#define utbound(a,b,c)                                               \
    {   register int uttemp;                                         \
        if ((a) < (uttemp = (b)) || (a) > (uttemp = (c)))            \
            (a) = uttemp;                                            \
    }

#define utuplim(a,b)                                                 \
    {   register int uttemp;                                         \
        if ((a) > (uttemp = (b)))                                    \
            (a) = uttemp;                                            \
    }

#define utlowlim(a,b)                                                \
    {   register int uttemp;                                         \
        if ((a) < (uttemp = (b)))                                    \
            (a) = uttemp;                                            \
    }

/********************************************************************/
/* Function declarations.                                           */
/********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

int         _Cdecl utcreate (void far * fname);
int         _Cdecl utopen   (void far * fname, int mode);
long        _Cdecl utlseek  (int handle, long pos, int from);
word        _Cdecl utread   (int handle, void far * pbuf, word len);
bool        _Cdecl utwrite  (int handle, void far * pbuf, word len);
long        _Cdecl utfsize  (int handle);
bool        _Cdecl utchsize (int handle, long size);
int         _Cdecl utclose  (int handle);
bool        _Cdecl utremove (void far * fname);
bool        _Cdecl utrename (void far * oldname, void far * newname);
void        _Cdecl utuninam (char * pname, char * pext);
int         _Cdecl utcopy   (char * to, char * from, bool overwrite);
int         _Cdecl uthndcopy (int handto, int handfrom, long len);


void        _Cdecl utput_ds (int new_ds);
int         _Cdecl utget_ds (void);
void        _Cdecl utmovabs (void far *, void far *, unsigned int, int);
void        _Cdecl utexblnk (void far * to, void far * from, unsigned num);
void        _Cdecl utmovsc  (void far *, char, unsigned int);
void        _Cdecl utmovmem (char far *sour,  char far *targ, unsigned amount);
void        _Cdecl utch2prn (byte sym);
int         _Cdecl utmmctrl (unsigned memblk, MEMCTRL *pctrl, unsigned *pnext);
unsigned    _Cdecl utmmsize (void);
int         _Cdecl utintflg (int want_on);
int         _Cdecl utgetclk (long *pcount);
unsigned    _Cdecl utsleept (unsigned period);
void        _Cdecl utspeakr (unsigned freq);
void        _Cdecl utalarm  (void);
int         _Cdecl utctlbrk (int set, int new_state);
unsigned    _Cdecl utnulchk (void);
unsigned char far * _Cdecl utcritsf (void);

int         _Cdecl utl2crdx (long num, void far * p, int wid, int base);
int         _Cdecl utl2c    (long num, char * pbuf, int opt);
bool        _Cdecl utc2lrdx (char ** pp, long * num, int base);
bool        _Cdecl utisempt (void * pbuf);
bool        _Cdecl utc2dbl  (char ** pp, double * val, bool doub);
int         _Cdecl utrewchr (void * pbuf, int sym);
void        _Cdecl utfree   (void * p);
void        _Cdecl utchext  (char *, char *);
char  *     _Cdecl utallocb (unsigned int *);
int         _Cdecl utdbl2ce (double x, char * buf, int prec, int opt);
int         _Cdecl utdbl2cf (double x, char * buf, int wid, int prec, int opt);
double      _Cdecl utdblcorr(double x, int prec, bool doub);
bool        _Cdecl utisleap (register int year);
long        _Cdecl utc2date (char **pp, int centure);
int         _Cdecl utdate2c (long date, char * buf, char separ, int opt);
long        _Cdecl utmkdate (int day, int month, int year);
void        _Cdecl utspdate (long date, int *pd, int *pm, int *py);
long        _Cdecl utdtoday (void);
int         _Cdecl utcurcen (void);
long        _Cdecl utbegmon (long date);
long        _Cdecl utendmon (long date);
long        _Cdecl utprvmon (long date);
long        _Cdecl utnxtmon (long date);
int         _Cdecl utmndays (int month, int year);


int         _Cdecl utcupper (int sym);
int         _Cdecl utclower (int sym);
char *      _Cdecl utstrcvt (char *psource, int conv);
char *      _Cdecl utstrexp (register char *ptarget, char *psource, int incr,
                            int tarsize);
char *      _Cdecl utstrtab (char *psource, int incr);
void        _Cdecl utstrjst (char * to, char * from, int len, char fill, int w,
                                                                    int code);
char *      _Cdecl utstrxlt (register char *psource, char *ptable, char *ptrans);
int         _Cdecl utstrind (char check, char *psearch);

bool        _Cdecl utqsort (void * pb, int n, int w, int (* f) (void *, void *));

int         _Cdecl utmony2c (byte * pin, byte * pout,
                                               VALUTA * pbig, VALUTA * psmall);
void        _Cdecl utp2env  (void * p);
void        _Cdecl utsetenv (void);
int         _Cdecl utspawn  (char * tasknam, void * point);
void        _Cdecl utoutdat (void);
void        _Cdecl utworkin (void);
bool        _Cdecl utvfymem (long remain);
void        _Cdecl utcode   (void far * pto, void far * pfrom, int len);
void        _Cdecl utdecode (void far * pto, void far * pfrom, int len);
int         _Cdecl utbufcmp (void far * pbuf1, void far * pbuf2, unsigned len);


#ifdef __cplusplus
}
#endif

#endif
