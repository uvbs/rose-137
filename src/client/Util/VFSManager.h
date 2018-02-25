#ifndef _VFSMANAGER_
#define _VFSMANAGER_

#include "JSingleton.h"
#include <list>
#include "CFileSystem.h"
#include "TriggerVFS.h"

enum
{
	VFS_NORMAL = 0,
	VFS_TRIGGER_VFS,
};

/// VFS manager class
class CVFSManager : public CJSingleton< CVFSManager >
{
private:
	std::list< CFileSystem* >			m_VFSList;
	std::list< CFileSystem* >			m_VFSUsedList;

	int									m_iVFSType;
	VHANDLE								m_hVFile;

public:	
	CVFSManager();
	~CVFSManager();

	void			SetVFS( VHANDLE pVFile ){ m_hVFile = pVFile; };
	VHANDLE			GetVFS(){ return m_hVFile; };

	bool			InitVFS( int iVFSType, int iReserveCount = 10 );	/// Initializes all resources
	CFileSystem*	GetNewFileSystem( int iVFSType );
	void			ReleaseAll();										/// Release all resources

	CFileSystem*	GetFileSystem();
	CFileSystem*	GetNormalFileSystem();
	CFileSystem*	GetVFSFileSystem();
	void			ReturnToManager( CFileSystem* pFileSystem );


	bool			IsExistFile( const char* pFileName );
};

#endif //_VFSMANAGER_