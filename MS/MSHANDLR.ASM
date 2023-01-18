        Title MOUSE HANDLER ROUTINES

        include msmacro.mac

; ********************** MAIN CODE *******************
;++
; void (_Cdecl far * _Cdecl mshandlr (int *event, void (far *func)()))();
;
;  This function sets a mouse interrupt routine <func>, which
; called directly from mouse driver ( for example: mouse.com )
; as far procedure after manipulator used (i.e. move, buttons
; pressed or released). You may set different condition for
; <func> call (see event Definitions for mshandlr).
; Term 'event' is the same as 'manipulator using'.
; if (*event == MS_NO_EVENT) func() will be no more called
;   from mouse driver.
;
; Use:
;
; pragma check_stack( off )
; pragma check_pointer( off )
; void _far func ( int event, int butstat,
;                  int xpos,  int ypos,
;                  int xdist, int ydist );
;
;   Mouse driver tells <func>:
;       -which event has occured at a time - <event>;
;       -mouse button status - <butstat>;
;       -mouse pointer coordinates - row(ypos), col(xpos);
;       -distances of motion (in mickeys value) since last interrupt
;        may be negative or positive sign.
;
;    Note: 1 mickey = 1/200th of an inch.
;
; Returns:
;       - old_event_mask to event address.
;       - old_interrupt_routine_address.
;****************************************************
;
; void (_Cdecl far * _Cdecl msshadow( int *event, void (far *ShadowFunc)()))();
;
;   This function adds alternate (shadow) mouse handler
;   routine which will work with natural (setted by mshandlr())
;   handler together. It called just before mshandlr's function.
;   if (*event == MS_NO_EVENT) SahdowFunc() will be no more called
;   from mouse driver.
;   The original handler not be called if Shadow function
;   changes event to 0 (event = 0;).
;
; Returns:
;       - old_event_mask to event address.
;       - old_interrupt_routine_address.
;****************************************************
;
;   Added by Yudin at 02/22/91 01:35pm. (;* - style comment)
;--
EXTRN wShadowEvent:WORD
EXTRN pfShadowFunc:DWORD
EXTRN wUserEvent:WORD
EXTRN pfUserFunc:DWORD

event   equ     DPTR_     [bp + stkoff +  0]
func    equ     dword ptr [bp + stkoff +  sizeDPointer] ; inerrupt type func

beginCseg       mshandlr

;------------------------------------------------------------
beginProc       mshandlr
        CheckMouse

        push    bp              ; Save registers to be used
        mov     bp, sp          ;
        push    bx              ;
        push    cx              ;
        push    es              ;

ifdef NHUGE
        push    ds
        mov     ax, SEG wUserEvent
        mov     ds, ax
endif

        cli                     ; Interrupt disable.
        les     ax, func        ;
        xchg    word ptr [pfUserFunc], ax ; Save interrupt func IP
        mov     dx, es
        xchg    word ptr [pfUserFunc+2], dx ; Save interrupt func CS


        pushDS_
        mov     cx, [wUserEvent];
        LDS_    bx, event       ;
        xchg    cx, word ptr [bx]     ; set event mask
        popDS_
        mov     [wUserEvent], cx ; save event mask

        sti                     ; Interrupt enable

ifdef NHUGE
        pop     ds
endif

        pop     es
        pop     cx              ;
        pop     bx              ;
        pop     bp              ;

        ret                     ; DX:AX old address.

endProc         mshandlr

;------------------------------------------------------------
beginProc       msshadow

        push    bp              ; Save registers to be used
        mov     bp, sp          ;
        push    bx              ;
        push    cx              ;
        push    es              ;

ifdef NHUGE
        push    ds
        mov     ax, SEG wShadowEvent
        mov     ds, ax
endif

        cli                     ; Interrupt disable.
        les     ax, func        ;
        xchg    word ptr [pfShadowFunc], ax ; Save interrupt func IP
        mov     dx, es
        xchg    word ptr [pfShadowFunc+2], dx ; Save interrupt func CS


        pushDS_
        mov     cx, [wShadowEvent];
        LDS_    bx, event       ;
        xchg    cx, word ptr [bx]     ; set event mask
        popDS_
        mov     [wShadowEvent], cx ; save event mask

        sti                     ; Interrupt enable

ifdef NHUGE
        pop     ds
endif

        pop     es
        pop     cx              ;
        pop     bx              ;
        pop     bp              ;

        ret                     ; DX:AX old address.

endProc         msshadow
;------------------------------------------------------------

endCseg         mshandlr

        end
