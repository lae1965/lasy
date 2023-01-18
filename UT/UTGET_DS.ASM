;     utget_ds - Возвращает значение сегмента данных
;
;      int utget_ds (void);
;

include     begasm.mac

beginProg   utget_ds

    StartCode

    mov     ax, ds

    FinishCode
endProg     utget_ds

end

