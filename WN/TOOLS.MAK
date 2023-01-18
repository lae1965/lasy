acomp=tasm /Mx /i$(apath)
ccomp=bcc -c -m$(model) -w -O -N -V -vi- -r-

$(path)tools_$(model).lib :       WNMOVPS.ASM
        $(acomp) WNMOVPS;   >> con
        echo -+WNMOVPS &   >>~libresp

$(path)tools_$(model).lib :       WNMOVSP.ASM
        $(acomp) WNMOVSP;   >> con
        echo -+WNMOVSP &   >>~libresp

$(path)tools_$(model).lib :       WNMOVPC.ASM
        $(acomp) WNMOVPC;   >> con
        echo -+WNMOVPC &   >>~libresp

$(path)tools_$(model).lib :       WNMOVPA.ASM
        $(acomp) WNMOVPA;   >> con
        echo -+WNMOVPA &   >>~libresp

$(path)tools_$(model).lib :       WNMOVPCA.ASM
        $(acomp) WNMOVPCA;   >> con
        echo -+WNMOVPCA &   >>~libresp

$(path)tools_$(model).lib :       WNMOVPSA.ASM
        $(acomp) WNMOVPSA;   >> con
        echo -+WNMOVPSA &   >>~libresp

$(path)tools_$(model).lib :       WNVICOPY.ASM
        $(acomp) WNVICOPY;   >> con
        echo -+WNVICOPY &   >>~libresp

$(path)tools_$(model).lib :       WNADDJOI.C
        $(ccomp) WNADDJOI.C   >> con
        echo -+WNADDJOI &   >>~libresp

$(path)tools_$(model).lib :       WNATRBLK.C
        $(ccomp) WNATRBLK.C   >> con
        echo -+WNATRBLK &   >>~libresp

$(path)tools_$(model).lib :       WNBLINKS.C
        $(ccomp) WNBLINKS.C   >> con
        echo -+WNBLINKS &   >>~libresp

$(path)tools_$(model).lib :       WNCOLORS.C
        $(ccomp) WNCOLORS.C   >> con
        echo -+WNCOLORS &   >>~libresp

$(path)tools_$(model).lib :       WNCOVERW.C
        $(ccomp) WNCOVERW.C   >> con
        echo -+WNCOVERW &   >>~libresp

$(path)tools_$(model).lib :       WNCREATE.C
        $(ccomp) WNCREATE.C   >> con
        echo -+WNCREATE &   >>~libresp

$(path)tools_$(model).lib :       WNCURMOV.C
        $(ccomp) WNCURMOV.C   >> con
        echo -+WNCURMOV &   >>~libresp

$(path)tools_$(model).lib :       WNCURSOR.C
        $(ccomp) WNCURSOR.C   >> con
        echo -+WNCURSOR &   >>~libresp

$(path)tools_$(model).lib :       WNCUSTOM.C
        $(ccomp) WNCUSTOM.C   >> con
        echo -+WNCUSTOM &   >>~libresp

$(path)tools_$(model).lib :       WNDISFON.C
        $(ccomp) WNDISFON.C   >> con
        echo -+WNDISFON &   >>~libresp

$(path)tools_$(model).lib :       WNDISTXT.C
        $(ccomp) WNDISTXT.C   >> con
        echo -+WNDISTXT &   >>~libresp

$(path)tools_$(model).lib :       WNDISWRK.C
        $(ccomp) WNDISWRK.C   >> con
        echo -+WNDISWRK &   >>~libresp

$(path)tools_$(model).lib :       WNDSPLAY.C
        $(ccomp) WNDSPLAY.C   >> con
        echo -+WNDSPLAY &   >>~libresp

$(path)tools_$(model).lib :       WNDSTROY.C
        $(ccomp) WNDSTROY.C   >> con
        echo -+WNDSTROY &   >>~libresp

$(path)tools_$(model).lib :       WNFUNHLP.C
        $(ccomp) WNFUNHLP.C   >> con
        echo -+WNFUNHLP &   >>~libresp

$(path)tools_$(model).lib :       WNFUNKEY.C
        $(ccomp) WNFUNKEY.C   >> con
        echo -+WNFUNKEY &   >>~libresp

$(path)tools_$(model).lib :       WNGETCUR.C
        $(ccomp) WNGETCUR.C   >> con
        echo -+WNGETCUR &   >>~libresp

$(path)tools_$(model).lib :       WNGETHLP.C
        $(ccomp) WNGETHLP.C   >> con
        echo -+WNGETHLP &   >>~libresp

$(path)tools_$(model).lib :       WNHIDEWN.C
        $(ccomp) WNHIDEWN.C   >> con
        echo -+WNHIDEWN &   >>~libresp

$(path)tools_$(model).lib :       WNINIWIN.C
        $(ccomp) WNINIWIN.C   >> con
        echo -+WNINIWIN &   >>~libresp

$(path)tools_$(model).lib :       WNKEYHLP.C
        $(ccomp) WNKEYHLP.C   >> con
        echo -+WNKEYHLP &   >>~libresp

$(path)tools_$(model).lib :       WNMOVWIN.C
        $(ccomp) WNMOVWIN.C   >> con
        echo -+WNMOVWIN &   >>~libresp

$(path)tools_$(model).lib :       WNNUMINP.C
        $(ccomp) WNNUMINP.C   >> con
        echo -+WNNUMINP &   >>~libresp

$(path)tools_$(model).lib :       WNOVRLAP.C
        $(ccomp) WNOVRLAP.C   >> con
        echo -+WNOVRLAP &   >>~libresp

$(path)tools_$(model).lib :       WNPAGADD.C
        $(ccomp) WNPAGADD.C   >> con
        echo -+WNPAGADD &   >>~libresp

$(path)tools_$(model).lib :       WNPAGREM.C
        $(ccomp) WNPAGREM.C   >> con
        echo -+WNPAGREM &   >>~libresp

$(path)tools_$(model).lib :       WNPUTBOR.C
        $(ccomp) WNPUTBOR.C   >> con
        echo -+WNPUTBOR &   >>~libresp

$(path)tools_$(model).lib :       WNREDRAW.C
        $(ccomp) WNREDRAW.C   >> con
        echo -+WNREDRAW &   >>~libresp

$(path)tools_$(model).lib :       WNREMOVE.C
        $(ccomp) WNREMOVE.C   >> con
        echo -+WNREMOVE &   >>~libresp

$(path)tools_$(model).lib :       WNRESIZE.C
        $(ccomp) WNRESIZE.C   >> con
        echo -+WNRESIZE &   >>~libresp

$(path)tools_$(model).lib :       WNSCRBLK.C
        $(ccomp) WNSCRBLK.C   >> con
        echo -+WNSCRBLK &   >>~libresp

$(path)tools_$(model).lib :       WNSCREAD.C
        $(ccomp) WNSCREAD.C   >> con
        echo -+WNSCREAD &   >>~libresp

$(path)tools_$(model).lib :       WNSELECT.C
        $(ccomp) WNSELECT.C   >> con
        echo -+WNSELECT &   >>~libresp

$(path)tools_$(model).lib :       WNSETCUR.C
        $(ccomp) WNSETCUR.C   >> con
        echo -+WNSETCUR &   >>~libresp

$(path)tools_$(model).lib :       WNSETPAL.C
        $(ccomp) WNSETPAL.C   >> con
        echo -+WNSETPAL &   >>~libresp

$(path)tools_$(model).lib :       WNSIZCUR.C
        $(ccomp) WNSIZCUR.C   >> con
        echo -+WNSIZCUR &   >>~libresp

$(path)tools_$(model).lib :       WNSPLITW.C
        $(ccomp) WNSPLITW.C   >> con
        echo -+WNSPLITW &   >>~libresp

$(path)tools_$(model).lib :       WNSTREDT.C
        $(ccomp) WNSTREDT.C   >> con
        echo -+WNSTREDT &   >>~libresp

$(path)tools_$(model).lib :       WNSTRINP.C
        $(ccomp) WNSTRINP.C   >> con
        echo -+WNSTRINP &   >>~libresp

$(path)tools_$(model).lib :       WNTOGCUR.C
        $(ccomp) WNTOGCUR.C   >> con
        echo -+WNTOGCUR &   >>~libresp

$(path)tools_$(model).lib :       WNUNDJOI.C
        $(ccomp) WNUNDJOI.C   >> con
        echo -+WNUNDJOI &   >>~libresp

$(path)tools_$(model).lib :       WNUNHIDE.C
        $(ccomp) WNUNHIDE.C   >> con
        echo -+WNUNHIDE &   >>~libresp

$(path)tools_$(model).lib :       WNUPDATE.C
        $(ccomp) WNUPDATE.C   >> con
        echo -+WNUPDATE &   >>~libresp

$(path)tools_$(model).lib :       WNUPDBLK.C
        $(ccomp) WNUPDBLK.C   >> con
        echo -+WNUPDBLK &   >>~libresp

$(path)tools_$(model).lib :       WNVIHORZ.C
        $(ccomp) WNVIHORZ.C   >> con
        echo -+WNVIHORZ &   >>~libresp

$(path)tools_$(model).lib :       WNVIPTRL.C
        $(ccomp) WNVIPTRL.C   >> con
        echo -+WNVIPTRL &   >>~libresp

$(path)tools_$(model).lib :       WNVIROLL.C
        $(ccomp) WNVIROLL.C   >> con
        echo -+WNVIROLL &   >>~libresp

$(path)tools_$(model).lib :       WNWRTBUF.C
        $(ccomp) WNWRTBUF.C   >> con
        echo -+WNWRTBUF &   >>~libresp

$(path)tools_$(model).lib :       WNSCRBAR.C
        $(ccomp) WNSCRBAR.C   >> con
        echo -+WNSCRBAR &   >>~libresp

