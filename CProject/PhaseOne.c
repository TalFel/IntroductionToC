#include "PhaseOne.h"
#include "PhasesUtil.h"

/*headers*/
int HandleLinePhase1(char*, int);
RET_STATUS AnalyzeCommandPhaseOne(char*, int, int*);
int AnalyzeDataLabel(char*, int);
void AddDataToArray(int, int);
int HandleString(char*, int);
int HandleData(char*, int);

/*externs*/
extern int FirstGroup(char*, int, int, int, int*);
extern int SecondGroup(char*, int, int, int, int*);
extern int ThirdGroup(char*);
extern RET_STATUS AddSymbol(char*, int, int);
extern RET_STATUS AddExternSymbol(char*, int);

/*globals*/
int g_IC, g_DC;

/*main function for phase one*/
RET_STATUS PhaseOneMain()
{
	char line[D_MAX_LINE_SIZE + 1] = { '\0' };
	char ch;
	int ind = 0;
	int compiled = SUCCESS;
	int lineNumber = 1;
	struct label* tempPtr;
	
	/*init lists*/
	labelHead = (struct label*)malloc(sizeof(struct label));
	dataArrayHead = (struct dataArray*)malloc(sizeof(struct dataArray));
	if (labelHead == NULL || dataArrayHead == NULL)
		return AddError(MEMORY_ALLOCATION_FAILURE, lineNumber);
	labelHead->next_p = NULL;
	dataArrayHead->next = NULL;

	/*init ic and dc*/
	g_IC = 0; g_DC = 0;

	lineNumber += MoveAfterWhiteSpaces(g_amFile);
	while ((ch = fgetc(g_amFile)) != EOF)
	{
		line[ind++] = ch;
		if (ch == '\n')
		{
			line[ind] = '\0';
			compiled = (HandleLinePhase1(line, lineNumber) == SUCCESS) ? compiled : FAILURE;
			ind = 0;
			/*moves the file reader after the white spaces, if there were empty lines the line counter will be updated*/
			while (MoveAfterWhiteSpaces(g_amFile) == 1)
				lineNumber++;
			lineNumber++;
		}
		if (ind == D_MAX_LINE_SIZE)
		{
			ch = fgetc(g_amFile);
			while (ch != EOF && ch != '\n')
				ch = fgetc(g_amFile);
			ind = 0;
			lineNumber += MoveAfterWhiteSpaces(g_amFile) + 1;
		}
	}
	line[ind++] = '\n';
	line[ind] = '\0';
	compiled = (HandleLinePhase1(line, lineNumber) == SUCCESS) ? compiled : FAILURE;
	/*goes through labels and if mark=DATA add current IC counter to value*/
	tempPtr = labelHead;
	while (tempPtr != NULL)
	{
		if(tempPtr->mark == DATA)
			tempPtr->value += g_IC;
		tempPtr = tempPtr->next_p;
	}
	fprintf(g_obFile, "%d %d\n", g_IC, g_DC);
	return compiled;
}

/*handle line funciton for phase one, receives a line and analyzes it according to the algorithm*/
int HandleLinePhase1(char* line, int lineNumber)
{
	int symbolSize;
	int issymbol;
	char* symbol;
	int L;
	int temp;
	char* ptr;
	struct label* current_p = labelHead->next_p;
	issymbol = 0;
	symbolSize = CheckSymbol(line);
	symbol = (char*)malloc(symbolSize + 1);
	/*comment line*/
	if (line[0] == ';')
		return SUCCESS;
	/*empty sentence*/
	if (IsEmptySentence(line) == SUCCESS)
		return SUCCESS;
	/*first char is not a letter or a dot*/
	if (isalpha(line[0]) == 0 && line[0] != '.')
		return AddError(FIRST_CHARACTER_DOES_NOT_ADHERE, lineNumber);
	
	/*checks for a symbol*/
	if (symbolSize != 0)
	{
		issymbol = 1;
		/*gets symbol name*/
		if (symbol == NULL)
			return AddError(MEMORY_ALLOCATION_FAILURE, lineNumber);
		if(symbolSize > D_MAX_LABEL_SIZE)
			return AddError(INCREASING_LABEL_MAX_SIZE, lineNumber);
		symbol[symbolSize] = '\0';
		strncpy(symbol, line, symbolSize);

		/*checks if the label name is a saved word*/
		if (IsSavedWord(lineNumber, symbol) == SUCCESS)
			return AddError(SAVED_WORD_DEFINITION, lineNumber);

		/*not a space after label*/
		if (isspace(line[symbolSize + 1]) == 0)
			return AddError(MISSING_SPACE, lineNumber);
		/*reached end of the line after label def*/
		if (isspace(line[symbolSize + 2]) == '\n' || line[symbolSize + 2] == '\0')
			return TOO_LITTLE_OPERANDS;
		/*extra spaces after label*/
		if (isspace(line[symbolSize + 2]) != 0)
			return AddError(EXTRANEOUS_SPACE, lineNumber);
		
		while (current_p != NULL)
		{
			if (strcmp(current_p->name, symbol) == 0)
				return AddError(DOUBLE_LABEL_DEFINITION, lineNumber);
			current_p = current_p->next_p;
		}
	}
	
	/*data label*/
	if (DataLabel(line + M_POS_AFTER_LABEL(issymbol, symbolSize)) == SUCCESS)
	{
		if (issymbol == 1)
		{
			if (AddSymbol(symbol, DATA, g_DC) == FAILURE)
				AddError(DOUBLE_LABEL_DEFINITION, lineNumber);
		}
		/*add code to memory and update DC counter*/
		temp = AnalyzeDataLabel(line + M_POS_AFTER_LABEL(issymbol, symbolSize), lineNumber);
		if (temp < 0)
			return AddError(temp, lineNumber);
		g_DC += temp;
		return SUCCESS;
	}

	/*external label*/
	ptr = line + M_POS_AFTER_LABEL(issymbol, symbolSize);
	if (IsExternalCommand(ptr, lineNumber) == SUCCESS)
	{
		if (IsDotExternCommand(ptr) == SUCCESS)/*.extern*/
		{
			if (AddExternSymbol(ptr, lineNumber) == FAILURE)
				return FAILURE;
		}
		else/*.entry*/
		{
			/*checks if defined  as entry and extern*/
			struct label* current_p = labelHead;
			while (current_p->next_p != NULL)
			{
				current_p = current_p->next_p;
				if (strcmp(current_p->name, ptr + M_ENTRY_LENGTH) == 0 && current_p->mark == EXTERNAL)
					return DOUBLE_LABEL_DEFINITION;
			}
		}
		return SUCCESS;
	}
	else if (M_IS_SPECIFIC_FAILURE(IsExternalCommand(ptr, lineNumber)))
		return AddError(IsExternalCommand(ptr, lineNumber), lineNumber);

	/*add regular label to label list*/
	if (issymbol == 1)
	{
		if (AddSymbol(symbol, CODE, g_IC) != SUCCESS)
			return AddError(DOUBLE_LABEL_DEFINITION, lineNumber);
	}

	/*check for command name*/
	if (AnalyzeCommandPhaseOne(ptr, lineNumber, &L) != SUCCESS)
		return FAILURE;
	g_IC = g_IC + L;
	return SUCCESS;
}

/*receives a line and analyzes it, returns the number of memory lines that will be coded in phase two*/
RET_STATUS AnalyzeCommandPhaseOne(char* ptr, int lineNumber, int* L)
{
	RET_STATUS ret = 0;
	int memSize = 0;
	int commandInd = GetCommandInd(ptr);
	if (commandInd == -1)
		return AddError(ILLEGAL_COMMAND, lineNumber);
	if (isspace(*(ptr + M_COMMAND_LENGTH(commandInd))) == 0)
		return AddError(MISSING_SPACE, lineNumber);
	/*check first group of commands*/
	if (commandInd <= D_FIRST_GROUP)
	{
		ret = FirstGroup(ptr + M_COMMAND_LENGTH(commandInd) + 1, lineNumber, commandInd, g_IC, &memSize);
		if (ret == SUCCESS)
		{
			*L = memSize;
			return SUCCESS;
		}
		return AddError(ret, lineNumber);
	}
	/*check second group of commands*/
	if (commandInd > D_FIRST_GROUP && commandInd <= D_SECOND_GROUP)
	{
		ret = SecondGroup(ptr + M_COMMAND_LENGTH(commandInd) + 1, lineNumber, commandInd, g_IC, &memSize);
		if (ret == SUCCESS)
		{
			*L = memSize;
			return SUCCESS;
		}
		return AddError(ret, lineNumber);
	}
	/*check third group of commands*/
	ret = ThirdGroup(ptr + M_COMMAND_LENGTH(commandInd) + 1);
	if (ret == SUCCESS)
	{
		*L = 1;
		return SUCCESS;
	}
	return AddError(ret, lineNumber);
}

/*receives a line with a data instruction and analyzes it */
int AnalyzeDataLabel(char* line, int lineNumber)
{
	/*.string*/
	if (strncmp(line, GetData(D_STRING_INDEX), strlen(GetData(D_STRING_INDEX))) == 0)
		return HandleString(line, lineNumber);

	/*.data*/
	return HandleData(line, lineNumber);
}

/*receives a line with .data instruction and adds to the data array the needed numbers*/
int HandleData(char* line, int lineNumber)
{
	int count = 0;
	int ind = 0;
	int numBegin = 0;
	char* temp = (char*)malloc(D_MAX_LINE_SIZE);
	int num;
	if (temp == NULL)
		return MEMORY_ALLOCATION_FAILURE;
	line += strlen(GetData(D_DATA_INDEX));
	if (isspace(*line) == 0)
		return MISSING_SPACE;
	
	while (isspace(*(line + ind)) != 0)
		ind++;
	numBegin = ind;

	while (ind < (int)strlen(line))
	{
		if (isdigit(*(line + ind)) == 0 && *(line + ind) != '-' && *(line + ind) != '+')
			return AddError(EXPECTED_NUMBER, lineNumber);
		while (isdigit(*(line + ind)) != 0 || *(line + ind) == '-' || *(line + ind) == '+')
			ind++;
		strncpy(temp, (line + numBegin), ind - numBegin);
		temp[ind - numBegin] = '\0';
		num = atoi(temp);
		if (num == 0 && temp[0] != '0')
			return ILLEGAL_NUMBER;
		/*add atoi code to memory*/
		AddDataToArray(g_DC + count, num);

		while (isspace(*(line + ind)) != 0)
			ind++;
		if (*(line + ind) == '\n' || *(line + ind) == '\0')
		{
			count++;
			break;
		}

		if (*(line + ind) != ',' && ind < (int)strlen(line) - 1)
			return AddError(ILLEGAL_COMMAS, lineNumber);
		
		ind++;
		if (ind < (int)strlen(line) - 1)
		{
			while (isspace(*(line + ind)) != 0)
				ind++;
		}
		
		if (*(line + ind) == '\n' || *(line + ind) == '\0')
		{
			return AddError(EXPECTED_NUMBER, lineNumber);
		}

		numBegin = ind;
		count++;
	}
	
	if (count == 0)
		return AddError(TOO_LITTLE_OPERANDS, lineNumber);
	return count;
}

/*receives a line with .string instruction and adds to the data array the needed characters*/
int HandleString(char* line, int lineNumber)
{
	int count = 0;
	line += strlen(GetData(D_STRING_INDEX));
	/*edge cases*/
	if (isspace(*line) == 0)
		return AddError(MISSING_SPACE, lineNumber);
	line++;
	if (*line != '\"')
		return AddError(EXPECTED_QUOTATION_AFTER_DOT_STRING, lineNumber);

	if (*(line + 1) == '\"' && *(line + 2) != '\n')
		return AddError(ILLEGAL_STRING_DEF, lineNumber);
	if (*(line + strlen(line) - D_EOL_SIZE) != '\"' || (line + strlen(line) - D_EOL_SIZE) == line)
		return AddError(EXPECTED_QUOTATION_END_OF_LINE, lineNumber);
	/*adding characters to data array and returning count*/
	line++;
	while (*(line + count) != '\"')
	{
		if (isprint(*(line + count)) == 0)
			return AddError(COULD_NOT_IDENTIFY_CHARACTERS, lineNumber);
		/*add character to memory*/
		AddDataToArray(g_DC + count, (int)*(line + count));
		count++;
	}
	if (*(line + count + D_EOL_SIZE) != '\0')
		return AddError(ILLEGAL_STRING_DEF, lineNumber);
	AddDataToArray(g_DC + count, 0);
	return ++count;
}

/*add to data array function. receives a number(int) and adds it to the list*/
void AddDataToArray(int dc, int num)
{
	struct dataArray* dataPtr = dataArrayHead;
	struct dataArray* current_p;
	while (dataPtr->next != NULL)
		dataPtr = dataPtr->next;
	dataPtr->next = (struct dataArray*)malloc(sizeof(struct dataArray));
	if (dataPtr->next == NULL)
		return;
	current_p = dataPtr->next;
	current_p->next = NULL;
	current_p->dc = dc;
	current_p->data = num;
}

/*a function that frees all of the mallocs from phase one*/
void FreePhaseOneHead()
{
	struct dataArray* currData_p = dataArrayHead;
	struct dataArray* nextData_p;
	struct label* currLbl_p = labelHead;
	struct label* nextLbl_p;
	if (dataArrayHead == NULL)
		return;
	if (labelHead == NULL)
		return;
	while (currLbl_p->next_p != NULL)
	{
		nextLbl_p = currLbl_p->next_p;
		free(currLbl_p);
		currLbl_p = nextLbl_p;
	}
	while (currData_p->next != NULL)
	{
		nextData_p = currData_p->next;
		free(currData_p);
		currData_p = nextData_p;
	}

	free(currData_p);
	free(currLbl_p);
}
