#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fat.h>
#include <sys/dir.h>
#include <PA9.h>

#include "DSFileIO.h"
#include "SimpleConsole.h"
#include "Utils.h"

//#define DEBUG
bool g_bFATFoundFirst;

//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------
bool DSFATDevice_Init()
{
	bool bResult = false;

	// initialize the FAT system
	g_bFATFoundFirst = false;
	//check for fat record
	if(!fatInitDefault())
	{
		SimpleConsolePrintStringCR(SCREEN_BOTTOM, "FAT failed to initialise.");
		waitVBLDelay(30);
		//waitVBLDelay( 600 );
	}
	else
	{
		bResult = true;
		SimpleConsolePrintStringCR(SCREEN_BOTTOM, "FAT initialized...");
	}
	return bResult;
}

//------------------------------------------------------------
//------------------------------------------------------------
bool DSIODevice_Init()
{
	// set default working directory
	SimpleConsolePrintStringCR(SCREEN_BOTTOM, "CWD initialized...");

	DSFATDevice_Init();

	return true;
}

//------------------------------------------------------------
//------------------------------------------------------------
void DSFATDevice_CleanUp()
{
}

//------------------------------------------------------------
//------------------------------------------------------------
void DSIODevice_CleanUp()
{
	fatUnmount (PI_DEFAULT);
}

//------------------------------------------------------------
//------------------------------------------------------------
const char * getPathWithoutDevice(const char * szPath)
{
	if(szPath[0] == '\0')
	{
		return szPath;
	}

	char * cpTemp;
	int nLen;
	if(szPath[0] == '/')
	{
		++szPath;
	}
	cpTemp = strchr(szPath, '/');
	if(NULL == cpTemp)
	{
		nLen = strlen(szPath);
		return(szPath + nLen);
	}
	return cpTemp;
}

//------------------------------------------------------------
//------------------------------------------------------------
void  buildPath(char * szBuffer, const char * szPath, const char * szFile)
{
	if(szBuffer != szPath)
	{
		strcpy(szBuffer, szPath);
	}
	if(!strEndsWith(szBuffer, "/") && !strStartsWith(szFile, "/"))
	{
		strcat(szBuffer, "/");
	}
	strcat(szBuffer, szFile);
}

//------------------------------------------------------------
//------------------------------------------------------------
size_t   getFileSize(FILE * dfHandle)
{
#ifdef DEBUG
	SimpleConsolePrintStringCR(SCREEN_TOP, "GetFileSize");
#endif
	fseek(dfHandle, 0, SEEK_END);
	int nResult = ftell(dfHandle);
	rewind (dfHandle);

	return nResult;
}


