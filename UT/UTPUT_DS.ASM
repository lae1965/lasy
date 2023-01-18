;     utput_ds - Устанавливает значение сегмента данных
;
;      void utput_ds (int new_ds);
;

include     begasm.mac

beginProg   utput_ds

    beginCode

    mov     ds, word ptr [bp + stkoff]

    endCode
endProg     utput_ds

end

