#ifndef PHASE_TWO_H
#define PHASE_TWO_H

#include "Util.h"

#define M_PRINT_AND_ADV_IC_COUNT(file) fprintf(file, "0%d ", (g_IC++ + D_MEMORY_MARGIN))
#define M_REGISTER_NUMBER(line) ((int)(*(++line)) - D_ASCII_MARGIN)
#define M_IS_LABEL_AND_EXTERNAL(adress, line, lineNumber) (adress == DIRECT && GetLabelMark(line, lineNumber) == EXTERNAL)


#define D_SUFFIX_REGISTER_BLANK "........\n"
#define D_PREFIX_REGISTER_BLANK "......"
#define D_EMPTY_ARE "..\n"
#define D_EMPTY_ADRESS_BITS "...."

#define D_ABSOLUTE_ARE ".."
#define D_EXTERNAL_ARE "./"
#define D_RELOCATABLE_ARE "/."

#define D_2ND_ADRESS_METHOD_INSTANT ".."
#define D_2ND_ADRESS_METHOD_REGISTER "//"
#define D_2ND_ADRESS_METHOD_DIRECT "./"


typedef enum E_ADRESS_METHOD
{
	INSTANT = 0,
	DIRECT,
	JUMP,
	REGISTER,
	NO_ADRESS
}ADRESS_METHOD;

typedef enum E_ARE
{
	ABSOLUTE_ARE = 0,
	RELOCATEABLE_ARE,
	EXTERNAL_ARE
}ARE_BITS;

RET_STATUS PhaseTwoMain();
int GetWasExt();
int GetWasEnt();

#endif
