#ifndef _DSFILEIO_H_
#define _DSFILEIO_H_

#include <stddef.h>
#include <fat.h>
#include "gbfs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define  MAX_FILENAME_LENGTH        256
#define  DS_MAX_FILENAME_LENGTH     (MAX_FILENAME_LENGTH + 16)
#define  DS_MAX_FILES_PER_DIRECTORY 128
size_t   getFileSize(FILE *);
#ifdef __cplusplus
}          // extern "C"
#endif

#endif //_DSFILEIO_H_

