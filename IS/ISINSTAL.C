/*
    isinstal - Install interrupt service routine

bool  isinstal (int     intype,  void    (*pfunc)(ALLREG *,ISRCTRL *,ISRMSG *),
                char    *pident, ISRCTRL *pisrblk, char  *pstack,
                int     stksize, int     stknum);

int       intype    Interrupt type number
void      cdecl (*pfunc)(ALLREG *,ISRCTRL *,ISRMSG *)
                    Pointer to interrupt service routine
char    * pident    Pointer to array of 16 bytes of identifying information
ISRCTRL * pisrblk   Pointer to ISR control block (already allocated)
char    * pstack    Address of beginning of memory block to be used for
                    the stack(s) for this ISR (already allocated)
int       stksize   Size (in bytes) of one ISR stack ( > 64 b)
int       stknum    Maximum depth of nested invocations of this ISR ( > 1)

Description ISINSTAL installs an interrupt service routine (ISR) in
            the interrupt vector of the specified type.  This
            includes (1) completing all the values in the ISR
            control block; and (2) storing the address of the ISR
            control block in the interrupt vector.

            The ISR is in effect (and may be invoked) as soon as
            operation (2) is complete, i.e., even before ISINSTAL
            returns to its caller.

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
            in the ISR control block so that other programs may
            examine the interrupt vector and detect the presence of
            this ISR.

Example     The following code fragment illustrates one way to
            declare the arguments to ISINSTAL and how to use
            ISPUTVEC to restore the contents of the interrupt vector
            when the ISR is no longer in use.

            #include <slintr.h>

            #define MY_INTYPE       0x60
            #define MY_STK_SIZE     2000
            #define MY_NUM_STKS        5

            void   my_isr (ALLREG *,ISRCTRL *,ISRMSG *);
            static ISRCTRL isrblk;
            char   pstacks;

            if (NIL == (pstacks = malloc(MY_STK_SIZE*MY_NUM_STKS)))
                exit(3);
            isinstal (MY_INTYPE,my_isr,"THIS IS MY ISR.",
                &isrblk,pstacks,MY_STK_SIZE,MY_NUM_STKS);

            .......    (Use the ISR.)

            isputvec (intype,isrblk.prev_vec);
            free (pstacks);

Returns     YES     if the vector is is successfully set
            NO      Interrupt type out of range
*/

#include "language.h"
#include <slintr.h>

typedef char CHAR16[16];

bool isinstal (int     intype,
               void    (*pfunc)(ALLREG *,ISRCTRL *,ISRMSG *),
               char    *pident,
               ISRCTRL *pisrblk,
               char    *pstack,
               int     stksize,
               int     stknum)
{
    int ints_were_on;

    if (utrange (intype,0,255))
        return (NO);
    isprepbl (pfunc,pident,pisrblk,pstack,stksize,stknum);
    ints_were_on = utintflg (UT_INTOFF);
    pisrblk->prev_vec = isgetvec (intype);
    isputvec (intype,((char far *) pisrblk) + ICB_ENTRY_OFFSET);
    if (ints_were_on)
        utintflg (UT_INTON);
    return (YES);
}
