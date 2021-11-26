#include "ClipBoardTypes.h"
#include "ClipBoardProcessing.h"
#include <stdio.h>
#include "LinkedList.h"

void main()
{
	wchar_t* str = L"3";
	wchar_t* str2 = L"3";
	wchar_t* arr = L"32";
	CLIPBOARDDATA dataA;
	dataA.data = arr;
	dataA.dateTime = str;
	dataA.type = str2;
	dataA.dataLength = wcslen(arr)*2;

	SYSTEMTIME time;
	GetLocalTime(&time);

	wchar_t timeStr[17];
	swprintf(timeStr, 17, L"%d.%d.%d %d:%d", time.wDay, time.wMonth, time.wYear, time.wHour, time.wDay);

	Node* list = GetListTermFromFile("textfile.txt");
	//UnloadList(&list);
	//list = GetListTermFromFile("textfile.txt");

	//int a = GetListSize(list);
	//CLIPBOARDDATA* data = DeleteNth(&list, 3);
	//AddToFile("textfile.txt", &dataA);
	//UpdateFile("textfile.txt", 1, &dataA);
	//CLIPBOARDDATA* dataB;
	//dataB = GetFileItem("textfile.txt", 1);
	//wchar_t* asd = dataB->data;
	//wprintf(L"%ls", (wchar_t*)dataB->data);
}