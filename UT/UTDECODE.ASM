;           utdecode - разкодирует строку текста pfrom, помещая разкодированное
;                      значение в pto. Pfrom должно быть в 2 раза больше pto.
;
;
;           void utdecode (void far * pto, void far * pfrom, int len);
;
;

include begasm.mac
extrn      _rand:far

    pto      equ     dword  ptr [bp + stkoff +  0]
    pfrom    equ     dword  ptr [bp + stkoff +  4]
    len      equ      word  ptr [bp + stkoff +  8]

beginProg utdecode

    StartCode

    cld

    mov     cx,     len
    cmp     cx,     0
    jle     final

    lds     si,     pfrom
    les     di,     pto

cycle:
    lodsw
    shr     ax,     1
    shr     ax,     1
    shr     ax,     1
    shr     ax,     1
    not     ax
    stosb
    loop    cycle

final:
    FinishCode
endprog utdecode

end
