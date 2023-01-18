;
; dbfmtscr - форматирует строку текста для показа в поле.
;
; int dbfmtscr (void far *to, void far *from, int above, int remabove);
;
; void far    * to         Указатель на выходную строку текста
; void far    * from       Указатель на исходную строку текста
; int           above      Количество регулярных пробелов
; int           remabove   Количество нерегулярных пробелов
;
; Возвращает:   Длину выходной строки

include begasm.mac
beginProg dbfmtscr

    to          equ     dword ptr [bp + stkoff +  0]
    from        equ     dword ptr [bp + stkoff +  4]
    above       equ     word  ptr [bp + stkoff +  8]
    remabove    equ     word  ptr [bp + stkoff +  10]

    StartCode

        cld                            ; forward

        lds     si, from
        les     di, to
        mov     bx, di
        mov     dx, remabove
        mov     ax, ds
        cmp     ax, 0
        jne     cycle
        cmp     si, 0
        jne     cycle
        xor     al, al
        stosb
        jmp     short final
cycle:
        lodsb
        cmp     al, 32
        jne     cycle2
        stosb
        jmp     short cycle

cycle2:
        stosb
        cmp     al, 0
        je      final
        cmp     al, 32
        je      blank
cont:
        lodsb
        jmp     short cycle2

blank:
        mov     cx, above
        rep     stosb
        cmp     dx, 0
        je      cont
        stosb
        dec     dx
        jmp     short cont

final:
        mov     ax, di
        sub     ax, bx
        dec     ax
    FinishCode
endProg dbfmtscr
end

