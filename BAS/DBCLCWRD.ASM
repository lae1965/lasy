;
; dbclcwrd - ????????? (???????⢮ ᫮?) - 1 ? ????ଠ??஢????? ??ப?.
;
; int dbclcwrd (void far * from);
;
; void far    * from       ?????⥫? ?? ??室??? ??ப? ⥪???
;
; ??????頥?:   (???????⢮ ᫮? ? ??ப?) - 1

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

