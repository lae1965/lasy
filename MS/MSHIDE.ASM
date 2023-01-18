        Title MOUSE POINTER HIDE

        include msmacro.mac

; ********************** MAIN CODE *******************
;            bool    _cdecl   mshide(void);
;
;   Hide text (graphics) mouse pointer
;--
        beginProg       mshide
        CheckMouse
        pushDS_
if      longData
        mov     ax, SEG MS_unhide@
        mov     ds, ax
endif

        test    [MS_unhide@], 1   ;
        jnz     cont              ;
        popDS_
        return_bool     NO        ; Marker allredy hide.
cont:
        dec     [MS_unhide@]      ; GLOBAL change. (see msmacro.mac)
        popDS_

        mov     ax, 2   ; Hide function
        callM
        return_bool     YES

        endProg         mshide

        end

