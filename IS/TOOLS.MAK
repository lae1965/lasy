acomp=tasm /Mx /i$(apath)
ccomp=bcc -c -m$(model) -w -O

$(path)tools_$(model).lib :       ISCALLIN.ASM
        $(acomp) ISCALLIN;  >> con
        echo -+ISCALLIN &   >>~libresp

$(path)tools_$(model).lib :       ISDISPAT.ASM
        $(acomp) ISDISPAT;  >> con
        echo -+ISDISPAT &   >>~libresp

$(path)tools_$(model).lib :       IVCTRLBL.ASM
        $(acomp) IVCTRLBL;  >> con
        echo -+IVCTRLBL &   >>~libresp

$(path)tools_$(model).lib :       ISCURPRC.C
        $(ccomp) ISCURPRC.C   >> con
        echo -+ISCURPRC &   >>~libresp

$(path)tools_$(model).lib :       ISINSTAL.C
        $(ccomp) ISINSTAL.C   >> con
        echo -+ISINSTAL &   >>~libresp

$(path)tools_$(model).lib :       ISPREPBL.C
        $(ccomp) ISPREPBL.C   >> con
        echo -+ISPREPBL &   >>~libresp

$(path)tools_$(model).lib :       ISREMOVE.C
        $(ccomp) ISREMOVE.C   >> con
        echo -+ISREMOVE &   >>~libresp

$(path)tools_$(model).lib :       ISSENSIN.C
        $(ccomp) ISSENSIN.C   >> con
        echo -+ISSENSIN &   >>~libresp

$(path)tools_$(model).lib :       IVDISABL.C
        $(ccomp) IVDISABL.C   >> con
        echo -+IVDISABL &   >>~libresp

$(path)tools_$(model).lib :       IVINSTAL.C
        $(ccomp) IVINSTAL.C   >> con
        echo -+IVINSTAL &   >>~libresp

$(path)tools_$(model).lib :       IVSENSIN.C
        $(ccomp) IVSENSIN.C   >> con
        echo -+IVSENSIN &   >>~libresp

$(path)tools_$(model).lib :       IVVECSIN.C
        $(ccomp) IVVECSIN.C   >> con
        echo -+IVVECSIN &   >>~libresp

