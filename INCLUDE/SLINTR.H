/********************************************************************/
/*                                                                  */
/*  SLINTR.H   Header file for Intervention and Interrupt Service   */
/*                                                                  */
/********************************************************************/

#ifndef DEF_SLINTR
#define DEF_SLINTR        1

#if __STDC__
#define _Cdecl
#else
#define _Cdecl	cdecl
#endif

#include <dos.h>

#ifndef DEF_SLUTIL
#include <slutil.h>
#endif

typedef struct {              /* IV_KEY:  Specifies one hot key.    */
    char    ch;               /* Character value.                   */
    char    keycode;          /* Key code (pseudo-scan code).       */
    int     action;           /* Action to be taken when this key is*/
}   IV_KEY;                   /* detected (see possibilities below) */

                              /* Key actions                        */
#define IV_KY_NONE        0   /* No hot key pressed.                */
#define IV_KY_SERVICE     1   /* Invoke intervention function.      */
#define IV_KY_SLEEP       2   /* Put intervention function to sleep */
#define IV_KY_WAKE        3   /* Awaken intervention function.      */

typedef struct {              /* IV_TIME:  Specifies time of day or */
                              /* time interval for intervention code*/
    long    ticks;            /* Clock ticks since midnight or      */
                              /* between invocations.               */
    int     action;           /* Type of time event (moment/interv) */
}   IV_TIME;
                              /* Time actions                       */
#define IV_TM_NONE        0   /* No time event occurred.            */
#define IV_TM_INTERVAL    1   /* Regular interv between invocations */
#define IV_TM_MOMENT      2   /* Time of day.                       */

typedef struct {              /* IV_EVENT: Event that triggered the */
                              /* intervention function.             */
    IV_KEY  key;              /* Hot key found, if any.             */
    int     time_action;      /* Type of timer event (moment or     */
                              /* interval), if any.                 */
    int     time_index;       /* Index of timer event in table.     */
    long    time;             /* Current time of day, measured in   */
}   IV_EVENT;                 /* clock ticks since midnight.        */

typedef struct {              /* IV_VECTORS:  set of interrupt      */
                              /* vectors used by intervention code. */
    void far *ptimer;         /* INT 08h:  Timer tick.              */
    void far *pkeybd;         /* INT 09h:  Keystroke.               */
    void far *pdisk;          /* INT 13h:  BIOS disk services.      */
    void far *pdos;           /* INT 21h:  DOS functions.           */
    void far *pidle;          /* INT 28h:  DOS idle.                */
}   IV_VECTORS;

typedef struct {              /* IV_CTRL: intervention control block*/
    unsigned int sign;        /* Signature identifying this struct  */
    unsigned int sign2;       /* One's complement of "signature".   */
    char         ident[16];   /* Identifying name.                  */
    unsigned int psp;         /* PSP of this program.               */
    int          enabled;     /* Nonzero if enabled (this prevents  */
                              /* filters from being installed twice)*/
    void   far  *pgate;       /* Gateway for invoking this program  */
                              /* from outside.                      */
    IV_VECTORS  prev_vec;     /* Previous values of interrupt vector*/
    char   dos_need;          /* Nonzero if intervention function   */
                              /* uses DOS services.                 */
    char   dkey_need;         /* Nonzero if intervention function   */
                              /* uses DOS functions 1-12.           */
    IV_KEY far *pkeytab;      /* Table of hot keys.                 */
    int    ktsize;            /* Size of hot key table.             */
    IV_KEY key_event;         /* Found key, if any.                 */
                              /* Internal state of filters:         */
    char   wait;              /* Nonzero if INT 21h / INT 28h filter*/
                              /* is cycling, waiting for the        */
                              /* intervention function to be invoked*/
    char   req;               /* Nonzero if intervention function   */
                              /* needs invocation.                  */
    char   awake;             /* Zero if intervention function is   */
                              /* asleep.                            */
                              /* Busy flags:                        */
    char   timer_busy;        /* INT 08h filter.                    */
    char   kb_busy;           /* INT 09h filter.                    */
    char   disk_busy;         /* INT 13h filter.                    */
    char   idle_busy;         /* INT 28h filter.                    */
    char   idle_safe;         /* Nonzero if INT 0x28 is in progress */
                              /* and DOS functions are available    */
                              /* (but not functions 1-12).          */
    void   far *pschedblk;    /* Entry point into scheduler's ISR   */
                              /* control block.                     */
    unsigned char             /* Address of DOS critical section    */
              far *pdos_crit; /* flag (busy flag).                  */
    char   _reserved[8];      /* Reserved for future use.           */
}   IV_CTRL;

#define IV_SIGNATURE   (('0' << 8) | 'B')   /* "B0"                 */

                              /* Option values for IVINSTAL.        */
#define IV_DOS_NEED         1 /* Intervention function uses DOS.    */
#define IV_NO_DOS_NEED      0 /* Intervention doesn't use DOS       */
#define IV_DKEY_NEED        2 /* Intervention uses DOS funct 1-12   */
#define IV_NO_DKEY_NEED     0 /* Intervention doesn't use 1-12      */
#define IV_FLOAT_NEED       4 /* Intervention uses floating point   */
#define IV_NO_FLOAT_NEED    0 /* Doesn't use float                  */

                              /* Option values for IVVECSIN         */
#define  IV_RETVEC          1 /* Return current values of int. vect */
#define  IV_SETVEC          0 /* Set interrupt vectors.             */

#define  IV_NO_ERROR        0 /* Symbols for error codes            */
#define  IV_NOT_ENABLED     1
#define  IV_NOT_INSTALLED   2
#define  IV_NULL_POINTER    3
#define  IV_BAD_OPT         4
#define  IV_INSTALLED       5

/********************************************************************/
/* Function declarations                                            */
/********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

int             _Cdecl ivdisabl (IV_CTRL far *pctrl);
int             _Cdecl ivvecsin (int option, IV_VECTORS far *pvectors);
IV_CTRL far *   _Cdecl ivsensin (IV_VECTORS far *pvectors, char far *pident);
IV_CTRL far *   _Cdecl ivctrlbl (void);
int             _Cdecl ivinstal (void     (*pfunc)(IV_EVENT *),
                                char     *pident,     char     *pstack,
                                int      stksize,     IV_KEY   *pkeytable,
                                int      numkeys,     IV_TIME  *ptimetable,
                                int      numtimes,    int      option);

#ifdef __cplusplus
}
#endif


typedef struct {                      /* ISRMSG: structure for pass */
                                      /* messages between ISR       */
                                      /* and the dispatcher.        */
    int      exit_style;              /* Exit style:  see possible  */
                                      /* values below.              */
    unsigned working_flags;           /* Working value of flags     */
                                      /* register.  This will be    */
                                      /* be propagated to an ISR's  */
                                      /* caller if the ISR returns  */
}   ISRMSG;                           /* without doing an IRET.     */

                                      /* Exit styles from an ISR:   */
#define IEXIT_NORMAL    0             /* Normal: return via IRET    */
#define IEXIT_RETF      1             /* Special:return via far RET */

#define ISRCTRL   struct isr_control  /* ISRCTRL: ISR control block */
struct isr_control {                  /*                            */
    unsigned fcall_opcode;            /* NOP + Far call (0x9A90)    */
    void (far *isrdisp)();            /* Address of ISR dispatcher  */
    unsigned iret_opcode;             /* IRET + RETF    (0xCBCF)    */
    char far *isrstk;                 /* Beginning of space         */
                                      /* allocated for ISR stack(s) */
    unsigned isrstksize;              /* ISR stack size             */
    unsigned isrsp;                   /* ISR stack pointer value at */
                                      /* start of current ISR call  */
    unsigned isrds;                   /* DS value required by ISR   */
    unsigned isres;                   /* ES value required by ISR   */
    void (far *isr)(ALLREG *,         /* Address of ISR itself      */
                    ISRCTRL *,        /*                            */
                    ISRMSG *);        /*                            */
    unsigned isrpsp;                  /* PSP of program contain ISR */
    void far *prev_vec;               /* Previous value of vector   */
    unsigned level;                   /* Number of calls in progress*/
                                      /* (0 if not in use)          */
    unsigned limit;                   /* Max number of nested calls */
    unsigned sign;                    /* Structure signature        */
    unsigned sign2;                   /* One's complement of "sign" */
    char     ident[16];               /* Identifying name           */
    unsigned control;                 /* Bit fields to control      */
                                      /* dispatcher options         */
    unsigned status;                  /* Status info left by disp.  */
    char     scratch[10];             /* Scratch space for use by   */
};                                    /* dispatcher & related prog. */

                                      /* Offsets of various items   */
                                      /* in an ISRCTRL structure.   */
#define ICB_ENTRY_OFFSET    0         /* Address to install in      */
                                      /* interrupt vector.          */
#define ICB_PSP_OFFSET     24
#define ICB_PREV_OFFSET    26
#define ICB_SGN_OFFSET     34
#define ICB_S2_OFFSET      36
#define ICB_IDENT_OFFSET   38

#define ICB_SIGNATURE   (('0' << 8) | 'B')  /* "B0"                 */

typedef struct {                      /* DEV_HEADER:  Device header */
                                      /* structure available to     */
                                      /* DOS critical error         */
                                      /* (INT 0x24) handlers.       */
    void far *next_dev;               /* Physical address of next   */
                                      /* device header in chain     */
                                      /* (0xffff:0xffff if end)     */
    unsigned attr;                    /* Attribute bits             */
    unsigned strategy;                /* Offset of "strategy"routine*/
    unsigned service;                 /* Offset of service routine  */
    char     name[8];                 /* If character device, this  */
                                      /* is its name;if block device*/
                                      /* name[0] is the number of   */
}   DEV_HEADER;                       /* drives served by this driv */

                              /* Symbols for use with ISCURPRC      */
#define  IS_SETPROC  1        /* Set currently executing process.   */
#define  IS_RETPROC  0        /* Return currently executing process.*/

                              /* Floating point interrupt vectors   */
                              /* used by math library               */
#define  IS_1ST_FLOAT_VEC    0x34
#define  IS_LAST_FLOAT_VEC   0x3e
#define  IS_NUM_FLOAT_VECS  ((IS_LAST_FLOAT_VEC) - (IS_1ST_FLOAT_VEC) + 1)

/********************************************************************/
/* Macros                                                           */
/********************************************************************/

#define isgetvec(intype)     ((void far *) getvect(intype))
#define isputvec(intype,ptr) (setvect((intype),(void interrupt (*)())(ptr)))
#define isresext(excode)     (keep((excode),utmmsize()))

/********************************************************************/
/* Function declarations                                            */
/********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

bool            _Cdecl iscallin (void far * pisr, ALLREG *preg);
int             _Cdecl isremove (unsigned psp_seg);
unsigned        _Cdecl iscurprc (int option, unsigned newproc);
ISRCTRL far *   _Cdecl issensin (void far *ptr, char *pident);
bool            _Cdecl isinstal (int     intype,
                                void    (*pfunc)(ALLREG *,ISRCTRL *,ISRMSG *),
                                char    *pident,
                                ISRCTRL *pisrblk,
                                char    *pstack,
                                int     stksize,
                                int     stknum);
void            _Cdecl isprepbl (void    (*pfunc)(ALLREG *,ISRCTRL *,ISRMSG *),
                                char    *pident,
                                ISRCTRL *pisrblk,
                                char    *pstack,
                                int     stksize,
                                int     stknum);

#ifdef __cplusplus
}
#endif

#endif
