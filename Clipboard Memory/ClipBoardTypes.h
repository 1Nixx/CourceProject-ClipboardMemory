#pragma once
#include <shtypes.h>

typedef struct _ClipBoard {
	wchar_t* type;
	wchar_t* dateTime;
	void* data;
	unsigned int dataLength;
} CLIPBOARDDATA;

typedef struct _Node {
	CLIPBOARDDATA value;
    struct _Node* next;
} Node;