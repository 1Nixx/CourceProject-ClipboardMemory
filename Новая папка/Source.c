#include "ClipBoardTypes.h"
#include "ClipBoardProcessing.h"


void main()
{
	wchar_t* str = L"1";
	wchar_t* str2 = L"1";
	char* arr = "1";
	CLIPBOARDDATA dataA;
	dataA.data = arr;
	dataA.dateTime = str;
	dataA.type = str2;
	dataA.dataLength = strlen(arr);

	Node* list = GetListTermFromFile("textfile.txt");
	
	//CLIPBOARDDATA* data = DeleteNth(&list, 3);
	//AddToFile("textfile.txt", &dataA);
	//UpdateFile("textfile.txt", 1, &dataA);
	//CLIPBOARDDATA* dataB;
	//dataB = GetFileItem("textfile.txt", 1);
	//wchar_t* asd = dataB->data;
	//wprintf(L"%ls", (wchar_t*)dataB->data);
}