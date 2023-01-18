;
; utintflg - Change state of maskable hardware interrupts flag.
;
; int utintflg (int want_on);
;
; int want_on           1       if interrupts will be enabled after the call,
;                       0       if not.
;
; Description   This function sets the state of the hardware interrupt flag.
;
; Returns               1       if interrupts were enabled before the call,
;                       0       if not.
;

include begasm.mac
beginProg utintflg

want_on equ	word ptr [bp + stkoff]

    beginCode

    pushf              ; Get flags in BX to examine later--
    pop     bx         ; Bit 9 of flags is Interrupt flag.

    mov     cx, want_on

    test    bh, 2
    jz      wereoff
    mov     ax, 1
    jmp     short continue
wereoff:
    mov     ax, 0
continue:
    cmp     cx, 0
    je      clrflag
    sti
    jmp     short done
clrflag:
    cli
done:
    endCode
endProg utintflg
end
