        Title MOUSE SET EXCLUSION AREA

        include msmacro.mac

r1      equ     word ptr [bp + stkoff +  0]
c1      equ     word ptr [bp + stkoff +  sizeInt]
r2      equ     word ptr [bp + stkoff +  sizeInt*2]
c2      equ     word ptr [bp + stkoff +  sizeInt*3]

; ********************** MAIN CODE *******************
;++
;   bool    _cdecl   msexclus( int r1, int c1, int r2, int c2 );
;
;   Set exclucion area for mouse pointer in the motion region.
;--

        beginProg       msexclus
        CheckMouse

        push    bp              ; Save registers to be used
        mov     bp, sp          ;
        push    cx              ;
        push    dx              ;
        push    di              ;
        push    si              ;

        mov     cx,cs:[$ms_mode]
        shl     r1, cl          ; Multiplay parameters by 8
        shl     r2, cl          ;
        xchg    cl,ch           ;
        shl     c1, cl          ;
        shl     c2, cl          ;

        mov     cx, c1          ; Define exclusion area
        mov     dx, r2          ;
        mov     si, c2          ;
        mov     di, r2          ;
        mov     ax, 10h         ;
        callM

        pop     si              ;
        pop     di              ;
        pop     dx              ;
        pop     cx              ;
        pop     bp              ;

        return_bool     YES

        endProg         msexclus

        end
