#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include "PreAssembler.h"
#include "Util.h"
#include "PhaseOne.h"
#include "PhaseTwo.h"

void handleArgs(int, char* []);
void handleFile(char*);
RET_STATUS checkAndOpenFiles(char*);
void EraseFiles(char*);
void CloseFiles();
void PrintAllErrors();

/*the main function of the program. calls the handleArgs when activated.*/
int main(int argc, char *argv[])
{
	handleArgs(argc, argv);
	return 0;
}
/*receives an arg array and for each arg calls the handleArg function that encodes the file*/
void handleArgs(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("Error, no files given in the command line.\n");
		exit(0);
	}
	while (--argc > 0)
		handleFile(argv[argc]);
}

/*receives an arg and calls all the needed functions to encode*/
void handleFile(char *arg)
{
	int compiled = 1;

	printf("\n-----------------------\n");
	printf("file: %s\n", arg);
	printf("-----------------------\n\n");

	/*if failed to open .as and new files*/
	if (checkAndOpenFiles(arg) == FAILURE)
	{
		EraseFiles(arg);
		return;
	}

	/*calls the pre assembler*/
	if (PreAssemblerMain() == FAILURE)
		compiled = 0;

	fseek(g_amFile, 0, SEEK_SET);
	
	/*calls the 1st phase*/
	if (PhaseOneMain() == FAILURE)
		compiled = 0;

	fseek(g_amFile, 0, SEEK_SET);
	
	if (PhaseTwoMain() == FAILURE)
		compiled = 0;
	
	/*closes the files and deletes them if needed*/
	CloseFiles();
	
	if (compiled == 0)
		EraseFiles(arg);
	
	/*frees the mem allocations*/
	FreePhaseOneHead();
	
	/*checks if .ent and .ext files needs to be deleted*/
	if (GetWasEnt() == 0)
		remove(GetEntName(arg));
	if (GetWasExt() == 0)
		remove(GetExtName(arg));

	/*prints all the errors*/
	if(compiled == FAILURE)	
		printf("File compiled, no errors found.\n");
	PrintAllErrors();
	printf("\n\n");
}

void EraseFiles(char* arg)
{
	remove(GetAmName(arg));
	remove(GetObName(arg));
	remove(GetEntName(arg));
	remove(GetExtName(arg));
}

void CloseFiles()
{
	fcloseall();
}

RET_STATUS checkAndOpenFiles(char* arg)
{
	char* fileName = GetAsName(arg);

	g_asFile = fopen(fileName, "rb");
	if (g_asFile == NULL)
	{
		printf("Error, Couldn't open .as file- %s\n", fileName);
		return FAILURE;
	}

	fileName = GetAmName(arg);
	g_amFile = fopen(fileName, "w+");
	if (g_amFile == NULL)
	{
		printf("Error, Couldn't open a new file- %s\n", fileName);
		return FAILURE;
	}

	fileName = GetObName(arg);
	g_obFile = fopen(fileName, "w");
	if (g_obFile == NULL)
	{
		printf("Error, Couldn't open a new file- %s\n", fileName);
		return FAILURE;
	}

	fileName = GetEntName(arg);
	g_entFile = fopen(fileName, "w");
	if (g_obFile == NULL)
	{
		printf("Error, Couldn't open a new file- %s\n", fileName);
		return FAILURE;
	}

	fileName = GetExtName(arg);
	g_extFile = fopen(fileName, "w");
	if (g_obFile == NULL)
	{
		printf("Error, Couldn't open a new file- %s\n", fileName);
		return FAILURE;
	}

	free(fileName);
	return SUCCESS;
}
