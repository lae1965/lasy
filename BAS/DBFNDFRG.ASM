;
; dbfndfrg - Ищет в строке pstr фрагмент pfrg, не рассматривая лишние пробелы
;
; bool dbfndfrg (byte far * pstr, byte far * pfrg);
;
; byte far     * pstr    Указатель на строку pstr
; byte far     * pfrg    Указатель на строку pfrg
;
;   Возвращает:
;       YES - фрагмент найден
;       NO  - фрагмент не найден

include begasm.mac
beginProg dbfndfrg

    pstr    equ     dword ptr [bp + stkoff +  0]
    pfrg    equ     dword ptr [bp + stkoff +  4]

    StartCode

        cld

        les     di,     pfrg                 ; ES:DI contain ptr pfrg
        lds     si,     pstr                 ; DS:SI contain ptr pstr

        mov     bx,     di                   ; bx save offset of ptr pfrg
        xor     dx,     dx

cont:
        lodsb                                ; al contain value of pstr
        cmp     al,     32
        je      vfyblnk
        xor     dh,     dh
cont1:
        mov     ah,     byte ptr es:[di]     ; ah contain value of pfrg

        cmp     ah,     0
        je      final

        cmp     al,     0
        je      clrret

        cmp     al,     ah
        jne     restor

        cmp     dl,     1
        je      lab

        mov     dl,     1
        mov     cx,     si
lab:
        inc     di
        jmp     short cont

restor:
        cmp     dl,     0
        je      cont
        xor     dl,     dl
        mov     di,     bx
        mov     si,     cx
        jmp     short cont

vfyblnk:
        cmp     dh,     1
        je      cont
        mov     dh,     1
        jmp     short cont1

clrret:
        xor     dl,     dl

final:
        xor     ah,     ah
        mov     al,     dl

    FinishCode
endProg dbfndfrg
end

