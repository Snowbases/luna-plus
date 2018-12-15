#pragma once

#include ".\Interface\cDialog.h"
#include ".\interface\cButton.h"
#include ".\interface\cGuagen.h"
#include ".\interface\cStatic.h"
#include ".\interface\cPushupButton.h"
#include "cImageSelf.h"
#include "Item.h"
#include "ItemShow.h"

#define FISHING_AUTO_RETRY_DELAY	2000
#define FISHING_STARTBTN_DELAY		3000

class cButton;
class cIconDialog;
class CObjectGuagen;

class CFishingDialog :
	public cDialog
{
	struct ImageData
	{
		cImageSelf*	pImage;
		VECTOR2*	pImageRect;
		VECTOR3		vDir;
		DWORD		dwCreateTime;
		float		fAlpha;

		void Clear() {pImage=NULL; pImageRect=NULL; dwCreateTime=0; fAlpha=0.0f;}
		void SetImage(cImageSelf* image, VECTOR2* imagerect, VECTOR3 dir) {
			pImage=image; pImageRect=imagerect; vDir=dir; dwCreateTime=gCurTime; fAlpha=1.0f;
		}
	};

	cPushupButton *		m_pStartBtn;
	cIconDialog*		m_pIconBait;
	cIconDialog*		m_pIconMission;
	CObjectGuagen*		m_pFishingExp;		// 낚시 경험지게이지
	cStatic*			m_pExpStatic;
	cStatic*			m_pPointStatic;
	cStatic*			m_pProcessTime;

	// 090811 ShinJS --- CVirualItem -> CItemShow로 수정
	CItemShow			m_ItemBait;
	cIcon				m_Icons[MAX_FISHING_MISSION];			// 미션아이콘
	DWORD				m_dwMission[MAX_FISHING_MISSION];		// 미션물고기Idx
	WORD				m_wMissonPos;							// 미션체크위치
	ImageData			m_ImageMission[MAX_FISHING_MISSION];	//미션상태이미지

	int m_nPerpect;
	int m_nGreat;
	int m_nGood;
	int m_nMiss;

	cImageSelf			m_ImagePerpect;
	VECTOR2				m_ImagePerpectRT;
	cImageSelf			m_ImageGreat;
	VECTOR2				m_ImageGreatRT;
	cImageSelf			m_ImageGood;
	VECTOR2				m_ImageGoodRT;
	cImageSelf			m_ImageMiss;
	VECTOR2				m_ImageMissRT;
	ImageData			m_CurImage;

	cImageSelf			m_ImageSuccessMission;
	VECTOR2				m_ImageSuccessMissionRT;
	cImageSelf			m_ImageFailMission;
	VECTOR2				m_ImageFailMissionRT;

	DWORD				m_dwLastFishingEndTime;
	
	BOOL				m_bLockStartBtn;
	DWORD				m_dwLockStartBtnTime;

	int					m_nMissionTime;
	DWORD				m_dwMissionStartTime;
	DWORD				m_dwMissionProcessTime;

public:
	CFishingDialog(void);
	virtual ~CFishingDialog(void);

	void Linking();
	virtual void SetActive(BOOL val);
	virtual void Render();

	virtual BOOL FakeMoveIcon( LONG x, LONG y, cIcon* );

	void ChangeBaitItem(CItem* pItem);		// 낚시창에 미끼아이템 등록
	void SetBaitItem(CBaseItem* pBait);		// 아이콘 링크용도로 사용
	void UpdateBaitItem();					// 아이콘 링크용도로 사용
	void RefreshBaitItem();					// 미끼아이템 수량갱신
	// 090811 ShinJS --- 미끼 Item의 Index 반환 함수 추가
	DWORD GetBaitItemIdx()						{ return m_ItemBait.GetItemIdx(); }

	void SetMissionItem(DWORD* pItemIdx);
	void CheckMissionState(DWORD dwItemIdx);

	void ClearPoint()							{m_nPerpect=m_nGreat=m_nGood=m_nMiss = 0;}
	int GetPointPerpect()						{return m_nPerpect;}
	int GetPointGreat()							{return m_nGreat;}
	int GetPointGood()							{return m_nGood;}
	int GetPointMiss()							{return m_nMiss;}
	void SetPushStartBtn(BOOL bVal)				{m_pStartBtn->SetPush(bVal);}
	void SetExpValue(GUAGEVAL val, DWORD estTime);
	void SetExpText(char* pText);
	void SetPointText(char* pText);
	void SetProcessTimeText(char* pText);

	void SetLastFishingEndTime(DWORD dwEndTime) {m_dwLastFishingEndTime=dwEndTime;}
	DWORD GetLastFishingEndTime()				{return m_dwLastFishingEndTime;}
	BOOL IsPushedStartBtn()						{if(!m_bActive) return FALSE; return m_pStartBtn->IsPushed();}

	void SetLockStartBtn(BOOL bVal);
	BOOL IsLockStartBtn()						{return m_bLockStartBtn;}
	void SetLockStartBtnTime(DWORD dwTime)		{m_dwLockStartBtnTime = dwTime;}

	void SetMissionStartTime(DWORD dwTime)		{m_dwMissionStartTime = dwTime; if(dwTime==0) SetProcessTimeText("");}
	void SetMissionProcessTime(DWORD dwTime)	{m_dwMissionProcessTime = dwTime; if(dwTime==0) SetProcessTimeText("");}

	// GlobalEventFunc 에서 호출
	void Fishing_Start();
	void Fishing_Pullling();
	void Fishing_Cancel();
};
