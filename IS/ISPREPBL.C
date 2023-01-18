/*
    isprepbl - Prepare interrupt service routine control block

void isprepbl (void    (*pfunc)(ALLREG *,ISRCTRL *,ISRMSG *),
               char    *pident, ISRCTRL *pisrblk, char    *pstack,
               int     stksize, int     stknum);

void cdecl (*pfunc)(ALLREG *,ISRCTRL *,ISRMSG *)
                    Pointer to interrupt service routine
char    * pident    Pointer to array of 16 bytes of identifying information
ISRCTRL * pisrblk   Pointer to ISR control block (already allocated)
char    * pstack    Address of beginning of memory block to be used for
                    the stack(s) for this ISR (already allocated)
int       stksize   Size (in bytes) of one ISR stack ( > 64)
int       stknum    Maximum depth of nested invocations of this ISR ( > 1)

Description This function prepares an interrupt service routine
            (ISR) control block for installation.  This includes
            everything necessary to execute the ISR via the ISR
            dispatcher (ISDISPAT).

            The calling function must allocate space for the ISR
            control block and pass its address as pisrblk.  This
            space must be static during the time that the ISR is in
            use.

            The calling function must also allocate space to be used
            for the ISR's stack(s).  The address of this space must
            be passed as pstack.  The space must be at least
            (stksize*stknum) bytes long.  This space must be static
            during the time that the ISR is in use.

            The sixteen bytes of identifying information are stored
            in the ISR control block so that (when the ISR is
            installed in an interrupt vector) other programs may
            examine the interrupt vector and detect the presence of
            this ISR.

Example     The following code fragment illustrates one way to
            declare the arguments to ISINSTAL and how to use
            ISPUTVEC to restore the contents of the interrupt vector
            when the ISR is no longer in use.

Returns     None.

*/

#include "language.h"
#include <slintr.h>

typedef char CHAR16[16];

void isprepbl (void    (*pfunc)(ALLREG *,ISRCTRL *,ISRMSG *),
               char    *pident,
               ISRCTRL *pisrblk,
               char    *pstack,
               int     stksize,
               int     stknum)
{
    int 	i;
    extern void isdispat(void);
    struct SREGS segregs;

    pisrblk->fcall_opcode = 0x9a90;
    pisrblk->isrdisp	 = (void (far *) ()) isdispat;
    pisrblk->iret_opcode = 0xcbcf;
    pisrblk->isrstk = pstack;
    pisrblk->isrstksize = stksize;
    pisrblk->isrsp  = utoff(pisrblk->isrstk);
    segread (&segregs);
    pisrblk->isrds  = segregs.ds;
    pisrblk->isres  = segregs.es;
    pisrblk->isr    = (void (far *)(ALLREG *, ISRCTRL *, ISRMSG *)) pfunc;
    pisrblk->isrpsp = utpspseg;
    pisrblk->level  = 0;
    pisrblk->limit  = stknum;
    pisrblk->sign2  = ~(pisrblk->sign = ICB_SIGNATURE);
    utmcopy(pisrblk->ident,pident,CHAR16);
    pisrblk->control = 0;
    pisrblk->status  = 0;
    for (i = 0; i < sizeof(pisrblk->scratch); i++)
        pisrblk->scratch[i] = 0;
    pisrblk->prev_vec = FARNIL;
}
