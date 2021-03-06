/*
	$Header: /Client/LUA_Func.cpp 14    06-10-26 5:56p Antonio $
*/
#include "stdAFX.h"
#include "LUA_Func.h"
#include "zz_script_lua.h"


void set_global (lua_State * L, const char * variable_name, int value)
{
	lua_pushnumber(L, value);
	lua_setglobal(L, variable_name);
}

void set_global (lua_State * L, const char * variable_name, float value)
{
	lua_pushnumber(L, value);
	lua_setglobal(L, variable_name);
}

void set_global (lua_State * L, const char * variable_name, const char * value)
{
	lua_pushstring(L, value);
	lua_setglobal(L, variable_name);
}

//-------------------------------------------------------------------------------------------------
//
// After calling a Lua function returns the number of results returned value.
int lua_CallFUNC (lua_State *L, const char* function_name, va_list va)
{
	//ClientLog(LOG_DEBUG,"Called lua_CallFUNC on script function %s",function_name );
	int param_count = 0;
    int iBeforeStackIDX = lua_gettop (L);

	lua_getglobal(L, function_name);
	if (!lua_isfunction(L, -1)) {	// if no lua function exists
		lua_pop(L, param_count+1);	// restore function name and arguments in stack
		ClientLog(LOG_DEBUG,"Invalid script function %s",function_name );
		char *szMsg=CStr::Printf( "invalid script function( %s ) name ...", function_name);

		g_pCApp->ErrorBOX ("ERROR :: function not found ", szMsg);

		return -1;
	}

	zz_param_type type;
	while ((type = va_arg(va, zz_param_type)) != ZZ_PARAM_END) {
		switch (type) {
			case ZZ_PARAM_UINT:
			{
				set_param_uint		(L, param_count, va_arg(va, unsigned int)	);
				break;
			}
			case ZZ_PARAM_INT:
			{
				set_param_int		(L, param_count, va_arg(va, int)			);
				break;
			}
			case ZZ_PARAM_FLOAT:
			{
				set_param_float		(L, param_count, va_arg(va, float)			);
				break;
			}
			case ZZ_PARAM_FLOAT3:
			{
				set_param_float3	(L, param_count, va_arg(va, const float*)	);
				break;
			}
			case ZZ_PARAM_STRING:
			{
				set_param_string	(L, param_count, va_arg(va, const char*)	);
				break;
			}
		}
	}
	
#ifdef ZZ_LUA500
	int err = lua_pcall(L, param_count, LUA_MULTRET /* nresults */, 0 /* errfunc */);
#else // 40
	int err = lua_call(L, param_count, LUA_MULTRET /* nresults */);
#endif

	param_count = 0;
	if (err == 0) {
		return (lua_gettop (L)-iBeforeStackIDX);
	}

	// error code print
	char errMsgBuffer[512];
	const char* luaErrorMessage = lua_tostring(L, lua_gettop(L));

	switch ( err ) {
		case LUA_ERRRUN:	// 1
			snprintf(errMsgBuffer, sizeof(errMsgBuffer), "Runtime: %s", luaErrorMessage);
			break;
		case LUA_ERRFILE:	// 2
			snprintf(errMsgBuffer, sizeof(errMsgBuffer), "script: error opening the file (only for lua_dofile). \n");
			break;
		case LUA_ERRSYNTAX:	// 3
			snprintf(errMsgBuffer, sizeof(errMsgBuffer), "script: syntax error during pre-compilation");
			break;
		case LUA_ERRMEM:	// 4
			snprintf(errMsgBuffer, sizeof(errMsgBuffer), "script: memory allocation error. For such errors, Scriptor does not call _ERRORMESSAGE.");
			break;
		case LUA_ERRERR:	// 5
			snprintf(errMsgBuffer, sizeof(errMsgBuffer), "script: error while running _ERRORMESSAGE. For such errors, Scriptor does not call _ERRORMESSAGE again, to avoid loops.");
			break;
	}

	g_pCApp->ErrorBOX (errMsgBuffer, "Error: LUA Script Error");

	return -1;
}

//-------------------------------------------------------------------------------------------------
int lua_CallIntFUNC (lua_State *pLUA, const char *szFuncName, ...)
{
	if ( !szFuncName )
		return 0x80000000;

	va_list va;
	va_start(va, szFuncName);
    int iResultCnt = lua_CallFUNC(pLUA, szFuncName, va);
	va_end(va);

	if ( iResultCnt > 0 ) {
		// Because there is a returned result and retrieves the result.
		lua_GetRETURN(pLUA, iResultCnt);

		LogString (LOG_DEBUG, "lua function( %s ) return %d ... \n", szFuncName, iResultCnt );

	    return iResultCnt;
	}

	LogString (LOG_DEBUG, "lua function( %s ) return %d result ... \n", szFuncName, iResultCnt );

	return 0x80000000;
}

//-------------------------------------------------------------------------------------------------
char*lua_CallCharFUNC (lua_State *pLUA, const char *szFuncName, ...)
{
	va_list va;
	va_start(va, szFuncName);
    int iResultCnt = lua_CallFUNC(pLUA, szFuncName, va);
	va_end(va);

    char *pStr = NULL;
    if ( iResultCnt > 0 )
        lua_GetRETURN(pLUA, &pStr);

    return pStr;
}

//-------------------------------------------------------------------------------------------------
bool lua_GetRETURN (lua_State *pLUA, int &iValue)
{
	int iResult;

	iResult = lua_gettop(pLUA);
    if ( iResult < 1 ) {
		return false;
	}

	if ( !lua_isnumber(pLUA, -1) ) {
        return false;
    }

    iValue = (int)lua_tonumber(pLUA, -1);
    lua_pop(pLUA, -1);

    return true;
}
bool lua_GetRETURN (lua_State *pLUA, char **ppChar)
{
    if ( lua_gettop(pLUA) < 1 ||
        !lua_isstring(pLUA, -1) ) {
        return false;
    }

    *ppChar = (char*)lua_tostring (pLUA, -1);
    lua_pop(pLUA, -1);

    return true;
}

bool lua_GetRETURN (lua_State *pLUA, float& fValue )
{
    int iResult;

	iResult = lua_gettop(pLUA);
    if ( iResult < 1 ) {
		return false;
	}

	if ( !lua_isnumber(pLUA, -1) ) {
        return false;
    }

    fValue = (float)lua_tonumber(pLUA, -1);
    lua_pop(pLUA, -1);

    return true;
}


//-------------------------------------------------------------------------------------------------
