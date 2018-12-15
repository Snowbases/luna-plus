// cScriptManager.h: interface for the cScriptManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CSCRIPTMANAGER_H__B9C3CDD2_6869_4A8A_801C_BA26B097B799__INCLUDED_)
#define AFX_CSCRIPTMANAGER_H__B9C3CDD2_6869_4A8A_801C_BA26B097B799__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../4DyuchiGXGFunc/global.h"
#include "cWindowDef.h"

class cWindow;
class cImage;
class CMHFile;


#define SCRIPTMGR	USINGTON(cScriptManager)
enum ePATH_FILE_TYPE
{
	PFT_HARDPATH = 0,
	PFT_ITEMPATH	,
	PFT_SKILLPATH	,
	PFT_BUFFPATH ,
	PFT_MINIMAPPATH ,
	PFT_CONDUCTPATH,
	PFT_DATEMATCHINGPATH,			// 데이트 매칭.

	// 070607 LYW --- cScriptManager : Add enum for favor icon part.
	PFT_FAVOR_ICON_PATH,

	// 070714 LYW --- cScriptManager : Add enum for class icon part.
	PFT_CLASS_ICON_PATH,
};

class cScriptManager  
{
	// 080114 LYW --- cScriptManager : Dialog 해쉬 테이블을 사용하지 않도록 처리 함.
	//VBHASH_HANDLE		m_hDlgInfoHash;	//GetDlgInfoFromFile()을 위한 토큰 해쉬.
	VBHASH_HANDLE		m_hCtrlInfoHash;	//GetCtrlInfo()을 위한 토큰 해쉬.
	VBHASH_HANDLE		m_hPropertyInfoHash;	// 		GetInfoFromFile()

public:
//	MAKESINGLETON(cScriptManager);
	cScriptManager();
	virtual ~cScriptManager();

	cWindow * GetDlgInfoFromFile(char * filePath, char* mode = "rt");
	cWindow * GetInfoFromFile(cWindow * wnd, CMHFile * fp);
	cWindow * GetCtrlInfo(cWindow * wnd, char * buff, CMHFile * fp);

	void GetImage( int idx, cImage * pImage, cImageRect * rect );
	void GetImage( int hard_idx, cImage * pImage , ePATH_FILE_TYPE type = PFT_HARDPATH );

protected:
	void GetImage( CMHFile * fp, cImage * pImage );
	char * GetMsg( int idx );

	void	InitTokenHash();			// 파서를 위한 해시 초기화.
	void	ReleaseTokenHash();			// 파서를 위한 해시 말기화(?).

};
EXTERNGLOBALTON(cScriptManager);
#endif // !defined(AFX_CSCRIPTMANAGER_H__B9C3CDD2_6869_4A8A_801C_BA26B097B799__INCLUDED_)
