@echo off
if exist language.h     del language.h
if exist compiler.mac   del compiler.mac
if exist *.obj          del *.obj

echo>language.h #define TC_lang
rem echo>>language.h #define ENGLISH

echo: >c:\bc\include\compiler.mac NLARGE equ 1

echo /C c:\bc\lib\TOOLS_L  &                   >~libresp
echo:                                            > con
ntmake /S model=l path=c:\bc\lib\ apath=c:\bc\include\ tools.mak
if errorlevel 2 goto end
echo ,NUL                                       >>~libresp

tlib @~libresp                                  >> con

if exist ~libresp       del ~libresp
if exist language.h     del language.h
if exist compiler.mac   del compiler.mac
if exist *.obj          del *.obj
:end
