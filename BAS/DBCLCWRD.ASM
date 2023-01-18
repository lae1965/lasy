;
; dbclcwrd - Вычисляет (количество слов) - 1 в отформатированной строке.
;
; int dbclcwrd (void far * from);
;
; void far    * from       Указатель на исходную строку текста
;
; Возвращает:   (Количество слов в строке) - 1

include begasm.mac
beginProg dbclcwrd

    from        equ     dword ptr [bp + stkoff +  0]

    StartCode

        cld                            ; forward

        lds     si, from
        xor     cx, cx

rewblnk:
        lodsb
        cmp     al, 32
        je      rewblnk

cycle1:
        cmp     al, 0
        je      final
        lodsb
        cmp     al, 32
        jne     cycle1
        inc     cx
        jmp     short rewblnk

final:
        mov     ax, cx

    FinishCode
endProg dbclcwrd
end

