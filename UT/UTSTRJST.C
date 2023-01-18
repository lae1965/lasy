#include <slutil.h>

void utstrjst (char * to, char * from, int len, char fill, int w, int code) {
    int     space, left = 0, right = 0;

    if (w <= 0) return;

    if ((code & UT_SGN_BEG) && (*from == '+' || *from == '-' || *from == ' ')) {
        * to++ = * from++; len--; w--;
    }

    len = min (len, w);
    space = w - len;

    if (code & UT_RIGHT)         left  = space;
    else if (code & UT_CENTER) { left = space / 2; right = w - left - len; }
    else                         right = space;

    if (left != 0) { utmovsc  (to, fill, left);   to += left; }
    utmovabs (to, from, len, 0);   to += len;
    if (right != 0) { utmovsc  (to, fill, right); to += right; }
    *to = EOS;
    return;
}

