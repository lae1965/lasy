#include <slutil.h>

int utstrind (char check, char *psearch) {
    char *p;

    if ((p = strchr (psearch, check)) == NULL)  return (-1);
    return ((int) (p - psearch));
}
