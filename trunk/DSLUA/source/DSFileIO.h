#ifndef _DSFILEIO_H_
#define _DSFILEIO_H_

#include <stddef.h>
#include "gba_nds_fat.h"
#include "gbfs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define  DS_MAX_FILENAME_LENGTH     (MAX_FILENAME_LENGTH + 16)
#define  DS_MAX_FILES_PER_DIRECTORY 128

// typedefs
typedef struct
{
    FILE_TYPE ftType;
    char szFileName[ DS_MAX_FILENAME_LENGTH ];
} CURRENT_DIR_ENTRIES;

typedef struct
{
    int nEntries;
    CURRENT_DIR_ENTRIES cdeDirList[ DS_MAX_FILES_PER_DIRECTORY ];
} DIR_LIST;

typedef enum {DSF_INTERNAL, DSF_STD, DSF_FAT, DSF_SRAM, DSF_GBFS, DSF_UNKNOWN} DSF_DEVICE_TYPE;

typedef struct
{
    DSF_DEVICE_TYPE ddtType;
    char *             szName;
    bool bAvailable;
} DS_DEVICE_INFO;

typedef struct
{
    int nDeviceID;
    char szPath[ DS_MAX_FILENAME_LENGTH ];
} DS_DIRECTORY_INFO;

typedef struct
{
    u32 nIndex;
    u32 nLen;
    const void * vpData;
} DS_GBFS;

typedef struct
{
    DSF_DEVICE_TYPE ddtDev;
    void *             vpData;
} DS_FILE;

bool DSFATDevice_Init();
bool DSIODevice_Init();
void DSFATDevice_CleanUp();
void DSIODevice_CleanUp();

bool  isDeviceNeeded(const DSF_DEVICE_TYPE);
void  buildPath(char *, const char *, const char *);
void  buildAbsolutePath(char *, const char *);
void  removeTerminalSlash(char *);
void  stripLastDirFromPath(char *);
bool  isRootPath(const char *);
int   compareDirEntries(DIR_LIST *, int, int);
void  sortDirectoryList(DIR_LIST *);
bool  readDirectoryList(const char *, const int, DIR_LIST *);

DS_DEVICE_INFO *            getDeviceInfo(const char *);
int                        getDeviceID(const char *);
const DS_DIRECTORY_INFO *   getCurrentWorkingDir();
bool                       setCurrentWorkingDir(const char *);

bool        DS_chdir(const char *);
FILE_TYPE   DS_FindFirstFileLFN(char *);
FILE_TYPE   DS_FindNextFileLFN(char *);

DS_FILE * DS_fopen(const char *, const char *);
int      DS_fread(void *, size_t, size_t, DS_FILE *);
int      DS_feof(DS_FILE *);
size_t   DS_getFileSize(DS_FILE *);
int      DS_fclose(DS_FILE *);
int      DS_ferror(DS_FILE *);
int      DS_fputs(const char *, DS_FILE *);
int      DS_fprintf(DS_FILE *, const char * format, ...);
int      DS_fscanf(DS_FILE *, const char * format, ...);
char *    DS_fgets(char *, int, DS_FILE *);
int      DS_fgetc(DS_FILE *);
int      DS_fungetc(int, DS_FILE *);
size_t   DS_fwrite(const void *, size_t, size_t, DS_FILE *);
int      DS_fseek(DS_FILE *, long, int);
long     DS_ftell(DS_FILE *);
int      DS_fflush(DS_FILE *);
int      DS_remove(const char *);
int      DS_rename(const char *, const char *);


extern DIR_LIST g_dlList;
extern DS_FILE DSF_STDOUT;
#ifdef __cplusplus
}          // extern "C"
#endif

#endif //_DSFILEIO_H_

