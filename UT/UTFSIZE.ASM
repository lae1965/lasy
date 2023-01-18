;         utfsize () - Вычисляет размер указанного файла (in bytes)
;
;      long  utfsize (int handle);
;
; Параметры:
;           int         handle - Handle ранее открытого файла
;
; Возврат:
;            Размер файла, иначе -1L, если произошла ошибка
;

include     begasm.mac

beginProg   utfsize
    handle  equ  word  ptr  [bp + stkoff + 0]

    savposl equ  word  ptr  [bp - 2]
    savposh equ  word  ptr  [bp - 4]
    sizel   equ  word  ptr  [bp - 6]
    sizeh   equ  word  ptr  [bp - 8]

    StartCode 8

    mov     ax, 4201H      ; Get current logic file position
    mov     bx, handle
    xor     cx, cx
    xor     dx, dx
    int     21H
    jc      short unsuc

    mov     savposl, ax    ; Save current logic file position for restore
    mov     savposh, dx   

    mov     ax, 4202H      ; Get size of file
    xor     dx, dx
    int     21H
    jc      short unsuc

    mov     sizel, ax      ; Save size of file
    mov     sizeh, dx


    mov     ax, 4200H      ; Restore current logic file position
    mov     cx, savposh
    mov     dx, savposl
    int     21H
    jc      short unsuc

    mov     ax, sizel      ; Store size for return
    mov     dx, sizeh

    jmp     short final
unsuc:
    mov     ax, -1         ; If process was erroneus 
    mov     dx, -1
final:
    FinishCode
endProg     utfsize

end
