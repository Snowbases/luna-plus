// stdafx.h : 잘 변경되지 않고 자주 사용하는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Windows 헤더에서 거의 사용되지 않는 내용을 제외시킵니다.
#endif

// 아래 지정된 플랫폼보다 우선하는 플랫폼을 대상으로 하는 경우 다음 정의를 수정하십시오.
// 다른 플랫폼에 사용되는 해당 값의 최신 정보는 MSDN을 참조하십시오.
#ifndef WINVER				// Windows 95 및 Windows NT 4 이후 버전에서만 기능을 사용할 수 있습니다.
#define WINVER 0x0400		// Windows 98과 Windows 2000 이후 버전에 맞도록 적합한 값으로 변경해 주십시오.
#endif

#ifndef _WIN32_WINNT		// Windows NT 4 이후 버전에서만 기능을 사용할 수 있습니다.
#define _WIN32_WINNT 0x0500		// Windows 98과 Windows 2000 이후 버전에 맞도록 적합한 값으로 변경해 주십시오.
#endif						

#ifndef _WIN32_WINDOWS		// Windows 98 이후 버전에서만 기능을 사용할 수 있습니다.
#define _WIN32_WINDOWS 0x0410 // Windows Me 이후 버전에 맞도록 적합한 값으로 변경해 주십시오.
#endif

#ifndef _WIN32_IE			// IE 4.0 이후 버전에서만 기능을 사용할 수 있습니다.
#define _WIN32_IE 0x0400	// IE 5.0 이후 버전에 맞도록 적합한 값으로 변경해 주십시오.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 일부 CString 생성자는 명시적으로 선언됩니다.

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#include <afxtempl.h>
#include <afxmt.h>

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls

#endif // _AFX_NO_AFXCMN_SUPPORT

#include <process.h>

// CxImage library: http://www.xdp.it/cximage.htm
//
// MFC에서 쓰려면 cximage599c_full_unicode 버전을 다운받아 CxImageMFCDLL 프로젝트를 컴파일하여 쓴다.
#include "CxImage/ximage.h"
// 080515 LUJ, 텍스트 포맷이 ANSI/유니코드에 관계없이 읽어들임
#include "StdioFileEx.h"
#include <vector>
#include <list>

#ifdef _DEBUG
#include <crtdbg.h>
#include <conio.h>
#endif

class CLog
{
public:
	static void Put( const TCHAR*, ... );
	static void SetEnable( bool );

protected:
	CLog();
	~CLog();

	static bool mIsEnable;
};

void DrawText(CxImage&, const CPoint&, LPCTSTR, const RGBQUAD&, long size, LPCTSTR font, long weight = 400, long outline = 0, const RGBQUAD& outlineColor = RGBQUAD());
// 090720 ShinJS --- 서버 AutoPatch 시 Log, Launcher 시 MessageBox 실행
void MsgBoxLog( LPCTSTR msg, LPCTSTR caption, HWND hwnd = NULL );

#define WM_MOUSEMOVE_COMBO (WM_USER + 1000)

#define WM_DOWNLOAD_END	   (WM_USER + 1001)
#define WM_PACKING_END	   (WM_USER + 1002)
/*
080702 LUJ, 2.1.0:	다운로드 오류 시 코드 대신 에러 메시지를 콘솔 창에 표시하도록 함
					잘못된 버전 파일을 다운받을 경우 패치가 진행되지 않도록 함
080919 LUJ, 2.1.1:	대만 버전 추가
081201 LUJ, 2.2.0:	클라이언트가 요청한 패치 파일이 없을 경우, 전체 패치를 시도하도록 함
081202 LUU, 2.3.0:	잘못 받아진 파일을 삭제하도록 함
					패치 파일을 메모리에 다운받아 처리하는 옵션 추가
091214 LUJ, 2.4.0:	읽기 전용 파일을 삭제할 수 있도록 함
091221 LUJ, 2.5.0:	비디오 카드 가능 해상도를 인식하도록 함
*/
#define VERSION _T( "2.5.0" )