#pragma once
#include  <stdio.h>
#include "ClipboardTypes.h"

//int OpenFile(char *filePat);
//int CloseFile(char* filePat);
int AddToFile(char  *filePath, CLIPBOARDDATA *data);
int DeleteFromFile(char* filePath, int itemId);
CLIPBOARDDATA *GetFileItem(char* filePath, int itemId);
int UpdateFile(char* filePath, int itemId, CLIPBOARDDATA *data);
int ClearFile(char* filePath);
int ReadRecord(CLIPBOARDDATA* buffer, FILE* hFile);