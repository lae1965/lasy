        Title MOUSE SET POSITION

        include msmacro.mac

row     equ     word ptr [bp + stkoff +  0]
col     equ     word ptr [bp + stkoff +  sizeInt]

; ********************** MAIN CODE *******************
;++
;     bool    _cdecl   mssetpos( int row, int col);
;
;   Set mouse pointer to  row, col ( Y, X) position.
;--

        beginProg       mssetpos
        CheckMouse

        push    bp              ; Save registers to be used
        mov     bp, sp          ;
        push    cx              ;
        push    dx              ;

        mov     cl, 3           ; Shift counter
        mov     dx, row         ;
        mov     ax, col         ;
        mov     cx,cs:[$ms_mode]

        shl     ax, cl          ; *8
        xchg    cl,ch           ;
        shl     dx, cl          ; *8  set row text coordinate

        mov     cx, ax          ; set column text coordinate
        mov     ax, 4           ; Move mouse pointer
        callM

        pop     dx              ;
        pop     cx              ;
        pop     bp              ;

        return_bool     YES

        endProg         mssetpos

        end

