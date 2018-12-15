// Console.cpp: implementation of the CConsole class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Console.h"
#include <stdio.H>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CConsole g_Console;

CConsole::CConsole()
{
	m_pIConsole = NULL;
//	CoInitialize(NULL);
}

CConsole::~CConsole()
{
//	CoUninitialize();
}

/*
BOOL CConsole::Init(int MaxButton,CUSTOM_BUTTON* pButtonInfo, void (*CommandFunc)(char* szCommand))
{	

	// ¿ì¼± I4DyuchiCOSOLEÄÄÆ÷³ÍÆ® »ý¼º
	HRESULT hr;
    
	hr = CoCreateInstance(
           CLSID_4DyuchiCONSOLE,
           NULL,
           CLSCTX_INPROC_SERVER,
           IID_4DyuchiCONSOLE,
           (void**)&m_pIConsole);

	
	if(FAILED(hr))
		return FALSE;

	// ÄÄÆ÷³ÍÆ®?Ç ÄÜ¼Ö?» »ý¼º.»ý¼ºÇÏ±â ?§ÇØ DESC_CONSOLE±¸Á¶Ã¼¸¦ Ã¤¿ö¼­ ±× Æ÷?ÎÅÍ¸¦ ³Ö¾î¾ßÇÑ´Ù.
	// DESC_CONSOLE±¸Á¶Ã¼´Â ÆÄ?Ï·Î±ë?» ?§ÇÑ FILE_WRITE_INFO¿Í È­¸é·Î±ë?» ?§ÇÑ DISPLAY_INFO
	// ¸¦ Æ÷ÇÔÇÑ´Ù. FILE_WRITE_INFO?º ÆÄ?Ï·Î±ë?» ÇÏÁö ¾Ê?» °æ¿ì NULL·Î ³Ö?¸¸é µÈ´Ù.±×·¯³ª 
	// DISPLAY_INFO´Â ¹Ýµå½Ã Ã¤¿ö³Ö¾î¾ß ÇÑ´Ù.

	// ¾Æ·¡?Ç ¼öÄ¡´Â ±Ç?å¼öÄ¡?Ì´Ù.´ë·« ¾Æ·¡ Á¤µµ?Ç °ª?¸·Î ¼¼ÆÃÇÏ¸é ¹«¸®°¡ ¾ø´Ù.
	DISPLAY_INFO	display;
	display.dwMaxAccessibleThreadNum = 5;		// ?Ì ÄÜ¼Ö ÄÄÆ÷³ÍÆ®?Ç µð½ºÇÃ·¹?Ì ¸Å´ÏÁ®¸¦ µ¿½Ã¿¡ ¾ï¼¼½ºÇÒ ½º·¹µå ÃÖ´ë°¹¼ö
	display.dwMaxStringBufferNum = 10000;		// ÃÖ´ë 10000¶ó?Î?Ç ¹öÆÛ¸¦ °¡Áø´Ù.
	display.dwMinMaintainBufferNum = 5000;		// È­¸é¿¡ º¸¿©Áö´Â ÃÖ´ë ¶ó?Î¼ö´Â 10000-5000?¸·Î 5000¶ó?Î?» º¼¼ö ?Ö´Ù.
	display.dwDisplayRefreshRate = 1000;		// °»½ÅÁÖ±â´Â 500ms?Ì´Ù.
	display.dwFilteredLevel = LOG_DEBUG;		// ·Î±×¸Þ½ÃÁö ¿ì¼±¼ø?§°¡ LOG_DEBUGº¸´Ù ³·?¸¸é Ç¥½ÃÇÏÁö ¾Ê´Â´Ù.
	
	
	WRITE_FILE_INFO	file;
	file.dwMaxAccessibleThreadNum = 5;			// ?Ì ÄÜ¼Ö ÄÄÆ÷³ÍÆ®?Ç ÆÄ?Ï ·Î±× ¸Å´ÏÁ®¸¦ ¾ï¼¼½ºÇÒ ½º·¹µå ÃÖ´ë°¹¼ö
	file.dwLimitedBufferSizeToWriteFile = 8192;	// ·Î±× ¸Þ½ÃÁö°¡ 8192¹Ù?ÌÆ® ½×?Ï¶§¸¶´Ù ½ÇÁ¦·Î ÆÄ?Ï¿¡ WRITEÇÑ´Ù.
	file.dwFilteredLevel = LOG_DEBUG;			// ·Î±×¸Þ½ÃÁö ¿ì¼±¼ø?§°¡ LOG_DEBUGº¸´Ù ³·?¸¸é ÆÄ?Ï¿¡ ±â·ÏÇÏÁö ¾Ê´Â´Ù.

	DESC_CONSOLE	desc;

	desc.OnCommandFunc = CommandFunc;				// Å°º¸µå ÄÄ¸Çµå ÇÁ·ÒÇÁÆ®¿¡¼­ ÄÄ¸Çµå¸¦ ³Ö¾ú?»¶§ parsingÇÒ ÄÝ¹éÇÔ¼ö ¼¼ÆÃ
	desc.dwCustomButtonNum = MaxButton;			// »ç¿ë?Ú Á¤?Ç ¹öÆ° 2°³ »ç¿ëÇÔ.
	desc.pCustomButton = pButtonInfo;			// Ä¿½ºÅÒ ?Ìº¥Æ® Æ÷?ÎÅÍ ¼¼ÆÃ
	desc.szFileName = "log.txt";				// writeÇÒ ÆÄ?Ï¸í.ÆÄ?Ï·Î±ëÇÏÁö ¾Ê?» °æ¿ì NULL
	desc.pDisplayInfo = &display;				// È­¸é µð½ºÇÃ·¹?Ì¸¦ ?§ÇÑ DISPLAY_INFO ±¸Á¶Ã¼ Æ÷?ÎÅÍ.¹«Á¶°Ç ¼¼ÆÃÇØ¾ßÇÑ´Ù.
	desc.pWriteFileInfo = &file;				// ÆÄ?Ï·Î±ë?» ?§ÇÑ WRITE_FILE_INFO ±¸Á¶Ã¼ Æ÷?ÎÅÍ.»ç¿ëÇÏÁö ¾Ê?¸¸é NULL

	m_pIConsole->CreateConsole(&desc);			// ÄÜ¼Ö »ý¼º

	return true;
}
*/

BOOL CConsole::Init(int MaxButton,MENU_CUSTOM_INFO* pMenuInfo, cbRetrnFunc commandFunc)
{
	HRESULT hr;
	
	hr = CoCreateInstance(
           CLSID_ULTRA_TCONSOLE,
           NULL,
           CLSCTX_INPROC_SERVER,
           IID_ITConsole,
           (void**)&m_pIConsole);

	
	if(FAILED(hr))
		return FALSE;

	LOGFONT logFont;
	logFont.lfHeight		= 17; 
	logFont.lfWidth			= 0; 
	logFont.lfEscapement	= 0; 
	logFont.lfOrientation	= 0; 
	logFont.lfWeight		= FW_BOLD; 
	logFont.lfItalic		= 0; 
	logFont.lfUnderline		= 0; 
	logFont.lfStrikeOut		= 0; 
	logFont.lfCharSet		= HANGUL_CHARSET; 
	logFont.lfOutPrecision	= 0; 
	logFont.lfClipPrecision	= 0; 
	logFont.lfQuality		= 0; 
	logFont.lfPitchAndFamily	= 0; 
	strcpy(logFont.lfFaceName, "±¼¸²"); 

	HWND hWnd;
	MHTCONSOLE_DESC	desc;
	desc.szConsoleName = "Darkstory";
	desc.dwRefreshRate = 1000;
	desc.wLogFileType = LFILE_DESTROYLOGFILEOUT;//0;//LFILE_LOGOVERFLOWFILEOUT;
	desc.szLogFileName = "./Log/ConsoleLog.txt";
	desc.dwFlushFileBufferSize = 10000;
	desc.dwDrawTimeOut	= 1000*60*3;
	desc.wMaxLineNum = 1000;
	desc.dwListStyle = TLO_NOTMESSAGECLOSE|TLO_LINENUMBER|TLO_SCROLLTUMBTRACKUPDATE;
	desc.Width	= 800;
	desc.Height = 400;
	desc.pFont = &logFont;

	desc.nCustomMunuNum = MaxButton;
	desc.cbReturnFunc = commandFunc;
	desc.pCustomMenu = pMenuInfo;
	m_pIConsole->CreateConsole(&desc, &hWnd);

	return TRUE;
}

void CConsole::Release()
{
	if(m_pIConsole)
	{
		m_pIConsole->Release();
		m_pIConsole = NULL;
	}
	
}

void CConsole::Log(int LogType,int MsgLevel,char* LogMsg,...)
{
	static char   va_Temp_Buff[1024];
	
	va_list vl;

	va_start(vl, LogMsg);
	vsprintf(va_Temp_Buff, LogMsg, vl);
	va_end(vl);


	switch(LogType) 
	{
	case eLogDisplay:
		{
			// 080130 LUJ, 	���ڿ��� NULL�� ������ ���� ��� �ܼ��� �������鼭
			//				�������� ����Ǵ� ���� �����ϱ� ���� �ִ� ���ڿ� ����
			//				ũ�⸸ŭ�� ����ϵ��� ��
			m_pIConsole->OutputFile(va_Temp_Buff, sizeof( va_Temp_Buff ) );
			break;
		}		
	case eLogFile:
		{
			//m_pIConsole->WriteFile(va_Temp_Buff,strlen(va_Temp_Buff),MsgLevel);
		}
		break;
	default:
		MessageBox(NULL,"Not Defined LogType",0,0);
	}
}

void CConsole::LOG(int MsgLevel,char* LogMsg,...)
{
	static char   va_Temp_Buff[1024];
	
	va_list vl;

	va_start(vl, LogMsg);
	vsprintf(va_Temp_Buff, LogMsg, vl);
	va_end(vl);

	if( m_pIConsole )
	{
		// 080130 LUJ, 	���ڿ��� NULL�� ������ ���� ��� �ܼ��� �������鼭
		//				�������� ����Ǵ� ���� �����ϱ� ���� �ִ� ���ڿ� ����
		//				ũ�⸸ŭ�� ����ϵ��� ��
		m_pIConsole->OutputDisplay(va_Temp_Buff, sizeof( va_Temp_Buff ) );
	}
	
}

void CConsole::WaitMessage()
{
	m_pIConsole->MessageLoop();
}
