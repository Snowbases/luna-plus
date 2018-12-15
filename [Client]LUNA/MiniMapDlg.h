#pragma once


#include "Interface/cDialog.h"
#include "MiniMapHeroIcon.h"
#include "cImageSelf.h"

class cStatic;
class cPushupButton;
class cChannelComboBox;

struct MINIMAPIMAGE
{
	cImageSelf	image;
	SIZE		size;
};

class CMiniMapDlg : public cDialog  
{
	cStatic* m_pMapName;
	// 080121 LYW --- MiniMapDlg : 채널용 스태틱 추가.
	cStatic* m_pPosXY;	

	cPushupButton* m_pLevelUpBTN;
	DWORD		   m_dwLevelUPBTNStartTime ;

	// 100614 체널 정보 콤보박스
	cChannelComboBox*	m_pChannelComboBox;

	// 091210 ShinJS --- 녹화 버튼 추가
	cButton*	m_pVideoCaptureStartBtn;
	cButton*	m_pVideoCaptureStopBtn;

	MAPTYPE m_MapNum;

	CMiniMapHeroIcon m_HeroIcon;	
	CYHHashTable<CMiniMapIcon> m_IconTable;
	CMemoryPoolTempl<CMiniMapIcon> m_IconPool;
	CMiniMapIconImage m_pIconImage[eMNMICON_MAX];
	// 090909 ShinJS --- 길찾기시 Move Point 출력용 CMiniMapIcon
	CMiniMapIcon m_MovePoint;
	
	int m_CurMode;
	MINIMAPIMAGE m_MinimapImage[eMINIMAPMODE_MAX];

	// 웅주 070522
	struct stMiniMap
	{
		// cImageSelf*를 쓰고 사용하는 이미지를 연결해서 쓸 경우 DLL에서 Access violation 발생. 
		// 이애 cImage에 스프라이트 이미지를 복사하는 방법 사용
		cImage mImage;
		cImageRect mRect;
		VECTOR2 mScale;
		
		cImageSize mViewport;
		VECTOR2 mTranslation;
	}
	m_MiniMap;

	cImage m_TooltipImage;

	void InitIconImage();

	void SetZoomRate(float zoomrate);

	void AddIcon( WORD Kind, DWORD dwObjectID, VECTOR3* pObjectPos, CObject* pObject, PARTY_MEMBER* pMember, CSHFarm* pFarm );
	void ReleaseMinniMapIcon(CMiniMapIcon* pIcon);
	void LoadMinimapImageInfo(MAPTYPE MapNum);

public:
	CMiniMapDlg();
	virtual ~CMiniMapDlg();
	virtual DWORD ActionEvent( CMouse* mouseInfo );
	void InitMiniMap(MAPTYPE MapNum);
	
	void Linking();
	virtual void SetActive( BOOL val );

	void AddMonsterIcon(CObject* pObject);
	void AddBossMonsterIcon(CObject* pObject);
	void AddStaticNpcIcon(CObject* pObject);

	void AddPartyMemberIcon(PARTY_MEMBER* pMember);
	void AddPartyMasterIcon(CObject* pObject);
	void AddQuestIcon( CObject* pTarget, int nKind );
	void AddHeroIcon(CObject* pObject);
	void AddFarmIcon(CSHFarm* pFarm);
	void RemoveIcon(DWORD ObjectID);

	void Process();
	void Render();

	void SetPartyIconTargetPos(DWORD PlayerID, int posX, int posZ);
	void SetPartyIconObjectPos(DWORD PlayerID, int posX, int posZ);
	
	int GetCurMode()	{ return m_CurMode; }
	
	BOOL CanActive();

	void UpdateQuestMark();

	// 090909 ShinJS --- 길찾기 작업
	void ClearMovePoint() { m_MovePoint.ShowQuestMark( eQM_NONE ); }		// Move Point 제거(미출력상태로 변경)
	void SetMovePoint( VECTOR3* pDestPos );									// Move Point 설정

	// 091209 ShinJS --- 길찾기 실행
	void Move_UsePath();

	// 091210 ShinJS --- 비디오 녹화 버튼 추가
	void ShowVideoCaptureStartBtn( BOOL bShow );

	// 100111 ONS 미니맵을 표시하지 않는 맵인지 여부를 알려준다.
	BOOL HaveMinimap() { return !m_MiniMap.mImage.IsNull(); }

	// 100610 레벨업 버튼이 유효한지 검사
	void ChangeLevelUpBTNState();

	// 레벨업 버튼 Disable여부 검사
	void ChangePushLevelUpBTN();

	// 레벨업 버튼 활성화되면 Alpha 시작
	void ActiveLevelUpButton();
	
	void SetChanelComboBox( MSG_CHANNEL_INFO* pInfo );

	void RequestChannelInfo();

	void ChangeChannel();
};