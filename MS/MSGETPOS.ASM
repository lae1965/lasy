        Title MOUSE GET POSITION AND BUTTON STATUS

        include msmacro.mac

row     equ     DPTR_ [bp + stkoff +  0]
col     equ     DPTR_ [bp + stkoff +  sizeDPointer]
buttons equ     DPTR_ [bp + stkoff +  sizeDPointer*2]

; ********************** MAIN CODE *******************
;++
;  bool    _cdecl   msgetpos( int *row, int *col, int *buttons );
;
;   Returns row, col ( Y, X) mouse
; position and button status, i.e.
; witch buttons are pressed at the time.
;--

        beginProg       msgetpos
        CheckMouse

        push    bp              ; Save registers to be used
        mov     bp, sp          ;
        push    bx              ;
        push    cx              ;
        push    dx              ;
        push    di              ;
        pushDS_

        mov     ax, 3           ; Query Position / Button status
        callM

        LDS_    di, buttons
        assume  ds:nothing

        mov     word ptr [di], bx ; returns button status

        mov     ax, cx          ; horizontal coordinate
        mov     cx,cs:[$ms_mode]
        shr     ax, cl          ; /?
        LDS_    di, col         ;
        mov     word ptr [di], ax ; return column

        mov     ax, dx          ; vertical coordinate
        xchg    cl,ch           ;
        shr     ax, cl          ; /?
        LDS_    di, row         ;
        mov     word ptr [di], ax ; return row

        popDS_                  ; Restore registers
        pop     di              ;
        pop     dx              ;
        pop     cx              ;
        pop     bx              ;
        pop     bp              ;

        return_bool     YES

        endProg         msgetpos

        end

