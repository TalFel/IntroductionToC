#define _GNU_SOURCE
#include "Util.h"
#include "MacroUtil.h"

void PrintLineToFileWithoutSpacesAfterOperands(FILE* file, char* line);


/*fprintfs the current macro to file*/
void FprintMacro(struct macro* mcr)
{
	int currPos = mcr->begin;
	int ch;
	int pos = ftell(g_asFile);
	char line[D_MAX_LINE_SIZE] = { 0 };
	int ind = 0;
	fseek(g_asFile, mcr->begin, SEEK_SET);
	MoveAfterWhiteSpaces(g_asFile);
	while (currPos < mcr->end - 1)
	{
		ch = getc(g_asFile);
		if (isascii(ch) != 0) {
			line[ind++] = ch;

			if (ch == '\n')
			{
				line[ind] = '\0';
				ind = 0;
				PrintLineToFileWithoutSpacesAfterOperands(g_amFile, line);
				MoveAfterWhiteSpaces(g_asFile);
			}

			/*line increasing maximum length of 80*/
			if (ind == D_MAX_LINE_SIZE)
			{
				while ((ch = fgetc(g_asFile)) != EOF && ch != '\n');
				ind = 0;
			}
			currPos = ftell(g_asFile);
		}
	}
	PrintLineToFileWithoutSpacesAfterOperands(g_amFile, line);
	fseek(g_asFile, pos, SEEK_SET);
}

void PrintLineToFileWithoutSpacesAfterOperands(FILE* file, char* line)
{
	int pos;
	for (pos = strlen(line) - 1; pos >= 0; pos--)
	{
		if (isspace(line[pos]) == 0)
			break;
	}
	line[++pos] = '\n';
	line[++pos] = '\0';
	fprintf(file, "%s", line);
}

/*adds a new macro to the macro list*/
RET_STATUS NewMacro(char* line, struct macro* head_p, int beg, int lineNumber)
{
	struct macro* currMcr = head_p;
	struct macro* newMacro = (struct macro*)malloc(sizeof(struct macro));
	while (currMcr->next_p != NULL)
		currMcr = currMcr->next_p;
	if (newMacro == NULL)
		return MEMORY_ALLOCATION_FAILURE;
	currMcr->next_p = newMacro;
	newMacro->name = GetMacroName(line, lineNumber);
	newMacro->next_p = NULL;
	newMacro->begin = beg;
	if (strcmp(newMacro->name, "") == 0)
		return ILLEGAL_MACRO_DEF; /*macro definition is illegal*/
	return SUCCESS;
}

/*checks if there is a macro definition, mention or definition ending in the current line*/
enum E_MACRO_IN_LINE CheckMacro(char* line, struct macro* head_p)
{
	char* newLine = line;

	if (strncmp(newLine, D_MACRO_DEF_END, strlen(D_MACRO_DEF_END)) == 0)
		return MACRO_END;

	if (strncmp(newLine, D_MACRO_DEF_BEG, strlen(D_MACRO_DEF_BEG)) == 0)
		return MACRO_BEGIN;

	if (GetMacro(line, head_p) != head_p)
		return MACRO_REF;

	return REGULAR_LINE;
}

/*checks a macro exists in the current line, returns the macro if there is or returns the head pointer otherwise*/
struct macro* GetMacro(char* line, struct macro* head_p)
{
	struct macro* curr = head_p;
	while (curr->next_p != NULL)
	{
		curr = curr->next_p;
		if (curr->name != NULL)
		{
			if (strstr(line, curr->name) != NULL)
				return curr;
		}
	}
	return head_p;
}

/*gets the macro name from the code line*/
char* GetMacroName(char* str, int lineNumber)
{
	char* ptr = (char*)malloc(strlen(str) + 1);
	int size = 0;
	if (ptr == NULL)
		return NULL;
	strcpy(ptr, str);
	ptr += 4;

	/*extraneous spaces after macro def*/
	if (isalpha(*ptr) == 0)
	{
		fprintf(stderr, "Compiler failed on line %d  (in .as). Illegal macro definition.\n", lineNumber);
		return "";
	}
	while (isalpha(*(ptr + size)) || isdigit(*(ptr + size)))
		size++;
	ptr[size] = '\0';
	if (IsSavedWord(lineNumber, ptr) == SUCCESS)
	{
		fprintf(stderr, "Compiler failed on line %d. redefinition of saved words of the language.\n", lineNumber);
		return "";
	}
	return ptr;
}

/*frees the macro list in the end of phase two*/
void FreeList(struct macro* head_p)
{
	struct macro* curr_p = head_p;
	struct macro* next_p;
	if (head_p == NULL)
		return;

	while (curr_p->next_p != NULL)
	{
		next_p = curr_p->next_p;
		free(curr_p);
		curr_p = next_p;
	}
	free(curr_p);
}

