;
; wnmovpc - Заполняет image окна символом, не изменяя атрибутов окна.
;
; void wnmovpa (CELLSC far *to, char sym, int num);
;
; CELLSC far *  to      Указатель на массив структур типа CELLSC в image окна
; char          sym     Символ, которым будет заполняться image.
; int           num     Длина заполняемой области (in CELLSC).

include begasm.mac

beginProg wnmovpc

    to      equ     dword ptr [bp + stkoff +  0]
    sym     equ      byte ptr [bp + stkoff +  4]
    num     equ      word ptr [bp + stkoff +  6]

    StartCode
        mov     cx, num
        cmp     cx, 0
        jle     loopp
        mov     al, sym
        les     di, to
        cld                            ; forward
move:
        stosb
        inc     di
        loop    move
loopp:
   FinishCode
endProg wnmovpc
end

