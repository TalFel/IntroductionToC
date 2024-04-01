#include "PhasesUtil.h"

/*gets a string ptr and returns the index of the command in the string or -1 otherwise*/
int GetCommandInd(char* ptr){
	int arrayInd;
	int commandInd = -1;
	
	for (arrayInd = 0; arrayInd < D_COMMANDS_SIZE; arrayInd++)
	{
		if (strncmp(GetCommand(arrayInd), ptr, strlen(GetCommand(arrayInd))) == 0)
			commandInd = arrayInd;
	}

	return commandInd;
}

/*checks if an operand fits to the instat adress group, return SUCCESS if does, FAILURE if it doesn't, or a specific FAILURE if it does but there's an error*/
RET_STATUS Check0AdressGroup(char* line, char* end)
{
	char* numberStr = (char*)malloc(strlen(line));
	char* tempLine = (char*)malloc(strlen(line));
	int number;

	if (numberStr == NULL || tempLine == NULL)
		return MEMORY_ALLOCATION_FAILURE;
	if (*line != '#')
		return FAILURE;
	
	line++;
	strcpy(numberStr, line);
	tempLine = strtok(numberStr, end);
	number = atoi(tempLine);
	
	if (number == 0 && *numberStr != '0')
		return ILLEGAL_NUMBER;
	
	return SUCCESS;
}

/*checks if an operand fits to the direct adress group, return SUCCESS if does, FAILURE if it doesn't*/
RET_STATUS Check1AdressGroup(char* line)
{
	if (GetLabelName(line) == NULL)
		return FAILURE;
	return SUCCESS;
}

/*checks if an operand fits to the jump adress group, return SUCCESS if does, FAILURE if it doesn't, or a specific FAILURE if it does but there's an error*/
RET_STATUS Check2AdressGroup(char* line, char* end)
{
	int commaCount = 0;

	/*checks label*/
	while (*line != *end && *line != '(' && *line!='\0')
	{
		if (isalpha(*line) == 0 && isdigit(*line) == 0)
			return FAILURE;
		line++;
	}
	/*checks (*/
	if (*line != '(')
		return FAILURE;
	if (*(line + 1) == ',')
		return ILLEGAL_OPERAND;
	
	while (*line != ')' && *line != '\0' && isspace(*line) == 0)
	{
		line++;
		if (*line == ',')
		{
			if (*(line+1)==')')
				return ILLEGAL_OPERAND;
			commaCount++;
		}
	}

	if (*line == '\n')
		return MISSING_BRACKET;
	if (isspace(*line) != 0)
		return EXTRANEOUS_SPACE;
	if (*line != ')')
		return MISSING_BRACKET;
	if (commaCount != 1)
		return ILLEGAL_COMMAS;

	return SUCCESS;
}

/*checks if an operand fits to the instat adress group, return SUCCESS if does, FAILURE if it doesn't*/
RET_STATUS Check3AdressGroup(char* line)
{
	return CheckRegister(line);
}

/*checks if there is a label definition in the beginning of the line. returns the size of the label or 0 otherwise*/
int CheckSymbol(char* line)
{
	char* ptr = line;
	int size = 0;

	/*checks if the first field is a label (: after a name that doesnt contain any spaces)*/
	while (*(ptr + size) != ':' && isspace(*(ptr + size)) == 0 && size <= (int)strlen(line))
		size++;

	if (*(ptr + size) == ':')
		return size;

	return 0;
}

/*checks if line contains a data instruction in the beginning*/
RET_STATUS DataLabel(char* line)
{
	int dataInd;

	for (dataInd = 0; dataInd < D_DATA_SIZE; dataInd++)
	{
		if (strncmp(line, GetData(dataInd), strlen(GetData(dataInd))) == 0)
			return SUCCESS;
	}
	return FAILURE;
}

/*checks if the line contains a .extern instruction in the beginning*/
RET_STATUS IsDotExternCommand(char* ptr)
{
	return (strncmp(ptr, GetExternal(D_EXTERN_INDEX), M_EXTERNAL_LENGTH) == 0) ? SUCCESS : FAILURE;
}

/*checks if the line contains a .extern or a .entry instruction in the beginning and an operand after it*/
RET_STATUS IsExternalCommand(char* ptr, int lineNumber)
{
	int ind;

	if (*ptr != '.')
		return FAILURE;
	
	for (ind = 0; ind < D_EXTERNAL_SIZE; ind++)
	{
		if (strncmp(ptr, GetExternal(ind), strlen(GetExternal(ind))) == 0)
			break;
	}
	
	if(ind == D_EXTERNAL_SIZE)
		return ILLEGAL_AFTER_DOT;

	ptr += strlen(GetExternal(ind));
	if (isspace(*ptr) == 0)
		return MISSING_SPACE;
	
	ptr++;
	ind = 0;

	while (*ptr != '\n' && *ptr != '\0')
	{
		if (isspace(*ptr) != 0)
			return EXTRANEOUS_SPACE;
		else if (isalpha(*ptr) != 0 || isdigit(*ptr) != 0)
			ind++;
		else
			return ILLEGAL_OPERAND;
		ptr++;
	}
	
	if (ind == 0)
		return TOO_LITTLE_OPERANDS;
	
	return SUCCESS;
}

/*returns the label name in ptr. cuts it on the first non alpha/non digit character*/
char* GetLabelName(char* ptr)
{
	char* tempPtr;
	int ind = 0;
	char* lbl;
	ind = 0;

	while (isspace(*ptr) != 0)
		ptr++;
	
	/*label starts with a number*/
	if (isalpha(*ptr) == 0)
		return NULL;
	
	tempPtr = ptr;
	while (isalpha(*(tempPtr + ind)) != 0 || isdigit(*(tempPtr + ind)) != 0)
		ind++;

	lbl = (char*)malloc(ind + 2);
	if (lbl == NULL)
	{
		AddError(MEMORY_ALLOCATION_FAILURE, -1);
		return NULL;
	}
	
	strncpy(lbl, ptr, ind);
	lbl[ind] = '\0';
	
	return lbl;
}
