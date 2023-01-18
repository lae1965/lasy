#ifndef DEF_SLLDRAW
#define DEF_SLLDRAW     1

typedef union {
    struct {
        unsigned    lt_s    :1;
        unsigned    lt_d    :1;
        unsigned    rt_s    :1;
        unsigned    rt_d    :1;
        unsigned    up_s    :1;
        unsigned    up_d    :1;
        unsigned    dn_s    :1;
        unsigned    dn_d    :1;
        unsigned    reserv  :8;
    } b;
    int x;
} SYM_LD;

int     ldgetcod (char cld);
char    ldgetchr (int x, int single);

#endif
