/********************************************************************/
/*                                                                  */
/*  SLKEYB.H    Header file for BIOS keyboard functions for TURBO C */
/*                                                                  */
/********************************************************************/


#ifndef DEF_SLKEYB
#define DEF_SLKEYB 1

#if __STDC__
#define _Cdecl
#else
#define _Cdecl	cdecl
#endif

#include <ctype.h>

#ifndef DEF_SLUTIL
#include <slutil.h>
#endif

#define KB_BIOS_INT 0x16        /* General BIOS keyboard gate.      */
                                /*                                  */
#define KB_HEAD     0           /* Whether place char for kbplace() */
#define KB_TAIL     1           /*                                  */
                                /* kbplace() error returns:         */
#define KB_OK       0           /*   no error,                      */
#define KB_FULL     1           /*   buffer full,                   */
#define KB_PLACE    2           /*   bad "where" specification.     */
                                /*                                  */
#define KB_DATASEG  0x40        /* Segment of BIOS keyboard data.   */
#define KB_HEADLOC  0x1A        /* Location of queue head pointer.  */
#define KB_TAILOC   0x1C        /* Location of queue tail pointer.  */
#define KB_SHIFTLOC 0x17        /* Location of shift status register*/
#define KB_BUFEND   0x3E        /* Address of keyboard buffer end.  */
#define KB_BUFSTART 0x1E        /* Address of keyboard buffer start.*/
#define KB_101KEYS  0x96        /* Bit 10h = 1 -- 101-keys          */
                                /* Size of buffer in bytes.         */
                                /* Size of buffer in keystrokes.    */
#define KB_BUFSIZE    (KB_BUFEND - KB_BUFSTART)
#define KB_BUFACSIZE  (((KB_BUFEND - KB_BUFSTART) >> 1) - 1)

#define KB_BUFHEADADDR (uttofaru (KB_DATASEG, KB_HEADLOC))
#define KB_BUFTAILADDR (uttofaru (KB_DATASEG, KB_TAILOC))
#define KB_SHIFTADDR   (uttofaru (KB_DATASEG, KB_SHIFTLOC))

#define KB_SHIFTSTAT    0x100
#define KB_CTRLSTAT     0x200
#define KB_ALTSTAT      0x400

                                /* Maps keyboard status.            */
                                /* register onto unsigned integer   */
typedef struct kbstat {         /* Keyboard status structure.       */
   unsigned right_shift  : 1;   /* Right Shift key depressed.       */
   unsigned left_shift   : 1;   /* Left  Shift key depressed.       */
   unsigned ctrl_shift   : 1;   /* Ctrl        key depressed.       */
   unsigned alt_shift    : 1;   /* Alt         key depressed.       */
   unsigned scroll_state : 1;   /* Scroll Lock has been toggled.    */
   unsigned num_state    : 1;   /* Num Lock has been toggled.       */
   unsigned caps_state   : 1;   /* Caps Lock has been toggled.      */
   unsigned ins_state    : 1;   /* Insert state is active.          */
                                /*                                  */
   unsigned filler       : 3;   /* Filler for word alignment.       */
   unsigned hold_state   : 1;   /* Suspend key has been toggled.    */
   unsigned scroll_shift : 1;   /* Scroll Lock key depressed.       */
   unsigned num_shift    : 1;   /* Num Lock    key depressed.       */
   unsigned caps_shift   : 1;   /* Caps Lock   key depressed.       */
   unsigned ins_shift    : 1;   /* Insert      key depressed.       */
}  KBSTAT;


/********************************************************************/
/* Macros & Global vars                                             */
/********************************************************************/

extern  void    (* Kb_fun)  (void);
extern  KBSTAT  Kb_status;
extern  int     Kb_ch, Kb_sc, Kb_sym;
extern  bool    Key_Hot_Flag, Key_Buf_Flag, Kb_redraw;
extern  int     Kb_2_buf [17], * Kb_2_head, * Kb_2_tail;

#define kbstatus(pkeybd) ((*((int *) (pkeybd))) = utpeekw (KB_SHIFTADDR))

/********************************************************************/
/* Function declarations                                            */
/********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

int     _Cdecl kbgetkey (void (*fn1)(void));
int     _Cdecl kbplace  (int where, int ch, int sc);
bool    _Cdecl kbready  (void);
bool    _Cdecl kb101key (void);
bool    _Cdecl kbanypre (bool flush);
bool    _Cdecl kbescpre (bool flush);
void    _Cdecl kbflushb (void);
bool    _Cdecl kbvfysym (int sym);
void    _Cdecl kbsetsta (KBSTAT *pkeybd);
void    _Cdecl kbgetsta (void);
void    _Cdecl kbsetsym (int sym);
void (* _Cdecl kbhotkey (int sym, void (*fn)(void)))();

#ifdef __cplusplus
}
#endif
#endif
