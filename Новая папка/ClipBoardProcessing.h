#pragma once
#include "ClipBoardTypes.h"

int AddTerm(Node** list, char* filePath, CLIPBOARDDATA *dataToAdd);
int DeleteTerm(Node** list, char* filePath, int termInd);
int EditTerm(Node** list, char* filePath, int termInd, CLIPBOARDDATA *dataToAdd);
CLIPBOARDDATA *GetTerm(Node** list, char* filePath, int termInd);
int ClearTerms(Node** list, char* filePath);
Node* GetListTermFromFile(char* filePath);