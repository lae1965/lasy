;
; dboutstr - Копирует строку текста в image окна с указанным атрибутом.
;
; void dboutstr (CELLSC far *to, void far *from, int visicol, int w, char attr);
;
; CELLSC far  * to      Указатель на массив структур типа CELLSC в image окна
; void far    * from    Указатель на копируемую строку текста
; int           visicol Величина невидимой части экрана слева
; int           w       Ширина окна
; char          attr    Цвет копируемой строки

include begasm.mac
beginProg dboutstr

    to      equ     dword ptr [bp + stkoff +  0]
    from    equ     dword ptr [bp + stkoff +  4]
    visicol equ     word  ptr [bp + stkoff +  8]
    w       equ     word  ptr [bp + stkoff +  10]
    attr    equ     byte  ptr [bp + stkoff +  12]

    StartCode

        cld                            ; forward

        xor     ah, ah
        mov     cx, w
        lds     si, from
        les     di, to
        mov     dh, attr
        mov     bx, visicol
        cmp     bx, 0
        jmp     short   cyc2
;****************** Отматываем visicol ********************;
cyc1:
        dec     bx
cyc2:
        je      short  c4
        lodsb
        cmp     al, 10
        je      short clear
        cmp     al, 1
        jne     short cyc1
        lodsb
        mov     dl, al
        lodsb
        sub     bx, ax
        jge     short cyc2
        neg     bx
;****************** Заполняем строку **********************;
;** BX contain rep count, DX - rep symbol, CX - disp count, AX - cur symbol
specsym:
        cmp     cx, bx
        ja      short c1
        mov     bx, cx
c1:
        sub     cx, bx
        xchg    cx, bx
        cmp     dl, 178
        jne     short c2
        mov     dl, 32
c2:
        mov     ax, dx
        rep     stosw
        mov     cx, bx
        cmp     cx, 0
        je      short final
storstr:
        lodsb
        cmp     al, 10
        je      short clear
        cmp     al, 1
        jne     short c3
        lodsb
        mov     dl, al
        lodsb
        mov     bl, al
        jmp     short specsym
c3:
        stosw
        loop    short storstr
        jmp     short final
c4:
        mov     ah, dh
        jmp    short storstr
;****************** Зачищаем концовку *********************;
clear:
        mov     ah, dh
        mov     al, 32
        rep     stosw

final:
    FinishCode
endProg dboutstr
end

