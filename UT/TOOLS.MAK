acomp=tasm /Mx /i$(apath)
ccomp=bcc -c -m$(model) -w -O -N -V -vi- -r- -v

$(path)tools_$(model).lib :       UTCREATE.ASM
        $(acomp) UTCREATE;  >> con
        echo -+UTCREATE &   >>~libresp

$(path)tools_$(model).lib :       UTCODE.ASM
        $(acomp) UTCODE;  >> con
        echo -+UTCODE &   >>~libresp

$(path)tools_$(model).lib :       UTDECODE.ASM
        $(acomp) UTDECODE;  >> con
        echo -+UTDECODE &   >>~libresp

$(path)tools_$(model).lib :       UTREMOVE.ASM
        $(acomp) UTREMOVE;  >> con
        echo -+UTREMOVE &   >>~libresp

$(path)tools_$(model).lib :       UTRENAME.ASM
        $(acomp) UTRENAME;  >> con
        echo -+UTRENAME &   >>~libresp

$(path)tools_$(model).lib :       UTLSEEK.ASM
        $(acomp) UTLSEEK;  >> con
        echo -+UTLSEEK &   >>~libresp

$(path)tools_$(model).lib :       UTFSIZE.ASM
        $(acomp) UTFSIZE;  >> con
        echo -+UTFSIZE &   >>~libresp

$(path)tools_$(model).lib :       UTCHSIZE.ASM
        $(acomp) UTCHSIZE;  >> con
        echo -+UTCHSIZE &   >>~libresp

$(path)tools_$(model).lib :       UTREAD.ASM
        $(acomp) UTREAD;  >> con
        echo -+UTREAD &   >>~libresp

$(path)tools_$(model).lib :       UTWRITE.ASM
        $(acomp) UTWRITE;  >> con
        echo -+UTWRITE &   >>~libresp

$(path)tools_$(model).lib :       UTCLOSE.ASM
        $(acomp) UTCLOSE;  >> con
        echo -+UTCLOSE &   >>~libresp

$(path)tools_$(model).lib :       UTOPEN.ASM
        $(acomp) UTOPEN;  >> con
        echo -+UTOPEN &   >>~libresp

$(path)tools_$(model).lib :       UTCH2PRN.ASM
        $(acomp) UTCH2PRN;  >> con
        echo -+UTCH2PRN &   >>~libresp

$(path)tools_$(model).lib :       UTGET_DS.ASM
        $(acomp) UTGET_DS;  >> con
        echo -+UTGET_DS &   >>~libresp

$(path)tools_$(model).lib :       UTPUT_DS.ASM
        $(acomp) UTPUT_DS;  >> con
        echo -+UTPUT_DS &   >>~libresp

$(path)tools_$(model).lib :       UTC2LRDX.ASM
        $(acomp) UTC2LRDX;  >> con
        echo -+UTC2LRDX &   >>~libresp

$(path)tools_$(model).lib :       UTL2CRDX.ASM
        $(acomp) UTL2CRDX;  >> con
        echo -+UTL2CRDX &   >>~libresp

$(path)tools_$(model).lib :       UTINTFLG.ASM
        $(acomp) UTINTFLG;  >> con
        echo -+UTINTFLG &   >>~libresp

$(path)tools_$(model).lib :       UTISEMPT.ASM
        $(acomp) UTISEMPT;  >> con
        echo -+UTISEMPT &   >>~libresp

$(path)tools_$(model).lib :       UTMOVABS.ASM
        $(acomp) UTMOVABS;  >> con
        echo -+UTMOVABS &   >>~libresp

$(path)tools_$(model).lib :       UTEXBLNK.ASM
        $(acomp) UTEXBLNK;  >> con
        echo -+UTEXBLNK &   >>~libresp

$(path)tools_$(model).lib :       UTMOVSC.ASM
        $(acomp) UTMOVSC;  >> con
        echo -+UTMOVSC &   >>~libresp

$(path)tools_$(model).lib :       UTREWCHR.ASM
        $(acomp) UTREWCHR;  >> con
        echo -+UTREWCHR &   >>~libresp

$(path)tools_$(model).lib :       UTBUFCMP.ASM
        $(acomp) UTBUFCMP;  >> con
        echo -+UTBUFCMP &   >>~libresp

$(path)tools_$(model).lib :       UTALARM.C
        $(ccomp) UTALARM.C   >> con
        echo -+UTALARM &   >>~libresp

$(path)tools_$(model).lib :       UTQSORT.C
        $(ccomp) UTQSORT.C   >> con
        echo -+UTQSORT &   >>~libresp

$(path)tools_$(model).lib :       UTCRITSF.C
        $(ccomp) UTCRITSF.C   >> con
        echo -+UTCRITSF &   >>~libresp

$(path)tools_$(model).lib :       UTCTLBRK.C
        $(ccomp) UTCTLBRK.C   >> con
        echo -+UTCTLBRK &   >>~libresp

$(path)tools_$(model).lib :       UTCVDATE.C
        $(ccomp) UTCVDATE.C   >> con
        echo -+UTCVDATE &   >>~libresp

$(path)tools_$(model).lib :       UTALLOCB.C
        $(ccomp) UTALLOCB.C   >> con
        echo -+UTALLOCB &   >>~libresp

$(path)tools_$(model).lib :       UTC2DBL.C
        $(ccomp) UTC2DBL.C   >> con
        echo -+UTC2DBL &   >>~libresp

$(path)tools_$(model).lib :       UTCHEXT.C
        $(ccomp) UTCHEXT.C   >> con
        echo -+UTCHEXT &   >>~libresp

$(path)tools_$(model).lib :       UTDBL2CE.C
        $(ccomp) UTDBL2CE.C   >> con
        echo -+UTDBL2CE &   >>~libresp

$(path)tools_$(model).lib :       UTDBL2CF.C
        $(ccomp) UTDBL2CF.C >> con
        echo -+UTDBL2CF &   >>~libresp

$(path)tools_$(model).lib :       UTFREE.C
        $(ccomp) UTFREE.C >> con
        echo -+UTFREE &   >>~libresp

$(path)tools_$(model).lib :       UTGETCLK.C
        $(ccomp) UTGETCLK.C   >> con
        echo -+UTGETCLK &   >>~libresp

$(path)tools_$(model).lib :       UTL2C.C
        $(ccomp) UTL2C.C   >> con
        echo -+UTL2C &   >>~libresp

$(path)tools_$(model).lib :       UTMMCTRL.C
        $(ccomp) UTMMCTRL.C   >> con
        echo -+UTMMCTRL &   >>~libresp

$(path)tools_$(model).lib :       UTMMSIZE.C
        $(ccomp) UTMMSIZE.C   >> con
        echo -+UTMMSIZE &   >>~libresp

$(path)tools_$(model).lib :       UTMOVMEM.C
        $(ccomp) UTMOVMEM.C   >> con
        echo -+UTMOVMEM &   >>~libresp

$(path)tools_$(model).lib :       UTSLEEPT.C
        $(ccomp) UTSLEEPT.C   >> con
        echo -+UTSLEEPT &   >>~libresp

$(path)tools_$(model).lib :       UTSPEAKR.C
        $(ccomp) UTSPEAKR.C   >> con
        echo -+UTSPEAKR &   >>~libresp

$(path)tools_$(model).lib :       UTSTRCVT.C
        $(ccomp) UTSTRCVT.C   >> con
        echo -+UTSTRCVT &   >>~libresp

$(path)tools_$(model).lib :       UTSTREXP.C
        $(ccomp) UTSTREXP.C   >> con
        echo -+UTSTREXP &   >>~libresp

$(path)tools_$(model).lib :       UTSTRIND.C
        $(ccomp) UTSTRIND.C   >> con
        echo -+UTSTRIND &   >>~libresp

$(path)tools_$(model).lib :       UTSTRJST.C
        $(ccomp) UTSTRJST.C   >> con
        echo -+UTSTRJST &   >>~libresp

$(path)tools_$(model).lib :       UTSTRTAB.C
        $(ccomp) UTSTRTAB.C   >> con
        echo -+UTSTRTAB &   >>~libresp

$(path)tools_$(model).lib :       UTSTRXLT.C
        $(ccomp) UTSTRXLT.C   >> con
        echo -+UTSTRXLT &   >>~libresp

$(path)tools_$(model).lib :       UTUNINAM.C
        $(ccomp) UTUNINAM.C   >> con
        echo -+UTUNINAM &   >>~libresp

$(path)tools_$(model).lib :       UTUPPLOW.C
        $(ccomp) UTUPPLOW.C   >> con
        echo -+UTUPPLOW &   >>~libresp

$(path)tools_$(model).lib :       UTOUTDAT.C
        $(ccomp) UTOUTDAT.C   >> con
        echo -+UTOUTDAT &   >>~libresp

$(path)tools_$(model).lib :       UTMONY2C.C
        $(ccomp) UTMONY2C.C   >> con
        echo -+UTMONY2C &   >>~libresp

$(path)tools_$(model).lib :       UTP2ENV.C
        $(ccomp) UTP2ENV.C   >> con
        echo -+UTP2ENV &   >>~libresp

$(path)tools_$(model).lib :       UTSETENV.C
        $(ccomp) UTSETENV.C   >> con
        echo -+UTSETENV &   >>~libresp

$(path)tools_$(model).lib :       UTSPAWN.C
        $(ccomp) UTSPAWN.C   >> con
        echo -+UTSPAWN &   >>~libresp

$(path)tools_$(model).lib :       UTVFYMEM.C
        $(ccomp) UTVFYMEM.C   >> con
        echo -+UTVFYMEM &   >>~libresp

$(path)tools_$(model).lib :       UTCOPY.C
        $(ccomp) UTCOPY.C   >> con
        echo -+UTCOPY &   >>~libresp

$(path)tools_$(model).lib :       UTHNDCOP.C
        $(ccomp) UTHNDCOP.C   >> con
        echo -+UTHNDCOP &   >>~libresp

