; isdispat - Interrupt service routine dispatcher.
;
; Synopsis	This routine is NOT to be called from any user program!
;		It is designed to be called from the "Far Call"
;		instruction at the beginning of an ISR control block.
;
; Description	Blaise C TOOLS interrupt service routines (ISRs) are each
;		governed by their own ISR control block which is set up
;		by ISINSTAL.  When an ISR is installed, the address of
;		its control block is put into the relevant interrupt
;		vector.  The first item in each control block is a far
;		call instruction to this routine.  Therefore, when the
;		interrupt occurs, the next address in the control block
;		is pushed onto the stack and ISDISPAT is called.
;
;		This routine then performs the remaining operations
;		needed to invoke the user's ISR (which is assumed to be
;		written in C).	This consists of the following:
;
;               (1)     saving all registers;
;               (2)     finding the ISR control block and using the information
;                       therein;
;               (3)     deciding whether a new stack may be set up for this
;                       invocation of the ISR (if not, then ISDISPAT
;                       just returns);
;               (4)     setting up and using a new stack;
;               (5)     preparing copies of the ISR's arguments, including
;                       an ALLREG structure containing the values of the
;                       machine registers that were in effect at the time of
;                       the interrupt;
;               (6)     calling the ISR as a normal C function;
;               (7)     retrieving the register values in the ALLREG structure;
;               (8)     restoring the original stack;
;               (9)     discarding the stack that was created for this
;                       invocation of the ISR (as well as any stacks left
;                       by intervening invocations that did not return);
;               (10)    restoring the machine registers using values from
;                       the ALLREG structure;
;               (11)    performing an IRET or RETF, as selected by the ISR.
;
;		The interrupt state (that is, whether interrupts are
;		enabled or disabled) is preserved unless the ISR
;		requests otherwise.
;
; Returns       0       if okay
;               1       if interrupt vector is invalid.
;------------------------------------------------------------
;  02/01/91 - Patch remove STK_GROWTH_LIMIT manipulations for Turbo.
;  (Yudin V)  Therefore, user must disabling of stack-checking
;             for ISR     SELF!
;

include   begasm.mac
beginMod  isdispat

beginCseg isdispat

	 public    _isdispat
_isdispat proc	     far


ISRCTRL struc			    ; ISRCTRL:	ISR control block
                            ; (This must match declaration in
                            ; BINTRUPT.H.)
                            ;
icb_fcall_opcode    dw  ?   ; NOP + Far call opcode (0x9A90)
icb_isrdisp	    dd	?	    ; Address of ISR dispatcher
icb_iret_opcode     dw  ?   ; IRET + RETF opcodes (0xcbcf)
                            ;   (Offset of icb_iret_opcode
                            ;   is on stack on entry to ISDISPAT.)
                            ;
icb_isrstk_r        dw  ?   ; ISR stack region:  offset
icb_isrstk_s        dw  ?   ;            segment
icb_isrstksize      dw  ?   ; ISR stack size
icb_isrsp	    dw	?	    ; ISR SP value at start of
                            ;   current ISR call
                            ;
icb_isrds	    dw	?	    ; DS value required by ISR
icb_isres	    dw	?	    ; ES value required by ISR
icb_isr 	    dd	?	    ; Address of ISR itself
icb_isrpsp	    dw	?	    ; PSP of program containing ISR
                            ;
icb_prev_vec        dd  ?   ; Previous value of vector
                            ;
icb_level	    dw	?	    ; Number of calls in progress
                            ;   (0 if not in progress)
icb_limit	    dw	?	    ; Maximum number of nested calls
icb_signature       dw  ?   ; Signature identifying this
                            ;   structure
icb_sign2	    dw	?	    ; One's complement of
                            ;   "signature"
icb_ident	    db	16 dup (?)  ; Identifying name
icb_control	    dw	?	    ; Bit fields to control
                            ;   dispatcher options
icb_status	    dw	?	    ; Status info left by
                            ;   dispatcher
icb_scratch	    db	10 dup (?)  ; Scratch space for use by
                            ;   dispatcher & related programs
ISRCTRL ends

;   Exit style codes -- these must match BINTRUPT.H.

IEXIT_NORMAL	equ	0
IEXIT_RETF	equ	1

ALLREG	 struc			    ; ALLREG structure
reg_ax   dw ?               ; (This must match declaration in
reg_bx   dw ?               ; BUTIL.H).
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

allreg_size	db  ?
ALLREG	 ends

; ****************   BEGIN CODE HERE   ******************

	 assume ds:nothing,es:nothing,ss:nothing

;   Save initial register values so we can have some workspace.

     push   bp                  ; Save original BP (used only here >--\ )
     mov    bp,sp               ;                                     |
     push   bp                  ; (Entry value of SP) - 2             |
     push   [bp]                ; Original BP                    <----/
     push   ax                  ; Original AX
     push   bx                  ; Original BX
     push   cx                  ; Original CX
     push   dx                  ; Original DX
     push   ds                  ; Original DS
     pushf                      ; Original flags
     push   es                  ; Original ES
     push   si                  ; Original SI

	 add	word ptr [bp-2],6   ; Convert entry SP-2 to "Caller's SP"
                                ;   thus ignoring the first BP we pushed
                                ;   and segment and offset pushed by
                                ;   the far call in the control block.

;   Set up DS:BX to address these values we just saved on caller's stack.

	 mov	bx,ss
	 mov	ds,bx
	 assume ds:nothing
	 mov	bx,bp

                                    ; Current stack contents
                                    ; (high addresses to low):
                                    ;
caller_flags equ word ptr [bx+10]   ;	Caller's flags
caller_cs   equ word ptr [bx+8]     ;	Caller's CS
                                    ; "Caller's SP" points to . . .
caller_ip   equ word ptr [bx+6]     ;	Caller's IP (see note below)
ret_seg     equ word ptr [bx+4]     ;	Segment of ISR Control Block
                                    ; Entry SP pointed to . . .
ret_offset  equ word ptr [bx+2]     ;	Offset of icb_iret_opcode in
                                    ;     ISR Control Block
                                    ; BX and BP point to . . .
                                    ;   Original BP (won't be used again)
caller_sp   equ word ptr [bx-2]     ;  "Caller's SP value"
orig_bp     equ word ptr [bx-4]     ;	Original BP again
orig_ax     equ word ptr [bx-6]     ;	Original AX
orig_bx     equ word ptr [bx-8]     ;	Original BX
orig_cx     equ word ptr [bx-10]    ;	Original CX
orig_dx     equ word ptr [bx-12]    ;	Original DX
orig_ds     equ word ptr [bx-14]    ;	Original DS
orig_flags  equ word ptr [bx-16]    ;	Original flags
orig_es     equ word ptr [bx-18]    ;	Original ES
orig_si     equ word ptr [bx-20]    ;	Original SI

	; Note:  the value called "caller's SP" is the SP value
	;   immediately after the INT instruction that invoked
	;   this interrupt.  Any adjustments to "caller's SP" should
	;   be RELATIVE (e.g., to discard some stack items just before
	;   returning to caller, let the ISR add a fixed value to SP in
	;   the ALLREG structure).

;   Set up ES:SI to point to beginning of ISR control block.

	 mov	es,ret_seg
	 assume es:nothing
	 mov	si,ret_offset
	 sub	si,(icb_iret_opcode-icb_fcall_opcode)

;   Make sure nesting depth isn't over limit.

	 mov	ax,es:[si].icb_level
	 cmp	ax,es:[si].icb_limit
	 jb	level_ok

     jmp    exit                    ; Nested too deep:  just exit.

                                    ; Check that stack size is adequate.

;   Generate a new stack for the ISR:  increment level & icb_isrsp.

level_ok:                           ; Nesting depth is okay.

     inc    ax                      ; Increment nesting depth.
	 cli
	 mov	es:[si].icb_level,ax
     push   ax                      ; Local copy of nesting depth
                                    ;   (will be popped later).

     mov    ax,es:[si].icb_isrsp    ; Former ISR stack pointer (plus 6
     mov    dx,ax                   ;   for safety) will serve as
     add    dx,6                    ;   stack growth limit.
	 add	ax,es:[si].icb_isrstksize  ; Raise stack pointer
	 mov	es:[si].icb_isrsp,ax	   ;   for a new stack.

;   Switch to the ISR's own stack (which we just generated).
;   Interrupts must be disabled during this.

     mov    ax,sp                   ; Save old SP (old SS is in DS register)
     mov    ss,es:[si].icb_isrstk_s ; Switch to ISR's own stack.
	 assume ss:nothing
	 mov	sp,es:[si].icb_isrsp

     push   orig_flags              ; Restore original flags to
     popff                          ;  (possibly) re-enable interrupts.

;   Now create an ALLREG structure on the ISR's own stack.

	 push	caller_ip
	 push	caller_sp
	 push	orig_bp
	 push	caller_flags
	 push	caller_cs
	 push	ds		    ; Caller's SS
	 push	orig_es
	 push	orig_ds
	 push	di
	 push	orig_si
	 push	orig_dx
	 push	orig_cx
	 push	orig_bx
	 push	orig_ax
     mov    bp,sp                       ; Now BP points to ALLREG structure.
regs	 equ	[bp]

;   Create two-word area for messages between dispatcher (us) and ISR.

     push   orig_flags                  ; "Working" flags -- depending on how
                                        ;   dispatcher exits, these may persist
                                        ;   on return to caller.
	 mov	cx,IEXIT_NORMAL
     push   cx                          ; Default exit style.
message  equ	dword ptr [bp-4]

                                        ;   Create local vector to the ISR

IF longProg
	 push	word ptr es:[si].icb_isr+2
	 push	word ptr es:[si].icb_isr
vector_to_isr	equ	dword ptr [bp-8]
ELSE
	 push	word ptr es:[si].icb_isr
vector_to_isr	equ	word ptr [bp-6]
ENDIF

;   Save items needed locally by dispatcher (us)

     push   ds                          ; Caller's SS
     push   ax                          ; SP in caller's stack just before
                                        ;   stack switch
     push   bx                          ; Frame pointer in caller's stack.
     push   es                          ; Segment of ISR control block.
     push   si                          ; Offset  of ISR control block.

;   Point to ISR's data segment

	 mov	ds,es:[si].icb_isrds
	 assume ds:nothing


;   Push ISR's arguments.

if longData

     push   ss                          ; Address of message area.
	 lea	ax,message
	 push	ax

     push   es                          ; Address of ISR control block.
	 push	si

     push   ss                          ; Address of ALLREG structure.
	 lea	ax,regs
	 push	ax

else

     lea    ax,message                  ; Address of message area.
	 push	ax

     push   si                          ; Address of ISR control block.
                                        ;  (For S, P, and M model programs
                                        ;  this must be in ISR's default
                                        ;  data segment.)

     lea    ax,regs                     ; Address of ALLREG structure.
	 push	ax

endif

;   Set up remaining registers required by ISR.

	 cld
	 mov	es,es:[si].icb_isres
	 assume es:nothing

;   Actually call the ISR.

	 call	vector_to_isr

;   Discard the ISR's arguments in usual C fashion.

if longData
	 add	sp,12
else
	 add	sp,6
endif

;   Restore dispatcher's various pointers.

	 pop	si
     pop    es                          ; Now ES:SI point to ISR control block.
	 assume es:nothing
	 pop	bx
	 pop	ax
     pop    ds                          ; DS:AX is SS:SP in caller's stack
     assume ds:nothing                  ;   just before stack switch.
                                        ; DS:BX is frame pointer in caller's
                                        ;   stack.

;   Discard the local vector to the ISR.

if longProg
	 add	sp,4
else
	 inc	sp
	 inc	sp
endif

;   Read the message area & react to it.

     pop    dx                          ; "Exit style"
     pop    orig_flags                  ; "Working flags"

;   Transfer ALLREG structure values into proper registers or into
;   holding locations.	These values may have been modified by the ISR.

	 pop	orig_ax
	 pop	orig_bx
	 pop	orig_cx
	 pop	orig_dx
	 pop	orig_si
	 pop	di
	 pop	orig_ds
	 pop	orig_es
     inc    sp                          ; Discard SS value from structure.
	 inc	sp
	 pop	caller_cs
	 pop	caller_flags
	 pop	orig_bp
	 pop	caller_sp
	 pop	caller_ip

;   Switch back to caller's stack.

	 mov	cx,ds
	 cli
	 mov	ss,cx
	 assume ss:nothing
	 mov	sp,ax
                                        ; Now both DS and SS refer to
                                        ;  caller's stack segment.
                                        ; Leave interrupts disabled.

;   Discard the ISR stack created above, i.e. reset icb_level and icb_isrsp.
;
;   If icb_level exceeds our local (stacked) copy of level, then
;     this ISR invocation returned BEFORE a deeper-nested invocation returned.
;     In this case, assume that deeper invocations will never return.
;     Adjust icb_level and icb_isrsp to discard deeper stacks.
;
;   Interrupts are NOT okay during this operation.

	 mov	cx,es:[si].icb_level
     pop    ax                          ; Local copy of nesting depth
	 dec	ax
	 mov	es:[si].icb_level,ax
     sub    cx,ax                       ; CX = number of stacks to discard
                                        ;      (normally 1)

	 jbe	stacks_discarded
	 mov	ax,es:[si].icb_isrstksize
discard_stack:
     sub    es:[si].icb_isrsp,ax        ; Decrement beginning_SP once
     loop   discard_stack               ;   for each stack discarded.
stacks_discarded:
                                        ; Interrupts now okay.

;   Consider optional exit style

	 cmp	dx,IEXIT_RETF
	 je	retf_exit

                                        ; If exit style code not recognized,
                                        ;   fall through to normal exit.

;   Normal exit:  restore everything and do IRET.
;
;   Some of the values we restore were copied from the ALLREG structure,
;     so they may have been modified by the ISR.

exit:    pop    si
	 pop	es
     popff                              ; Install "original" flags.
                                        ; These may have been modified via
                                        ;   the "WORKING_FLAGS" item in the
                                        ;   message area.
	 pop	ds
	 pop	dx
	 pop	cx
	 pop	bx
	 pop	ax
	 pop	bp
     pop    sp                          ; Unless modified by ISR,
                                        ;   SP now contains the value
                                        ;   it had after the caller
                                        ;   performed an INT.
	 iret

;   Special exit:  restore everything and do a RETF.
;
;   Some of the values we restore were copied from the ALLREG structure,
;     so they may have been modified by the ISR.

retf_exit:
	 pop	si
	 pop	es
     popff                              ; Install "original" flags.
                                        ; These may have been modified via
                                        ;   the "working flags" item in the
                                        ;   message area.
	 pop	ds
	 pop	dx
	 pop	cx
	 pop	bx
	 pop	ax
	 pop	bp
     pop    sp                          ; Unless modified by ISR,
                                        ;   SP now contains the value
                                        ;   it had after the caller
                                        ;   performed an INT.
	 ret

_isdispat endp

endCseg    isdispat
end
