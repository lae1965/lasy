;
; dbprnstr - ???????? ??ப? ⥪??? from ? to.
;
; int dbprnstr (void far *to, void far *from);
;
; void far    * to      ?????⥫? ?? ??室??? ?????
; void far    * from    ?????⥫? ?? ?????㥬?? ??ப? ⥪???

include begasm.mac
beginProg dbprnstr

    to      equ     dword ptr [bp + stkoff +  0]
    from    equ     dword ptr [bp + stkoff +  4]

    StartCode

        cld                            ; forward

        lds     si, from
        les     di, to
        mov     dx, di
        xor     cx, cx
;****************** ??????塞 ??ப? **********************;
;** BX contain rep count, DX - rep symbol, CX - disp count, AX - cur symbol
storstr:
        lodsb
        cmp     al, 1
        je      short specsym
        cmp     al, 10
        je      short final
        stosb
        jmp     short storstr
specsym:
        lodsb
        mov     ah, al
        lodsb
        mov     cl, al

        cmp     ah, 178
        jne     short c2
        mov     ah, 32
c2:
        mov     al, ah
        rep     stosb
        jmp     short storstr
final:
        mov     ax, di
        sub     ax, dx
    FinishCode
endProg dbprnstr
end

