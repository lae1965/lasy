;
; wnmovpca - Заполняет image окна символом с указанным атрибутом.
;
; void wnmovpca (CELLSC far *to, char sym, int num, char attr);
;
; CELLSC far *  to      Указатель на массив структур типа CELLSC в image окна
; char          sym     Символ, которым будет заполняться буфер.
; int           num     Длина копируемой строки (in bytes).
; char          attr    Цвет заполняющего символа.

include begasm.mac

beginProg wnmovpca

    to      equ     dword ptr [bp + stkoff +  0]
    sym     equ      byte ptr [bp + stkoff +  4]
    num     equ      word ptr [bp + stkoff +  6]
    attr    equ      byte ptr [bp + stkoff +  8]

    StartCode
        mov     cx, num
        cmp     cx, 0
        jle     loopp
        mov     al, sym
        mov     ah, attr
        les     di, to
        cld                            ; forward
        rep     stosw
loopp:

    FinishCode
endProg wnmovpca
end

