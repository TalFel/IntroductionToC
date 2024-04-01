#include <stdio.h>
#include "myheader.h"
int my_bcmp2 (const void *b1, const void *b2, int len){
    char *c1 = (char *)b1;
    char *c2 = (char *)b2;
    for(; len>0; len--){
        if(*c1 != *c2){
            return 1;
        }
        c1++;
        c2++;
    }
    return 0;
}
