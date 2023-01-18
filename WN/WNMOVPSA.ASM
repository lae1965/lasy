;
; wnmovpsa - Копирует строку текста в image окна с указанным атрибутом.
;
; void wnmovpsa (CELLSC far *to, void far *from, int num, char attr);
;
; CELLSC far  * to      Указатель на массив структур типа CELLSC в image окна
; void far    * from    Указатель на копируемую строку текста
; int           num     Длина копипруемой строки (in bytes).
; char          attr    Цвет копируемой строки

include begasm.mac
beginProg wnmovpsa

    to      equ     dword ptr [bp + stkoff +  0]
    from    equ     dword ptr [bp + stkoff +  4]
    num     equ     word  ptr [bp + stkoff +  8]
    attr    equ     byte  ptr [bp + stkoff +  10]

    StartCode
        mov     cx, num
        cmp     cx, 0
        jle     loopp
        lds     si, from
        les     di, to
        mov     ah, attr
        cld                            ; forward
move:
        lodsb
        stosw
        loop    move
loopp:

    FinishCode
endProg wnmovpsa
end

