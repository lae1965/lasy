;
; wnmovpa - Заполняет image окна  указанным атрибутом.
;
; void wnmovpca (CELLSC far *to, int num, char attr);
;
; CELLSC far *  to      Указатель на массив структур типа CELLSC в image окна
; int           num     Длина заполняемой области (in CELLSC).
; char          attr    Цвет заполняемой области.

include begasm.mac

beginProg wnmovpa

    to      equ     dword ptr [bp + stkoff +  0]
    num     equ      word ptr [bp + stkoff +  4]
    attr    equ      byte ptr [bp + stkoff +  6]

    StartCode
        mov     cx, num
        cmp     cx, 0
        jle      loopp
        mov     al, attr
        les     di, to
        cld                            ; forward
move:
        inc     di
        stosb
        loop    move
loopp:

    FinishCode
endProg wnmovpa
end

