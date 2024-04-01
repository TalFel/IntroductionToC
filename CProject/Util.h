#ifndef UTIL_H
#define UTIL_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/*defines*/
#define D_MAX_LINE_SIZE 80
#define D_NO_OF_LINES 256
#define D_MAX_LABEL_SIZE 20

#define D_MACRO_DEF_BEG "mcr"
#define D_MACRO_DEF_END "endmcr"

#define D_ASSEMBLY_ENDING ".as"
#define D_MACRO_ENDING ".am"
#define D_OBJECT_ENDING ".ob"
#define D_ENTRY_ENDING ".ent"
#define D_EXTERN_ENDING ".ext"

#define D_COMMANDS_SIZE 16
#define D_EXTERNAL_SIZE 2
#define D_DATA_SIZE 2

#define D_DATA_INDEX 0
#define D_STRING_INDEX 1

#define D_EXTERN_INDEX 0
#define D_ENTRY_INDEX 1

#define D_JMP_INDEX 9
#define D_BNE_INDEX 10
#define D_JSR_INDEX 13

#define D_ASCII_MARGIN 48
#define D_WORD_LENGTH 14

#define D_MEMORY_MARGIN 100

/*macros*/
#define M_ARRAY_LENGTH(arr) (sizeof(arr)/sizeof(arr[0]))
#define M_IS_JMP_ADRESSED(commandInd)  (commandInd == D_JMP_INDEX || commandInd == D_BNE_INDEX || commandInd == D_JSR_INDEX)
#define M_IS_SPECIFIC_FAILURE(retVal) (retVal != SUCCESS && retVal != FAILURE)


/*enums*/
typedef enum E_RET_STATUS
{
	SUCCESS = 0,
	FAILURE,
	FATAL_FAILURE,
	ILLEGAL_COMMAS,
	MISSING_BRACKET,
	EXTRANEOUS_SPACE,
	MISSING_SPACE,
	MEMORY_ALLOCATION_FAILURE,
	ILLEGAL_NUMBER,
	ILLEGAL_LABEL_DEFINITION,
	DOUBLE_LABEL_DEFINITION,
	TOO_MANY_OPERANDS,
	TOO_LITTLE_OPERANDS,
	ILLEGAL_OPERAND,
	EXPECTED_NEW_LINE_AFTER_COMMAND,
	EXPECTED_NEW_LINE_AFTER_OPERAND,
	ILLEGAL_ADRESS_METHOD,
	ILLEGAL_AFTER_DOT,
	LABEL_DOESNOT_EXIST,
	LINE_INCREASING_MAX_LENGTH,
	ILLEGAL_MACRO_DEF,
	ILLEGAL_COMMAND,
	EXPECTED_NUMBER,
	EXPECTED_QUOTATION_AFTER_DOT_STRING,
	EXPECTED_QUOTATION_END_OF_LINE,
	COULD_NOT_IDENTIFY_CHARACTERS,
	ILLEGAL_STRING_DEF,
	FIRST_CHARACTER_DOES_NOT_ADHERE,
	SAVED_WORD_DEFINITION,
	UNIDENTIFIED_CHAR,
	INCREASING_LABEL_MAX_SIZE
}RET_STATUS;

/*files pointers*/
FILE* g_asFile;
FILE* g_amFile;
FILE* g_obFile;
FILE* g_entFile;
FILE* g_extFile;

/*headers*/
char* GetAsName(char*);
char* GetAmName(char*);
char* GetObName(char*);
char* GetEntName(char*);
char* GetExtName(char*);

RET_STATUS IsEmptySentence(char*);

char* GetCommand(int);
char* GetData(int);
char* GetExternal(int);

void PrintBinaryCodeToFile(FILE*, int);

RET_STATUS AddError(RET_STATUS, int);

int MoveAfterWhiteSpaces(FILE*);

RET_STATUS IsSavedWord(int, char*);

RET_STATUS CheckRegister(char*);

RET_STATUS CreateErrsArray(int);

#endif
