#include "PhaseOneUtil.h"
#include "PhasesUtil.h"

RET_STATUS CheckRetFirstGroup(RET_STATUS, RET_STATUS, RET_STATUS);
RET_STATUS CheckAfterOperand(char*);

/*adds an extern label to the label list*/
RET_STATUS AddExternSymbol(char* ptr, int lineNumber)
{
	char* lbl;
	ptr += M_EXTERNAL_LENGTH;
	if (isspace(*ptr) == 0)
		return MISSING_SPACE;
	lbl = GetLabelName(ptr);
	if (lbl == NULL)
		return ILLEGAL_LABEL_DEFINITION;

	return AddSymbol(lbl, EXTERNAL, 0);
}

/*adds a label to the label list*/
RET_STATUS AddSymbol(char* symbol, int mark, int value)
{
	struct label* current_p = labelHead;
	struct label* newLabel = (struct label*)malloc(sizeof(struct label));
	while (current_p->next_p != NULL)
	{
		current_p = current_p->next_p;
		if (strcmp(current_p->name, symbol) == 0)
			return DOUBLE_LABEL_DEFINITION;
	}
	if (newLabel == NULL)
		return MEMORY_ALLOCATION_FAILURE;
	current_p->next_p = newLabel;
	newLabel->next_p = NULL;
	strncpy(newLabel->name, symbol, strlen(symbol));
	(newLabel->name)[strlen(symbol)] = '\0';
	newLabel->mark = mark;
	newLabel->value = value;
	return SUCCESS;
}

/*checks the first group of commands, returns SUCCESS if the operands are valid and updates the size_p memory counter, or a specific failure code otherwise*/
int FirstGroup(char* line, int lineNumber, int commandInd, int IC, int* size_p)
{
	RET_STATUS ret1 = SUCCESS, ret2 = SUCCESS, ret3 = SUCCESS;

	/*2 operands, divided by a ','*/
	int size = 0; int L1 = 0; int L2 = 0; int isRegister = 0;
	if (*line == '\0')
		return AddError(TOO_LITTLE_OPERANDS, lineNumber);
	if (isspace(*line) != 0)
		return EXTRANEOUS_SPACE;

	while (*(line + size) != ',' && *(line + size) != '\n')
		size++;

	if (*(line + size) != ',')
		return ILLEGAL_COMMAS;

	if (*(line + size) == '\n')
		return TOO_LITTLE_OPERANDS;

	isRegister = Check3AdressGroup(line) == SUCCESS ? 1:0;

	/*1 if command is lea, or 0, 1, 3 otherwise*/
	if ((ret1 = Check1AdressGroup(line)) == SUCCESS || (strncmp(GetCommand(commandInd), "lea", 3) != 0
		&& ((ret2 = Check0AdressGroup(line, ",")) == SUCCESS || isRegister)))
	{
 		while (*line != ',' && *line!='\0')
			line++;
		L1++;
	}

	if (M_IS_SPECIFIC_FAILURE(CheckRetFirstGroup(ret1, ret2, ret3)))
		return CheckRetFirstGroup(ret1, ret2, ret3);

	if (L1 == 0)
		return ILLEGAL_OPERAND;
	
	line++;
	while (*line != '\n' && isspace(*line) != 0)
		line++;
	
	if (*line == '\n')
		return TOO_LITTLE_OPERANDS;
	
	if (isalpha(*line) == 0 && *line!='#')
		return ILLEGAL_OPERAND;

	if ((ret1 = Check1AdressGroup(line)) == SUCCESS || (ret2 = Check3AdressGroup(line)) == SUCCESS ||
		(strncmp(GetCommand(commandInd), "cmp", 3) == 0 && ((ret3 = Check0AdressGroup(line, "\n")) == SUCCESS)))
		L2++;
	
	if (M_IS_SPECIFIC_FAILURE(CheckRetFirstGroup(ret1, ret2, ret3)))
		return CheckRetFirstGroup(ret1, ret2, ret3);
	
	/*both operands are registers and will be coded in the same word*/
	if (isRegister && (ret1 = Check3AdressGroup(line)) == SUCCESS) 
		L1 = 0;
	
	if (M_IS_SPECIFIC_FAILURE(ret1))
		return ret1;
	
	if (L2 == 0)
		return ILLEGAL_OPERAND;
	
	while (isspace(*line) == 0 && *line != ',' && *line != '\n')
		line++;
	
	if (*line != '\n')
		return EXPECTED_NEW_LINE_AFTER_OPERAND;

	/*updateing the memory size value*/
	*size_p = L1 + L2 + 1;
	return SUCCESS;
}

/*util for the FirstGroup function, checks if one of the ret values are specific failure codes*/
RET_STATUS CheckRetFirstGroup(RET_STATUS ret1, RET_STATUS ret2, RET_STATUS ret3)
{
	if (M_IS_SPECIFIC_FAILURE(ret1))
		return ret1;
	if (M_IS_SPECIFIC_FAILURE(ret2))
		return ret2;
	if (M_IS_SPECIFIC_FAILURE(ret3))
		return ret3;
	return SUCCESS;
}

/*checks the second group of commands, returns SUCCESS if the operands are valid and updates the size_p memory counter, or a specific failure code otherwise*/
int SecondGroup(char* line, int lineNumber, int commandInd, int IC, int* size_p)
{
	RET_STATUS ret;
	if (M_IS_JMP_ADRESSED(commandInd)) {
		ret = Check3AdressGroup(line);
		if (ret == FAILURE)
		{
			ret = Check2AdressGroup(line, "\n");
			if (ret == SUCCESS)
			{
				while (*line != '(')
					line++;
				/*its a register*/
				if (Check3AdressGroup(++line) == SUCCESS)
				{
					while (*line != ',')
						line++;
					line++;
					while (isspace(*line) != 0)
						line++;
					/*both operands are registers and will be coded in the same word*/
					if (Check3AdressGroup(line) == SUCCESS)
					{
						*size_p = 3;
						return SUCCESS;
					}
				}
				/*there will be 4 words for the current line*/
				*size_p = 4;
				return SUCCESS;
			}
			else if (M_IS_SPECIFIC_FAILURE(ret))
				return ret;
			ret = Check1AdressGroup(line);
			/*its a label*/
			if (ret == SUCCESS)
			{
				if (CheckAfterOperand(line + strlen(GetLabelName(line))) != SUCCESS)
					return CheckAfterOperand(line + strlen(GetLabelName(line)));
				*size_p = 2;
				return SUCCESS;
			}
			return ILLEGAL_ADRESS_METHOD;
		}
		/*not a label or label jump with params*/
		else if(ret == SUCCESS)
			return ILLEGAL_ADRESS_METHOD;
		return ret;
	}
	if (Check1AdressGroup(line) == SUCCESS)
	{
		if (CheckAfterOperand(line + strlen(GetLabelName(line))) != SUCCESS)
			return CheckAfterOperand(line + strlen(GetLabelName(line)));
		*size_p = 2;
		return SUCCESS;
	}
	if (Check3AdressGroup(line) == SUCCESS)
	{
		if(CheckAfterOperand(line + 2) != SUCCESS)
			return CheckAfterOperand(line + 2);
		*size_p = 2;
		return SUCCESS;
	}
	
	if (Check0AdressGroup(line, "\n") == SUCCESS)
	{
		if (strcmp(GetCommand(commandInd), "prn") != 0)
			return ILLEGAL_ADRESS_METHOD;

		line++;
		while (isdigit(*line) != 0 || *line == '-' || *line == '+')
			line++;
		CheckAfterOperand(line);
		/*command is prn and the adress methid is instant*/
		*size_p = 2;
		return SUCCESS;
	}
	return ILLEGAL_COMMAND;
}

RET_STATUS CheckAfterOperand(char* line)
{
	if (isspace(*line) == 0 && (*line) != '\n')
		return EXPECTED_NEW_LINE_AFTER_OPERAND;
	while (*line != '\n')
	{
		if (isspace(*line) == 0)
			return TOO_MANY_OPERANDS;
		line++;
	}
	return SUCCESS;
}


/*checks the third group of commands, returns SUCCESS if the operands are valid, or a specific failure code otherwise*/
int ThirdGroup(char* line)
{
	/*NO operands*/
	if (*line != '\0')
		return EXPECTED_NEW_LINE_AFTER_COMMAND;
	return SUCCESS;
}
