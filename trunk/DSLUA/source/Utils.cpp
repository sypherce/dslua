#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "DSLScreen.h"
#include "SimpleConsole.h"
#include "Utils.h"
#include "DSLVersion.h"

char g_szVersion[16] = "";

//------------------------------------------------------------
//------------------------------------------------------------
char const * const getSystemVersion()
{
    if(strlen(g_szVersion) <= 0)
    {
        sprintf(g_szVersion, "%d.%d", DSLVERSION_MAJOR, DSLVERSION_MINOR);
    }
    return g_szVersion;
}

//------------------------------------------------------------
//------------------------------------------------------------
bool  strStartsWith(const char * szSrc, const char * szSubStr, bool bCaseSensitive)
{
    int nIndex = 0;

    if(bCaseSensitive)
    {
        while((szSubStr[ nIndex ]) && (szSrc[ nIndex ] == szSubStr[ nIndex ]))
        {
            nIndex++;
        }
    }
    else
    {
        while((szSubStr[ nIndex ]) && (toupper(szSrc[ nIndex ]) == toupper(szSubStr[ nIndex ])))
        {
            nIndex++;
        }
    }
    if(szSubStr[ nIndex ])
    {
        return false;
    }
    return true;
}

//------------------------------------------------------------
//------------------------------------------------------------
bool  strEndsWith(const char * szSrc, const char * szSubStr, bool bCaseSensitive)
{
    int nLenA = strlen(szSrc),
        nLenB = strlen(szSubStr);

    if(nLenB > nLenA)
    {
        return false;
    }
    if(bCaseSensitive)
    {
        if(!strcmp(szSrc + (nLenA - nLenB), szSubStr))
        {
            return true;
        }
    }
    else
    {
        if(!strcmpi(szSrc + (nLenA - nLenB), szSubStr))
        {
            return true;
        }
    }
    return false;
}

//------------------------------------------------------------
//------------------------------------------------------------
void  waitVBLDelay(const int nWait)
{
    int nDelay = nWait;

    while(nDelay > 0)
    {
        screenWaitForVBL();
        --nDelay;
    }
}

//------------------------------------------------------------
//------------------------------------------------------------
void  waitUntilKeyPressed(bool * bButton)
{
    // check to see if already pressed, wait until not pressed
    if(*bButton)
    {
        while(*bButton)
        {
            screenWaitForVBL();
        }
    }

    // wait for the button to be pressed
    while(!(*bButton))
    {
        screenWaitForVBL();
    }

    // wait for the button to be released
    while(*bButton)
    {
        screenWaitForVBL();
//      PA_WaitForVBL();
    }
}

//------------------------------------------------------------
//------------------------------------------------------------
void  waitUntilDMAFree(int nChannel)
{
    while(((*(volatile unsigned int *)(0x040000B8 + (nChannel * 12))) & 0x80000000)) ;
}

//------------------------------------------------------------
//------------------------------------------------------------
unsigned int   calcCheckSum(const void * vpData, const int nSize)
{
    int nLoop;
    char * cbTemp = (char *)vpData;

    unsigned int nSum = 0;

    for(nLoop = 0; nLoop < nSize; ++nLoop)
    {
        if(nSum & 0x80000000)
        {
            nSum = (((nSum << 1) | 1) ^ cbTemp[ nLoop ]);
        }
        else
        {
            nSum = ((nSum << 1) ^ cbTemp[ nLoop ]);
        }
    }
    return nSum;
}

AlignedMemory::AlignedMemory(unsigned int nBits, unsigned int nSize)
    :  vpAlignedMem(NULL),
    vpRealMem(NULL)
{
    unsigned int uBad = ((1 << nBits) - 1);
    unsigned int nNewSize = ((nSize + (2 << nBits)) & (~uBad));
    int nLoop;

    for(nLoop = 0; nLoop < 5; ++nLoop)
    {
        vpRealMem = malloc(nNewSize);
        if(vpRealMem)
        {
            vpAlignedMem = (void *)((((unsigned int)vpRealMem) + uBad) & (~uBad));
            m_nSize = nSize;
            m_nRealSize = nNewSize;
            break;
        }
        screenWaitForVBL();
    }
}

AlignedMemory::AlignedMemory()
    :  vpAlignedMem(NULL),
    vpRealMem(NULL)
{
}

AlignedMemory::~AlignedMemory()
{
    if(vpRealMem)
    {
        free(vpRealMem);
        vpRealMem = NULL;
        vpAlignedMem = NULL;
    }
}

