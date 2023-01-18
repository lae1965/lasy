        Title MOUSE SET TEXT POINTER HARDWARE MASK

        include msmacro.mac

startl  equ     word ptr [bp + stkoff +  0]
endl    equ     word ptr [bp + stkoff +  sizeInt]

; ********************** MAIN CODE *******************
;++
;    bool    _cdecl   mspthmsk( int start, int  end);
;
;   Set hardware mask for mouse pointer in TEXT mode only.
; start, end - define start, end  scan lines of standart 
; text cursor.
;--

        beginProg       mspthmsk
        CheckMouse

        push    bp              ; Save registers to be used
        mov     bp, sp          ;
        push    bx
        push    cx              ;
        push    dx              ;

        mov     cx, startl      ; cursor start line
        mov     dx, endl        ; cursor end line
        mov     bx, 1           ; set hardware marker
        mov     ax, 0Ah         ;
        callM

        pop     dx              ; restore register
        pop     cx              ;
        pop     bx              ;
        pop     bp              ;

        return_bool     YES

        endProg         mspthmsk

        end
