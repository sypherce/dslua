#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <PA9.h>

#include "DSFileIO.h"
#include "SimpleConsole.h"
#include "Utils.h"
#include "DSLFileObj.h"
#include "gba_nds_fat.h"
extern "C"
{
        #include "disc_io.h"
}
#include "sanos_input.h"

//#define DEBUG

DIR_LIST g_dlList;
DS_DIRECTORY_INFO g_diCWD;
DS_FILE DSF_STDOUT = {DSF_STD, stdout};
DS_DEVICE_INFO g_AllDevs[] =
{
	{DSF_INTERNAL, "", true},                              // this is assumed to always be first position
#ifdef USE_CF
	{DSF_FAT, "cf", true},
	{DSF_GBFS, "gbfs", false},
#else
	{DSF_FAT, "cf", false},
	{DSF_GBFS, "gbfs", true},
#endif
	{DSF_SRAM, "sram", true}
};
typedef struct
{
	u8 nNumFiles;
	u8 nCurrentIndex;
} DS_DEVICE_DIR_INFO;

DS_DEVICE_DIR_INFO g_GBFSDeviceInfo;
DS_DEVICE_DIR_INFO g_SRAMDeviceInfo;
DS_DEVICE_DIR_INFO g_RootDeviceInfo;
bool g_bFATFoundFirst;

//------------------------------------------------------------
//------------------------------------------------------------
bool isDeviceNeeded(const DSF_DEVICE_TYPE dtType)
{
	int nCount, nLoop;

	nCount = countof(g_AllDevs);
	for(nLoop = 0; nLoop < nCount; ++nLoop)
	{
		if((dtType == g_AllDevs[ nLoop ].ddtType) && (g_AllDevs[ nLoop ].bAvailable))
		{
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------
//------------------------------------------------------------
void enableDevice(const DSF_DEVICE_TYPE dtType, const bool bEnable)
{
	int nCount, nLoop;

	nCount = countof(g_AllDevs);
	for(nLoop = 0; nLoop < nCount; ++nLoop)
	{
		if(dtType == g_AllDevs[ nLoop ].ddtType)
		{
			g_AllDevs[ nLoop ].bAvailable = bEnable;
		}
	}
}

//------------------------------------------------------------
//------------------------------------------------------------
bool DSFATDevice_Init()
{
	bool bResult = false;

	// initialize the FAT system
	g_bFATFoundFirst = false;
	if(!disc_Init())
	{
		SimpleConsolePrintStringCR(SCREEN_BOTTOM, "Media card not found.");
		enableDevice(DSF_FAT, FALSE);
		//waitVBLDelay( 30 );
	}
	else
	{
		waitVBLDelay(60);
		SimpleConsolePrintStringCR(SCREEN_BOTTOM, "Disc initialized...");

		//check for fat record
		if(!FAT_InitFiles())
		{
			SimpleConsolePrintStringCR(SCREEN_BOTTOM, "FAT failed to initialise.");
			enableDevice(DSF_FAT, FALSE);
			waitVBLDelay(30);
			//waitVBLDelay( 600 );
		}
		else
		{
			bResult = true;
			SimpleConsolePrintStringCR(SCREEN_BOTTOM, "FAT initialized...");
		}
	}
	return bResult;
}

//------------------------------------------------------------
//------------------------------------------------------------
bool DSIODevice_Init()
{
	// init Root system
	g_RootDeviceInfo.nNumFiles = countof(g_AllDevs);
	g_RootDeviceInfo.nCurrentIndex = 1;
	SimpleConsolePrintStringCR(SCREEN_BOTTOM, "Root device initialized...");

	// set default working directory
	setCurrentWorkingDir("/");
	SimpleConsolePrintStringCR(SCREEN_BOTTOM, "CWD initialized...");

	// init GBFS system
	if(isDeviceNeeded(DSF_GBFS))
	{
		g_GBFSDeviceInfo.nNumFiles = PA_InitGBFS();
		g_GBFSDeviceInfo.nCurrentIndex = 0;
		SimpleConsolePrintStringCR(SCREEN_BOTTOM, "GBFS initialized...");
	}

	// init SRAM system
	if(isDeviceNeeded(DSF_SRAM))
	{
		g_SRAMDeviceInfo.nNumFiles = 0;
		g_SRAMDeviceInfo.nCurrentIndex = 0;
		SimpleConsolePrintStringCR(SCREEN_BOTTOM, "SRAM initialized...");
	}

	// init FAT system
	if(isDeviceNeeded(DSF_FAT))
	{
		DSFATDevice_Init();
	}

	return true;
}

//------------------------------------------------------------
//------------------------------------------------------------
void DSFATDevice_CleanUp()
{
	FAT_FreeFiles();
	disc_Shutdown();
}

//------------------------------------------------------------
//------------------------------------------------------------
void DSIODevice_CleanUp()
{
	if(isDeviceNeeded(DSF_FAT))
	{
		DSFATDevice_CleanUp();
	}
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
void _AbsPathResolver(const char * szPath, char * szAbsPath)
{
	char szNewPath[ DS_MAX_FILENAME_LENGTH ];
	char * szToken;
	bool bDoSlash;

	// start with an empty string
	bDoSlash = strStartsWith(szPath, "/");
	szAbsPath[ 0 ] = '\0';
	strcpy(szNewPath, szPath);

	// parse string
	szToken = strtok(szNewPath, "/");
	while(szToken)
	{
		if(0 != strcmp(szToken, "."))
		{
			if(0 == strcmp(szToken, ".."))
			{
				// pop last dir off
				stripLastDirFromPath(szAbsPath);
			}
			else
			{
				// valid dir name
				if(bDoSlash)
				{
					strcat(szAbsPath, "/");
				}
				else
				{
					bDoSlash = true;
				}
				strcat(szAbsPath, szToken);
			}
		}
		szToken = strtok(NULL, "/");
	}

	// copy last slash over if there was one
	if(strEndsWith(szPath, "/"))
	{
		strcat(szAbsPath, "/");
	}
}

//------------------------------------------------------------
//------------------------------------------------------------
void buildAbsolutePath(char * szAbsPath, const char * szPath)
{
	char szNewPath[ DS_MAX_FILENAME_LENGTH ];

	if(!strStartsWith(szPath, "/"))
	{
		buildPath(szNewPath, getCurrentWorkingDir()->szPath, szPath);
	}
	else
	{
		strcpy(szNewPath, szPath);
	}
	_AbsPathResolver(szNewPath, szAbsPath);
}

//------------------------------------------------------------
//------------------------------------------------------------
void  removeTerminalSlash(char * szPath)
{
	int nLen = strlen(szPath);

	while((nLen > 0) && (szPath[ --nLen ] == '/'))
	{
		szPath[ nLen ] = '\0';
	}
}

//------------------------------------------------------------
//------------------------------------------------------------
void  stripLastDirFromPath(char * szPath)
{
	char * cpLastSlash;

	removeTerminalSlash(szPath);
	if('\0' == szPath[0])
	{
		strcpy(szPath, "/");
	}
	else
	{
		cpLastSlash = strrchr(szPath, '/');
		if(cpLastSlash != szPath)
		{
			(*cpLastSlash) = '\0';
		}
		else
		{
			cpLastSlash++;
			(*cpLastSlash) = '\0';
		}
	}
}

//------------------------------------------------------------
//------------------------------------------------------------
bool  isRootPath(const char * szPath)
{
	return(!strcmp(szPath, "/"));
}

//------------------------------------------------------------
//------------------------------------------------------------
bool  readDirectoryList(const char * szPathName, const int nMaxDirEntries, DIR_LIST * dlList)
{
	bool bSuccess;

	// reset directory list entries
	dlList->nEntries = 0;

	// see if we can change into the directory requested
#ifdef DEBUG
	SimpleConsolePrintStringCR(SCREEN_TOP, szPathName);
	SimpleConsolePrintStringCR(SCREEN_TOP, "Changing dir");
#endif
	//   bSuccess = DS_chdir( szPathName );
	bSuccess = setCurrentWorkingDir(szPathName);
	if(!bSuccess)
	{
		dlList->nEntries = 0;
		dlList->cdeDirList[ 0 ].ftType = FT_NONE;
		dlList->cdeDirList[ 0 ].szFileName[ 0 ] = '\0';
		return false;
	}
#ifdef DEBUG
	SimpleConsolePrintStringCR(SCREEN_TOP, "Path changed");
#endif

	// read directory and fill up the entries list
	int nIndex = dlList->nEntries;
	dlList->cdeDirList[ nIndex ].ftType = DS_FindFirstFileLFN(dlList->cdeDirList[ nIndex ].szFileName);
	if(dlList->cdeDirList[ nIndex ].ftType != FT_NONE)
	{
		for(nIndex = 1; nIndex < nMaxDirEntries; nIndex++)
		{
			dlList->cdeDirList[ nIndex ].ftType = DS_FindNextFileLFN(dlList->cdeDirList[ nIndex ].szFileName);
			if(dlList->cdeDirList[ nIndex ].ftType == FT_NONE)
			{
				break;
			}
		}
	}
	dlList->nEntries = nIndex;

	// sort directory list
	sortDirectoryList(dlList);

	return true;
}

//------------------------------------------------------------
//------------------------------------------------------------
int   compareDirEntries(DIR_LIST * dlList, int nFirst, int nSecond)
{
	if(dlList->cdeDirList[ nFirst ].ftType == dlList->cdeDirList[ nSecond ].ftType)
	{
		return strcmp(dlList->cdeDirList[ nFirst ].szFileName, dlList->cdeDirList[ nSecond ].szFileName);
	}
	else
	{
		if(dlList->cdeDirList[ nFirst ].ftType != FT_FILE)
		{
			return -1;
		}
		else
		{
			return 1;
		}
	}
}

//------------------------------------------------------------
//------------------------------------------------------------
void  sortDirectoryList(DIR_LIST * dlList)
{
	int nLoop1, nLoop2, nSmallest;
	CURRENT_DIR_ENTRIES cdeTemp;

	nLoop1 = 0;
	if(0 == strcmp(dlList->cdeDirList[0].szFileName, ".."))
	{
		nLoop1 = 1;
	}
	for(; nLoop1 < dlList->nEntries; ++nLoop1)
	{
		nSmallest = nLoop1;

		// loop through and find the smallest entries
		for(nLoop2 = nLoop1 + 1; nLoop2 < dlList->nEntries; ++nLoop2)
		{
			if(compareDirEntries(dlList, nSmallest, nLoop2) > 0)
			{
				nSmallest = nLoop2;
			}
		}

		// put the smallest entry in front
		if(nSmallest != nLoop1)
		{
			cdeTemp.ftType = dlList->cdeDirList[ nLoop1 ].ftType;
			strcpy(cdeTemp.szFileName, dlList->cdeDirList[ nLoop1 ].szFileName);

			dlList->cdeDirList[ nLoop1 ].ftType = dlList->cdeDirList[ nSmallest ].ftType;
			strcpy(dlList->cdeDirList[ nLoop1 ].szFileName, dlList->cdeDirList[ nSmallest ].szFileName);

			dlList->cdeDirList[ nSmallest ].ftType = cdeTemp.ftType;
			strcpy(dlList->cdeDirList[ nSmallest ].szFileName, cdeTemp.szFileName);
		}
	}
}

//------------------------------------------------------------
//------------------------------------------------------------
DS_DEVICE_INFO * getDeviceInfo(const char * szPath)
{
	unsigned int nID = getDeviceID(szPath);

	if((nID < 0) || (nID >= countof(g_AllDevs)))
	{
		return NULL;
	}
	return &g_AllDevs[ nID ];
}

//------------------------------------------------------------
//------------------------------------------------------------
int getDeviceID(const char * szPath)
{
	int nLoop, nMax, nLen;

	if(isRootPath(szPath))
	{
		return 0;
	}
	if(strStartsWith(szPath, "/"))
	{
		nMax = countof(g_AllDevs);
		for(nLoop = 1; nLoop < nMax; ++nLoop)
		{
			nLen = strlen(g_AllDevs[ nLoop ].szName);
			if(strStartsWith(&szPath[1], g_AllDevs[ nLoop ].szName))
			{
				return nLoop;
			}
		}
		return -1;
	}
	return getCurrentWorkingDir()->nDeviceID;
}

//------------------------------------------------------------
//------------------------------------------------------------
const DS_DIRECTORY_INFO *   getCurrentWorkingDir()
{
	return &g_diCWD;
}

//------------------------------------------------------------
//------------------------------------------------------------
bool setCurrentWorkingDir(const char * szPath)
{
	bool bResult;

#ifdef DEBUG
	SimpleConsolePrintString(SCREEN_TOP, "setCWS:");
	SimpleConsolePrintStringCR(SCREEN_TOP, szPath);
#endif
	bResult = DS_chdir(szPath);
	if(bResult)
	{
		g_diCWD.nDeviceID = getDeviceID(szPath);
		strcpy(g_diCWD.szPath, szPath);
#ifdef DEBUG
		SimpleConsolePrintString(SCREEN_TOP, "CWD:");
		SimpleConsolePrintStringCR(SCREEN_TOP, szPath);
#endif
		return true;
	}
#ifdef DEBUG
	SimpleConsolePrintStringCR(SCREEN_TOP, "SetCWD failed!");
#endif
	return false;
}

//------------------------------------------------------------
//------------------------------------------------------------
bool  DS_chdir(const char * szDirPath)
{
	bool bResult = false;
	DS_DEVICE_INFO *   pDevInfo;
	char *             cpPath;
	char szPath[ DS_MAX_FILENAME_LENGTH ];

	// convert to absolute path
	buildAbsolutePath(szPath, szDirPath);
#ifdef DEBUG
	SimpleConsolePrintString(SCREEN_TOP, "Abs:");
	SimpleConsolePrintStringCR(SCREEN_TOP, szPath);
#endif
	if(isRootPath(szPath))
	{
		return true;
	}

	// if we are not at the root, then we should be able to get a device name
	pDevInfo = getDeviceInfo(szPath);
#ifdef DEBUG
	SimpleConsolePrintString(SCREEN_TOP, "Dev:");
	SimpleConsolePrintStringCR(SCREEN_TOP, pDevInfo->szName);
#endif

	// check to see if device is valid
	if(pDevInfo == NULL)
	{
		return false;
	}

	// base on device type, do stuff
	cpPath = (szPath + strlen(pDevInfo->szName) + 1);
#ifdef DEBUG
	SimpleConsolePrintString(SCREEN_TOP, "cpPath[");
	SimpleConsolePrintString(SCREEN_TOP, cpPath);
	SimpleConsolePrintStringCR(SCREEN_TOP, "]");
#endif
	switch(pDevInfo->ddtType)
	{
	case DSF_FAT:
#ifdef DEBUG
		SimpleConsolePrintStringCR(SCREEN_TOP, "FAT chdir");
#endif
		bResult = FAT_chdir(cpPath);
		break;

	case DSF_SRAM:
	case DSF_GBFS:
		if((0 == strlen(cpPath)) || isRootPath(cpPath))
		{
#ifdef DEBUG
			SimpleConsolePrintStringCR(SCREEN_TOP, "GBFS/SRAM chdir");
#endif
			bResult = true;
		}
		break;

	case DSF_STD:
	default:
		// TODO:
		return false;
		break;
	}

	return bResult;
}

//------------------------------------------------------------
//------------------------------------------------------------
FILE_TYPE   DS_FindFirstFileLFN(char * szFileName)
{
	const DS_DIRECTORY_INFO *   dipCWD = getCurrentWorkingDir();
	FILE_TYPE ftType = FT_NONE;

	switch(g_AllDevs[ dipCWD->nDeviceID ].ddtType)
	{
	case DSF_INTERNAL:
		g_RootDeviceInfo.nCurrentIndex = 1;
		while(g_RootDeviceInfo.nCurrentIndex < g_RootDeviceInfo.nNumFiles)
		{
			if(g_AllDevs[ g_RootDeviceInfo.nCurrentIndex ].bAvailable)
			{
				ftType = FT_DIR;
				strcpy(szFileName, g_AllDevs[ g_RootDeviceInfo.nCurrentIndex ].szName);
				++g_RootDeviceInfo.nCurrentIndex;
				break;
			}
			++g_RootDeviceInfo.nCurrentIndex;
		}
		break;

	case DSF_FAT:
		g_bFATFoundFirst = false;
		ftType = FT_DIR;
		strcpy(szFileName, "..");
		break;

	case DSF_SRAM:
		ftType = FT_DIR;
		strcpy(szFileName, "..");
		g_SRAMDeviceInfo.nCurrentIndex = 0;
		break;

	case DSF_GBFS:
		ftType = FT_DIR;
		strcpy(szFileName, "..");
		g_GBFSDeviceInfo.nCurrentIndex = 0;
		break;

	case DSF_STD:
	default:
		// TODO:
		break;
	}
#ifdef DEBUG
	if(ftType != FT_NONE)
	{
		SimpleConsolePrintString(SCREEN_TOP, "1st Find ");
		SimpleConsolePrintStringCR(SCREEN_TOP, szFileName);
	}
	else
	{
		SimpleConsolePrintStringCR(SCREEN_TOP, "1st Not Found!");
	}
#endif
	return ftType;
}

//------------------------------------------------------------
//------------------------------------------------------------
FILE_TYPE   DS_FindNextFileLFN(char * szFileName)
{
	const DS_DIRECTORY_INFO *   dipCWD = getCurrentWorkingDir();
	FILE_TYPE ftType = FT_NONE;

	switch(g_AllDevs[ dipCWD->nDeviceID ].ddtType)
	{
	case DSF_INTERNAL:
		while(g_RootDeviceInfo.nCurrentIndex < g_RootDeviceInfo.nNumFiles)
		{
			if(g_AllDevs[ g_RootDeviceInfo.nCurrentIndex ].bAvailable)
			{
				ftType = FT_DIR;
				strcpy(szFileName, g_AllDevs[ g_RootDeviceInfo.nCurrentIndex ].szName);
				++g_RootDeviceInfo.nCurrentIndex;
				break;
			}
			++g_RootDeviceInfo.nCurrentIndex;
		}
		break;

	case DSF_FAT:
		while(1)
		{
			if(g_bFATFoundFirst)
			{
				ftType = FAT_FindNextFileLFN(szFileName);
			}
			else
			{
				ftType = FAT_FindFirstFileLFN(szFileName);
				g_bFATFoundFirst = true;
			}
			if((ftType == FT_NONE) || (ftType == FT_FILE))
			{
				break;
			}
			if((0 != strcmp(szFileName, ".")) && (0 != strcmp(szFileName, "..")))
			{
				break;
			}
		}
		break;

	case DSF_SRAM:
		if(g_SRAMDeviceInfo.nCurrentIndex < g_SRAMDeviceInfo.nNumFiles)
		{
			ftType = FT_FILE;
			strcpy(szFileName, g_AllDevs[ g_SRAMDeviceInfo.nCurrentIndex ].szName);
			++g_SRAMDeviceInfo.nCurrentIndex;
		}
		else
		{
			ftType = FT_NONE;
			g_SRAMDeviceInfo.nCurrentIndex = 0;
		}
		break;

	case DSF_GBFS:
		if(g_GBFSDeviceInfo.nCurrentIndex < g_GBFSDeviceInfo.nNumFiles)
		{
			ftType = FT_FILE;
			strcpy(szFileName, PA_GBFSfile[ g_GBFSDeviceInfo.nCurrentIndex ].Name);
			strcat(szFileName, ".");
			strcat(szFileName, PA_GBFSfile[ g_GBFSDeviceInfo.nCurrentIndex ].Ext);
			++g_GBFSDeviceInfo.nCurrentIndex;
		}
		else
		{
			ftType = FT_NONE;
			g_GBFSDeviceInfo.nCurrentIndex = 0;
		}
		break;

	case DSF_STD:
	default:
		// TODO:
		break;
	}

#ifdef DEBUG
	if(ftType != FT_NONE)
	{
		SimpleConsolePrintString(SCREEN_TOP, "Next Find ");
		SimpleConsolePrintStringCR(SCREEN_TOP, szFileName);
	}
	else
	{
		SimpleConsolePrintStringCR(SCREEN_TOP, "Next Not Found!");
	}
#endif
	return ftType;
}

//------------------------------------------------------------
//------------------------------------------------------------
DS_FILE * DS_fopen(const char * szFileName, const char * szMode)
{
	char szAbsName[ DS_MAX_FILENAME_LENGTH ];
	const char *       szFilePath;
	DS_DEVICE_INFO *   dipDevice;

	// get the absolute full file name
	buildAbsolutePath(szAbsName, szFileName);
	dipDevice = getDeviceInfo(szAbsName);

#ifdef DEBUG
	SimpleConsolePrintStringCR(SCREEN_TOP, "Enter fopen");
	SimpleConsolePrintStringCR(SCREEN_TOP, szAbsName);
#endif
	// check to see if we got a valid device
	if(NULL == dipDevice)
	{
#ifdef DEBUG
		SimpleConsolePrintStringCR(SCREEN_TOP, "Invalid device");
		waitVBLDelay(60);
#endif
		return NULL;
	}

	// get file name for device
	szFilePath = getPathWithoutDevice(szAbsName);

#ifdef DEBUG
	SimpleConsolePrintStringCR(SCREEN_TOP, szFilePath);
	waitVBLDelay(60);
#endif
	// make sure we got some memory
	DS_FILE * dfResult = (DS_FILE *)malloc(sizeof(DS_FILE));
	if(NULL == dfResult)
	{
#ifdef DEBUG
		SimpleConsolePrintStringCR(SCREEN_TOP, "Unable to alloc DS_FILE");
		waitVBLDelay(60);
#endif
		return NULL;
	}
	memset(dfResult, 0, sizeof(DS_FILE));

	// now open file base on device type
	switch(dipDevice->ddtType)
	{
	case DSF_FAT:
		{
			CFFSFile *   cffsTemp = new CFFSFile(szFilePath, szMode);
			if(cffsTemp != NULL)
			{
				if(cffsTemp->openedSuccessfully())
				{
					dfResult->ddtDev = DSF_FAT;
					dfResult->vpData = (void *)cffsTemp;
				}
				else
				{
					dfResult->ddtDev = DSF_UNKNOWN;
					dfResult->vpData = NULL;
					delete (cffsTemp);
					cffsTemp = NULL;
				}
			}

			// if we didn't get a proper filesystem, reset
			if(NULL == dfResult->vpData)
			{
				free(dfResult);
				dfResult = NULL;
#ifdef DEBUG
				SimpleConsolePrintStringCR(SCREEN_TOP, "fopen failed");
				waitVBLDelay(120);
#endif
			}
			break;
		}

	case DSF_GBFS:
		{
			// TODO: Error checking + file access modes
			// TODO: get proper file name
			GBFSFile *   gbfsTemp = new GBFSFile(szFilePath, szMode);
			if(gbfsTemp != NULL)
			{
				if(gbfsTemp->openedSuccessfully())
				{
					dfResult->ddtDev = DSF_GBFS;
					dfResult->vpData = (void *)gbfsTemp;
				}
				else
				{
					delete (gbfsTemp);
					gbfsTemp = NULL;
				}
			}

			// if we didn't get a proper filesystem, reset
			if(NULL == dfResult->vpData)
			{
				free(dfResult);
				dfResult = NULL;
#ifdef DEBUG
				SimpleConsolePrintStringCR(SCREEN_TOP, "fopen failed");
				waitVBLDelay(120);
#endif
			}
			break;
		}

	case DSF_INTERNAL:
	case DSF_STD:
	case DSF_SRAM:
	case DSF_UNKNOWN:
	default:
		// TODO:
		break;
	}

	return dfResult;
}

//------------------------------------------------------------
//------------------------------------------------------------
int   DS_fread(void * vpBuffer, size_t nSize, size_t nCount, DS_FILE * dfHandle)
{
	int nResult = 0;

#ifdef DEBUG
	SimpleConsolePrintStringCR(SCREEN_TOP, "FRead");
#endif
	switch(dfHandle->ddtDev)
	{
	case DSF_FAT:
		nResult = ((CFFSFile *)dfHandle->vpData)->_fread(vpBuffer, nSize, nCount);
		break;

	case DSF_GBFS:
		nResult = ((GBFSFile *)dfHandle->vpData)->_fread(vpBuffer, nSize, nCount);
		break;

	case DSF_INTERNAL:
	case DSF_STD:
	case DSF_SRAM:
	case DSF_UNKNOWN:
	default:
		// TODO:
		break;
	}

	return nResult;
}

//------------------------------------------------------------
//------------------------------------------------------------
int   DS_feof(DS_FILE * dfHandle)
{
#ifdef DEBUG
	SimpleConsolePrintStringCR(SCREEN_TOP, "FEOF");
#endif
	int nResult = -1;

	switch(dfHandle->ddtDev)
	{
	case DSF_FAT:
		nResult = ((CFFSFile *)dfHandle->vpData)->_feof();
		break;

	case DSF_GBFS:
		nResult = ((GBFSFile *)dfHandle->vpData)->_feof();
		break;

	case DSF_INTERNAL:
	case DSF_STD:
	case DSF_SRAM:
	case DSF_UNKNOWN:
	default:
		// TODO:
		break;
	}

	return nResult;
}

//------------------------------------------------------------
//------------------------------------------------------------
size_t   DS_getFileSize(DS_FILE * dfHandle)
{
#ifdef DEBUG
	SimpleConsolePrintStringCR(SCREEN_TOP, "GetFileSize");
#endif
	int nResult = 0;

	switch(dfHandle->ddtDev)
	{
	case DSF_FAT:
		nResult = ((CFFSFile *)dfHandle->vpData)->getFileSize();
		break;

	case DSF_GBFS:
		nResult = ((GBFSFile *)dfHandle->vpData)->getFileSize();
		break;

	case DSF_INTERNAL:
	case DSF_STD:
	case DSF_SRAM:
	case DSF_UNKNOWN:
	default:
		// TODO:
		break;
	}

	return nResult;
}

//------------------------------------------------------------
//------------------------------------------------------------
int   DS_fclose(DS_FILE * dfHandle)
{
#ifdef DEBUG
	SimpleConsolePrintStringCR(SCREEN_TOP, "FClose");
#endif
	int nResult = 0;

	switch(dfHandle->ddtDev)
	{
	case DSF_FAT:
		{
			CFFSFile *   cffsTemp = ((CFFSFile *)dfHandle->vpData);
			if(cffsTemp)
			{
				nResult = cffsTemp->_fclose();
				delete cffsTemp;
				dfHandle->vpData = NULL;
			}
			free(dfHandle);
			break;
		}

	case DSF_GBFS:
		{
			GBFSFile *   gbfsTemp = ((GBFSFile *)dfHandle->vpData);
			if(gbfsTemp)
			{
				nResult = gbfsTemp->_fclose();
				delete gbfsTemp;
				dfHandle->vpData = NULL;
			}
			free(dfHandle);
			break;
		}

	case DSF_INTERNAL:
	case DSF_STD:
	case DSF_SRAM:
	case DSF_UNKNOWN:
	default:
		// TODO:
		break;
	}

	return nResult;
}

//------------------------------------------------------------
//------------------------------------------------------------
int   DS_ferror(DS_FILE * dfHandle)
{
#ifdef DEBUG
	SimpleConsolePrintStringCR(SCREEN_TOP, "FError");
#endif
	int nResult = 0;

	switch(dfHandle->ddtDev)
	{
	case DSF_FAT:
		nResult = ((CFFSFile *)dfHandle->vpData)->_ferror();
		break;

	case DSF_GBFS:
		nResult = ((GBFSFile *)dfHandle->vpData)->_ferror();
		break;

	case DSF_INTERNAL:
	case DSF_STD:
	case DSF_SRAM:
	case DSF_UNKNOWN:
	default:
		// TODO:
		break;
	}

	return nResult;
}

//------------------------------------------------------------
//------------------------------------------------------------
int   DS_fscanf(DS_FILE * dfHandle, const char * format, ...)
{
#ifdef DEBUG
	SimpleConsolePrintStringCR(SCREEN_TOP, "FScanf");
#endif
	va_list vlTemp;
	int nResult = EOF;

	switch(dfHandle->ddtDev)
	{
	case DSF_FAT:
		//      nResult  = ( ( CFFSFile* ) dfHandle->vpData )->_fprintf();
		{
			va_start(vlTemp, format);
			nResult = input((CFFSFile *)dfHandle->vpData, (const unsigned char *)format, vlTemp);
		}
		break;

	case DSF_GBFS:
		// TODO:
		//      nResult  = ( ( GBFSFile* ) dfHandle->vpData )->_fprintf();
		{
			va_start(vlTemp, format);
			nResult = input((GBFSFile *)dfHandle->vpData, (const unsigned char *)format, vlTemp);
		}
		break;

	case DSF_INTERNAL:
	case DSF_STD:
	case DSF_SRAM:
	case DSF_UNKNOWN:
	default:
		// TODO:
		break;
	}

	return nResult;
}

//------------------------------------------------------------
//------------------------------------------------------------
int   DS_fgetc(DS_FILE * dfHandle)
{
#ifdef DEBUG
	SimpleConsolePrintStringCR(SCREEN_TOP, "FGetC");
#endif
	int cResult = EOF;

	switch(dfHandle->ddtDev)
	{
	case DSF_FAT:
		cResult = ((CFFSFile *)dfHandle->vpData)->_fgetc();
		break;

	case DSF_GBFS:
		cResult = ((GBFSFile *)dfHandle->vpData)->_fgetc();
		break;

	case DSF_STD:
	case DSF_INTERNAL:
	case DSF_SRAM:
	case DSF_UNKNOWN:
	default:
		// TODO:
		break;
	}

	return cResult;
}

//------------------------------------------------------------
//------------------------------------------------------------
int   DS_fungetc(int nChar, DS_FILE * dfHandle)
{
#ifdef DEBUG
	SimpleConsolePrintStringCR(SCREEN_TOP, "FGetC");
#endif
	int nResult = EOF;

	switch(dfHandle->ddtDev)
	{
	case DSF_FAT:
		nResult = ((CFFSFile *)dfHandle->vpData)->_fungetc(nChar);
		break;

	case DSF_GBFS:
		nResult = ((GBFSFile *)dfHandle->vpData)->_fungetc(nChar);
		break;

	case DSF_STD:
	case DSF_INTERNAL:
	case DSF_SRAM:
	case DSF_UNKNOWN:
	default:
		// TODO:
		break;
	}

	return nResult;
}

//------------------------------------------------------------
//------------------------------------------------------------
char * DS_fgets(char * szString, int nNum, DS_FILE * dfHandle)
{
#ifdef DEBUG
	SimpleConsolePrintStringCR(SCREEN_TOP, "FGets");
#endif
	char * cbResult = NULL;

	switch(dfHandle->ddtDev)
	{
	case DSF_FAT:
		cbResult = ((CFFSFile *)dfHandle->vpData)->_fgets(szString, nNum);
		break;

	case DSF_GBFS:
		cbResult = ((GBFSFile *)dfHandle->vpData)->_fgets(szString, nNum);
		break;

	case DSF_STD:
	case DSF_INTERNAL:
	case DSF_SRAM:
	case DSF_UNKNOWN:
	default:
		// TODO:
		break;
	}

	return cbResult;
}

//------------------------------------------------------------
//------------------------------------------------------------
size_t   DS_fwrite(const void * vpBuffer, size_t nSize, size_t nCount, DS_FILE * dfHandle)
{
#ifdef DEBUG
	SimpleConsolePrintStringCR(SCREEN_TOP, "FWrite");
#endif
	size_t nResult = 0;

	switch(dfHandle->ddtDev)
	{
	case DSF_FAT:
		nResult = ((CFFSFile *)dfHandle->vpData)->_fwrite(vpBuffer, nSize, nCount);
		break;

	case DSF_GBFS:
	case DSF_INTERNAL:
	case DSF_STD:
	case DSF_SRAM:
	case DSF_UNKNOWN:
	default:
		// TODO:
		break;
	}

	return nResult;
}

//------------------------------------------------------------
//------------------------------------------------------------
int   DS_fseek(DS_FILE * dfHandle, long lOffset, int lOrigin)
{
#ifdef DEBUG
	SimpleConsolePrintStringCR(SCREEN_TOP, "FSeek");
#endif
	int nResult = -1;

	switch(dfHandle->ddtDev)
	{
	case DSF_FAT:
		nResult = ((CFFSFile *)dfHandle->vpData)->_fseek(lOffset, lOrigin);
		break;

	case DSF_GBFS:
		nResult = ((GBFSFile *)dfHandle->vpData)->_fseek(lOffset, lOrigin);
		break;

	case DSF_INTERNAL:
	case DSF_STD:
	case DSF_SRAM:
	case DSF_UNKNOWN:
	default:
		// TODO:
		break;
	}

	return nResult;
}
//------------------------------------------------------------
//------------------------------------------------------------
int   DS_fprintf(DS_FILE * dfHandle, const char * format, ...)
{
#ifdef DEBUG
	SimpleConsolePrintStringCR(SCREEN_TOP, "FPrintf");
#endif
	int nResult = -1;

	switch(dfHandle->ddtDev)
	{
	case DSF_FAT:
		//      nResult  = ( ( CFFSFile* ) dfHandle->vpData )->_fprintf();
		{
			va_list vlTemp;
			int nCount = 0;
			int nWriteRes = -1;
			char cbOutput[1024];

			// make sure we have a handle first
			if(dfHandle != NULL)
			{
				// print to a temp string buffer first
				// TODO: potential buffer overflow
				va_start(vlTemp, format);
				nCount = vsprintf(cbOutput, format, vlTemp);
				va_end(vlTemp);

				// now output it to the file
				nWriteRes = DS_fwrite(cbOutput, 1, nCount, dfHandle);
				if(nWriteRes == nCount)
				{
					nResult = nCount;
				}
			}
		}
		break;

	case DSF_GBFS:
	case DSF_INTERNAL:
	case DSF_STD:
	case DSF_SRAM:
	case DSF_UNKNOWN:
	default:
		// TODO:
		break;
	}

	return nResult;
}

//------------------------------------------------------------
//------------------------------------------------------------
long     DS_ftell(DS_FILE * dfHandle)
{
#ifdef DEBUG
	SimpleConsolePrintStringCR(SCREEN_TOP, "FTell");
#endif
	long nResult = -1L;

	switch(dfHandle->ddtDev)
	{
	case DSF_FAT:
		nResult = ((CFFSFile *)dfHandle->vpData)->_ftell();
		break;

	case DSF_GBFS:
		nResult = ((GBFSFile *)dfHandle->vpData)->_ftell();
		break;

	case DSF_INTERNAL:
	case DSF_STD:
	case DSF_SRAM:
	case DSF_UNKNOWN:
	default:
		// TODO:
		break;
	}

	return nResult;
}

//------------------------------------------------------------
//------------------------------------------------------------
int   DS_fflush(DS_FILE * dfHandle)
{
#ifdef DEBUG
	SimpleConsolePrintStringCR(SCREEN_TOP, "FFlush");
#endif
	int nResult = EOF;

	switch(dfHandle->ddtDev)
	{
	case DSF_FAT:
		nResult = ((CFFSFile *)dfHandle->vpData)->_fflush();
		break;

	case DSF_GBFS:
	case DSF_INTERNAL:
	case DSF_STD:
	case DSF_SRAM:
	case DSF_UNKNOWN:
	default:
		// TODO:
		break;
	}

	return nResult;
}

//------------------------------------------------------------
// TODO: implement
//------------------------------------------------------------
int   DS_remove(const char *)
{
#ifdef DEBUG
	SimpleConsolePrintStringCR(SCREEN_TOP, "Remove");
#endif
	int nResult = -1;

	//   switch( dfHandle->ddtDev ) {
	//   case DSF_FAT:
	////      nResult  = ( ( CFFSFile* ) dfHandle->vpData )->_fprintf();
	//      break;
	//   case DSF_GBFS:
	//   case DSF_INTERNAL:
	//   case DSF_STD:
	//   case DSF_SRAM:
	//   case DSF_UNKNOWN:
	//   default:
	//      // TODO:
	//      break;
	//   }

	return nResult;
}

//------------------------------------------------------------
// TODO: implement
//------------------------------------------------------------
int   DS_rename(const char *, const char *)
{
#ifdef DEBUG
	SimpleConsolePrintStringCR(SCREEN_TOP, "Rename");
#endif
	int nResult = -1;

	//   switch( dfHandle->ddtDev ) {
	//   case DSF_FAT:
	////      nResult  = ( ( CFFSFile* ) dfHandle->vpData )->_fprintf();
	//      break;
	//   case DSF_GBFS:
	//   case DSF_INTERNAL:
	//   case DSF_STD:
	//   case DSF_SRAM:
	//   case DSF_UNKNOWN:
	//   default:
	//      // TODO:
	//      break;
	//   }

	return nResult;
}



