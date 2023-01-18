#include <sldbas.h>
#include <slkeyb.h>
#include <slsymb.def>


int     edt_doc (void) {


    if (Db_main_edit) switch (Kb_sym) {
        case ShiftF4:          dbed_jst ();          break;
        case F4:
            Cur_Edt->opt.autojump = !Cur_Edt->opt.autojump;           break;
        case ShiftF6:          dbed_mup (NULL);                       break;
        case F6:               dbed_mup (Cur_Edt->cur->pfld->curcrt); break;
        case ShiftF8:          dbed_mdn (NULL);                       break;
        case F8:               dbed_mdn (Cur_Edt->cur->pfld->curcrt); break;
        case ShiftF9:          dbed_del (NULL);                       break;
        case F9:               dbed_del (Cur_Edt->cur->pfld->curcrt); break;
        case F10:              dbed_svr (Cur_Edt, YES);               break;
        case CtrlF9:           dbed_cld (Cur_Edt);                    return 1;
        default :              break;
    }
    if (Db_edit) switch (Kb_sym) {
        case F1:               dbed_clb ();          break;
        case F2:               dbed_sde ();          break;
        case ShiftF2:          dbed_mov ();          break;
        case F3:               dbed_par ();          break;
        case F5:               dbed_cdt ();          break;
        default :                                    break;
    }

    switch (Kb_sym) {
        case CtrlF4:           dbed_srt (YES);       break;
        case AltF4:            dbed_srt (NO);        break;
        case ShiftF5:          dbed_2fl (Cur_Edt, YES);       break;
        case F7:               dbprprn  (dbpreview); return 1;
        case ShiftF7:          dbed_prf ();          break;
        case CtrlF7:           dbed_prd ();          break;
        case ShiftF10:         dbed_ldr (YES);       return 1;  /* newdoc */
        case AltF10:           dbed_ldr (NO);        return 1;
        case CtrlF10:          dbed_dlr (Cur_Edt);   return 1;
        case Enter:
        case GrayEnter:        dbed_ent (NO);        break;
        case CtrlEnter:
        case GrayCtrlEnter:    dbed_ent (YES);       break;

        case Esc:
        case AltX:
            if (dbed_c2v ()) return 2;
            break;                                              /* end of edit */
        default :                                    return -2;
    }
    return 0;
}

int     edt_query (void) {

    switch (Kb_sym) {
        case F4:
            Cur_Edt->opt.autojump = !Cur_Edt->opt.autojump;  break;
        case F5:               dbed_cdt ();          break;
        case ShiftF5:          dbed_2fl (Cur_Edt, YES);       break;
        case ShiftF6:          dbed_mup (NULL);      break;
        case F6:               dbed_mup (Cur_Edt->cur->pfld->curcrt);  break;
        case F7:               dbprprn  (dbpreview); return 1;
        case ShiftF7:          dbed_prf ();          break;
        case CtrlF7:           dbed_prd ();          break;
        case Enter:
        case GrayEnter:        dbed_tab (NO);        break;
        case ShiftF8:          dbed_mdn (NULL);      break;
        case F8:               dbed_mdn (Cur_Edt->cur->pfld->curcrt);  break;
        case ShiftF9:          dbed_del (NULL);      break;
        case F9:               dbed_del (Cur_Edt->cur->pfld->curcrt);  break;
        case CtrlF9:           dbed_cld (Cur_Edt);   return  1;  /* newdoc */
        case F10:
            if (dbed_c2v ()) return 2;                /* Поиск */
            break;
        case Esc:
        case AltX:
            dbed_c2v ();
            return -1;                               /* Отказ */
        default :                                    return -2;
    }
    return 0;
}

