.XLIST
        PAGE
comment %
****************************************************************
*                                                              *
*       Data segment for mouse functions use define here.      *
*                                                              *
****************************************************************
%
        include begasm.mac
        beginDseg

        public _MS_buttons
        public _MS_mode
        public _MS_unhide

        public wShadowEvent   
        public pfShadowFunc   
        public wUserEvent     
        public pfUserFunc     


_MS_buttons     dw      0   ; Number of mouse buttons.
_MS_mode        dw      0   ; low byte is column corrector
                            ; high byte is row corrector
                            ; MS_mode = 0 for graphics
                            ; (no any correction).
_MS_unhide      dw      0   ; 1=marker shown, 0=if not.

; Internal variables which used in MSINST and MSHANDLR modules.
wShadowEvent    dw      0   ; Event mask for SHADOW mouse handler.
pfShadowFunc    dd      0   ; Address of the SHADOW interrupt function.
wUserEvent      dw      0   ; Event mask for User's mouse.
pfUserFunc      dd      0   ; Address of the User  interrupt function. 

        endDseg
        end
