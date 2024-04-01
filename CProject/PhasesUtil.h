#ifndef PHASES_UTIL_H
#define PHASES_UTIL_H
#include <stdint.h>

#include "Util.h"

#define D_FIRST_GROUP 4
#define D_SECOND_GROUP 13
#define D_EOL_SIZE 2
#define D_EXTERN_SUFFIX (".extern")
#define D_ENTRY_SUFFIX (".entry")
#define D_MAX_COMMAND_ARRAY_WORD_SIZE 30
#define D_MAX_COMMAND_ARRAY_LENGTH 256


/*macros*/
#define M_POS_AFTER_LABEL(isSimbol, simbolSize) (isSimbol ? (2 + simbolSize) : 0)
#define M_COMMAND_LENGTH(commandInd) (strlen(GetCommand(commandInd)))
#define M_EXTERNAL_LENGTH strlen(D_EXTERN_SUFFIX)
#define M_ENTRY_LENGTH strlen(D_ENTRY_SUFFIX)

#define setBit(map, _bit) (map=(1 << _bit) | map)
#define getBit(map, _bit) ((map >> _bit) & 1)



/*enum for label mark*/
enum E_LABEL_MARK
{
   NO_MARK = -1,
   DATA,
   EXTERNAL,
   CODE
};
/*structs*/
struct label
{
   char name[D_MAX_LINE_SIZE];
   enum E_LABEL_MARK mark;
   int value;
   struct label* next_p;
}*labelHead;

struct dataArray
{
   int dc;
   int data;
   struct dataArray* next;
}*dataArrayHead;

int GetCommandInd(char* ptr);

RET_STATUS Check0AdressGroup(char*, char*);
RET_STATUS Check1AdressGroup(char*);
RET_STATUS Check2AdressGroup(char*, char*);
RET_STATUS Check3AdressGroup(char*);

int CheckSymbol(char*);

RET_STATUS IsDotExternCommand(char*);
RET_STATUS DataLabel(char*);
RET_STATUS IsExternalCommand(char*, int);

char* GetLabelName(char*);

#endif
