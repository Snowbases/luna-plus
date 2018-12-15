// MapChange.cpp: implementation of the CMapChange class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MapChange.h"

#include "MainGame.h"
#include "./Interface/cWindowManager.h"
#include "./Interface/cResourceManager.h"
#include "cWindowSystemFunc.h"
#include "MHMap.h"
#include "Input/Mouse.h"
#include "Input/Keyboard.h"
#include "Interface/cScriptManager.h"
#include "Input/UserInput.h"
#include "Effect/EffectManager.h"
#include "MotionManager.h"
#include "UserInfoManager.h"
#include "ChatManager.h"
#include "GMNotifyManager.h"
#include "GuildTournamentMgr.h"
#include "cFont.h"
#include "cImageSelf.h"
#include "cImage.h"

extern HWND _g_hWnd;

GLOBALTON(CMapChange)
CMapChange::CMapChange() :
mImageBar(new cImage),
m_MapNum(0),
m_GameInInitKind(eGameInInitKind_Login),
mIsNoResourceLoaded(TRUE)
{
	const VECTOR2 emptyVector = {0};
	mImageTipPosition = emptyVector;
	mImageTipScale = emptyVector;
	mImageBlackScreenPosition = emptyVector;
	mImageBlackScreenScale = emptyVector;
	mImageBarScale = emptyVector;
	mImageBarPosition = emptyVector;
	ZeroMemory(
		&mTip,
		sizeof(mTip));

	LoadTipText();
	LoadTipImage();
}

CMapChange::~CMapChange()
{
	SAFE_DELETE(mImageBar);
	SAFE_DELETE(mImageGradeMark);

	while(false == mTipImageQueue.empty())
	{
		TipImagePair imagePair = mTipImageQueue.front();
		SAFE_DELETE(imagePair.second);
		mTipImageQueue.pop();
	}
}

void CMapChange::LoadTipText()
{
	CMHFile file;
	file.Init(
		"Data/Interface/Windows/LoadingMsg.bin",
		"rb");

	while(FALSE == file.IsEOF())
	{
		TCHAR buffer[MAX_PATH] = {0};
		file.GetLine(
			buffer,
			sizeof(buffer) / sizeof(*buffer));

		LPCTSTR seperator = _T("\"\t");
		LPCTSTR textIndex = _tcstok(
			buffer,
			seperator);
		LPCTSTR textTip = _tcstok(
			0,
			seperator);

		if(0 == textIndex)
		{
			continue;
		}
		else if(0 == textTip)
		{
			continue;
		}

		mUnusedTipSet.insert(textTip);
	}
}

void CMapChange::LoadTipImage()
{
	CMHFile file;
	file.Init(
		"Data/Interface/Windows/LoadingMsg.bin",
		"rb");

	while(FALSE == file.IsEOF())
	{
		TCHAR buffer[MAX_PATH] = {0};
		file.GetLine(
			buffer,
			sizeof(buffer) / sizeof(*buffer));

		LPCTSTR seperator = _T("\"\t");
		LPCTSTR textImageSize = _tcstok(
			buffer,
			seperator);

		if(0 == textImageSize)
		{
			continue;
		}

		for(int i = _ttoi(textImageSize); 0 < i; --i)
		{
			mTipImageQueue.push(
				TipImagePair(i, 0));
		}
		
		break;
	}
}

void CMapChange::GetTipText(LPTSTR textTip, size_t size)
{
	ZeroMemory(
		textTip,
		size);

	if(mUnusedTipSet.empty())
	{
		if(mUsedTipSet.empty())
		{
			return;
		}

		mUnusedTipSet = mUsedTipSet;
		mUsedTipSet.clear();
	}

	TipSet::iterator iterator = mUnusedTipSet.begin();
	std::advance(
		iterator,
		rand() % mUnusedTipSet.size());
	const std::string& tip = *iterator;

	SafeStrCpy(
		textTip,
		tip.c_str(),
		size);
	mUsedTipSet.insert(tip);
	mUnusedTipSet.erase(iterator);
}

cImageSelf* CMapChange::GetTipImage()
{
	if(mTipImageQueue.empty())
	{
		return 0;
	}

	TipImagePair tipImagePair = mTipImageQueue.front();

	if(0 == tipImagePair.second)
	{
		tipImagePair.second = new cImageSelf;

		const SIZE imageSize = {1280, 1024};

		TCHAR path[MAX_PATH] = {0};
		_stprintf(
			path,
			"Data/Interface/2DImage/image/maploadingimage%02d.tga",
			tipImagePair.first);

		if(FALSE == tipImagePair.second->LoadSprite(
			path,
			imageSize.cx,
			imageSize.cy))
		{
			tipImagePair.second->LoadSprite(
				"Data/Interface/2DImage/image/maploadingimage01.tga",
				imageSize.cx,
				imageSize.cy);
		}
	}

	mTipImageQueue.pop();
	mTipImageQueue.push(tipImagePair);

	return tipImagePair.second;
}

BOOL CMapChange::Init(void* pInitParam)
{
	m_MapNum = *((MAPTYPE*)pInitParam);
	
	GetTipText(
		mTip.mText,
		sizeof(mTip.mText) / sizeof(*mTip.mText));
	mImageTip = GetTipImage();

	const RECT rectGradeMark = {0, 0, 477, 278};
	mImageGradeMark = new cImageSelf;

	mImageGradeMark->LoadSprite(
		"Data/Interface/2DImage/image/grade_mark.tif",
		rectGradeMark.right,
		rectGradeMark.bottom);
	mImageGradeMark->SetImageSrcRect(
		&rectGradeMark);

	CreateGameLoading_m();
	WINDOWMGR->AfterInit();
	NETWORK->SetCurState(this);	
	g_UserInput.SetInputFocus(FALSE);
	USERINFOMGR->SetMapChage(FALSE);
	SetPositionScale();

	cImageRect rt = {951, 217, 952, 256};
	SCRIPTMGR->GetImage(
		4,
		mImageBar,
		&rt);

	AfterRender();

	// 091221 LUJ, 맵 이동이 빠를 경우, 맵 정보가 설정되기 전에
	//			서버에서 맵에 관한 설정(예를 들어 카메라 변경)이
	//			전달되고, 널 포인터 참조로 인해 클라이언트가 강제
	//			종료 된다. 이를 막기 위해 바로 초기화하도록 한다
	MAP->InitMap(m_MapNum);

	if(mIsNoResourceLoaded)
	{
		EFFECTMGR->Init();
		MOTIONMGR->LoadMotionList();

		mIsNoResourceLoaded = FALSE;
	}

	return TRUE;
}


// 070208 LYW --- MapChange : Add function to setting position and scale of image.
void CMapChange::SetPositionScale()
{
	SHORT_RECT screenRect = {0};
	WORD value = 0;
	g_pExecutive->GetRenderer()->GetClientRect(
		&screenRect,
		&value,
		&value);

	const SIZE standardResolution = {1280, 1024};
	const float ratioWidth = float(screenRect.right) / standardResolution.cx;
	const float ratioHeight = float(screenRect.bottom) / standardResolution.cy;
	float fixedScale = min(ratioWidth, ratioHeight);

	// 091221 LUJ, 표준 해상도일 경우 팁 이미지를 화면에 가득 차게 표시한다
	switch(screenRect.right)
	{
	case 800:
	case 1024:
	case 1280:
		{
			fixedScale = max(ratioWidth, ratioHeight);
			break;
		}
	}

	mImageBarScale.x = float(screenRect.right);
	mImageBarScale.y = 1;

	mImageBarPosition.x = 0;
	mImageBarPosition.y = float(screenRect.bottom) * 0.8f;

	mImageTipScale.x = fixedScale;
	mImageTipScale.y = fixedScale;
	mImageTipPosition.x = (screenRect.right - standardResolution.cx * fixedScale) / 2;
	mImageTipPosition.y = 0;

	mImageBlackScreenScale.x = ratioWidth;
	mImageBlackScreenScale.y = ratioHeight;

	// 091221 LUJ, 폰트의 X좌표가 0보다 작을 경우, 출력이 되지 않는다
	//			이를 막기 위해, 폰트 크기를 작게 하면서 맞춘다
	const LONG length = CFONT_OBJ->GetTextExtentEx(
		8,
		mTip.mText,
		_tcslen(mTip.mText));
	mTip.mRect.left = (LONG(screenRect.right) - length) / 2;
	mTip.mRect.right = (LONG(screenRect.right) + length) / 2;
	mTip.mRect.top = LONG(mImageBarPosition.y) + 10;
	mTip.mRect.bottom = mTip.mRect.top + 39;
	mTip.mFont = cFont::FONT8;

	if(0 > mTip.mRect.left)
	{
		mTip.mRect.left = 0;
		mTip.mRect.right = screenRect.right;
	}

	// 사용 등급 이미지 위치/크기 설정
	{
		mImageGradeMarkScale.x = mImageGradeMarkScale.y = ratioHeight;

		RECT rectGradeMark = *mImageGradeMark->GetImageRect();
		const float gapForGradeMark = 10.0f;
		mImageGradeMarkPosition.x = screenRect.right - mImageTipPosition.x - (rectGradeMark.right * mImageGradeMarkScale.x) - gapForGradeMark;
		mImageGradeMarkPosition.y = gapForGradeMark;
	}
}


void CMapChange::Release(CGameState* pNextGameState)
{
	WINDOWMGR->DestroyWindowAll();
	RESRCMGR->ReleaseResource(28);	//28:로딩이미지레이어
}

void CMapChange::Process()
{
	if(MAPTYPE(-1) == m_MapNum)
	{
		return;
	}

	MAINGAME->LoadPack(
		"data/3dData/effect.pak");
	MAINGAME->LoadPack(
		"data/3dData/monster.pak");
	MAINGAME->LoadPack(
		"data/3dData/npc.pak");
	MAINGAME->LoadPack(
		"data/interface/2dImage/npcImage.pak");

	MAINGAME->SetGameState(
		eGAMESTATE_GAMEIN,
		&m_GameInInitKind,
		sizeof(m_GameInInitKind));
}

void CMapChange::BeforeRender()
{}

void CMapChange::AfterRender()
{
	WINDOWMGR->Render();

	mImageTip->RenderSprite(
		&mImageBlackScreenScale,
		0,
		0,
		&mImageBlackScreenPosition,
		RGBA_MAKE(0,0,0,255));
	mImageTip->RenderSprite(
		&mImageTipScale,
		0,
		0,
		&mImageTipPosition,
		RGBA_MAKE(255,255,255,255));
	mImageBar->RenderSprite(
		&mImageBarScale,
		0,
		0,
		&mImageBarPosition,
		RGBA_MAKE(255,255,255,255));
#ifdef _KOR_LOCAL_
	mImageGradeMark->RenderSprite(
		&mImageGradeMarkScale,
		0,
		0,
		&mImageGradeMarkPosition,
		RGBA_MAKE(255,255,255,255));
#endif


	CFONT_OBJ->RenderFont(
		mTip.mFont,
		mTip.mText,
		_tcslen(mTip.mText),
		&mTip.mRect, 
		RGBA_MAKE(0,0,0,255));
	RECT rect = mTip.mRect;
	++rect.left;
	++rect.top;
	CFONT_OBJ->RenderFont(
		mTip.mFont,
		mTip.mText,
		_tcslen(mTip.mText),
		&rect,
		RGBA_MAKE(255,255,255,255));
}

void CMapChange::NetworkMsgParse(BYTE Category,BYTE Protocol,void* pMsg, DWORD dwMsgSize)
{
	switch(Category)
	{
	case MP_USERCONN:
		{
			switch(Protocol) 
			{
			case MP_USERCONN_CHARACTERSELECT_ACK:
				{
					MSG_BYTE* pmsg = (MSG_BYTE*)pMsg;
					m_MapNum = pmsg->bData;
				}
				return;

			case MP_USERCONN_CHARACTERSELECT_NACK:
				{
					LOG(
						EC_MAPSERVER_CLOSED);
					MAINGAME->SetGameState(
						eGAMESTATE_CHARSELECT,
						&m_GameInInitKind,
						sizeof(m_GameInInitKind));
				}
				return;
			}
		}
		break;
	case MP_CHAT:
		CHATMGR->NetworkMsgParse(Protocol, pMsg);
		break;
	case MP_GTOURNAMENT:
		GTMGR->NetworkMsgParse(Protocol, pMsg);
		break;
	case MP_CHEAT:
		{
			switch(Protocol) 
			{
				case MP_CHEAT_EVENTNOTIFY_ON:
				{
					MSG_EVENTNOTIFY_ON* pmsg = (MSG_EVENTNOTIFY_ON*)pMsg;

					NOTIFYMGR->SetEventNotifyStr(pmsg->strTitle, pmsg->strContext);
					NOTIFYMGR->SetEventNotify(TRUE);
					NOTIFYMGR->SetEventNotifyChanged(TRUE);

					NOTIFYMGR->ResetEventApply();
					for(int i=0; i<eEvent_Max; ++i)
					{
						if( pmsg->EventList[i] )
							NOTIFYMGR->SetEventApply( i );
					}
				}
				break;
		
				case MP_CHEAT_EVENTNOTIFY_OFF:
				{
					NOTIFYMGR->SetEventNotify( FALSE );
					NOTIFYMGR->SetEventNotifyChanged( FALSE );
				}	
				break;

			}
		}
		break;
	}
}