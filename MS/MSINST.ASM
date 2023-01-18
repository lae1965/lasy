        Title MOUSE INSTALL

        include msmacro.mac

; ********************** MAIN CODE *******************
;++
;       int     _cdecl   msinst (void);
;
;   Returns number of mouse buttons and
; reset driver for text (graphics) use.
;--
; Exist in DGROUP
EXTRN wShadowEvent:WORD
EXTRN pfShadowFunc:DWORD
EXTRN wUserEvent:WORD
EXTRN pfUserFunc:DWORD
; Exist in MSHANDLR
EXTRN _mshandlr:PROC
EXTRN _msshadow:PROC

Event	equ  word ptr [bp-2]

beginCseg
beginProc       msinst
        assume  DS:DGROUP

        push    bp
        mov     bp, sp
        sub     sp, 2           ; automatic var. Event

ifdef NHUGE
        push    ds
        mov     ax, SEG _MS_buttons
        mov     ds, ax
        assume  DS:NOTHING
endif

        push    bx              ;
        xor     ax, ax          ;
        mov     bx, ax          ;
        callM                   ;
        or      bx, bx          ;
        jz      q0              ;

        pushDS_
        push    bx
        push    ds
        mov     ax,cs
        mov     ds,ax
        mov     ah, 0Fh
        int     10h
        and     al,7Fh
        mov     bx, offset CountTable
        xlat
        xor     ah,ah
        or      al,al
        je      $$10
        mov     ah,3
$$10:   pop     ds
        pop     bx

        mov     [MS_mode@], ax    ;
        mov     [MS_buttons@], bx ; GLOBAL change. (see msmacro.mac)

        xor     ax, ax            ; Clear AX (=0)
        mov     [MS_unhide@], ax  ; Suggest that mouse poiner is
                                  ; invisible now.
        ; Setup internal handler routine.
        push    cx                ;*
        push    es                ;*
        push    dx                ;*
        xor     ax, ax            ;* AX = 0
        ; Initialization of handler's static variables.
        push    ax                ;1. NULL
        push    ax                ;2. NULL
if longData
		push    ss                ; segment of var. which MS_NO_EVENT
endif
		mov		Event, 0		  ; Set MS_NO_EVENT
		lea     ax, Event         ; offset of MS_NO_EVENT
		push    ax                ;
		call    _mshandlr         ; For initialize statics.
		mov		Event, 0		  ; Set MS_NO_EVENT
		call    _msshadow         ; For initialize statics.
if longData
        add     sp, 8
else
        add     sp, 6
endif

        mov     cx, 7Fh           ;* Setup all possible event.
        push    cs
        pop     es
        mov     dx, OFFSET handler
        mov     ax, 0Ch           ; Set mouse event handler
        callM

        pop     dx                ;*
        pop     es                ;*
        pop     cx                ;*

        mov     ax, bx            ; number of mouse buttons.

        popDS_

q0:                               ;
		pop     bx                ;

ifdef NHUGE
        pop     ds
endif

		mov		sp, bp			  ; Restore old SP
		pop bp					  ; Restore a frame pointer.
        ret

CountTable      db      4,4     ; 40x25 modes
                db      3,3     ; 80x25 modes
                db      0,0,0   ; three graph modes
                db      3       ; mode 7 80x25
				db      (256-($-CountTable)) dup (0);
endProc msinst
;------------------------------------------------------------
handler proc    far

        push    ax

        mov     ax, DGROUP      ; Adjust data segment
        mov     ds, ax
        assume  DS:DGROUP

        mov     ax,cx
        mov     cx, [MS_mode@]
        shr     ax, cl          ; Convert to text coordinates   ( /8 )
        xchg    cl,ch           ;
        shr     dx, cl          ; Convert to text coordinates   ( /8 )
        mov     cx,ax
        pop     ax

        push    di              ; distance of last vertical motion
        push    si              ; distance of last horizontal motion
        push    dx              ; vertical pozition
        push    cx              ; horizontal pozition
        push    bx              ; button status
        push    ax              ; event has occured

        ;* At the first call SHADOW routine.
        test    [wShadowEvent], ax ;* Check for call validity.
        jz      not_shadow      ;* Not call the SHADOW handler.
        call    [pfShadowFunc]  ;* call Shadow routine.

        or      ax, ax          ;* Analizing return code.
        jnz     not_natural     ;* if function return YES

        pop     ax              ;* Restore event original mask.
        push    ax              ;* Back to stack.

        ;* Call the original.
not_shadow:
        test    [wUserEvent], ax ;*
        jz      not_natural   ;* Not call the natural handler.
        call    [pfUserFunc]  ; call original routine
not_natural:

        add     sp, 12

        ret
handler endp

endCseg

        end

