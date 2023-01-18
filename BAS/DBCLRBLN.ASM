;
; dbclrbln - Копирует строку from в to, очищая лишние пробелы.
;
; int dbclrbln (void far * to, void far * from, bool first);
;
; void far    * to         Указатель на выходную строку текста
; void far    * from       Указатель на исходную строку текста
; bool          first      Первая строка абзаца
;
; Возвращает:   Длину выходной строки

include begasm.mac
beginProg dbclrbln

    to          equ     dword ptr [bp + stkoff +  0]
    from        equ     dword ptr [bp + stkoff +  4]
    first       equ     word  ptr [bp + stkoff +  8]

    StartCode

        cld                            ; forward

        lds     si, from
        les     di, to
        mov     bx, di

        cmp     first, 0
        je      clrblnk

cycle1:
        lodsb
        cmp     al, 32
        jne     cycle2
        stosb
        jmp     short cycle1

clrblnk:
        lodsb
        cmp     al, 32
        je      clrblnk

cycle2:
        stosb
        cmp     al, 0
        je      final
        cmp     al, 32
        je      clrblnk
        lodsb
        jmp     short cycle2

final:
        mov     ax, di
        sub     ax, bx
        dec     ax

    FinishCode
endProg dbclrbln
end

