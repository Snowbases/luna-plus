#pragma once

//#include "Interface_RainFTP.h"
//#include "../4dyuchiGRX_common/IExecutive.h"


#define _assert _ASSERT

#define FIND_DATA_MAX			500

//	Window Message
#define SAFETY_RANGE			WM_USER + 5000
#define WM_RAIN_FTP_NOTIFY		SAFETY_RANGE
#define WM_RUNPROGRAM			SAFETY_RANGE+1
#define WM_FTP_VERSIONCHK		SAFETY_RANGE+2
#define WM_FTP_FILEDOWNLOAD		SAFETY_RANGE+3
#define WM_FTP_CONNECT			SAFETY_RANGE+4
#define WM_REFRESH				SAFETY_RANGE+5

#define VERSION_FILE				"LunaVerInfo.ver"
#define MH_VERSION_INFO_FINE_NEW	"MHVerInfo_New.ver"
#define GAME_PATCH					"LunaPatch.mhp"
#define LAUNCHER_PATCH				"LunaLauncher.new"

#define VERSION_HEADER_TEXT_LENGTH			4
#define VERSION_INFO_LENGTH					8
#define VERSION_TOTAL_LENGTH				VERSION_HEADER_TEXT_LENGTH + VERSION_INFO_LENGTH


//	Global 
extern char			newverFilename[256];
extern CFtpClient*	g_pFtp;
extern HMODULE		g_hExecutiveHandle;

struct I4DyuchiGXExecutive;
extern I4DyuchiGXExecutive* g_pExecutive;