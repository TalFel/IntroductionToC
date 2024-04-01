#ifndef PHASE_ONE_UTIL_H
#define PHASE_ONE_UTIL_H

#include "Util.h"


RET_STATUS AddSymbol(char*, int, int);
RET_STATUS AddExternSymbol(char*, int);

int FirstGroup(char*, int, int, int, int*);
int SecondGroup(char*, int, int, int, int*);
int ThirdGroup(char* line);


#endif
