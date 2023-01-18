include begasm.mac
	extrn	_malloc:far
	extrn	_free:far
;
; void txt2bin (int handto, int handfrom, void far * pfrom, int len);
;
;
    handto      equ     word  ptr [bp + stkoff +  0]
    handfrom    equ     word  ptr [bp + stkoff +  2]
    pfrom       equ    dword  ptr [bp + stkoff +  4]
    len         equ     word  ptr [bp + stkoff +  8]

    pto         equ    dword  ptr [bp - 4]

beginProg flushtxt

    push    ds
    push    cx

    mov     ah, 40h
    mov     bx, handto
    lds     dx, pto
    mov     cx, di
    sub     cx, dx
    int     21h

    pop     cx
    pop     ds

    les     di, pto
    mov     dx, len

    ret
endprog flushtxt

beginProg txt2bin
    StartCode      4

    push    ds
    mov     dx, len
    inc     dx
    push    dx
    call    far ptr _malloc
    mov     word ptr [bp - 4], ax
    mov     word ptr [bp - 2], dx

    pop     dx
    pop     ds
    dec     dx
    cld
    les     di, pto

lodtxt:
    push    ds
    push    dx
    mov     ah, 3Fh
    mov     bx, handfrom
    lds     dx, pfrom
    mov     cx, len
    int     21h
    pop     dx
    pop     ds
    cmp     ax, 0
    je      short final
    mov     cx, ax

    lds     si, pfrom

cycle:
    lodsb
    cmp     al, 10                          ;\n
    je      rest
cont1:
    stosb
    dec     dx
    cmp     dx, 0
    jg      cont
    call    _flushtxt
cont:
    loop    cycle
    jmp     short lodtxt

rest:
    mov     al, 13                          ;\r
    stosb
    dec     dx
    mov     al, 10                          ;\n
    jmp     short cont1

final:
    call    _flushtxt
    push    ds
    push    word ptr [bp-2]
	push	word ptr [bp-4]
	call	far ptr _free
	pop	cx
	pop	cx
    pop ds

    FinishCode
endprog txt2bin

end
