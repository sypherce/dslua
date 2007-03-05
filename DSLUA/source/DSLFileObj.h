#ifndef _DSLFILEOBJ_H_
#define _DSLFILEOBJ_H_

#define  DSLFO__MAX_NAME_LENGTH  256
//#define  DEBUG

//------------------------------------------------------------
//----- DSLFileObj base class
//------------------------------------------------------------
class DSLFileObj
{
public:
virtual ~DSLFileObj() = 0;
DSLFileObj(const char *, const char *);
bool  openedSuccessfully()
{
	return m_bOpened;
};

virtual unsigned int getFileSize() = 0;
virtual int          _fread(void *, size_t, size_t) = 0;
virtual int          _feof() = 0;
virtual int          _fclose() = 0;
virtual int          _ferror() = 0;
virtual int          _fputs(const char *) = 0;
virtual int          _fprintf(const char * format, ...) = 0;
virtual int          _fscanf(const char * format, ...) = 0;
virtual char *        _fgets(char *, int) = 0;
virtual int          _fgetc() = 0;
virtual int          _fungetc(int) = 0;
virtual size_t       _fwrite(const void *, size_t, size_t) = 0;
virtual int          _fseek(long, int) = 0;
virtual long         _ftell() = 0;
virtual int          _fflush() = 0;

protected:
	//   GBFSFile( const BGType bgType );
bool m_bOpened;
char m_szFileName[ DSLFO__MAX_NAME_LENGTH ];
char m_szMode[16];

private:
DSLFileObj();
};

//------------------------------------------------------------
//----- GBFSFile class
//------------------------------------------------------------
class GBFSFile : public DSLFileObj
{
public:
GBFSFile(const char *, const char *);
~GBFSFile();

unsigned int   getFileSize()
{
	return m_nLen;
};
int            _fread(void *, size_t, size_t);
int            _feof();
int            _fclose();
int            _ferror();
int            _fputs(const char *);
int            _fprintf(const char * format, ...);
int            _fscanf(const char * format, ...);
char *          _fgets(char *, int);
int            _fgetc();
int            _fungetc(int);
size_t         _fwrite(const void *, size_t, size_t);
int            _fseek(long, int);
long           _ftell();
int            _fflush();

private:
u32 m_nIndex;
u32 m_nLen;
const void * m_vpData;
};

//------------------------------------------------------------
//----- CFFSFile class
//------------------------------------------------------------
class CFFSFile : public DSLFileObj
{
public:
CFFSFile(const char *, const char *);
~CFFSFile();

unsigned int   getFileSize();
int            _fread(void *, size_t, size_t);
int            _feof();
int            _fclose();
int            _ferror();
int            _fputs(const char *);
int            _fprintf(const char * format, ...);
int            _fscanf(const char * format, ...);
char *          _fgets(char *, int);
int            _fgetc();
int            _fungetc(int);
size_t         _fwrite(const void *, size_t, size_t);
int            _fseek(long, int);
long           _ftell();
int            _fflush();

private:
void * m_vpData;
unsigned int m_uSize;
};

#endif   //_DSLFILEOBJ_H_

