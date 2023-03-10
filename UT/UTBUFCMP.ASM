;
; utbufcmp - ?ࠢ?????? ??ப? pbuf1 ? pbuf2 ?????? len.
;
; int utbufcmp (void far * pbuf1, void far * pbuf2, word len);
;
; void far     * pbuf1    ?????⥫? ?? ??ப? pbuf1
; void far     * pbuf2    ?????⥫? ?? ??ப? pbuf2
; word           len      ????? ??ப
;
;   ??????頥?:
;       0 - ??ப? ?????????
;       1 - ???祭?? ??ப? pbuf1 ?????? pbuf2
;      -1 - ???祭?? ??ப? pbuf1 ?????? pbuf2

include begasm.mac
beginProg utbufcmp

    pbuf1    equ     dword ptr [bp + stkoff +  0]
    pbuf2    equ     dword ptr [bp + stkoff +  4]
    len      equ      word ptr [bp + stkoff +  8]

    StartCode

        lds     si,     pbuf2                  ; DS:SI contain ptr pbuf2

        les     di,     pbuf1                  ; ES:DI contain ptr pbuf1

        mov     cx,     len
        cld

cont:
        cmp     cx,     0
        je      equal
        mov     ah,     byte ptr es:[di]
        inc     di
        lodsb
        dec     cx
                                              ; ah contain value of pbuf1
                                              ; al contain value of pbuf2
        cmp     ah,     al                    ; ah - al
        je      cont
        ja      lab

        mov     ax,     -1                    ; pbuf1 < pbuf2
        jmp     short final
equal:
        xor     ax,     ax                    ; pbuf1 == pbuf2
        jmp     short final
lab:
        mov     ax,      1                    ; pbuf1 > pbuf2

final:
    FinishCode
endProg utbufcmp
end

