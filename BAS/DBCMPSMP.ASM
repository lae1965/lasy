;
; dbcmpsmp - ?ࠢ?????? ??ப? str1 ? str2, ?? ???ᬠ?ਢ?? ??譨? ?஡???
;
; int dbcmpsmp (byte far * str1, byte far * str2);
;
; byte far     * str1    ?????⥫? ?? ??ப? str1
; byte far     * str2    ?????⥫? ?? ??ப? str2
;
;   ??????頥?:
;       0 - ??ப? ?????????
;       1 - ???祭?? ??ப? str1 ?????? str2
;      -1 - ???祭?? ??ப? str1 ?????? str2

include begasm.mac
beginProg dbcmpsmp

    str1    equ     dword ptr [bp + stkoff +  0]
    str2    equ     dword ptr [bp + stkoff +  4]

    StartCode

        lds     si,     str2                  ; DS:SI contain ptr str2

        les     di,     str1                  ; ES:DI contain ptr str1

        xor     cx,     cx
        cld

cont:
        mov     ah,     byte ptr es:[di]
        inc     di
        cmp     ah,     32
        je      vfyblnk1
        xor     ch,     ch
cont1:
        lodsb

        cmp     al,     32
        je      vfyblnk2
        xor     cl,     cl
cont2:
                                              ; ah contain value of str1
                                              ; al contain value of str2
        cmp     ax,     0                     ; str1 == str2
        je      final

        cmp     ah,     al                    ; ah - al
        je      cont
        ja      lab

        mov     ax,     -1                    ; str1 < str2
        jmp     short final


vfyblnk1:
        cmp     ch,     1
        je      cont
        mov     ch,     1
        jmp     short cont1

vfyblnk2:
        cmp     cl,     1
        je      cont1
        mov     cl,     1
        jmp     short cont2

lab:
        mov     ax,      1                    ; str1 > qsrt

final:
    FinishCode
endProg dbcmpsmp
end

