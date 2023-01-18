        Title MOUSE SET TEXT POINTER SOFTWARE MASK

        include msmacro.mac

bg      equ     byte ptr [bp + stkoff +  0]
fg      equ     byte ptr [bp + stkoff +  sizeInt]
symb    equ     byte ptr [bp + stkoff +  sizeInt*2]
opt     equ     byte ptr [bp + stkoff +  sizeInt*3]

; ********************** MAIN CODE *******************
;++
;   bool    _cdecl   msptsmsk( int bg, int  fg, int symb, int opt );
;
;   Set software mask for mouse pointer
; in TEXT mode only.
; bg - background color mouse marker;
; fg - foreground color mouse marker;
; symb - ascii character that always
; visible, with same color as <fg>, on
; a <bg> color field.
;   If symb == 0 than character in a pointer
; will be the same as on the screen at a
; given place and a given time.
;   Set mouse pointer to AND or XOR style
; with parameter <opt>, that may be equals
; MS_AND or MS_XOR values.
;--

        beginProg       msptsmsk
        CheckMouse

        push    bp              ; Save registers to be used
        mov     bp, sp          ;
        push    bx
        push    cx              ;
        push    dx              ;

        mov     dl, symb        ; symbol to dl
        mov     cl, 4           ; shift count
        mov     dh, bg          ; background to dh << 4
        shl     dh, cl          ;
        or      dh, fg          ; foreground to dh

        xor     cx, cx          ; zero to cx
        or      dl ,dl          ;
        jnz     cont            ;
		mov     cx, 0FFh        ; screen value masked
		mov     al, opt         ; XOR or AND
		or      al, al          ;
		jz      cont            ; AND options
		mov     ch, cl          ; make pointer XOR type
cont:
        xor     bx, bx          ; set software marker
        mov     ax, 0Ah         ;
        callM

        pop     dx              ; restore register
        pop     cx              ;
        pop     bx              ;
        pop     bp              ;

        return_bool     YES

        endProg         msptsmsk

        end
