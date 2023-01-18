;         utrename () - Изменяет имя существующего файла на новое
;                       (файл с новым именем не должен существовать
;                       иначе ошибка)
;
;      bool  utrename (void far * oldname, void far * newname);
;
; Параметры:
;   void far * oldname - Указатель  на строку со старым именем файла
;   void far * newname - Указатель  на строку с новым именем файла
;
; Возврат:
;                YES - Указанный файл переименован
;                NO  - Ошибка
;

include     begasm.mac

beginProg   utrename
    oldname   equ  dword ptr [bp + stkoff + 0]
    newname   equ  dword ptr [bp + stkoff + 4]

    StartCode

    mov     ah, 56H
    lds     dx, oldname
    les     di, newname
    int     21H
    jc      short unsuc
    mov     ax, YES
    jmp     short suc
unsuc:
    xor     ax, ax
suc:

    FinishCode
endProg     utrename

end
