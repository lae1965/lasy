acomp=tasm /Mx /i$(apath)
ccomp=bcc -c -m$(model) -w -O -N -V -vi- -r- -v

$(path)tools_$(model).lib :       MNCREATE.C
        $(ccomp) MNCREATE.C   >> con
        echo -+MNCREATE &   >>~libresp

$(path)tools_$(model).lib :       MNCRESTD.C
        $(ccomp) MNCRESTD.C   >> con
        echo -+MNCRESTD &   >>~libresp

$(path)tools_$(model).lib :       MNDISVAR.C
        $(ccomp) MNDISVAR.C   >> con
        echo -+MNDISVAR &   >>~libresp

$(path)tools_$(model).lib :       MNDSKINP.C
        $(ccomp) MNDSKINP.C   >> con
        echo -+MNDSKINP &   >>~libresp

$(path)tools_$(model).lib :       MNDSPLAY.C
        $(ccomp) MNDSPLAY.C   >> con
        echo -+MNDSPLAY &   >>~libresp

$(path)tools_$(model).lib :       MNDSTROY.C
        $(ccomp) MNDSTROY.C   >> con
        echo -+MNDSTROY &   >>~libresp

$(path)tools_$(model).lib :       MNFNDITM.C
        $(ccomp) MNFNDITM.C   >> con
        echo -+MNFNDITM &   >>~libresp

$(path)tools_$(model).lib :       MNFNMINP.C
        $(ccomp) MNFNMINP.C   >> con
        echo -+MNFNMINP &   >>~libresp

$(path)tools_$(model).lib :       MNGETASC.C
        $(ccomp) MNGETASC.C   >> con
        echo -+MNGETASC &   >>~libresp

$(path)tools_$(model).lib :       MNGETCOD.C
        $(ccomp) MNGETCOD.C   >> con
        echo -+MNGETCOD &   >>~libresp

$(path)tools_$(model).lib :       MNGETITM.C
        $(ccomp) MNGETITM.C   >> con
        echo -+MNGETITM &   >>~libresp

$(path)tools_$(model).lib :       MNGETTXT.C
        $(ccomp) MNGETTXT.C   >> con
        echo -+MNGETTXT &   >>~libresp

$(path)tools_$(model).lib :       MNHARDER.C
        $(ccomp) MNHARDER.C   >> con
        echo -+MNHARDER &   >>~libresp

$(path)tools_$(model).lib :       MNITMADD.C
        $(ccomp) MNITMADD.C   >> con
        echo -+MNITMADD &   >>~libresp

$(path)tools_$(model).lib :       MNITMATR.C
        $(ccomp) MNITMATR.C   >> con
        echo -+MNITMATR &   >>~libresp

$(path)tools_$(model).lib :       MNMOVEIT.C
        $(ccomp) MNMOVEIT.C   >> con
        echo -+MNMOVEIT &   >>~libresp

$(path)tools_$(model).lib :       MNPALETT.C
        $(ccomp) MNPALETT.C   >> con
        echo -+MNPALETT &   >>~libresp

$(path)tools_$(model).lib :       MNPRNOPT.C
        $(ccomp) MNPRNOPT.C   >> con
        echo -+MNPRNOPT &   >>~libresp

$(path)tools_$(model).lib :       MNPRNPAG.C
        $(ccomp) MNPRNPAG.C   >> con
        echo -+MNPRNPAG &   >>~libresp

$(path)tools_$(model).lib :       MNPRNPG1.C
        $(ccomp) MNPRNPG1.C   >> con
        echo -+MNPRNPG1 &   >>~libresp

$(path)tools_$(model).lib :       MNREADOP.C
        $(ccomp) MNREADOP.C   >> con
        echo -+MNREADOP &   >>~libresp

$(path)tools_$(model).lib :       MNREMOVE.C
        $(ccomp) MNREMOVE.C   >> con
        echo -+MNREMOVE &   >>~libresp

$(path)tools_$(model).lib :       MNROLMNU.C
        $(ccomp) MNROLMNU.C   >> con
        echo -+MNROLMNU &   >>~libresp

$(path)tools_$(model).lib :       MNROLWIN.C
        $(ccomp) MNROLWIN.C   >> con
        echo -+MNROLWIN &   >>~libresp

$(path)tools_$(model).lib :       MNSETCUR.C
        $(ccomp) MNSETCUR.C   >> con
        echo -+MNSETCUR &   >>~libresp

$(path)tools_$(model).lib :       MNSETPRO.C
        $(ccomp) MNSETPRO.C   >> con
        echo -+MNSETPRO &   >>~libresp

$(path)tools_$(model).lib :       MNUPDMNU.C
        $(ccomp) MNUPDMNU.C   >> con
        echo -+MNUPDMNU &   >>~libresp

$(path)tools_$(model).lib :       MNWARNIN.C
        $(ccomp) MNWARNIN.C   >> con
        echo -+MNWARNIN &   >>~libresp
