;
; utexblnk - копирует строку from в строку to, исключая лишние пробелы
;
; void utexblnk (void far * to, void far * from, unsigned num);
;
; void far *    to      Where to move memory to.
; void far *    from    Where to move memory from.
; unsigned int  num     Amount of memory to move (in bytes).
;
; Returns	Nothing.
;

include begasm.mac

beginProg utexblnk

    to      equ dword ptr [bp + stkoff +  0]
    from    equ dword ptr [bp + stkoff +  4]
    num     equ word  ptr [bp + stkoff +  8]

    StartCode

    mov     cx, num
    cmp     cx, 0
    je      final
    lds     si, from
    les     di, to

	cld
    mov     bx, 1

cont:
    lodsb
    cmp     al, 32
    je      vfyblnk
    xor     bx, bx
store:
    stosb
    loop    cont
    jmp     short final

vfyblnk:
    cmp     bx, 0
    je      _1stblnk
    loop    cont

_1stblnk:
    mov     bx, 1
    jmp     short store

final:
    FinishCode
endProg utexblnk
end

