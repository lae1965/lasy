        Title MOUSE SET BNOUNDARY MOTION

        include msmacro.mac

r1      equ     word ptr [bp + stkoff +  0]
c1      equ     word ptr [bp + stkoff +  sizeInt]
r2      equ     word ptr [bp + stkoff +  sizeInt*2]
c2      equ     word ptr [bp + stkoff +  sizeInt*3]

; ********************** MAIN CODE *******************
;++
;   bool    _cdecl   msbound( int r1, int c1, int r2, int c2 );
;
; Define region for mouse pointer motion.
; r1, c1 ( Y1, X1 ) - upper left corner of the screen;
; r2, c2 ( Y2, X2 ) - down right corner of the screen;
;--

        beginProg       msbound
        CheckMouse

        push    bp              ; Save registers to be used
        mov     bp, sp          ;
        push    cx              ;
        push    dx              ;

        mov     cx,cs:[$ms_mode]
        shl     c1, cl          ;
        shl     c2, cl          ;
        xchg    cl,ch           ;
        shl     r1, cl          ; Multiply parameters by 8
        shl     r2, cl          ;

        mov     cx, c1          ; Define horizontal range
        mov     dx, c2          ;
        mov     ax, 7           ;
        callM

        mov     cx, r1          ; Define vertical range
        mov     dx, r2          ;
        mov     ax, 8           ;
        callM

        pop     dx              ;
        pop     cx              ;
        pop     bp              ;

        return_bool     YES

        endProg         msbound

        end
