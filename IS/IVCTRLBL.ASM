;
; ivctrlbl - Return address of intervention control block.
;
; IV_CTRL far *ivctrlbl (void);
;
; Description	This function returns the address of the intervention
;		control block (IV_CTRL structure).  This module also
;		defines the structure and the intervention filters.
;
;		Any program using intervention code contains a unique
;		intervention control block to control the intervention
;		filters that give control to the installed intervention
;		function at the proper time.  The block is located in
;		the code segment of the filters, so in general it must
;		be referenced via a far pointer.
;
;		It is seldom necessary to access the intervention
;		control block directly.  IVINSTAL performs all
;		operations necessary to install the intervention filter.
;		However, if the intervention mechanism should fail, it
;		may be useful to examine the following fields in the
;		structure:
;
;               key_event
;               wait
;               req
;               awake
;               timer_busy
;               kb_busy
;               disk_busy
;               idle_busy
;               idle_safe
;
; Returns       Address of intervention control block.
;

include    begasm.mac
beginMod   ivctrlbl

			    ; Hot key action codes:
IV_KY_NONE	equ	0   ;  No hot key or action.
IV_KY_WAKE	equ	3   ;  Invoke intervention function and
			    ;	awaken it if asleep.

BEGIN_IV_FILTER macro			    ;; BEGIN_IV_FILTER:  Macro
		local	begin_label	    ;;	 to build pointer to
					    ;;	 the IV_CTRL structure
		jmp	short begin_label   ;;	 at the beginning of each
					    ;;	 filter.
		dw	offset begin_block
begin_label:
		endm

IV_VECTORS	struc	    ; IV_VECTORS:  set of interrupt vectors
			    ;  used by intervention code.
			    ; (Must match version in BINTERV.H exactly!)

    ptimer	dd	0   ; INT 08h:	Timer tick.
    pkeybd	dd	0   ; INT 09h:	Keystroke.
    pdisk	dd	0   ; INT 13h:	BIOS disk services.
    pdos	dd	0   ; INT 21h:	DOS functions.
    pidle	dd	0   ; INT 28h:	DOS idle.
IV_VECTORS	ends

beginCseg  ivctrlbl

;Note:  the following sequence of nineteen variables must exactly
;       match the IV_CTRL structure as defined in BINTERV.H.

begin_block	 equ $

signature    dw  0          ; Signature identifying this structure.
sign2        dw  0          ; One's complement of "signature".
ident		 db  16 dup (0) ; Identifying name.
psp          dw  0          ; PSP of this program.
enabled      dw  0          ; Nonzero if enabled (this prevents the
                            ;  filters from being installed twice).
pgate        dd  0          ; Gateway for invoking this program
                            ;  from outside.
prev_vec	 IV_VECTORS <>	; Previous values of interrupt vectors.
                            ;
dos_need     db  0          ; Nonzero if intervention function
                            ;  uses DOS services.
dkey_need    db  0          ; Nonzero if intervention function
                            ;  uses DOS functions 1-12.
                            ;
pkeytab 	 dd  0		; Table of hot keys.
ktsize		 dw  0		; Size of hot key table.
				;
keyfound	 dw  0		; Found key, if any.
keyaction	 dw  IV_KY_NONE ;
				;
				; Internal state of filters:
				;
iv_wait 	 db  0		; Nonzero if INT 21h or INT 28h filter
				;  is cycling, waiting for the
				;  intervention function to be invoked.
req		 db  0		; Nonzero if intervention function
				;  needs invocation.
awake		 db  0		; Zero if intervention function is
				;  asleep.
				;
				; Busy flags:
timer_busy	 db  0		;  INT 08h filter.
kb_busy 	 db  0		;  INT 09h filter.
disk_busy	 db  0		;  INT 13h filter.
idle_busy	 db  0		;  INT 28h filter.
				;
idle_safe	 db  0		; Nonzero if INT 28h is in progress
				;  and DOS functions are available
				;  (but not functions 1-12).
				;
pschedblk	 dd  0		; Entry point into scheduler's ISR
				;  control block.
				;
pdos_crit	 dd  0		; Address of DOS critical section flag
				;  (busy flag).
				;
_reserved	 db  8 dup (0)	; Reserved for future use.

    ; End of IV_CTRL structure.


    ;*********************************************************
    ;
    ;	Internal variables

timer_caller_offset dw	?	; Return address for INT 08h.
timer_caller_seg    dw	?

kb_caller_offset    dw	?	; Return address for INT 09h.
kb_caller_seg	    dw	?

disk_caller_offset  dw	?	; Return address for INT 13h.
disk_caller_seg     dw	?

dos_caller_offset   dw	?	; Return address for INT 21h.
dos_caller_seg	    dw	?
dos_save_flags	    dw	?	; Flags pushed by INT 21h caller.

idle_caller_offset  dw	?	; Return address for INT 28h.
idle_caller_seg     dw	?


    ;*********************************************************
    ;
    ;	Begin code for IVCTRL function.

beginProc   ivctrlbl
	assume	    ds:nothing, es:nothing, ss:nothing

	mov	    dx,cs		    ; Load address of intervention
	mov	    ax,offset begin_block   ;  control block into DX:AX.
	ret

endProc     ivctrlbl


    ;*********************************************************
    ;
    ;	IVTIMER -- INT 8 filter
    ;
    ;	This function calls the scheduler on every clock tick,
    ;	except that it never does so in a nested fashion,
    ;	so it is reentrant.

beginProc   ivtimer             ; (IVTIMER will perform a far return.)

	assume	    ds:nothing, es:nothing, ss:nothing
	BEGIN_IV_FILTER

	pushf
	cli				; Disable interrupts until we can
					;  set the busy flag.
	cmp	    timer_busy,0	; If this filter is already active,
	je	    timer_enter
	popff
	jmp	    prev_vec.ptimer	;  jump to previous handler.
					;  The previous handler will return
					;  directly to the original caller.

timer_enter:
	mov	    timer_busy,1	; Prevent nesting.
	popff
	pop	    timer_caller_offset ; Save caller's address.
	pop	    timer_caller_seg

	call	    prev_vec.ptimer	; Call previous handler, which will
					;  pop the flags from the top of
					;  the stack, send the EOI to the
					;  interrupt controller, and
					;  reenable interrupts.

	pushf				; Call scheduler.
	call	    pschedblk

	push	    timer_caller_seg
	push	    timer_caller_offset
	mov	    timer_busy,0	; No longer any need to protect
					;  stored copy of caller's address.

	db	    0cbh		; Far return.

endProc     ivtimer

    ;*********************************************************
    ;
    ;	IVKEYBD -- INT 9 filter
    ;
    ;	This filter checks the most recent keystroke to see if it
    ;	is a hot key.  If so, it records the keystroke and
    ;	removes it from the type-ahead buffer.

beginProc   ivkeybd
	assume	    ds:nothing, es:nothing, ss:nothing
	BEGIN_IV_FILTER

	pushf
	cli				; Disable interrupts until we can
					;  set the busy flag.
	cmp	    kb_busy,0		; If this filter is already active,
	je	    kb_enter
	popff
	jmp	    prev_vec.pkeybd	;  jump to previous handler.
					;  The previous handler will return
					;  directly to the original caller.

kb_enter:
	mov	    kb_busy,1		; Prevent nesting.
	popff
	pop	    kb_caller_offset	; Save caller's address.
	pop	    kb_caller_seg
	call	    prev_vec.pkeybd	; Call previous handler, which will
					;  pop the flags from the top of
					;  the stack, send the EOI to the
					;  interrupt controller, and
					;  reenable interrupts.
	pushf
	push	    ax
	push	    bx
	push	    cx
	push	    dx
	push	    bp
	push	    si
	push	    di
	push	    ds
	push	    es

	cmp	    keyaction,IV_KY_NONE
	jne	    kb_exit	    ; Quit if any hot key has already
				    ;  been detected.

				    ; Scan table of hot keys:
	mov	    cx,ktsize	    ;
	jcxz	    kb_exit	    ;	Quit if table is empty.

	mov	    ah,1	    ; Examine most recent keystroke.
	int	    16h
	jz	    kb_exit	    ; Quit if none recorded.

				    ; AX contains char & key code.
	les	    di,pkeytab
	cld

kb_loop:
	scasw			    ; Check one hot key listing.
	je	    kb_found
	inc	    di		    ; Step over its action code.
	inc	    di
	loop	    kb_loop

	mov	    keyfound,0	    ; Most recent keystroke is not
				    ;  a hot key.
	mov	    keyaction,IV_KY_NONE
	jmp	    short kb_exit

kb_found:			    ; Found a hot key.
	mov	    bx,es:[di]	    ; Get key action for found key.

	cmp	    awake,0
	jne	    kb_awake	    ; If intervention function is asleep
	cmp	    bx,IV_KY_WAKE   ;  and this is not the wakeup key,
	jnz	    kb_exit	    ;  then quit.

kb_awake:
	mov	    keyfound,ax     ; Record hot key for access by the
	mov	    keyaction,bx    ;  scheduler.
	mov	    ah,0	    ; Drop the keystroke from the
	int	    16h 	    ;  type-ahead buffer.

kb_exit:
	pop	    es
	pop	    ds
	pop	    di
	pop	    si
	pop	    bp
	pop	    dx
	pop	    cx
	pop	    bx
	pop	    ax
				    ; Top of stack contains flags returned
				    ;  by previous INT 9 handler.

	push	    kb_caller_seg
	push	    kb_caller_offset
	mov	    kb_busy,0
	iret

endProc     ivkeybd


    ;*********************************************************
    ;
    ;	IVDISK -- INT 13h filter (BIOS disk services)
    ;
    ;	The sole purpose for this filter is to maintain the disk_busy
    ;	flag telling the scheduler when BIOS disk operations are in
    ;	progress.

	beginProc   ivdisk	    ; (IVDISK will perform a far return.)

	assume	    ds:nothing, es:nothing, ss:nothing
	BEGIN_IV_FILTER

	pushf
	cli				; Disable interrupts until we can
					;  set the busy flag.
	cmp	    disk_busy,0 	; If this filter is already active,
	je	    disk_enter
	popff
	jmp	    prev_vec.pdisk	;  jump to previous handler.
					;  The previous handler will return
					;  directly to the original caller.


disk_enter:
	mov	    disk_busy,1 	; Prevent nesting.
	popff
	pop	    disk_caller_offset	; Save caller's address.
	pop	    disk_caller_seg

	call	    prev_vec.pdisk	; Call previous handler, which will
					;  pop the flags from the top of
					;  the stack.

	push	    disk_caller_seg
	push	    disk_caller_offset
	mov	    disk_busy,0

	db	    0cbh		; Far return.

endProc     ivdisk

    ;*********************************************************
    ;
    ;	IVDOS -- INT 21h filter
    ;
    ;	The purpose of this filter is to delay the return from
    ;	DOS calls if the intervention function needs DOS and is
    ;	waiting for service.  After invoking DOS, this filter may
    ;	enter a wait loop that will soon be released by the
    ;	scheduler (which is invoked by a clock tick).

beginProc   ivdos           ; (IVDOS will perform a far return.)

	assume	    ds:nothing, es:nothing, ss:nothing
	BEGIN_IV_FILTER

	push	    bx
	xchg	    ax,bx	    ; Save AX in BX.
	lahf			    ; Save flags in AH.

	cmp	    dos_need,0	    ; If intervention function doesn't
	je	    dos_jmp_prev    ;  need DOS, just invoke DOS directly.

	cmp	    timer_busy,0    ; If timer tick is busy,
	jne	    dos_jmp_prev    ;  just invoke DOS directly.

				    ; Certain DOS functions examine their
				    ;  return address or do not return to
				    ;  their caller.  We must invoke them
				    ;  directly:
				    ;
				    ; (Remember that we saved the function
				    ;  number in BH.)
				    ;
	cmp	    bh,26h	    ; Function 26h:  Make PSP
	je	    dos_jmp_prev    ;  (examines caller's address)
				    ;
	cmp	    bh,4Bh	    ; Function 4Bh:  Exec
	je	    dos_jmp_prev    ;  (can destroy registers on return)
				    ;
	cmp	    bh,00h	    ; Function 00h:  Terminate
	je	    dos_jmp_prev    ;  (does not return)
				    ;
	cmp	    bh,31h	    ; Function 31h:  Keep process
	je	    dos_jmp_prev    ;  (does not return)
				    ;
	cmp	    bh,4ch	    ; Function 4ch:  Terminate
	jne	    dos_enter	    ;  (does not return)


dos_jmp_prev:
	sahf			    ; Restore the flags.
	xchg	    ax,bx
	pop	    bx
	jmp	    prev_vec.pdos   ; Jump to previous handler.
				    ;  The previous handler will return
				    ;  directly to the original caller.

dos_enter:			    ; This is a normal DOS function call.

	sahf			    ; Restore the flags.
	xchg	    ax,bx
	pop	    bx

	cli				; Rearrange the top items on the
	pop	    dos_caller_offset	; stack to bring the flags to
	pop	    dos_caller_seg	; the top.
	pop	    dos_save_flags
	push	    dos_caller_seg
	push	    dos_caller_offset
	push	    dos_save_flags

	call	    prev_vec.pdos   ; On return, DOS pops the flags from
				    ;  the top of the stack.

	pushf
	cli

	cmp	    req,0	    ; Skip the wait loop if the intervention
	je	    dos_exit	    ;  function doesn't require service
				    ;  at this time.

	push	    es		    ; Check the DOS critical section flag:
	push	    di
	les	    di,pdos_crit
	cmp	    es:byte ptr [di],0
	pop	    di
	pop	    es
	jne	    dos_exit	    ;  if DOS is busy, skip the wait loop
				    ;  so DOS can complete its business.

	mov	    iv_wait,1
	sti
dos_wait:			    ; Wait until somebody sets iv_wait
	cmp	    iv_wait,0	    ;  to zero.
	jne	    dos_wait

dos_exit:
				    ; Top of stack has flags returned by
				    ;  previous INT 21h handler.
	popff
	db	    0cbh	    ; Far return.

endProc     ivdos

    ;*********************************************************
    ;
    ;	IVIDLE -- INT 28h filter
    ;
    ;	The purpose of this filter (similar to the INT 21h filter above)
    ;	is to delay the return from INT 28h calls if the intervention
    ;	function is waiting for service.  After invoking the previous
    ;	handler, this filter may enter a wait loop that will soon be
    ;	released by the scheduler (which is invoked by a clock tick).

beginProc   ividle
	assume	    ds:nothing, es:nothing, ss:nothing
	BEGIN_IV_FILTER

	pushf
	cli			    ; Disable interrupts until we can
				    ;  set the busy flag.
	cmp	    idle_busy,0     ; If this filter is already active,
	je	    idle_enter
	popff
	jmp	    prev_vec.pidle  ;  jump to previous handler.
				    ;  The previous handler will return
				    ;  directly to the original caller.


idle_enter:
	mov	    idle_busy,1     ; Prevent nesting.
	popff
	pop	    idle_caller_offset	; Save caller's address.
	pop	    idle_caller_seg

	call	    prev_vec.pidle  ; Call previous handler.  On return,
				    ;  it pops the flags from the top
				    ;  of the stack.

	mov	    idle_safe,1     ; Now we're in the nice INT 28h
				    ;  environment:  all DOS functions are
				    ;  allowable except functions 1-12.

	pushf

	cli
	cmp	    req,0	    ; Skip the wait loop if the intervention
	je	    idle_exit	    ;  function doesn't require service
				    ;  at this time.

    ; Since we are inside an INT 28h call, DOS keyboard services are not
    ; available.  If the intervention function needs DOS keyboard services,
    ; exit immediately.

	cmp	    dkey_need,0
	jne	    idle_exit
				    ; Since we know that DOS keyboard
				    ;  services are not required,
				    ;  hold this nice INT 28 environment
				    ;  by entering the wait loop.

	sti
	mov	    iv_wait,1
idle_wait:
	cmp	    iv_wait,0	    ; Wait until somebody sets iv_wait
	jne	    idle_wait	    ;  to zero.

idle_exit:
				    ; Top of stack contains flags returned
				    ;  by previous INT 28h handler.
	push	    idle_caller_seg
	push	    idle_caller_offset

	mov	    idle_safe,0
	mov	    idle_busy,0

	iret

endProc     ividle
endCseg     ivctrlbl
endMod      ivctrlbl
end
