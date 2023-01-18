        Title MOUSE SET SPEED MOTION

        include msmacro.mac

horiz   equ     word ptr [bp + stkoff +  0]
vert    equ     word ptr [bp + stkoff +  sizeInt]

; ********************** MAIN CODE *******************
;++
;    bool    _cdecl   msspeed( int horiz, int vert );
;
;   Set determine beetwin display  pointer motion speed and
; manipulator motion ones.
;
;   Default settings are <horis> = 8, <vert> = 16.
;--

        beginProg       msspeed
        CheckMouse

        push    bp              ; Save registers to be used
        mov     bp, sp          ;
        push    cx              ;
        push    dx              ;

        mov     cx, horiz       ;
        mov     dx, vert        ;
        mov     ax, 0Fh         ; Set pointer speed
        callM

        pop     dx              ;
        pop     cx              ;
        pop     bp              ;

        return_bool     YES

        endProg         msspeed

        end

