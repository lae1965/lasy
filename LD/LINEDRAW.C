#include    <slldraw.h>
#include    <slkeyb.h>
#include    <slsymb.def>

static int      ldcorrect   (int sym, int single);


int ldgetcod (char cld) {
    switch (cld) {
        case  'À':    return      20;
        case  'Ğ':    return      37;
        case  'Á':    return      21;
        case  'Ñ':    return      74;
        case  'Â':    return      69;
        case  'Ò':    return     133;
        case  '³':    return      80;
        case  'Ã':    return      84;
        case  'Ó':    return      36;
        case  '´':    return      81;
        case  'Ä':    return      5 ;
        case  'Ô':    return      24;
        case  'µ':    return      82;
        case  'Å':    return      85;
        case  'Õ':    return      72;
        case  '¶':    return     161;
        case  'Æ':    return      88;
        case  'Ö':    return     132;
        case  '·':    return     129;
        case  'Ç':    return     164;
        case  '×':    return     165;
        case  '¸':    return     66 ;
        case  'È':    return     40 ;
        case  'Ø':    return      90;
        case  '¹':    return     162;
        case  'É':    return     136;
        case  'Ù':    return      17;
        case  'º':    return     160;
        case  'Ê':    return      42;
        case  'Ú':    return      68;
        case  '»':    return     130;
        case  'Ë':    return     138;
        case  '¼':    return      34;
        case  'Ì':    return     168;
        case  '½':    return      33;
        case  'Í':    return      10;
        case  '¾':    return      18;
        case  'Î':    return     170;
        case  '¿':    return      65;
        case  'Ï':    return      26;
        default:
            return 0;
    }
}

char ldgetchr (int x, int single) {

    switch (ldcorrect (x, single)) {
       case  20:    return ('À');
       case  37:    return ('Ğ');
       case  21:    return ('Á');
       case  74:    return ('Ñ');
       case  69:    return ('Â');
       case  133:   return ('Ò');
       case  80:    return ('³');
       case  84:    return ('Ã');
       case  36:    return ('Ó');
       case  81:    return ('´');
       case  5 :    return ('Ä');
       case  24:    return ('Ô');
       case  82:    return ('µ');
       case  85:    return ('Å');
       case  72:    return ('Õ');
       case 161:    return ('¶');
       case  88:    return ('Æ');
       case 132:    return ('Ö');
       case 129:    return ('·');
       case 164:    return ('Ç');
       case 165:    return ('×');
       case 66 :    return ('¸');
       case 40 :    return ('È');
       case 90:     return ('Ø');
       case 162:    return ('¹');
       case 136:    return ('É');
       case  17:    return ('Ù');
       case 160:    return ('º');
       case  42:    return ('Ê');
       case  68:    return ('Ú');
       case 130:    return ('»');
       case 138:    return ('Ë');
       case  34:    return ('¼');
       case 168:    return ('Ì');
       case  33:    return ('½');
       case  10:    return ('Í');
       case  18:    return ('¾');
       case 170:    return ('Î');
       case  65:    return ('¿');
       case  26:    return ('Ï');
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
                { s.x = (single) ? 80 : 160;  goto fin; }   /* ³ */
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
                { s.x = (single) ? 80 : 160;  goto fin; }   /* ³ */
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
                { s.x = (single) ? 5 : 10;  goto fin; }     /* Ä */
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
                { s.x = (single) ? 5 : 10;  goto fin; }      /* Ä */
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