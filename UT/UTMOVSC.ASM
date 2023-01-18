;
; utmovsc - Заполняет буфер указанным символом.
;
; void utmovsc (void far *to, char sym, word num);
;
; void far *    to      Указатель на заполняемый буфер.
; char          sym     Символ, которым будет заполняться буфер.
; word          num     Длина заполняемого буфера.

include begasm.mac

beginProg utmovsc

    to      equ     dword ptr [bp + stkoff +  0]
    sym     equ      byte ptr [bp + stkoff +  4]
    num     equ      word ptr [bp + stkoff +  6]

    StartCode

        mov     cx, num
        cmp     cx, 0
        je      loopp
        mov     al, sym
        les     di, to
        cld                            ; forward
        rep     stosb
loopp:

    FinishCode
endProg utmovsc
end
