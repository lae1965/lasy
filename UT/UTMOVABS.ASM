;
; utmovabs - Move memory.
;
; void utmovabs (void far *to, void far *from, unsigned num, int dir);
;
; void far *    to      Where to move memory to.
; void far *    from    Where to move memory from.
; unsigned int  num     Amount of memory to move (in bytes).
; int           dir     UT_MV_FORW or UT_MV_BACK direction flag.
;
; Returns	Nothing.
;

include begasm.mac

beginProg utmovabs

        UT_MV_FORW equ  0          ; Direction flag settings
        UT_MV_BACK equ  1

    to      equ dword ptr [bp + stkoff +  0]
    from    equ dword ptr [bp + stkoff +  4]
    num     equ word  ptr [bp + stkoff +  8]
    direct  equ word  ptr [bp + stkoff + 10]

    StartCode

    mov     cx, num
    cmp     cx, 0
    je      loopp
    lds     si, from
    les     di, to

	cld
    cmp     direct, UT_MV_FORW
    jz      dset
	std

dset:
    rep     movsb
loopp:
    FinishCode
endProg utmovabs
end

