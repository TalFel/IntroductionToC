#include "PhaseTwo.h"
#include "PhasesUtil.h"
#include "Util.h"

RET_STATUS HandleLinePhase2(char*, int);
RET_STATUS AnalyzeCommandPhaseTwo(char*, int);
void AddDataToFile();
struct label* GetLabel(char*, int);
void PrintCode(int, int);
void Print2ndAdressMethod(ADRESS_METHOD);
void EncodeRegister(char*);
void EncodeInstant(char*);
RET_STATUS EncodeDirect(char*, int);
void EncodeFirstWord(int, ADRESS_METHOD, ADRESS_METHOD, ADRESS_METHOD, ADRESS_METHOD, ARE_BITS);
enum E_LABEL_MARK GetLabelMark(char*, int);
RET_STATUS PrintExternalToFile(char*);
void HandleRegisterRegister(char*, int);
RET_STATUS HandleRegisterDirect(char*, int, char*, int);
void HandleRegisterInstant(char*, int, char*);
RET_STATUS HandleDirect(char*, int, int, ADRESS_METHOD, char*);
void HandleInstant(char*, int, int, ADRESS_METHOD, char*);
void HandleRegister(char*);
RET_STATUS HandleJumpMethod(char*, int, int);
RET_STATUS CheckEntryAndAddToFile(char*, int);
void handleThirdGroupPhaseTwo(int);
RET_STATUS handleSecondGroupPhaseTwo(char*, int, int);
RET_STATUS handleFirstGroupPhaseTwo(char*, int, int, char*);
ADRESS_METHOD getAdressMethod(char*, char*);

int g_IC;
int g_wasExt;
int g_wasEnt;

/*the main function of phase two. when called, it goes through the .am file and encodes it in the 3 files*/
RET_STATUS PhaseTwoMain() {
	char line[D_MAX_LINE_SIZE + 1] = { '\0' };
	char ch;
	int ind = 0;
	int compiled = SUCCESS;
	int lineNumber = 1;
	g_IC = 0;
	g_wasExt = 0;
	g_wasEnt = 0;

	lineNumber += MoveAfterWhiteSpaces(g_amFile);
	while ((ch = fgetc(g_amFile)) != EOF)
	{
		line[ind++] = ch;
		if (ch == '\n')
		{
			line[ind] = '\0';
			compiled = (HandleLinePhase2(line, lineNumber) == SUCCESS) ? compiled : FAILURE;
			ind = 0;
			while (MoveAfterWhiteSpaces(g_amFile) == 1)
				lineNumber++;
			lineNumber++;
		}
	}
	line[ind++] = '\n';
	line[ind] = '\0';
	compiled = (HandleLinePhase2(line, lineNumber) == SUCCESS) ? compiled : FAILURE;

	AddDataToFile();

	return compiled;
}

/*getters for wasEnt and wasExt*/
int GetWasEnt()
{
	return g_wasEnt;
}
int GetWasExt()
{
	return g_wasExt;
}


/*function that adds the data array to the .ob file*/
void AddDataToFile()
{
	struct dataArray* data_p = dataArrayHead->next;
	while (data_p != NULL)
	{
		M_PRINT_AND_ADV_IC_COUNT(g_obFile);
		PrintCode(data_p->data, D_WORD_LENGTH);
		fprintf(g_obFile, "\n");
		data_p = data_p->next;
	}
}

/*function that receives a pointer to a line and encodes it accordingly*/
RET_STATUS HandleLinePhase2(char* line, int lineNumber)
{
	int symbolSize = CheckSymbol(line);
	line += M_POS_AFTER_LABEL(symbolSize > 0, symbolSize);
	if (line[0] == ';')
		return SUCCESS;
	/*empty sentence*/
	if (IsEmptySentence(line) == SUCCESS)
		return SUCCESS;
	if (IsDotExternCommand(line) == SUCCESS)
		return SUCCESS;
	if (DataLabel(line) == SUCCESS || IsDotExternCommand(line) == SUCCESS)
		return SUCCESS;
	if (IsExternalCommand(line, lineNumber) == SUCCESS) /*its .entry*/
		return CheckEntryAndAddToFile(line, lineNumber);
	
	return AnalyzeCommandPhaseTwo(line, lineNumber);
}

/*function that receives a pointer to a line with a command and encodes it in the files accordingly*/
RET_STATUS AnalyzeCommandPhaseTwo(char* line, int lineNumber)
{
	int commandInd = GetCommandInd(line);
	if (commandInd < 0)
		return FAILURE;
	line += strlen(GetCommand(commandInd));
	/*first group of commands*/
	if (commandInd <= D_FIRST_GROUP)
		return handleFirstGroupPhaseTwo(line, lineNumber, commandInd, "\n");
	/*second group of commands*/
	if (commandInd > D_FIRST_GROUP && commandInd <= D_SECOND_GROUP)
		return handleSecondGroupPhaseTwo(line, lineNumber, commandInd);
	handleThirdGroupPhaseTwo(commandInd);
	/*third group of commands*/
	return SUCCESS;
}

/*function that receives a pointer to a line that contains a third group command and encodes it in the .ob file*/
void handleThirdGroupPhaseTwo(int commandInd)
{
	EncodeFirstWord(commandInd, NO_ADRESS, NO_ADRESS, NO_ADRESS, NO_ADRESS, ABSOLUTE_ARE);
}

/*function that receives a pointer to a line that contains a second group command and encodes it in the .ob file*/
RET_STATUS handleSecondGroupPhaseTwo(char* line, int lineNumber, int commandInd)
{
	ADRESS_METHOD operandAdress;

	if(*line == '\0' || *(line+1) == '\0')
		return AddError(TOO_LITTLE_OPERANDS, lineNumber);

	operandAdress = getAdressMethod(++line, "\n");

	if (operandAdress == REGISTER)/*register*/
	{
		EncodeFirstWord(commandInd, NO_ADRESS, REGISTER, NO_ADRESS, NO_ADRESS, ABSOLUTE_ARE);
		EncodeRegister(line);
	}
	else if (operandAdress == INSTANT)/*command is prn*/
	{
		EncodeFirstWord(commandInd, NO_ADRESS, INSTANT, NO_ADRESS, NO_ADRESS, ABSOLUTE_ARE);
		EncodeInstant(line);
	}
	else if (operandAdress == DIRECT)/*label*/
		return HandleDirect(line, lineNumber, commandInd, NO_ADRESS, NULL);
	else
		return HandleJumpMethod(line, lineNumber, commandInd);
	return SUCCESS;
}

/*function that receives a pointer to a line that contains a jump method operand and encodes it in the .ob file*/
RET_STATUS HandleJumpMethod(char* line, int lineNumber, int commandInd)
{
	ADRESS_METHOD firstOperandAdress, secondOperandAdress;
	struct label* lblPtr = GetLabel(line, lineNumber);
	char* secondOperand_p = line;
	if (lblPtr == NULL)
		return AddError(LABEL_DOESNOT_EXIST, lineNumber);
	while ((*line) != '\0' && *line != '(')
		line++;
	while ((*secondOperand_p) != '\0' && *secondOperand_p != ',')
		secondOperand_p++;

	if(secondOperand_p == '\0' || line == '\0')
		return AddError(TOO_LITTLE_OPERANDS, lineNumber);

	firstOperandAdress = getAdressMethod(line, ",");
	secondOperandAdress = getAdressMethod(secondOperand_p, ")");

	if(lblPtr->mark == EXTERNAL || M_IS_LABEL_AND_EXTERNAL(firstOperandAdress, line, lineNumber)
		|| M_IS_LABEL_AND_EXTERNAL(secondOperandAdress, secondOperand_p, lineNumber))
		EncodeFirstWord(commandInd, NO_ADRESS, DIRECT, firstOperandAdress, secondOperandAdress, EXTERNAL_ARE);
	else
		EncodeFirstWord(commandInd, NO_ADRESS, DIRECT, firstOperandAdress, secondOperandAdress, RELOCATEABLE_ARE);

	EncodeDirect(lblPtr->name, lineNumber);
	return handleFirstGroupPhaseTwo(--line, lineNumber, -1, ")");
}

/*function that receives a pointer to a line that contains a first group command and encodes it in the .ob file*/
RET_STATUS handleFirstGroupPhaseTwo(char* line, int lineNumber, int commandInd, char* end)
{
	char* secondOperandPointer;
	ADRESS_METHOD firstOperandAdress;
	ADRESS_METHOD secondOperandAdress;

	/*gets the first and second adress methods*/
	if (*line == '\n' || *line == '\0')
		return AddError(TOO_LITTLE_OPERANDS, lineNumber);

	secondOperandPointer = line++;
	firstOperandAdress = getAdressMethod(line, ",");

	while (*secondOperandPointer != ',' && *secondOperandPointer != '\0')
		secondOperandPointer++;

	if (*secondOperandPointer == '\0')
		return AddError(TOO_LITTLE_OPERANDS, lineNumber);

	secondOperandPointer++;
	while (isspace(*secondOperandPointer) != 0 && *secondOperandPointer != '\0')
		secondOperandPointer++;
	
	if (*secondOperandPointer == '\0')
		return AddError(TOO_LITTLE_OPERANDS, lineNumber);

	secondOperandAdress = getAdressMethod(secondOperandPointer, end);

	/*first operand is a register*/
	if (firstOperandAdress == REGISTER)
	{
		/*both operands are registers*/
		if (secondOperandAdress == REGISTER)
		{
			HandleRegisterRegister(line, commandInd);
			return SUCCESS;
		}
		/*second operand is a label*/
		if (secondOperandAdress == DIRECT)
			return HandleRegisterDirect(line, lineNumber, secondOperandPointer, commandInd);
		HandleRegisterInstant(line, commandInd, secondOperandPointer);

		return SUCCESS;
	}
	/*first opernad is a label*/
	if (firstOperandAdress == DIRECT)
	{
		if (HandleDirect(line, lineNumber, commandInd, secondOperandAdress, secondOperandPointer) != SUCCESS)
			return FAILURE;
	}
	/*first operand is a number*/
	if (firstOperandAdress == INSTANT)
		HandleInstant(line, lineNumber, commandInd, secondOperandAdress, secondOperandPointer);
	while (*(line++) != ',');
	while (isspace(*line) != 0)
		line++;
	/*second operand*/
	if (secondOperandAdress == REGISTER)
		HandleRegister(line);
	if (secondOperandAdress == DIRECT)
		return EncodeDirect(secondOperandPointer, lineNumber);
	if(secondOperandAdress == INSTANT)
		EncodeInstant(secondOperandPointer);
	return SUCCESS;
}

/*function that receives a pointer to a line that contains a register and incodes the register in the .ob file (full line)*/
void HandleRegister(char* line)
{
	M_PRINT_AND_ADV_IC_COUNT(g_obFile);
	fprintf(g_obFile, D_PREFIX_REGISTER_BLANK);
	PrintCode(M_REGISTER_NUMBER(line), 6);
	fprintf(g_obFile, D_EMPTY_ARE);
}

/*function that receives a pointer to a line that contains a number and incodes the line in the .ob file according to the other operands*/
void HandleInstant(char* line, int lineNumber, int commandInd, ADRESS_METHOD secondOperandAdress, char* secondOperandPointer)
{
	if M_IS_LABEL_AND_EXTERNAL(secondOperandAdress, secondOperandPointer, lineNumber)/*second mark = external*/
		EncodeFirstWord(commandInd, INSTANT, secondOperandAdress, NO_ADRESS, NO_ADRESS, EXTERNAL_ARE);
	else
		EncodeFirstWord(commandInd, INSTANT, secondOperandAdress, NO_ADRESS, NO_ADRESS, RELOCATEABLE_ARE);
	EncodeInstant(line);
}

/*function that receives a pointer to a line that contains a label and incodes the line in the .ob file according to the other operands*/
RET_STATUS HandleDirect(char* line, int lineNumber, int commandInd, ADRESS_METHOD secondOperandAdress, char* secondOperandPointer)
{
	if (GetLabel(line, lineNumber) == NULL)
		return AddError(LABEL_DOESNOT_EXIST, lineNumber);
	if (commandInd > -1)
	{
		/*checked in first phase*/
		if (strlen(GetLabelName(line)) > strlen(line) - 1)
			return FAILURE;
		/*checks the number of operands and acts accordingly*/
		if (secondOperandAdress == NO_ADRESS)
		{
			/*encodes first word of command*/
			if(GetLabelMark(line, lineNumber) == EXTERNAL)
				EncodeFirstWord(commandInd, NO_ADRESS, DIRECT, NO_ADRESS, NO_ADRESS, EXTERNAL_ARE);/*encode word with are external*/
			else
				EncodeFirstWord(commandInd, NO_ADRESS, DIRECT, NO_ADRESS, NO_ADRESS, RELOCATEABLE_ARE);
		}
		else
		{
			/*gets second word mark*/
			if (GetLabelMark(line, lineNumber) == EXTERNAL || (secondOperandPointer != NULL && M_IS_LABEL_AND_EXTERNAL(secondOperandAdress, secondOperandPointer, lineNumber)))/*first or second mark = external*/
				EncodeFirstWord(commandInd, DIRECT, secondOperandAdress, NO_ADRESS, NO_ADRESS, EXTERNAL_ARE);/*encode word with are external*/
			else
				EncodeFirstWord(commandInd, DIRECT, secondOperandAdress, NO_ADRESS, NO_ADRESS, RELOCATEABLE_ARE);
		}
	}
	return EncodeDirect(line, lineNumber);
}

/*function that receives a pointer to a line that contains a register and a number and incodes it in the .ob file*/
void HandleRegisterInstant(char* line, int commandInd, char* secondOperandPointer){
	if (commandInd > -1)
		EncodeFirstWord(commandInd, REGISTER, INSTANT, NO_ADRESS, NO_ADRESS, ABSOLUTE_ARE);
	EncodeRegister(line);
	fprintf(g_obFile, D_SUFFIX_REGISTER_BLANK);
	EncodeInstant(secondOperandPointer);
}

/*function that receives a pointer to a line that contains a register and a label and incodes it in the .ob file*/
RET_STATUS HandleRegisterDirect(char* line, int lineNumber, char* secondOperandPointer, int commandInd)
{
	if (GetLabel(secondOperandPointer, lineNumber) == NULL)
		return AddError(LABEL_DOESNOT_EXIST, lineNumber);
	if (GetLabelMark(secondOperandPointer, lineNumber) == EXTERNAL)
	{
		if (commandInd > -1)
			EncodeFirstWord(commandInd, REGISTER, DIRECT, NO_ADRESS, NO_ADRESS, EXTERNAL_ARE);
	}
	else
	{
		if (commandInd > -1)
			EncodeFirstWord(commandInd, REGISTER, DIRECT, NO_ADRESS, NO_ADRESS, RELOCATEABLE_ARE);
	}
	EncodeRegister(line);
	fprintf(g_obFile, D_SUFFIX_REGISTER_BLANK);
	return EncodeDirect(secondOperandPointer, lineNumber);
}

/*function that receives a pointer to a line that contains 2 registers and incodes it in the .ob file*/
void HandleRegisterRegister(char* line, int commandInd)
{
	if (commandInd > -1)
		EncodeFirstWord(commandInd, REGISTER, REGISTER, NO_ADRESS, NO_ADRESS, 0);
	EncodeRegister(line);
	line++;
	while (*line != 'r')
		line++;
	PrintCode(M_REGISTER_NUMBER(line), 6);
	fprintf(g_obFile, D_EMPTY_ARE);
}

/*function that receives a pointer to a line and returns the mark of the label, if exists or NO_MARK otherwise*/
enum E_LABEL_MARK GetLabelMark(char* line, int lineNumber)
{
	struct label* lblPtr = GetLabel(line, lineNumber);
	if (lblPtr == NULL)
		return NO_MARK;
	return lblPtr->mark;
}

/*function that receives a pointer to a line and returns a struct label* to the label, if exists or NULL otherwise*/
struct label* GetLabel(char* line, int lineNumber)
{
	struct label* tempPtr = labelHead;
	char* lblName = GetLabelName(line);
	struct label* lblPtr = NULL;

	if (lblName == NULL)
		return NULL;

	while (tempPtr != NULL)
	{
		if (strcmp(tempPtr->name, lblName) == 0)
			lblPtr = tempPtr;
		tempPtr = tempPtr->next_p;
	}
	return lblPtr;
}


/*function that receives a label operand of .entry, checks if it was defined in the code and adds to .ent file accordingly*/
RET_STATUS CheckEntryAndAddToFile(char* line, int lineNumber)
{
	struct label* lblPtr = GetLabel((line + M_ENTRY_LENGTH), lineNumber);
	if (lblPtr == NULL)
		return AddError(LABEL_DOESNOT_EXIST, lineNumber);
	g_wasEnt = 1;
	fprintf(g_entFile, "%s ", lblPtr->name);
	PrintBinaryCodeToFile(g_entFile, lblPtr->value + D_MEMORY_MARGIN);
	fprintf(g_entFile, "\n");
	return SUCCESS;
}

/*function that receives an operand and returns its adress method (0-3)*/
ADRESS_METHOD getAdressMethod(char* line, char* end)
{
	if (Check0AdressGroup(line, end) == SUCCESS)
		return INSTANT;
	if (Check3AdressGroup(line) == SUCCESS)
		return REGISTER;
	if (Check2AdressGroup(line, end) == SUCCESS)
		return JUMP;
	if (GetLabelName(line) != NULL)
		return DIRECT;
	return NO_ADRESS;
}

/*function that receives a register operand and encodes it in the .ob file (part of a line)*/
void EncodeRegister(char* line)
{
	M_PRINT_AND_ADV_IC_COUNT(g_obFile);
	PrintCode(M_REGISTER_NUMBER(line), 6);
}

/*function that receives a number operand and encodes it in the .ob file*/
void EncodeInstant(char* line)
{
	int ind = 0;
	char* temp;
	line++;
	while (isdigit(*(line + ind)) != 0 || *(line + ind) == '-' || *(line + ind) == '+')
		ind++;
	temp = (char*)malloc(ind + 2);
	if (temp == NULL)
		return;
	strncpy(temp, line, ind);
	temp[ind] = '\0';
	M_PRINT_AND_ADV_IC_COUNT(g_obFile);
	PrintCode(atoi(temp), 12);
	fprintf(g_obFile, D_EMPTY_ARE);
}

/*function that receives a label operand and encoodes it in the .ob file according to the mark (external or not)*/
RET_STATUS EncodeDirect(char* line, int lineNumber)
{
	struct label* temp_p = GetLabel(line, lineNumber);

	if (temp_p == NULL)
		return FAILURE;

	M_PRINT_AND_ADV_IC_COUNT(g_obFile);
	if(temp_p->mark == EXTERNAL)
		PrintCode(0, 12);
	else
		PrintCode(temp_p->value + D_MEMORY_MARGIN, 12);
	

	if (temp_p->mark == EXTERNAL)
	{
		fprintf(g_obFile, D_EXTERNAL_ARE);
		PrintExternalToFile(temp_p->name);
	}
	else
		fprintf(g_obFile, D_RELOCATABLE_ARE);

	fprintf(g_obFile, "\n");
	return SUCCESS;
}

/*function that prints an external label to the .ext file*/
RET_STATUS PrintExternalToFile(char* name)
{
	g_wasExt = 1;
	if (name == NULL)
		return ILLEGAL_LABEL_DEFINITION;
	fprintf(g_extFile, "%s ", name);
	PrintBinaryCodeToFile(g_extFile, g_IC + D_MEMORY_MARGIN -1);
	fprintf(g_extFile, "\n");
	return SUCCESS;
}

/*function that prints to the .ob file the first word of command*/
void EncodeFirstWord(int commandInd, ADRESS_METHOD firstAdress, ADRESS_METHOD secondAdress, ADRESS_METHOD thirdAdress, ADRESS_METHOD forthAdress, ARE_BITS are)
{
	if (commandInd < 0)
		return;
	M_PRINT_AND_ADV_IC_COUNT(g_obFile);
	/*--------2nd adress method---------*/
	if (thirdAdress != NO_ADRESS)
	{
		Print2ndAdressMethod(thirdAdress);
		Print2ndAdressMethod(forthAdress);
	}
	else
		fprintf(g_obFile, D_EMPTY_ADRESS_BITS);
	/*-------------command------------*/
	PrintCode(commandInd, 4);
	/*---first and second op adress---*/
	if(firstAdress == NO_ADRESS)
		PrintCode(0, 2);
	else
		PrintCode((int)firstAdress, 2);
	if (secondAdress == NO_ADRESS)
		PrintCode(0, 2);
	else
		PrintCode((int)secondAdress, 2);
	/*--------------ARE-------------*/
	switch (are)
	{
	case ABSOLUTE_ARE:
		fprintf(g_obFile, D_ABSOLUTE_ARE);
		break;
	case RELOCATEABLE_ARE:
		fprintf(g_obFile, D_RELOCATABLE_ARE);
		break;
	default:
		fprintf(g_obFile, D_EXTERNAL_ARE);
		break;
	}
	fprintf(g_obFile, "\n");
}

/*recursive function that prints decimal in binary of length len to the .ob file*/
void PrintCode(int decimal, int len)
{
	if (len > 1)
	{
		PrintCode(decimal >> 1, --len);
	}
	fprintf(g_obFile, "%c", ((decimal & 1) ? '/' : '.'));
}

/*prints the jump adress method part to the .ob file*/
void Print2ndAdressMethod(ADRESS_METHOD adress)
{
	switch (adress)
	{
	case REGISTER:
		fprintf(g_obFile, D_2ND_ADRESS_METHOD_REGISTER);
		break;
	case INSTANT:
		fprintf(g_obFile, D_2ND_ADRESS_METHOD_INSTANT);
		break;
	default:
		fprintf(g_obFile, D_2ND_ADRESS_METHOD_DIRECT);
	}
}
