#pragma once
#include "ClipboardTypes.h"

void Push(Node** head, CLIPBOARDDATA data);
CLIPBOARDDATA *Pop(Node** head);
Node* GetNth(Node* head, int n);
void PushBack(Node* head, CLIPBOARDDATA value);
CLIPBOARDDATA *DeleteNth(Node** head, int n);
Node* GetLast(Node* head);
void DeleteList(Node** head);
int GetListSize(Node* head);