#define _CRT_SECURE_NO_WARNINGS 
#include "ClipBoardFile.h"

typedef struct
{
	unsigned int timeSize;
	unsigned int typeSize;
	unsigned int dataSize;
} CLIPBOARDSIZE;


int AddToFile(char *filePath, CLIPBOARDDATA *data)
{
	FILE* HFile = fopen(filePath, "ab");
	if (HFile)
	{
		WriteRecord(data, HFile);
		fclose(HFile);
	}
	else
		return 0;
}

int GetRecordSize(CLIPBOARDSIZE *buffer,  FILE* hFile)
{
	//CLIPBOARDSIZE answer = {0};
	char* buff;

	if (!fread(&buffer->timeSize, sizeof(unsigned int), 1, hFile))
		return 0;
	fread(&buff, sizeof(char), 1, hFile);
	fread(&buffer->typeSize, sizeof(unsigned int), 1, hFile);
	fread(&buff, sizeof(char), 1, hFile);
	fread(&buffer->dataSize, sizeof(unsigned int), 1, hFile);
	fread(&buff, sizeof(char), 1, hFile);

	return 1;
}

int ReadRecord(CLIPBOARDDATA *buffer, FILE* hFile)
{
	CLIPBOARDSIZE sizes;
	if (!GetRecordSize(&sizes, hFile)) return NULL;

	buffer->dateTime = calloc(sizes.timeSize+2, sizeof(char));
	buffer->type = calloc(sizes.typeSize+2, sizeof(char));
	buffer->data = calloc(sizes.dataSize+2, sizeof(char));

	fread(buffer->dateTime, sizeof(char), sizes.timeSize, hFile);
	fread(buffer->type, sizeof(char), sizes.typeSize, hFile);
	fread(buffer->data, sizeof(char), sizes.dataSize, hFile);
	buffer->dataLength = sizes.dataSize;

	return (sizes.dataSize + sizes.timeSize + sizes.typeSize);
}

int WriteRecord(CLIPBOARDDATA* buffer, FILE* hFile)
{
	unsigned int timeSize = wcslen(buffer->dateTime) * 2;
	unsigned int typeSize = wcslen(buffer->type) * 2;

	fwrite(&timeSize, sizeof(unsigned int), 1, hFile);
	fwrite("\0", sizeof(char), 1, hFile);
	fwrite(&typeSize, sizeof(unsigned int), 1, hFile);
	fwrite("\0", sizeof(char), 1, hFile);
	fwrite(&buffer->dataLength, sizeof(unsigned int), 1, hFile);
	fwrite("\0", sizeof(char), 1, hFile);

	fwrite(buffer->dateTime, sizeof(char), timeSize, hFile);
	fwrite(buffer->type, sizeof(char), typeSize, hFile);
	fwrite(buffer->data, sizeof(char), buffer->dataLength, hFile);
}

int DeleteFromFile(char *filePath, int itemId)
{
	int currId = 1;
	int flag = 0;
	//CLIPBOARDSIZE currSize;
	CLIPBOARDDATA record;

	FILE* HFile = fopen(filePath, "rb");
	FILE* hTmp = fopen("temp.bin", "ab");
	while (ReadRecord(&record, HFile) != NULL)
	{
		if (currId != itemId)
			WriteRecord(&record, hTmp);
		else
			flag = 1;

		currId++;
	}

	fclose(HFile);
	fclose(hTmp);

	remove(filePath);
	rename("temp.bin", filePath);

	return flag;
}

CLIPBOARDDATA* GetFileItem(char* filePath, int itemId)
{
	CLIPBOARDDATA record;
	int currId = 1;
	int flag = 0;

	FILE* HFile = fopen(filePath, "rb");
	while (ReadRecord(&record, HFile) != NULL)
	{
		if (currId == itemId)
			return &record;

		currId++;
	}

	fclose(HFile);
	if (!flag)
		return NULL;
}

int UpdateFile(char* filePath, int itemId, CLIPBOARDDATA* data)
{
	int currId = 1;
	int flag = 0;
	CLIPBOARDDATA record;

	FILE* HFile = fopen(filePath, "rb");
	FILE* hTmp = fopen("temp.bin", "ab");
	while (ReadRecord(&record, HFile) != NULL)
	{
		if (currId != itemId)
			WriteRecord(&record, hTmp);
		else
		{
			WriteRecord(data, hTmp);
			flag = 1;
		}
		currId++;
	}

	fclose(HFile);
	fclose(hTmp);

	remove(filePath);
	rename("temp.bin", filePath);

	return flag;
}

int ClearFile(char* filePath)
{
	FILE* HFile = fopen(filePath, "w");
	fclose(HFile);
}