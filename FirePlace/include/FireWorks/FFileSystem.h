
#ifndef _FFILESYSTEM_H_
#define _FFILESYSTEM_H_

#include "FIFile.h"

// Implementation class for the file system.  This should only be accessed directly on the application side, DLLs should use FIFileSystem::GetInstance()
class FFileSystem : public FIFileSystem
{
public:
	static FFileSystem&	GetInstance();

	virtual FIFile*		Create( const  char*  szFileName = NULL, FIFile::OpenMode eOpenMode = FIFile::modeRead, dword dwFlags = FFILE_FLAG_DEFAULT, uint uiBufferSize = FFILE_DEFAULT_BUFFER_SIZE );
	virtual FIFile*		Create( const wchar* wszFileName       , FIFile::OpenMode eOpenMode = FIFile::modeRead, dword dwFlags = FFILE_FLAG_DEFAULT, uint uiBufferSize = FFILE_DEFAULT_BUFFER_SIZE );
	
	virtual FileErr		Delete( const  char*  szFileName );
	virtual FileErr		Delete( const wchar* wszFileName );

	virtual FileErr		ReadAll( const  char*  szFileName, byte** pucBufferOut, size_t *puiBufferSizeOut, dword dwFlags = FFILE_FLAG_DEFAULT);
	virtual FileErr		ReadAll( const  wchar*  wszFileName, byte** pucBufferOut, size_t *puiBufferSizeOut, dword dwFlags = FFILE_FLAG_DEFAULT);

	virtual FileErr		ReadAll( const  char*  szFileName, byte** pucBufferOut, size_t *puiBufferSizeOut, dword dwFlags, FIFile::Allocator& alloc);
	virtual FileErr		ReadAll( const  wchar*  wszFileName, byte** pucBufferOut, size_t *puiBufferSizeOut, dword dwFlags, FIFile::Allocator& alloc);

	virtual bool		Exist( const char* pszFileName );
	virtual bool		Exist( const wchar* pwszFileName );

	virtual uint		GetLength( const  char*  szFileName );
	virtual uint		GetLength( const wchar* wszFileName );

	virtual FFileTimestamp GetLastWriteTime( const char* pFileName );

	virtual	FFileErrorHandler* GetErrorHandler();

	virtual bool		IsFileCatalogEnabled();
};

#endif // _FFILESYSTEM_H_