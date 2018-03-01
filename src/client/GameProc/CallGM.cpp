#include "stdafx.h"
#include ".\callgm.h"
#include "System/SystemProcScript.h"


CCallGM::CCallGM(void)
{
}

CCallGM::~CCallGM(void)
{
}

void CCallGM::CallGM( std::string& strServerName, std::string& strChannelName, std::string& strMsg  )
{
	std::string strSendedString = strServerName + "_" + strChannelName + "_" + strMsg;

	DWORD dwCountryCode = 6; // Legacy: This means USA, country code logic was removed from source code (3/1/2018 - Ralph)

	CSystemProcScript::GetSingleton().CallLuaFunction( "SendGMCall", 
												ZZ_PARAM_INT,
												dwCountryCode,
												ZZ_PARAM_STRING,
												strSendedString,
												ZZ_PARAM_END );	
}