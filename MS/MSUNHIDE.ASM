        Title MOUSE POINTER UNHIDE

        include msmacro.mac

; ********************** MAIN CODE *******************
;           bool    _cdecl   msunhide(void);
;
;   Unhide text (graphics) mouse pointer.
;--
        beginProg       msunhide
        CheckMouse

        pushDS_
if      longData
        mov     ax, SEG MS_unhide@
        mov     ds, ax
endif

        test    [MS_unhide@], 1   ;
        jz      cont              ;
        popDS_
        return_bool     NO        ; Marker allredy shown.
cont:
        inc     [MS_unhide@]      ; GLOBAL change. (see msmacro.mac)
        popDS_

        mov     ax, 1   ; Hide function
        callM
        return_bool     YES

        endProg         msunhide

        end

