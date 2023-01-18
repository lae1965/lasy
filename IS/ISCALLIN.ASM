;
; iscallin - Simulate software-interrupt-style call to interrupt routine
;
; bool iscallin (void far *pisr, ALLREG *preg);
;
; void far *pisr        Far pointer to interrupt service routine
; ALLREG *preg          Standard registers
;
; Description   This function invokes the interrupt service routine
;		whose address is pisr and returns the results in the
;		most general fashion.  All values specified in *preg are
;		passed to the interrupt service routine in the
;		corresponding registers except for CS, IP, SS, and SP.
;		All the resulting register contents are returned in
;		*preg (again, except for CS, IP, SS, and SP).
;
;		The interrupt state (that is, whether interrupts are
;		enabled or disabled) is preserved.
;
; Returns       YES     if okay
;               NO      if pisr vector is invalid.
;
;

include begasm.mac
beginProg iscallin

YES      equ    1
NO       equ    0                    ; Error code invalid interrupt vector.

pisr	 equ	dword ptr [bp + stkoff]

	 if	LONGDATA
preg	 equ	dword ptr [bp + stkoff + 4]
	 else
preg	 equ	word ptr [bp + stkoff + 4]
	 endif

ALLREG	 struc			    ; ALLREG structure
reg_ax	 dw	?		    ; (This must match declaration in
reg_bx	 dw	?		    ; BUTIL.H).
reg_cx	 dw	?
reg_dx	 dw	?
reg_si	 dw	?
reg_di	 dw	?
reg_ds	 dw	?
reg_es	 dw	?
reg_ss	 dw	?
reg_cs	 dw	?
reg_flags dw	?
reg_bp	 dw	?
reg_sp	 dw	?
reg_ip	 dw	?
ALLREG	 ends

    StartCode

        les    si,pisr         ; Obtain interrupt vector
        assume es:nothing      ; Now ES:SI is address of interrupt
                               ; handler.
        mov    ax,es           ; Quit if pisr is 0:0.
        or ax,si
        jnz    ok_vector

bad_vector:
        mov    ax,NO
        jmp    short exit

ok_vector:
                               ; Obtain contents of preg.
        if LONGDATA
        lds    bx,preg
        assume ds:nothing
        else
        mov    bx,preg
        endif
                                ; Now DS:BX points to *preg
                                ; (ALLREG structure is at DS:BX).

                                ; Begin setting up for the call:

        push   ds               ; Step 1:  Save stack frame pointer
        push   bp               ; so we can restore it in Step 10.

        pushf                   ; Step 2:  Save our own flags
                                ; for restoration in Step 9.

                                ; Step 3:  Push the stack items that
                                ; the ISR will see:  a copy of its
                                ; flags and its return address
                                ; (CS:ret_addr).

        mov    ax,[bx.reg_flags]   ; Push the flags
        push   ax               ;   without clearing IF and TF.
        push   cs               ; Push CS.
        mov    cx,offset ret_addr
        push   cx               ; Push ret_addr.

                                ; Step 4:
                                ; We will enter the handler by doing
                                ; an IRET, so push the flags and the
                                ; vector.

        and    ah,0fch          ; Push the flags
        push   ax               ;   with IF and TF cleared.
        push   es               ; Push the vector.
        push   si

        mov    ax,[bx.reg_ax]   ; Step 5:  Load the easy registers.
        mov    cx,[bx.reg_cx]
        mov    dx,[bx.reg_dx]
        mov    si,[bx.reg_si]
        mov    di,[bx.reg_di]
        mov    es,[bx.reg_es]
        assume es:nothing
        mov    bp,[bx.reg_bp]

        push   [bx.reg_bx]      ; Step 6:  Load DS and BX
        mov    ds,[bx.reg_ds]   ; by temporarily using the stack.
        assume ds:nothing
        pop    bx

        iret                    ; Step 7:  Load the flags and jump
                                ; to the handler.  (This IRET uses &
                                ; removes what was pushed in Step 4.)

ret_addr:assume es:nothing,ds:nothing
                                ; The interrupt handler brought us here
                                ; by performing an IRET, thus popping
                                ; what was pushed in Step 3.
                                ; (However, the interrupt handler
                                ; may choose to modify the flags
                                ; before popping them.)

        push   bp               ; Step 8:  Temporarily preserve
        push   ds               ; returned values of BP, DS, BX,
        push   bx               ; and flags.
        pushf

        mov    bp,sp            ; Step 9:  Restore our own flags
        mov    bx,[bp+8]        ; (which were pushed in Step 2).
        push   bx               ; (They may contain special values
        popff                   ; required by the C environment).

        lds    bp,[bp+10]       ; Step 10:  Restore stack frame pointer
        assume ds:nothing       ; (pushed in Step 1) so we can find preg.

                                ; Obtain contents of preg.
        if LONGDATA
        lds    bx,preg
        assume ds:nothing
        else
        mov    bx,preg
        endif
                                ; Now DS:BX points to *preg
                                ; (ALLREG structure is at DS:BX).

        mov    [bx.reg_ax],ax   ; Store the easy registers in
        mov    [bx.reg_cx],cx   ; the *preg structure.
        mov    [bx.reg_dx],dx
        mov    [bx.reg_si],si
        mov    [bx.reg_di],di
        mov    [bx.reg_es],es

        pop    [bx.reg_flags]   ; Transfer values pushed in Step 8
        pop    [bx.reg_bx]      ; to the *preg structure.
        pop    [bx.reg_ds]
        pop    [bx.reg_bp]

        add    sp,6             ; Discard values pushed in
                                ; Steps 2 and 1.

        mov    ax,YES           ; Success code.

exit: 

    FinishCode
endProg iscallin
end
