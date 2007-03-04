#include <PA9.h>
#include "DSLFileObj.h"

extern "C" {
#include <stdio.h>
#include <stdarg.h>
#include "gba_nds_fat.h"
#include "disc_io.h"
}

#include "sanos_input.h"

//#define  DEBUG

#ifdef DEBUG
#include "Utils.h"
#include "SimpleConsole.h"
#endif   //DEBUG

//------------------------------------------------------------
//----- DSLFileObj base class
//------------------------------------------------------------
DSLFileObj::DSLFileObj( const char* szName, const char* szMode )
:  m_bOpened( false )
{
   strcpy( m_szFileName, szName );
   strcpy( m_szMode, szMode );
}

DSLFileObj::~DSLFileObj()
{
}

//------------------------------------------------------------
//----- GBFSFile class
//------------------------------------------------------------
GBFSFile::GBFSFile( const char* szFileName, const char* szMode )
:  DSLFileObj( szFileName, szMode ),
   m_nIndex( 0 ),
   m_nLen( 0 ),
   m_vpData( NULL )
{
   // TODO: Error checking + file access modes
   u32      nFSize;
   if ( '/' == szFileName[0] ) {
      m_vpData = gbfs_get_obj( PA_GBFS_FILE, &szFileName[1], &nFSize );
   } else {
      m_vpData = gbfs_get_obj( PA_GBFS_FILE, szFileName, &nFSize );
   }
   if ( NULL != m_vpData ) {
      m_nIndex    = 0;
      m_nLen      = nFSize;
      m_bOpened   = true;
   }
}

//------------------------------------------------------------
//------------------------------------------------------------
GBFSFile::~GBFSFile()
{
   if ( m_vpData ) {
      _fclose();
   }
}

//------------------------------------------------------------
//------------------------------------------------------------
int   GBFSFile::_fread ( void *vpBuffer, size_t nSize, size_t nCount )
{
   int   nResult  = 0;

   // perform read if we have data
   if ( m_vpData != NULL ) {
      u32      nTotalBytes = nSize * nCount;
      u32      nBytesLeft  = ( m_nLen - m_nIndex );
      u32      nItems;

      // figure out how many items we can read maximum
      nItems   = ( nBytesLeft / nSize );
      if ( nItems > nCount ) {
         nItems   = nCount;
      }
      nTotalBytes = nItems * nSize;
#ifdef DEBUG
      char szDbgTemp[128];
      sprintf( szDbgTemp, "Want %d items of size %d, has %d", nCount, nSize, nItems );
      SimpleConsolePrintStringCR( SCREEN_TOP, szDbgTemp );
#endif

      // perform "read"
#ifdef DEBUG
      sprintf( szDbgTemp, "GBFS Read %d bytes", nTotalBytes );
      SimpleConsolePrintStringCR( SCREEN_TOP, szDbgTemp );
#endif
      memcpy( vpBuffer, ( void* )( ( ( char* )m_vpData ) + m_nIndex ), nTotalBytes );
#ifdef DEBUG
      u32      nSum1, nSum2;
      nSum1 = calcCheckSum( vpBuffer, nTotalBytes );
      sprintf( szDbgTemp, "DestSum: %x", nSum1 );
      SimpleConsolePrintStringCR( SCREEN_TOP, szDbgTemp );
      nSum2 = calcCheckSum( ( void* )( ( ( char* )m_vpData ) + m_nIndex ), nTotalBytes );
      sprintf( szDbgTemp, "SrcSum : %x", nSum2 );
      SimpleConsolePrintStringCR( SCREEN_TOP, szDbgTemp );
      if ( nSum1 != nSum2 ) {
         SimpleConsolePrintStringCR( SCREEN_TOP, "ERROR: memcpy failed" );
      }
#endif
      m_nIndex += nTotalBytes;
      nResult  = nItems;
//#ifdef DEBUG
//      int nDbgTemp;
//      for ( nDbgTemp = 0; nDbgTemp < 20; ++nDbgTemp ) {
//         szDbgTemp[ nDbgTemp ]   = ( ( char*) vpBuffer )[ nDbgTemp ];
//      }
//      szDbgTemp[ nDbgTemp ]   = '\0';
//      SimpleConsolePrintStringCR( SCREEN_TOP, szDbgTemp );
//#endif
   }
   return nResult;
}

//------------------------------------------------------------
//------------------------------------------------------------
int   GBFSFile::_feof()
{
   int   nResult  = -1;

   if ( m_vpData != NULL ) {
      u32      nBytesLeft  = ( m_nLen - m_nIndex );

      if ( nBytesLeft > 0 ) {
#ifdef DEBUG
         SimpleConsolePrintStringCR( SCREEN_TOP, "0 returned" );
#endif
         nResult  = 0;
      }
   }
   return nResult;
}


//------------------------------------------------------------
//------------------------------------------------------------
int   GBFSFile::_fclose()
{
   // reset everything
   m_vpData    = NULL;
   m_nIndex    = 0;
   m_nLen      = 0;
   m_bOpened   = false;

   return 0;
}


//------------------------------------------------------------
//------------------------------------------------------------
int   GBFSFile::_ferror()
{
   // TODO:
   int   nResult  = 0;

   return nResult;
}


//------------------------------------------------------------
//------------------------------------------------------------
int   GBFSFile::_fputs ( const char* )
{
   // return EOF because can't write to GBFS
   int   nResult  = EOF;

   return nResult;
}


//------------------------------------------------------------
//------------------------------------------------------------
int   GBFSFile::_fprintf( const char *format, ... )
{
   int   nResult  = -1;

   return nResult;
}

//------------------------------------------------------------
//------------------------------------------------------------
int   GBFSFile::_fscanf( const char *format, ... )
{
  int nScanned;
  va_list vlTemp;

  va_start( vlTemp, format );
  nScanned  = input( this, ( const unsigned char* )format, vlTemp );

  return nScanned;
}

//------------------------------------------------------------
//------------------------------------------------------------
char* GBFSFile::_fgets( char* szBuffer, int nSize )
{
	u32 curPos;
	u32 readLength;
	char *returnChar;
	
	// invalid filehandle
	if ( NULL == m_vpData ) {
		return NULL ;
	}
	
	// end of file
	if ( _feof() == true ) {
		return NULL ;
	}
	
	// save current position
	curPos = _ftell();
	
	// read the full buffer (max string chars is nSize-1 and one end of string \0
	readLength = _fread( szBuffer, 1, nSize-1 );
	
	// mark least possible end of string
	szBuffer[readLength] = '\0' ;
	
	if (readLength==0) {
		// return error
		return NULL ;
	}
	
	// get position of first return '\r'
	returnChar = strchr(szBuffer,'\r');
	
	// if no return is found, search for a newline
	if (returnChar == NULL) {
		returnChar = strchr(szBuffer,'\n');
	}
	
	// Mark the return, if existant, as end of line/string
	if (returnChar!=NULL) {
		*( returnChar ) = 0 ;
		if ( * ( returnChar + 1 ) =='\n') {
         // catch newline too when jumping over the end
			// return to location after \r\n (strlen+2)
			_fseek( curPos + strlen( szBuffer ) + 2, SEEK_SET );
         strcat( szBuffer, "\r" );
			return szBuffer ;
		} else {
			// return to location after \r (strlen+1)
			_fseek( curPos + strlen( szBuffer ) + 1, SEEK_SET );
         strcat( szBuffer, "\r" );
			return szBuffer ;
		}
	}
	
	return szBuffer ;
}

//------------------------------------------------------------
//------------------------------------------------------------
int   GBFSFile::_fgetc()
{
	char c;
	return ( _fread( &c, 1, 1 ) == 1 ) ? c : EOF;
}

//------------------------------------------------------------
//------------------------------------------------------------
int   GBFSFile::_fungetc( int cOldChar )
{
   // TODO: put cOldChar back in stream
   if ( 0 != _fseek( -1, SEEK_CUR ) ) {
      return EOF;
   }
   return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
size_t   GBFSFile::_fwrite( const void*, size_t, size_t )
{
   // TODO:
   return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
int   GBFSFile::_fseek( long offset, int origin )
{
	u32 position;
	u32 curPos;
	
   // invalid filehandle
	if ( NULL == m_vpData ) {
		return -1;
	}

   // remember current position
	curPos = m_nIndex;
	
	switch( origin )
	{
	case SEEK_SET:
		if (offset >= 0) {
			position = offset;
		} else {
			// Tried to seek before start of file
			position = 0;
		}
		break;
	case SEEK_CUR:
		if ( offset >= 0 ) {
			position = curPos + offset;
		} else if ( (u32)( offset * -1) >= curPos ) {
			// Tried to seek before start of file
			position = 0;
		} else {
			// Using u32 to maintain 32 bits of accuracy
			position = curPos - (u32)(offset * -1);
		}
		break;
	case SEEK_END:
		if (offset >= 0) {
			// Seeking to end of file
			position = m_nLen;	// Fixed thanks to MoonLight
		} else if ( (u32)(offset * -1) >= m_nLen ) {
			// Tried to seek before start of file
			position = 0;
		} else {
			// Using u32 to maintain 32 bits of accuracy
			position = m_nLen - (u32)(offset * -1);
		}
		break;
	default:
		return -1;
	}

	if ( position > m_nLen ) {
		// Tried to go past end of file
		position = m_nLen;
	}
	
	// Save position
	m_nIndex = position;

   return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
long  GBFSFile::_ftell()
{
   // invalid filehandle
	if ( NULL == m_vpData ) {
		return -1;
	}

   return m_nIndex;
}

//------------------------------------------------------------
//------------------------------------------------------------
int   GBFSFile::_fflush()
{
   // TODO:
   return EOF;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//------------------------------------------------------------
//----- CFFSFile class
//------------------------------------------------------------
CFFSFile::CFFSFile( const char* szFileName, const char* szMode )
:  DSLFileObj( szFileName, szMode ),
   m_vpData( NULL ),
   m_uSize( 0 )
{
#ifdef DEBUG
      char szDbgTemp[128];
      sprintf( szDbgTemp, "CF fopen mode %s", szMode );
      SimpleConsolePrintStringCR( SCREEN_TOP, szDbgTemp );
#endif
   // TODO: Error checking + file access modes
   m_vpData = ( void * )FAT_fopen( szFileName, szMode );
   if ( NULL != m_vpData ) {
      m_uSize     = FAT_GetFileSize();
      m_bOpened   = true;
   } else {
      m_bOpened   = false;
   }
}

//------------------------------------------------------------
//------------------------------------------------------------
CFFSFile::~CFFSFile()
{
   if ( m_vpData ) {
      _fclose();
   }
}

//------------------------------------------------------------
//------------------------------------------------------------
unsigned int   CFFSFile::getFileSize()
{
   return m_uSize;
}

//------------------------------------------------------------
//------------------------------------------------------------
int   CFFSFile::_fread ( void *vpBuffer, size_t nSize, size_t nCount )
{
   int   nResult  = 0;

   // perform read if we have data
   if ( m_vpData != NULL ) {
      nResult  = FAT_fread( vpBuffer, nSize, nCount, ( FAT_FILE* )m_vpData );
#ifdef DEBUG
      u32   nSum1;
      char  szDbgTemp[128];
      nSum1 = calcCheckSum( vpBuffer, nResult * nSize );
      sprintf( szDbgTemp, "FATSum: %x", nSum1 );
      SimpleConsolePrintStringCR( SCREEN_TOP, szDbgTemp );
#endif
   }
   return nResult;
}

//------------------------------------------------------------
//------------------------------------------------------------
int   CFFSFile::_feof()
{
   int   nResult  = -1;

   if ( m_vpData != NULL ) {
      nResult  = ( FAT_feof( ( FAT_FILE* )m_vpData )? -1: 0 );
   }
   return nResult;
}


//------------------------------------------------------------
//------------------------------------------------------------
int   CFFSFile::_fclose()
{
   int   nResult  = 0;

   // reset everything
   if ( m_vpData != NULL ) {
      if ( FAT_fclose( ( FAT_FILE*) m_vpData ) ) {
         nResult  = 0;
      } else {
         // TODO: deal with error
         nResult  = EOF;
      }
   }
   m_vpData    = NULL;
   m_bOpened   = false;

   return nResult;
}


//------------------------------------------------------------
//------------------------------------------------------------
int   CFFSFile::_ferror()
{
   // TODO:
   int   nResult  = 0;

   return nResult;
}


//------------------------------------------------------------
//------------------------------------------------------------
int   CFFSFile::_fputs ( const char* szString )
{
   // return EOF because can't write to CFFS
   int   nResult  = EOF;

   if ( m_vpData != NULL ) {
      nResult  = FAT_fputs( szString, ( FAT_FILE* ) m_vpData );
   }

   return nResult;
}


//------------------------------------------------------------
//------------------------------------------------------------
int   CFFSFile::_fprintf( const char *format, ... )
{
   va_list  vlTemp;
   int      nCount   = 0;
   int      nResult  = -1;
   char     cbOutput[1024];

   // make sure we have a handle first
   if ( m_vpData == NULL ) {
      return -1;
   }

   // print to a temp string buffer first
   // TODO: potential buffer overflow
   va_start( vlTemp, format );
   nCount   = vsprintf( cbOutput, format, vlTemp );
   va_end( vlTemp );

   // now output it to the file
   nResult  = FAT_fwrite( cbOutput, 1, nCount, ( FAT_FILE* ) m_vpData );
   if ( nResult != nCount ) {
      return -1;
   }

   return nCount;
}

//------------------------------------------------------------
//------------------------------------------------------------
int   CFFSFile::_fscanf( const char *format, ... )
{
   int nScanned;
   va_list vlTemp;

   va_start( vlTemp, format );
   nScanned  = input( this, ( const unsigned char* )format, vlTemp );

   return nScanned;
}

//------------------------------------------------------------
//------------------------------------------------------------
char* CFFSFile::_fgets( char* tgtBuffer, int num )
{
   char* cbResult = NULL;

   if ( m_vpData != NULL ) {
      cbResult  = FAT_fgets( tgtBuffer, num, ( FAT_FILE* ) m_vpData );
   }
   return cbResult;
}

//------------------------------------------------------------
//------------------------------------------------------------
int   CFFSFile::_fgetc()
{
   int   nResult  = EOF;

   if ( m_vpData != NULL ) {
      nResult  = FAT_fgetc( ( FAT_FILE* ) m_vpData );
   }

   return nResult;
}

//------------------------------------------------------------
//------------------------------------------------------------
int   CFFSFile::_fungetc( int cOldChar )
{
   // TODO: put cOldChar back in stream
   if ( 0 != _fseek( -1, SEEK_CUR ) ) {
      return EOF;
   }
   return 0;
}

//------------------------------------------------------------
//------------------------------------------------------------
size_t   CFFSFile::_fwrite( const void* vpBuffer, size_t nSize, size_t nCount )
{
   size_t   nResult  = 0;

   if ( m_vpData != NULL ) {
      nResult  = FAT_fwrite( vpBuffer, nSize, nCount, ( FAT_FILE* ) m_vpData );
   }

   return nResult;
}

//------------------------------------------------------------
//------------------------------------------------------------
int   CFFSFile::_fseek( long lOffset, int nOrigin )
{
   int   nResult  = -1;

   if ( m_vpData != NULL ) {
      nResult  = FAT_fseek( ( FAT_FILE* ) m_vpData, lOffset, nOrigin );
   }

   return nResult;
}

//------------------------------------------------------------
//------------------------------------------------------------
long  CFFSFile::_ftell()
{
   long  lResult  = -1L;

   if ( m_vpData != NULL ) {
      lResult  = FAT_ftell( ( FAT_FILE* ) m_vpData );
   }

   return lResult;
}

//------------------------------------------------------------
//------------------------------------------------------------
int   CFFSFile::_fflush()
{
   // TODO:
   return ( disc_CacheFlush()? 0 : EOF );
}


