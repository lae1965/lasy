        Title MOUSE SET GRAPHICS POINTER SOFTWARE MASK

        include msmacro.mac

xhot    equ     word ptr [bp + stkoff +  0]
yhot    equ     word ptr [bp + stkoff +  sizeInt]
mask_    equ     DPTR_    [bp + stkoff +  sizeInt*2]

; ********************** MAIN CODE *******************
;++
;   bool    _cdecl   msptgmsk( int xhot, int  yhot, int *mask );
; 
; int mask[32]; - 64 bytes buffer for AND/XOR graphics pointer
; define.
;
;   First 16 word defines 16x16 bit "AND mask", that is, the background
; will shown through werever there is a 1-bit in that data.
; 
;   The second 32 byte set define "XOR mask"; that is after the "AND mask"
; has cleared out some of the background, the pixels matching the 1-bit
; in this data set are toggled.
;--

        beginProg       msptgmsk
        CheckMouse

        push    bp              ; Save registers to be used
        mov     bp, sp          ;
        push    bx
        push    cx              ;
        push    dx              ;
        push    es

        mov     bx, xhot        ; Horizontal pointer hot spot
        mov     cx, xhot        ; Vertical   pointer hot spot

if      longData                ; Load to ES:DX 64 byte buffer
        les     dx, mask_
else
        mov     dx, mask_
        mov     ax, ds
        mov     es, ax
endif
        mov     ax, 09h         ; Set graphics pointer
        callM

        pop     es              ; restore register
        pop     dx              ;
        pop     cx              ;
        pop     bx              ;
        pop     bp              ;

        return_bool     YES

        endProg         msptgmsk

        end
