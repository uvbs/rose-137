// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.
//

#pragma once


#define WIN32_LEAN_AND_MEAN		// 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일입니다.
#include <windows.h>

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// 전처리기 추가하면 주석 남겨주세요.
// 전처리기 세팅 수정하면 리빌드  꼭 해주세요.
//////////////////////////////////////////////////////////////////////////
// 전처리기 세팅 여기에.//////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//#define __KOREA_NEW
//#define __KOREA
//#define __ARUA
//#define	__HEBAN

//#define __JAPAN_NEW
//#define __JAPAN
//
//#define _TAIWAN
//
//#define _PHILIPPIN
//#define _PHILIPPIN_EVO
//#define _TALA
//#define _TALA_EVO
//
//#define _USA
#define _USA_EVO
//
//#define _EU
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// 새로 오픈하는 한국버전
#if defined( __KOREA_NEW )|| defined ( __JAPAN_NEW ) || defined(_PHILIPPIN) || defined ( _USA ) || defined( __ARUA ) || defined ( __HEBAN )

#define _NEWUI					// 홍근 : 리뉴얼 되는 인터페이스.

#endif
