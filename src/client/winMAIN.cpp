// Client.cpp : Defines the entry point for the application.


//*-------------------------------------------------------------------------------------------------------------------*
// 2005 / 06 / 20 : nAvy
// User-defined description of the preprocessor :
//  1. _TAIWAN  : TAIWAN, PHILIPHIN Country code when connecting to the login server (currently 950) to send a definition to distinguish
//  2. _DE		: The definition of what a duplicate version of the executable (KR) only applied.
//
// 20060703 raning
// Stdafx.h set preprocessor.
//*-------------------------------------------------------------------------------------------------------------------*

#define NDEBUG				//PY: this disables all instances of assert that come after this point
							//Might entirely screw up all the calls to JSingleton. We will have to see. commented out for now
#include "stdafx.h"

#include "CApplication.h"
#include "Game.h"

#include "Network\\CNetwork.h"
#include "util/cfilesystemnormal.h"
#include "CClientStorage.h"
#include "System/CGame.h"
#include "Interface/ExternalUI/CLogin.h"
#include "TriggerInfo.h"

#include "Hasher.h"


//*--------------------------------------------------------------------------------------*/
// 95,98, me from Unicode regard Api Wrapper dll Load
//HMODULE LoadUnicowsProc(void);
//HMODULE g_hUnicows = NULL;
//#ifdef _cplusplus
//extern "C" {
//#endif
//extern FARPROC _PfnLoadUnicows = (FARPROC) &LoadUnicowsProc;
//#ifdef _cplusplus
//}
//#endif
//
//
//// Unicode Load
//HMODULE LoadUnicowsProc(void)
//{
//	g_hUnicows = LoadLibraryA("unicows.dll");
//    return g_hUnicows;
//}
//*--------------------------------------------------------------------------------------*/
typedef struct sHashComp
{
	unsigned int h1;
	unsigned int h2;
	unsigned short h3;
} sHashComp;

typedef union uHash
{
	sHashComp hashVal;
	BYTE hashByte[ 20 ];
} uHash;

//-------------------------------------------------------------------------------------------------
bool Init_DEVICE (void)
{
	bool bRet = false;

	//--------------------------[ engine related ]-----------------------//
	ClientLog(LOG_DEBUG,"winMAIN  initializing Znzin");
	::initZnzin();
	ClientLog(LOG_DEBUG,"winMAIN  opening data.idx");
	::openFileSystem("data.idx");

	//ClientLog(LOG_DEBUG,"winMAIN  doscript lua stuff");
	//::doScript("scripts/init.lua");

	ClientLog(LOG_DEBUG,"winMAIN  setting resolution");
	t_OptionResolution Resolution = g_ClientStorage.GetResolution();
	ClientLog(LOG_DEBUG,"winMAIN  setting display quality");
	::setDisplayQualityLevel( c_iPeformances[g_ClientStorage.GetVideoPerformance()] );
	ClientLog(LOG_DEBUG,"winMAIN  applying anti-aliasing");
	g_ClientStorage.ApplyAntiAliasingOption( g_ClientStorage.GetVideoAntiAliasing() );

	if(!g_pCApp->IsFullScreenMode())
	{
		ClientLog(LOG_DEBUG,"winMAIN  it's not full screen mode. Running znzin::setscreen");
		RECT ClientRt;
		GetClientRect(g_pCApp->GetHWND(),&ClientRt);
		::setScreen(ClientRt.right, ClientRt.bottom, Resolution.iDepth, g_pCApp->IsFullScreenMode() );
	}
	else
	{
		ClientLog(LOG_DEBUG,"winMAIN  it's full screen mode. running znzin::setscreen");
		::setScreen(g_pCApp->GetWIDTH(), g_pCApp->GetHEIGHT(), Resolution.iDepth, g_pCApp->IsFullScreenMode() );

	}
	ClientLog(LOG_DEBUG,"winMAIN  running znzin::attachWindow");
	bRet = ::attachWindow((const void*)g_pCApp->GetHWND());

	ClientLog(LOG_DEBUG,"winMAIN  initializing CD3DUtil");
	CD3DUtil::Init( );

	g_pSoundLIST = new CSoundLIST( g_pCApp->GetHWND() );
	g_pSoundLIST->Load ( "3DDATA\\STB\\FILE_SOUND.stb" );

	return bRet;
}



//-------------------------------------------------------------------------------------------------
void Free_DEVICE (void)
{
	delete g_pSoundLIST;

	CD3DUtil::Free ();

	//--------------------------[ engine related ]-----------------------//
	::detachWindow();

	::closeFileSystem();
	::destZnzin();
}

//-------------------------------------------------------------------------------------------------
int APIENTRY WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
#ifndef _DEBUG
	// *-------------------------------------------------------------------* //
	// 2005. 5. 6. Joe Ho-dong
	HANDLE AppMutex = NULL;

	AppMutex = ::OpenMutex( MUTEX_ALL_ACCESS, false, __BUILD_REGION_NAME );
	if( AppMutex != NULL )
	{
		::ReleaseMutex( AppMutex);
		::CloseHandle( AppMutex);
		MessageBox( NULL, "Prevention of double execution", "TRose", MB_OK | MB_ICONERROR);
		return 0;
	}
#endif

	//-------------------------------------------------------------------------------
	/// Init System object
	//-------------------------------------------------------------------------------
	g_pCApp		= CApplication::Instance ();

	CGame::GetInstance().SetCurVersion("0.0");
	GetLocalTime(	&g_GameDATA.m_SystemTime );

	//-------------------------------------------------------------------------------
	/// Init System object
	//-------------------------------------------------------------------------------
	//	g_pCApp		= CApplication::Instance ();

	g_pNet		= CNetwork::Instance (hInstance);
	g_pCRange	= CRangeTBL::Instance ();

	//-------------------------------------------------------------------------------
	/// Load Range table
	//-------------------------------------------------------------------------------
	if ( !g_pCRange->Load_TABLE ("3DDATA\\TERRAIN\\O_Range.TBL") ) 
	{
		g_pCApp->ErrorBOX ( "3DDATA\\TERRAIN\\O_Range.TBL file open error", CUtil::GetCurrentDir (), MB_OK);
		return 0;
	}

	//-------------------------------------------------------------------------------
	/// IP/Port Setting
	//-------------------------------------------------------------------------------
	g_GameDATA.m_wServerPORT = TCP_LSV_PORT;
	g_GameDATA.m_ServerIP.Set( TCP_LSV_IP );

	if ( !g_pCApp->ParseArgument( lpCmdLine ) ) 
	{
		MessageBox( NULL, "Arg error!", "TRose", MB_OK | MB_ICONERROR);
		return 0;
	}

	//-------------------------------------------------------------------------------
	/// Data generated on the resolution of the window are required during the load here.
	//-------------------------------------------------------------------------------
	g_TblResolution.Load2( "3DDATA\\STB\\RESOLUTION.STB",	false, false );
	g_TblCamera.Load2( "3DDATA\\STB\\LIST_CAMERA.STB" ,false, false );
	ClientLog(LOG_DEBUG,"winMAIN LIST_CAMERA loaded");
	//-------------------------------------------------------------------------------
	/// Load the Data stored on the client.
	//-------------------------------------------------------------------------------
	g_ClientStorage.Load();
	ClientLog(LOG_DEBUG,"winMAIN g_ClientStorage loaded");
	//-------------------------------------------------------------------------------
	///The index of the adjusted previous option to import the resolution referring to g_TblResolution
	///The resolution is adjusted.
	//-------------------------------------------------------------------------------
#pragma message("You can run the stored resolution is determined by testing and initializing the message box does not work TriggerDetect Shall run")
	t_OptionResolution Resolution = g_ClientStorage.GetResolution();
	ClientLog(LOG_DEBUG,"winMAIN resolution set");
	/// Check range values
	UINT iFullScreen = g_ClientStorage.GetVideoFullScreen();
	ClientLog(LOG_DEBUG,"winMAIN iFullScreen initialized");

	g_pCApp->SetFullscreenMode( iFullScreen );
	ClientLog(LOG_DEBUG,"winMAIN iFullScreen set");
	if(!CGame::GetInstance().GetCurVersion().empty())
	{
		g_pCApp->CreateWND ("classCLIENT", CStr::Printf("%s [Ver. %s]", __BUILD_REGION_NAME, CGame::GetInstance().GetCurVersion().c_str()) , Resolution.iWidth, Resolution.iHeight,Resolution.iDepth, hInstance);
		ClientLog(LOG_DEBUG,"winMAIN instance empty created a new one");
	}
	else
	{
		g_pCApp->CreateWND ("classCLIENT", __BUILD_REGION_NAME, Resolution.iWidth, Resolution.iHeight,Resolution.iDepth, hInstance);
		ClientLog(LOG_DEBUG,"winMAIN instance already exists. make new one");
	}


#ifndef _DEBUG
	// Information collection system
	TI_ReadSysInfoFile ();
#endif


	// *-------------------------------------------------------------------* //
	g_pObjMGR = CObjectMANAGER::Instance ();
	g_pCApp->ResetExitGame();
	ClientLog(LOG_DEBUG,"winMAIN  Device initialization about to be called");
	bool bDeviceInitialized = Init_DEVICE();
	ClientLog(LOG_DEBUG,"winMAIN  Device initialization called");

	if ( bDeviceInitialized ) 
	{
		ClientLog(LOG_DEBUG,"winMAIN  game loop starting");
		CGame::GetInstance().GameLoop();		//game loop started. CGame.cpp
	}
	else
	{
		MessageBox( NULL, "Device error!", "TRose", MB_OK | MB_ICONERROR);
	}

	Free_DEVICE ();

	g_TblCamera.Free();
	g_TblResolution.Free();

	g_pCApp->Destroy ();
	g_pNet->Destroy ();
	g_pCRange->Destroy ();

#ifndef _DEBUG
	if (AppMutex)
	{
		::ReleaseMutex(AppMutex);
		::CloseHandle(AppMutex);
	}
#endif

	return 0;
}


extern std::string sCurVersion;