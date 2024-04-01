#define _GNU_SOURCE
#include "PreAssembler.h"
#include "MacroUtil.h"

int HandleLinePreAssembly(char*, struct macro*, int, int);
int GetNoOfLines(FILE*);

extern RET_STATUS CreateErrsArray(int);

/*the main for phase two*/
int PreAssemblerMain()
{
	char line[D_MAX_LINE_SIZE + 2] = {'\0'};
	char ch; int ind = 0; int mcrFlag = 0; int lineNumber = 1;
	/*init macro list*/
	struct macro* head_p = (struct macro*)malloc(sizeof(struct macro));
	if (head_p == NULL)
		return FAILURE;
	
	head_p->next_p = NULL;
	head_p->name = NULL;
	/*read from source file to new am file*/
	lineNumber += MoveAfterWhiteSpaces(g_asFile);
	while((ch = fgetc(g_asFile)) != EOF){/*inserts input*/

		if (isascii(ch) == 0)
			AddError(UNIDENTIFIED_CHAR, lineNumber);
		else
			line[ind++] = ch;

		/*end of line*/
		if (ch == '\n')
		{
			line[ind] = '\0';
			if ((mcrFlag = HandleLinePreAssembly(line, head_p, mcrFlag, lineNumber)) == FLAG_FAILURE)
				return FAILURE; /*returning with err code*/
			ind = 0;
			lineNumber++;
			lineNumber += MoveAfterWhiteSpaces(g_asFile);
		}
		/*line increasing maximum length of 80*/
		if (ind >= D_MAX_LINE_SIZE)
		{
			fprintf(stderr, "Error, line %d (in .as) exceeded the maximum length of %d.\n", lineNumber, D_MAX_LINE_SIZE);
			while ((ch = fgetc(g_asFile)) != EOF && ch != '\n');
			ind = 0;
			lineNumber++;
		}
	}
	line[ind] = '\0';
	if ((mcrFlag = HandleLinePreAssembly(line, head_p, mcrFlag, lineNumber)) == FLAG_FAILURE)
		return FAILURE; /*returning with err code*/
	FreeList(head_p); /*frees macro list*/
	
	CreateErrsArray(GetNoOfLines(g_amFile));
		
	return SUCCESS;
}

/*gets a new line from the .as file and acts accordingly*/
int HandleLinePreAssembly(char* line, struct macro* head_p, int mcrFlag, int lineNumber)
{
	struct macro* curr; int pos = 0;
	pos = ftell(g_asFile);
	switch (CheckMacro(line, head_p)) 
	{
		case MACRO_BEGIN:
			/*macro begin*/
			pos = ftell(g_asFile);
			if (NewMacro(line, head_p, pos, lineNumber) != SUCCESS)
				return FLAG_FAILURE;
			return FLAG_UP;
		case MACRO_END:
			/*macro end*/
			pos = ftell(g_asFile);
			curr = head_p;
			while (curr->next_p != NULL) curr = curr->next_p;
			curr->end = pos - strlen(line);
			return FLAG_DOWN;
		case MACRO_REF:
			/*there is a macro ref in this line*/
			FprintMacro(GetMacro(line, head_p));
			return FLAG_DOWN;
		case REGULAR_LINE:
			/*regular line*/
			if (!mcrFlag)
			{
				/*printing the line without spaces at the end*/
				for (pos = strlen(line) - 1; pos >= 0; pos--)
				{
					if (isspace(line[pos]) == 0)
						break;
				}
				line[++pos] = '\n';
				line[++pos] = '\0';
				fprintf(g_amFile, "%s", line);
			}
			return mcrFlag;
	}
	return FLAG_DOWN;
}

/*function to get the number of lines of a file*/
int GetNoOfLines(FILE* file)
{
	char ch;
	int size = 0;
	fseek(file, 0, SEEK_SET);
	while ((ch = fgetc(file)) != EOF)
	{
		if (ch == '\n')
			size++;
	}
	fseek(file, 0, SEEK_SET);
	return size;
}
