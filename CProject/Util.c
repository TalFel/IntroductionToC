#include "Util.h"

void MoveBackOnePos(FILE*);

/*defined tables*/
char* commands[16] = { "mov", "cmp", "add", "sub", "lea", "not", "clr", "inc", "dec", "jmp", "bne", "red", "prn", "jsr", "rts", "stop" };
char* external[2] = { ".extern", ".entry" };
char* data[2] = { ".data", ".string" };

/*errors arrray*/
RET_STATUS* g_errs = NULL;
int g_errsLen = 0;

/*getters for the file names*/
char* GetAsName(char* arg)
{
	char* fileName = (char*)malloc(strlen(arg) + 4);
	if (fileName == NULL)
		return "";
	fileName[strlen(arg)] = '\0';
	strncat(strncpy(fileName, arg, strlen(arg)), D_ASSEMBLY_ENDING, 3);
	return fileName;
}
char* GetAmName(char* arg)
{
	char* fileName = (char*)malloc(strlen(arg) + 4);
	if (fileName == NULL)
		return "";
	fileName[strlen(arg)] = '\0';
	strncat(strncpy(fileName, arg, strlen(arg)), D_MACRO_ENDING, 3);
	return fileName;
}
char* GetObName(char* arg)
{
	char* fileName = (char*)malloc(strlen(arg) + 4);
	if (fileName == NULL)
		return "";
	fileName[strlen(arg)] = '\0';
	strncat(strncpy(fileName, arg, strlen(arg)), D_OBJECT_ENDING, 3);
	return fileName;
}
char* GetEntName(char* arg)
{
	char* fileName = (char*)malloc(strlen(arg) + 5);
	if (fileName == NULL)
		return "";
	fileName[strlen(arg)] = '\0';
	strncat(strncpy(fileName, arg, strlen(arg)), D_ENTRY_ENDING, 4);
	return fileName;
}
char* GetExtName(char* arg)
{
	char* fileName = (char*)malloc(strlen(arg) + 5);
	if (fileName == NULL)
		return "";
	fileName[strlen(arg)] = '\0';
	strncat(strncpy(fileName, arg, strlen(arg)), D_EXTERN_ENDING, 4);
	return fileName;
}


/*checks if the line contains only white spaces*/
RET_STATUS IsEmptySentence(char* line)
{
	while (*(line) != '\0')
	{
		if (isspace(*(line)) == 0)
			return FAILURE;
		line++;
	}
	return SUCCESS;
}

/*recievs a file and a decimal and recursivly prints it in binary*/
void PrintBinaryCodeToFile(FILE* target, int decimal)
{
	if (decimal >> 1)
		PrintBinaryCodeToFile(target, decimal / 2);
	fprintf(target, "%c", ((decimal % 2) ? '/' : '.'));
}

/*moves the file reader after the white spaces of the line*/
int MoveAfterWhiteSpaces(FILE* file_p)
{
	char ch;
	while ((ch = fgetc(file_p)) != EOF && isspace(ch) != 0)
	{
		if (ch == '\n')
			return 1;
	}
	if (ch == EOF)
		return 0;
	MoveBackOnePos(file_p);
	return 0;
 }

/*moves back one step in file*/
void MoveBackOnePos(FILE* file_p)
{
	fseek(file_p, -1, SEEK_CUR);
}

/*getters for the arrays*/
char* GetCommand(int ind)
{
	return commands[ind];
}
char* GetExternal(int ind)
{
	return external[ind];
}
char* GetData(int ind)
{
	return data[ind];
}

/*function that receive the number of lines and mallocs the error array*/
RET_STATUS CreateErrsArray(int size)
{
	g_errsLen = size + 2;
	g_errs = (RET_STATUS*)malloc(g_errsLen * sizeof(RET_STATUS));
	if (g_errs == NULL)
		return MEMORY_ALLOCATION_FAILURE;
	memset(g_errs, 0, g_errsLen * sizeof(RET_STATUS));
	return SUCCESS;
}

/*function that prints all of the errors from the files*/
RET_STATUS PrintAllErrors()
{
	int lineNumber = 1;
	int wasErr = 0;

	/*printing all the errors from the .am&.as files in line order*/
	lineNumber = 1;
	while (lineNumber < g_errsLen)
	{
		if (g_errs[lineNumber] != 0)
		{
			wasErr = 1;
			switch (g_errs[lineNumber])
			{
				case ILLEGAL_COMMAS:
					fprintf(stderr, "Compiler failed on line %d. Illegal commas.\n", lineNumber);
					break;
				case MISSING_BRACKET:
					fprintf(stderr, "Compiler failed on line %d. Missing a bracket.\n", lineNumber);
					break;
				case EXTRANEOUS_SPACE:
					fprintf(stderr, "Compiler failed on line %d. Extraneous spaces.\n", lineNumber);
					break;
				case MISSING_SPACE:
					fprintf(stderr, "Compiler failed on line %d. Expected a space.\n", lineNumber);
					break;
				case MEMORY_ALLOCATION_FAILURE:
					fprintf(stderr, "Failed to allocated memory.\n");
					break;
				case ILLEGAL_NUMBER:
					fprintf(stderr, "Compiler failed on line %d. Illegal Number definition.\n", lineNumber);
					break;
				case ILLEGAL_LABEL_DEFINITION:
					fprintf(stderr, "Compiler failed on line %d. Illegal definition of label.\n", lineNumber);
					break;
				case DOUBLE_LABEL_DEFINITION:
					fprintf(stderr, "Compiler failed on line %d. Double definition of label.\n", lineNumber);
					break;
				case TOO_MANY_OPERANDS:
					fprintf(stderr, "Compiler failed on line %d. Too many operands were given.\n", lineNumber);
					break;
				case TOO_LITTLE_OPERANDS:
					fprintf(stderr, "Compiler failed on line %d. Expected an operand.\n", lineNumber);
					break;
				case ILLEGAL_OPERAND:
					fprintf(stderr, "Compiler failed on line %d. Illegal operand.\n", lineNumber);
					break;
				case EXPECTED_NEW_LINE_AFTER_COMMAND:
					fprintf(stderr, "Compiler failed on line %d. Expected a '\\n' directly after command name.\n", lineNumber);
					break;
				case EXPECTED_NEW_LINE_AFTER_OPERAND:
					fprintf(stderr, "Compiler failed on line %d. Expected a '\\n' directly after operand.\n", lineNumber);
					break;
				case ILLEGAL_ADRESS_METHOD:
					fprintf(stderr, "Compiler failed on line %d. Illegal adress method for command.\n", lineNumber);
					break;
				case ILLEGAL_AFTER_DOT:
					fprintf(stderr, "Compiler failed on line %d. Expected 'string', 'data', 'entry' or 'extern' after '.'.\n", lineNumber);
					break;
				case LABEL_DOESNOT_EXIST:
					fprintf(stderr, "Compiler failed on line %d. Label does not exist.\n", lineNumber);
					break;
				case LINE_INCREASING_MAX_LENGTH:
					break;
				case ILLEGAL_COMMAND:
					fprintf(stderr, "Compiler failed on line %d. Illegal command.\n", lineNumber);
					break;
				case EXPECTED_NUMBER:
					fprintf(stderr, "Compiler failed on line %d. Expected a number.\n", lineNumber);
					break;
				case EXPECTED_QUOTATION_AFTER_DOT_STRING:
					fprintf(stderr, "Compiler failed on line %d. Expected a quotation mark directrly after .string (\").\n", lineNumber);
					break;
				case EXPECTED_QUOTATION_END_OF_LINE:
					fprintf(stderr, "Compiler failed on line %d. Expected a quotation mark in the end of line.\n", lineNumber);
					break;
				case ILLEGAL_STRING_DEF:
					fprintf(stderr, "Compiler failed on line %d. Illegal string definition.\n", lineNumber);
					break;
				case COULD_NOT_IDENTIFY_CHARACTERS:
					fprintf(stderr, "Compiler failed on line %d. Could not identify character in string.\n", lineNumber);
					break;
				case FIRST_CHARACTER_DOES_NOT_ADHERE:
					fprintf(stderr, "Compiler failed on line %d. First character does not adhere to the specified rules.\n", lineNumber);
					break;
				case SAVED_WORD_DEFINITION:
					fprintf(stderr, "Compiler failed on line %d. redefinition of saved words of the language.\n", lineNumber);
					break;
				case INCREASING_LABEL_MAX_SIZE:
					fprintf(stderr, "Compiler failed on line %d(in .as). label size increased the maximum length of %d.\n", lineNumber, D_MAX_LABEL_SIZE);
					break;
				default:
					fprintf(stderr, "Compiler failed on line %d.\n", lineNumber);
					break;
			}
		}
		lineNumber++;
	}
	free(g_errs);
	return wasErr ? FAILURE : SUCCESS;
}

/*when an error is discoverd it will be added to an error array that will be printed in the end*/
RET_STATUS AddError(RET_STATUS err, int lineNumber)
{
	if (g_errs[lineNumber] != 0)
		return FAILURE;
	g_errs[lineNumber] = err;
	if (err == SUCCESS)
		return SUCCESS;
	if (err == ILLEGAL_AFTER_DOT)
		return FATAL_FAILURE;
	return FAILURE;
}

/*checks if the string (ptr) is a saves word of the assembly language*/
RET_STATUS IsSavedWord(int lineNumber, char* ptr)
{
	int ind = 0;
	if (strlen(ptr) < 2)
		return FAILURE;
	if (strlen(ptr) < 3)
	{
		if (CheckRegister(ptr) == SUCCESS)
			return SUCCESS;
		return FAILURE;
	}
	for (; ind < D_COMMANDS_SIZE; ind++)
	{
		if (strcmp(GetCommand(ind), ptr) == 0)
		{
			return SUCCESS;
		}
	}
	for (ind = 0; ind < D_DATA_SIZE; ind++)
	{
		if (strcmp(GetData(ind), ptr) == 0 || strcmp(GetExternal(ind), ptr) == 0)
		{
			return SUCCESS;
		}
	}
	if (strcmp(ptr, "mcr") == 0)
		return SUCCESS;
	return FAILURE;
}

/*checks if the pointer to the line contains a register*/
RET_STATUS CheckRegister(char* line)
{
	if (strlen(line) < 3)
		return FAILURE;
	if (*line == 'r' && (int)(*(line + 1)) >= (int)'1' && (int)(*(line + 1)) <= (int)'7'
		&& (isspace(*(line + 2)) != 0 || *(line + 2) == ',' || *(line + 2) == ')'))
		return SUCCESS;
	return FAILURE;
}
