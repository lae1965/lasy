/*
    ivinstal - Install intervention function

int ivinstal (void     (*pfunc)(IV_EVENT *),
              char     *pident,     char     *pstack,   int      stksize,
              IV_KEY   *pkeytable,  int      numkeys,   IV_TIME  *ptimetable,
              int      numtimes,    int      option);

void cdecl (*pfunc)(IV_EVENT *)     Address of intervention function
char    * pident    Pointer to array of 16 bytes of identifying information
char    * pstack    Address of beginning of memory block (already allocated)
                    to be used for the intervention function's stack
int       stksize   Size (in bytes) of the stack (must be at least 64 bytes).
IV_KEY  * pkeytable Address of array of IV_KEY structures listing hot keys.
int       numkeys   Number of hot keys listed.
IV_TIME * ptimtable Address of array of IV_TIME structures listing timer events.
int       numtimes  Number of timer events listed.
int       option    Requirements of the intervention function.
                    Three bits are relevant (all other bits should be zero):

                    IV_DOS_NEED         DOS functions used.
                    IV_NO_DOS_NEED      No DOS functions used.
                    IV_DKEY_NEED        DOS functions 1-12 used.
                    IV_NO_DKEY_NEED     DOS functions 1-12 not used.
                    IV_FLOAT_NEED       Reinstall floating point vectors
                                        upon invocation.
                    IV_NO_FLOAT_NEED    No need to reinstall floating
                                        point vectors.

Description This function installs an intervention function (*pfunc)
            and arranges for it to be invoked when a hot key is
            pressed or when a given time of day passes.

            The hot key(s) (if any) are specified by one or more
            IV_KEY structures which describe the character value and
            scan code of the key and the action to be taken if the
            key is pressed:

            to invoke the intervention function;
            to put the intervention function to sleep; or
            to wake the intervention function from sleep.

            The array of IV_KEY structures can be moved or changed
            in size by modifying the pkeytab and ktsize members in
            the intervention control block (which is defined in
            ivctrlbl.asm).  (Turn interrupts off while doing so.)

            The timer event(s) (if any) are specified by one or more
            IV_TIME structures which describe whether each event is
            a specific time of day or whether it will recur
            periodically.  Each IV_TIME structure specifies a number
            of clock ticks to indicate the specific time of day or
            the period between invocations.  The array of IV_TIME
            structures must not be moved as long as the intervention
            function is installed.

            The calling function must allocate space for the
            intervention function's stack and pass its address as
            pstack.  This space must be static as long as the
            intervention function is installed.  2048 bytes is a
            typical stack size, but more may be needed if the
            intervention function (or any function invoked by it)
            has large local arrays.

            Two bits of "option" must be set to indicate how the
            intervention function uses DOS functions (if at all).
            If the intervention function uses DOS and these bits are
            incorrect, serious system malfunctions will result.  If
            option indicates that DOS functions are used, then the
            intervention function will not be invoked as frequently
            as if option were zero.

            The IV_FLOAT_NEED option causes the floating point
            interrupt vectors (if used by the floating point math
            library) to be temporarily reloaded whenever the
            intervention function is invoked.  Use the IV_FLOAT_NEED
            option if the following conditions are all met:

            1) your intervention function (*pfunc) uses floating
                point arithmetic;
            2) your program may be linked with a math library
                that installs interrupt vectors (see the README.DOC
                file for instructions);
            3) your intervention function may be invoked while
                other programs are running.

            If condition (1) is met, it may be safest to specify
            IV_FLOAT_NEED without regard to the other two
            conditions.

            The sixteen bytes of identifying information are stored
            in the intervention control block to permit other
            applications to detect this program.

Warning     Do not specify IV_TM_NONE for any of the timer events.

Returns     0               is successfully set; other values
            IV_INSTALLED    Already installed.

*/

#include "language.h"
#include <slintr.h>

extern void ivtimer(void);
extern void ivkeybd(void);
extern void ivdisk(void);
extern void ivdos(void);
extern void ividle(void);

static IV_CTRL far *pctrl = FARNIL;
static IV_TIME     *ptimtab;
static int      timtabsize;
static int      timindex;
static int      timaction;
static long     idleticks;
static void     (*ptask)(IV_EVENT *);

#if  (IS_NUM_FLOAT_VECS > 0)
static char    float_need;
static void    far *float_vec[IS_NUM_FLOAT_VECS];
#endif

static void scheduler (ALLREG *,ISRCTRL *,ISRMSG *);
static void timer_req (void);
static int  readytask (void);

int ivinstal (void     (*pfunc)(IV_EVENT *),
              char     *pident,     char     *pstack,   int      stksize,
              IV_KEY   *pkeytable,  int      numkeys,   IV_TIME  *ptimetable,
              int      numtimes,    int      option)
{
    int 	   ints_were_on,i;
    IV_VECTORS	   vecs;
    static ISRCTRL schedblk = {0};
    pctrl   = ivctrlbl ();

    ints_were_on = utintflg(UT_INTOFF);
    if (pctrl->enabled) {
        utintflg(ints_were_on);
        return IV_INSTALLED;
    }
    pctrl->enabled = 1;
    utintflg(ints_were_on);

    ptask   = pfunc;
    ptimtab = ptimetable;
    timtabsize  = numtimes;
    timindex    = -1;
    timaction   = IV_TM_NONE;
    idleticks   = 0L;

#if  (IS_NUM_FLOAT_VECS > 0)
    float_need	= (char)(0 != (option & IV_FLOAT_NEED));
    for (i = 0; i < IS_NUM_FLOAT_VECS; i++)
        float_vec[i] = isgetvec(IS_1ST_FLOAT_VEC + i);
#endif

    isprepbl (scheduler,"TCT5.00 INTSCHED",&schedblk,pstack,stksize,1);

    pctrl->psp = utpspseg;
    ivvecsin (IV_RETVEC,&pctrl->prev_vec);
    pctrl->dos_need  = (char)(0 != (option & (IV_DOS_NEED | IV_DKEY_NEED)));
    pctrl->dkey_need = (char)(0 != (option & IV_DKEY_NEED));
    pctrl->pkeytab   = pkeytable;
    pctrl->ktsize    = numkeys;
    pctrl->key_event.ch      = 0;
    pctrl->key_event.keycode = 0;
    pctrl->key_event.action  = IV_KY_NONE;
    pctrl->wait   = 0;
    pctrl->req    = 0;
    pctrl->awake  = 1;
    pctrl->timer_busy = 0;
    pctrl->kb_busy    = 0;
    pctrl->disk_busy  = 0;
    pctrl->idle_busy  = 0;
    pctrl->idle_safe  = 0;
    pctrl->pschedblk  = ((char far *)(&schedblk)) + ICB_ENTRY_OFFSET;
    utcritsf ();
    pctrl->pdos_crit = Adr_Crit;

    for (i = 0; i < sizeof(pctrl->_reserved); i++)
        pctrl->_reserved[i] = '\0';
    utmovmem(pctrl->ident, pident, sizeof(pctrl->ident));
    pctrl->sign  = (unsigned int)IV_SIGNATURE;
    pctrl->sign2 = (unsigned int)~IV_SIGNATURE;

    vecs.ptimer = ivtimer;
    vecs.pkeybd = ivkeybd;
    vecs.pdisk	= ivdisk;
    vecs.pdos	= ivdos;
    vecs.pidle	= ividle;
    ivvecsin (IV_SETVEC,&vecs);
    return (IV_NO_ERROR);
}

/*
    scheduler -- Invoke intervention function if appropriate

Description This function checks whether the intervention function
            is eligible to be invoked and invokes it if appropriate,
            based on the time of day and whether DOS services are
            necessary and available.  It also maintains certain
            fields in the intervention control block to control the
            intervention filters.

            This function is built as an interrupt service routine
            (ISR) to be called from the intervention timer filter
            via the ISR dispatcher.

            Interrupts are enabled.

Returns     Members of intervention control block:

            wait        Set to zero to release INT 21h and
                        INT 28h filters from wait loop.
            awake       Whether intervention code is asleep or awake.
            key_event   The most recent detected hot key is cleared.
            req         1   to indicate that the intervention
                            function needs service but must wait
                            for DOS availability;
                        0   to indicate that the intervention
                            function does not need service.

            Member of user's table of timer events:

            ptimetable[xxx].ticks
                        If this event is a one-time timer
                        event, the time of day is reset to
                        0xffffffffL, effectively preventing
                        the event from occurring again.
                        (The user can reschedule the event
                        by changing the ticks member again.)

*/

static void scheduler (ALLREG  *pregs, ISRCTRL *pisrblk, ISRMSG  *pmsg)
{
    IV_EVENT	signal;
    static char iret = (char)0xcf;
    int       cbreak_state;
    void far *pint_1bh;
    void far *pint_23h;
    unsigned  oldproc;
    int       dos_saved;
    char     *p;
#if  (IS_NUM_FLOAT_VECS > 0)
    void far *save_float[IS_NUM_FLOAT_VECS];
    int       float_switched;
#endif

    p = (char *) pregs;
    p = (char *) pisrblk;
    p = (char *) pmsg;
    p++;

    pctrl->wait = 0;
    if ((!pctrl->awake) && pctrl->key_event.action != IV_KY_WAKE)
        return;

    if (timaction == IV_TM_NONE)
    timer_req();

    if (pctrl->key_event.action != IV_KY_NONE || timaction != IV_TM_NONE) {
        if (readytask()) {
            pctrl->req = 0;
            if (timaction == IV_TM_INTERVAL)
                idleticks = 0L;
            else if (timaction == IV_TM_MOMENT)
                ptimtab[timindex].ticks = (long)0xffffffffL;
            utmovmem ((char far *) &signal.key,
                    (char far *) &pctrl->key_event,
                    sizeof(IV_KEY));
            signal.time_action = timaction;
            signal.time_index  = timindex;
            utgetclk(&signal.time);

            if (pctrl->dos_need) {
                cbreak_state = utctlbrk(CBRK_SET,CBRK_OFF);
                pint_1bh     = isgetvec(0x1b);
                pint_23h     = isgetvec(0x23);
                isputvec(0x1b,(void far *) &iret);
                isputvec(0x23,(void far *) &iret);
                oldproc      = iscurprc(IS_SETPROC,utpspseg);
                dos_saved    = 1;
            }
            else
                dos_saved    = 0;

#if  (IS_NUM_FLOAT_VECS > 0)
            if (float_need) {
                utintflg(UT_INTOFF);
                utpeekn(uttofar(0,4 * IS_1ST_FLOAT_VEC,char),
                        save_float, sizeof(save_float));
                utpoken(float_vec, uttofar(0,4 * IS_1ST_FLOAT_VEC,char),
                        sizeof(float_vec));
                utintflg(UT_INTON);
                float_switched = 1;
            }
            else
                float_switched = 0;
#endif

            (*ptask)(&signal);

#if  (IS_NUM_FLOAT_VECS > 0)
            if (float_switched) {
                utintflg(UT_INTOFF);
                utpoken(save_float, uttofar(0,4 * IS_1ST_FLOAT_VEC,char),
                        sizeof(save_float));
                utintflg(UT_INTON);
            }
#endif

            if (dos_saved) {
                iscurprc(IS_SETPROC,oldproc);
                isputvec(0x23,pint_23h);
                isputvec(0x1b,pint_1bh);
                if (cbreak_state != CBRK_OFF)
                    utctlbrk(CBRK_SET,cbreak_state);
            }
            if (pctrl->key_event.action == IV_KY_SLEEP)
                pctrl->awake = 0;
            else if (pctrl->key_event.action == IV_KY_WAKE)
                pctrl->awake = 1;

            pctrl->key_event.ch      = 0;
            pctrl->key_event.keycode = 0;
            pctrl->key_event.action  = IV_KY_NONE;
            timaction            = IV_TM_NONE;
        }
        else
            pctrl->req = 1;
    }
    if (timaction != IV_TM_INTERVAL)
        idleticks++;
}

/*
    timer_req - Scan table of timer events to find one
                that is eligible for service.

void timer_req (void);

Returns     IV_TM_NONE      if no timer event is eligible;
            IV_TM_MOMENT    if a one-time event is eligible;
            IV_TM_INTERVAL  if a periodic event is eligible.
            timindex        Index of found event in user's table of
                            timer events.  This is meaningless
                            if IV_TM_NONE is returned.
*/

static void timer_req (void)
{
    long now;
    int  i;

    utgetclk(&now);

    for (i = 0; i < timtabsize && timaction == IV_TM_NONE; i++) {
        if ((ptimtab[i].action == IV_TM_MOMENT
             && ptimtab[i].ticks  <=  now)
             || (   ptimtab[i].action == IV_TM_INTERVAL
             && ptimtab[i].ticks  <= idleticks)) {
                timaction = ptimtab[i].action;
                timindex  = i;
        }
    }
}

/*
    readytask - Return nonzero if intervention function is eligible for service.

int readytask (void);

Returns     1   if intervention function can be safely executed,
            0   if not.
*/

static int readytask (void)
{
    if (pctrl->idle_busy) {
        if (pctrl->idle_safe && !pctrl->dkey_need)
            return 1;
        else
            return 0;
    }
    else if (pctrl->dos_need) {
        if (pctrl->disk_busy || !utdosrdy())
            return 0;
        else
            return 1;
    }
    else
        return 1;
}
