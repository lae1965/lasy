;
; dbkhlpat - Заполняет image окна to, воспринимая в строке from символ 'X'
;            как атрибут batt, а символ ' ' как tatt.
;
; void dbkhlpat (CELLSC far * to, void far * from, char tatt, char batt);
;
; CELLSC far *  to      Указатель на массив структур типа CELLSC в image окна
; void   far *  from    Указатель на строку, из которой берется атрибут
; char          tatt    Цвет заполняемой области.
; char          batt    Цвет заполняемой области.

include begasm.mac

beginProg dbkhlpat

    to      equ     dword ptr [bp + stkoff +  0]
    from    equ     dword ptr [bp + stkoff +  4]
    tatt    equ      byte ptr [bp + stkoff +  8]
    batt    equ      byte ptr [bp + stkoff + 10]

    StartCode
        mov     cx, 80
        mov     bl, tatt
        mov     bh, batt
        les     di, to
        lds     si, from
        cld                            ; forward
move:
        lodsb
        cmp     al, 88                 ; 'X'
        je      lab1
        mov     al, bl                 ; tatt
        jmp     short lab2
lab1:
        mov     al, bh                 ; batt
lab2:
        inc     di
        stosb
        loop    move

    FinishCode
endProg dbkhlpat
end

