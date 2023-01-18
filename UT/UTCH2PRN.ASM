;     utch2prn - помещает символ sym в буфер принтера LPT1:;
;
;      void utch2prn (byte sym);
;
;   byte      sym    - помещаемый символ;

include     begasm.mac

beginProg   utch2prn
    sym      equ  byte  ptr  [bp + stkoff + 0]

    StartCode

    mov     dl, sym
    mov     ah, 5H

    int     21H

    FinishCode
endProg     utch2prn

end

