#include    <slldraw.h>
#include    <slkeyb.h>
#include    <slsymb.def>

static int      ldcorrect   (int sym, int single);


int ldgetcod (char cld) {
    switch (cld) {
        case  '?':    return      20;
        case  '?':    return      37;
        case  '?':    return      21;
        case  '?':    return      74;
        case  '?':    return      69;
        case  '?':    return     133;
        case  '?':    return      80;
        case  '?':    return      84;
        case  '?':    return      36;
        case  '?':    return      81;
        case  '?':    return      5 ;
        case  '?':    return      24;
        case  '?':    return      82;
        case  '?':    return      85;
        case  '?':    return      72;
        case  '?':    return     161;
        case  '?':    return      88;
        case  '?':    return     132;
        case  '?':    return     129;
        case  '?':    return     164;
        case  '?':    return     165;
        case  '?':    return     66 ;
        case  '?':    return     40 ;
        case  '?':    return      90;
        case  '?':    return     162;
        case  '?':    return     136;
        case  '?':    return      17;
        case  '?':    return     160;
        case  '?':    return      42;
        case  '?':    return      68;
        case  '?':    return     130;
        case  '?':    return     138;
        case  '?':    return      34;
        case  '?':    return     168;
        case  '?':    return      33;
        case  '?':    return      10;
        case  '?':    return      18;
        case  '?':    return     170;
        case  '?':    return      65;
        case  '?':    return      26;
        default:
            return 0;
    }
}

char ldgetchr (int x, int single) {

    switch (ldcorrect (x, single)) {
       case  20:    return ('?');
       case  37:    return ('?');
       case  21:    return ('?');
       case  74:    return ('?');
       case  69:    return ('?');
       case  133:   return ('?');
       case  80:    return ('?');
       case  84:    return ('?');
       case  36:    return ('?');
       case  81:    return ('?');
       case  5 :    return ('?');
       case  24:    return ('?');
       case  82:    return ('?');
       case  85:    return ('?');
       case  72:    return ('?');
       case 161:    return ('?');
       case  88:    return ('?');
       case 132:    return ('?');
       case 129:    return ('?');
       case 164:    return ('?');
       case 165:    return ('?');
       case 66 :    return ('?');
       case 40 :    return ('?');
       case 90:     return ('?');
       case 162:    return ('?');
       case 136:    return ('?');
       case  17:    return ('?');
       case 160:    return ('?');
       case  42:    return ('?');
       case  68:    return ('?');
       case 130:    return ('?');
       case 138:    return ('?');
       case  34:    return ('?');
       case 168:    return ('?');
       case  33:    return ('?');
       case  10:    return ('?');
       case  18:    return ('?');
       case 170:    return ('?');
       case  65:    return ('?');
       case  26:    return ('?');
       default:
               utalarm();
               return (0);
    }
}

static int ldcorrect (int sym, int single) {
    SYM_LD  s;

    s.x = sym;
    switch (Kb_sym) {
        case ShiftUp:
            if (s.x == 0 || s.x == 16 || s.x == 32)
                { s.x = (single) ? 80 : 160;  goto fin; }   /* ? */
            if (single) {
                s.b.up_s = 1; s.b.up_d = 0;
                if (s.b.dn_d == 1) { s.b.dn_d = 0; s.b.dn_s = 1;}
			}
			else {
                s.b.up_d = 1; s.b.up_s = 0;
                if (s.b.dn_s == 1) { s.b.dn_d = 1; s.b.dn_s = 0;}
			}
            goto hor;
        case ShiftDown:
            if (s.x == 0 || s.x == 64 || s.x == 128)
                { s.x = (single) ? 80 : 160;  goto fin; }   /* ? */
            if (single) {
                s.b.dn_s = 1; s.b.dn_d = 0;
                if (s.b.up_d == 1) { s.b.up_d = 0; s.b.up_s = 1;}
			}
			else {
                s.b.dn_d = 1; s.b.dn_s = 0;
                if (s.b.up_s == 1) { s.b.up_d = 1; s.b.up_s = 0;}
			}
            goto hor;
        case ShiftLeft:
            if (s.x == 0 || s.x == 1 || s.x == 2)
                { s.x = (single) ? 5 : 10;  goto fin; }     /* ? */
            if (single) {
                s.b.lt_s = 1; s.b.lt_d = 0;
                if (s.b.rt_d == 1) { s.b.rt_d = 0; s.b.rt_s = 1;}
			}
            else {
                s.b.lt_d = 1; s.b.lt_s = 0;
                if (s.b.rt_s == 1) { s.b.rt_d = 1; s.b.rt_s = 0;}
			}
            goto ver;
        case ShiftRight:
            if (s.x == 0 || s.x == 4 || s.x == 8)
                { s.x = (single) ? 5 : 10;  goto fin; }      /* ? */
            if (single) {
                s.b.rt_s = 1; s.b.rt_d = 0;
                if (s.b.lt_d == 1) {s.b.lt_d = 0; s.b.lt_s = 1;}
			}
			else {
                s.b.rt_d = 1; s.b.rt_s = 0;
                if (s.b.lt_s == 1) {s.b.lt_d = 1; s.b.lt_s = 0;}
			}
            goto ver;
		default:
            return 0;
	}
hor:
    if ((s.x & 9) == 9) {                                 /* -  = */
       if (single)  { s.b.rt_d = 0; s.b.rt_s = 1;}
       else             { s.b.lt_d = 1; s.b.lt_s = 0;}
    } else if ((s.x & 6) == 6) {
       if (single)  { s.b.lt_d = 0; s.b.lt_s = 1;}    /* =  - */
       else             { s.b.rt_d = 1; s.b.rt_s = 0;}
	}
    goto fin;
ver:
    if ((s.x & 96) == 96)  {                              /*   ||   */
       if (single)  { s.b.up_d = 0; s.b.up_s = 1;}    /*        */
       else             { s.b.dn_d = 1; s.b.dn_s = 0;}    /*    |   */
    }
    else if ((s.x & 144) == 144) {                        /*   |    */
       if (single)  { s.b.dn_d = 0; s.b.dn_s = 1;}    /*        */
       else             { s.b.up_d = 1; s.b.up_s = 0;}    /*   ||   */
    }

fin:
    return (s.x);
}

