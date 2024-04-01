#ifndef MACRO_UTIL_H
#define MACRO_UTIL_H

#include <string.h>
#include <stdio.h>
#include "Util.h"

/*the struct that contains the macro linked list*/
struct macro
{
	char* name;
	int begin;
	int end;
	struct macro* next_p;
};

/*enums*/
enum E_MACRO_IN_LINE
{
	MACRO_REF = 0,
	REGULAR_LINE,
	MACRO_BEGIN,
	MACRO_END
};

enum E_FLAG_STATE
{
	FLAG_DOWN = 0,
	FLAG_UP,
	FLAG_FAILURE
};


/*headers*/
enum E_MACRO_IN_LINE CheckMacro(char*, struct macro*);
void FreeList(struct macro*);
RET_STATUS NewMacro(char*, struct macro*, int, int);
struct macro* GetMacro(char*, struct macro*);
void FprintMacro(struct macro*);
char* GetMacroName(char*, int);

#endif
