#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>
#include <stdlib.h>
#include "ClipBoardProcessing.h"
#include "ClipBoardFile.h"
#include "LinkedList.h"

int AddTerm(Node** list, char* filePath, CLIPBOARDDATA* dataToAdd)
{
	if (*list)
		Push(list, *dataToAdd);
	if (filePath)
		return AddToFile(filePath, dataToAdd);
	return 1;
}

int DeleteTerm(Node** list, char* filePath, int termInd)
{
	int listSize = GetListSize(*list);
	int filePos = listSize - termInd + 1;
	if (*list)
		DeleteNth(list, termInd);
	if (filePath)
		return DeleteFromFile(filePath, filePos);
	return 1;
}

int EditTerm(Node** list, char* filePath, int termInd, CLIPBOARDDATA* dataToAdd)
{
	int listSize = GetListSize(*list);
	int filePos = listSize - termInd + 1;
	if (*list)
	{
		Node *listEl = GetNth(*list, termInd);
		listEl->value = *dataToAdd;
	}
	if (filePath)
		return UpdateFile(filePath, filePos, dataToAdd);

	return 1;
}

CLIPBOARDDATA* GetTerm(Node** list, char* filePath, int termInd)
{
	int listSize = GetListSize(*list);
	int filePos = listSize - termInd + 1;
	if (*list)
	{
		Node* listEl = GetNth(*list, termInd);
		return &(listEl->value);
	}
	if (filePath)
		return GetFileItem(filePath, filePos);
	return NULL;
}

int ClearTerms(Node** list, char* filePath)
{
	if (*list)
		DeleteList(list);
	if (filePath)
		ClearFile(filePath);
	return 1;
}

Node* GetListTermFromFile(char* filePath)
{
	CLIPBOARDDATA record;
	Node* listHeader = NULL;

	FILE* hFile = fopen(filePath, "rb");
	if (!hFile)
		return NULL;
	while (ReadRecord(&record, hFile) != NULL)
		Push(&listHeader, record);


	fclose(hFile);
	return listHeader;
}

void UnloadList(Node** list)
{
	DeleteList(list);
}