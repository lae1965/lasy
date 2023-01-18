;
; wnmovps - Копирует буфер символов в image окна, не изменяя атрибутов окна.
;
; void wnmovps (CELLSC far *to, void far *from, int num);
;
; CELLSC far  *   to      Указатель на массив структур типа CELLSC в image окна
; void   far  *   from    Указатель на копируемый буфер.
; int             num     Длина буфера (in CELLSC).

include begasm.mac

beginProg wnmovps

    to      equ     dword ptr [bp + stkoff +  0]
    from    equ     dword ptr [bp + stkoff +  4]
    num     equ      word ptr [bp + stkoff +  8]

    StartCode
        mov     cx, num
        cmp     cx, 0
        jle     loopp

        lds     si, from
        les     di, to
        cld                            ; forward
move:
        lodsb
        stosb
        inc     di
        loop    move
loopp:

    FinishCode
endProg wnmovps
end

