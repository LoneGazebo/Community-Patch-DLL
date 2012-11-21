#ifndef _FILOGFILE_H_
#define _FILOGFILE_H_

//
// log file object
//
class FILogFile
{
public:
	enum EFlags
	{
		kDontTimeStamp		= 0x1,
		kDontFlushOnWrite	= 0x2,
		kWriteToConsole		= 0x4
	};

	virtual void Msg(const TCHAR* format, ... ) = 0;
	virtual void DebugMsg(const TCHAR* format, ... ) = 0;
	virtual void WarningMsg(const TCHAR* format, ... ) = 0;
	virtual void ErrorMsg(const TCHAR* format, ... ) = 0;
	virtual void SetFlags(uint uiFlags) = 0;
	virtual uint GetFlags() const = 0;
 	virtual void SetBufSize(int iSize) = 0;
	virtual LPCWSTR GetFileName() const = 0;
	virtual void Close( void ) = 0;
	virtual void Open( void ) = 0;
	virtual void Archive(LPCWSTR wszArchiveName) = 0;

protected:
	FILogFile() {};
	virtual ~FILogFile() =0 {};
};

//
// log file manager
//
class FILogFileMgr 
{
public:
	virtual ~FILogFileMgr() = 0 {};
	static FILogFileMgr& GetInstance();
	static FILogFileMgr* PeekInstance();

	virtual void		EnableLogging( void ) = 0;
	virtual void		DisableLogging( void ) = 0;
	virtual FILogFile*	GetLog(LPCWSTR wszFileName, uint uiFlags, const TCHAR* szTitleString = NULL) = 0;		// create if necessary
	virtual FILogFile*	GetLog(LPCSTR szFileName, uint uiFlags, const TCHAR* szTitleString = NULL) = 0;		// create if necessary

	virtual bool		DeleteLog(FILogFile* pLog) = 0;
	virtual bool		DeleteLog(LPCWSTR wszFileName) = 0;

	virtual bool		IsLoggingEnabled( void ) = 0;

protected:
	static FILogFileMgr* ms_pInstance;

	FILogFileMgr() {};
};

#ifdef FXS_IS_DLL
#define LOGFILEMGR FILogFileMgr::GetInstance()
#else
#define LOGFILEMGR FLogFileMgr::GetInstance()
#endif

#endif // _FILOGFILE_H_