;
; wnmovsp - Копирует символы из image окна в буфер.
;
; void wnmovsp (char far *to, CELLSC far *from, int num);
;
; char far    *   to      Указатель на буфер, в который копируются данные.
; CELLSC far  *   from    Указатель на массив структур типа CELLSC в image окна
; int             num     Длина буфера to.

include begasm.mac

beginProg wnmovsp

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
        inc     si
        stosb
        loop    move
loopp:

    FinishCode
endProg wnmovsp
end

