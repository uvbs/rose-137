#include "stdafx.h"
#include "VFSManager.h"
#include "CFileSystemNormal.h"
#include "CFileSystemTriggerVFS.h"

#include <algorithm>




CVFSManager		__SingletonVFSManager;



#pragma code_seg(".srnty")

CVFSManager::CVFSManager()
{
	// Construct
	m_hVFile = NULL;
	m_iVFSType = VFS_NORMAL;
}

CVFSManager::~CVFSManager()
{
}

/// Get current file system
CFileSystem* CVFSManager::GetNewFileSystem( int iVFSType )
{
	CFileSystem* pFileSystem = NULL;
	switch( iVFSType )
	{
		case VFS_NORMAL:
			{
				pFileSystem = ( CFileSystem* )new CFileSystemNormal();
			}
			break;
		case VFS_TRIGGER_VFS:
			{
				if( m_hVFile == NULL )
				{
					pFileSystem = ( CFileSystem* )new CFileSystemNormal();
					break;
				}

				CFileSystemTriggerVFS* pFsVFS = new CFileSystemTriggerVFS();
				pFsVFS->SetVFS( m_hVFile );				
				pFileSystem = ( CFileSystem* )pFsVFS;
				
			}
			break;		
	}

	return pFileSystem;
}

bool CVFSManager::InitVFS( int iVFSType, int iReserveCount /*=10*/)  //iReservedCount defined as 10 in header but not passed from winMain
{
	
	ClientLog(LOG_DEBUG,"VFS Manager Setting iVFSType: %i Reserved Count: %i",iVFSType,iReserveCount ); // just confirming the value here
	m_iVFSType = iVFSType;

	if( iVFSType == VFS_TRIGGER_VFS && m_hVFile == NULL ) //set to read from VFS and VFS file is not loaded into memory
	{
		if( g_pCApp )
			g_pCApp->ErrorBOX( "First, set VFS hash", "ERROR" );
		return false;
	}
	return true;
}

///util function for for_each
void ReleaseSingleVFS( CFileSystem* pFileSystem )
{
	if( pFileSystem != NULL )
	{
		delete pFileSystem;
		pFileSystem = NULL;
	}
}

CFileSystem* CVFSManager::GetFileSystem()
{
	CFileSystem* pFileSystem = NULL;		//creates pFilesystem
	pFileSystem = GetNewFileSystem( m_iVFSType );		//not going to take one from the stack any more. PY

	return pFileSystem;										//return pFileSystem as one of 2 types
}

CFileSystem* CVFSManager::GetNormalFileSystem() //PY: forces the return of a normal type filesystem object
{
	CFileSystem* pFileSystem = NULL;
	pFileSystem = GetNewFileSystem( VFS_NORMAL );
	//m_VFSUsedList.push_back( pFileSystem );

	return pFileSystem;
}

CFileSystem* CVFSManager::GetVFSFileSystem() //PY: forces the return of a VFS type filesystem object
{
	CFileSystem* pFileSystem = NULL;
	pFileSystem = GetNewFileSystem( VFS_TRIGGER_VFS );
	//m_VFSUsedList.push_back( pFileSystem );

	return pFileSystem;
}

void CVFSManager::ReturnToManager( CFileSystem* pFileSystem )
{
	//m_VFSList.push_back( pFileSystem );
	
	//m_VFSUsedList.remove( pFileSystem );
}


bool CVFSManager::IsExistFile(const char* pFileName)
{
	if( pFileName == NULL )
		return false;

	CFileSystem* pFileSystem = GetFileSystem();	
	if( pFileSystem == NULL )
		return false;

	return pFileSystem->IsExist( pFileName );
}

#pragma code_seg()
