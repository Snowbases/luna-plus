#pragma once


#include "Interface/cDialog.h"
#include "MiniMapHeroIcon.h"
#include "cImageSelf.h"

// desc_hseos_농장시스템_01
// S 농장시스템 추가 added by hseos 2007.10.31
class CSHFarm;
// E 농장시스템 추가 added by hseos 2007.10.31

class CBigMapDlg : public cDialog
{
protected:

	CMiniMapHeroIcon				m_HeroIcon;	
	CYHHashTable<CMiniMapIcon>		m_IconTable;
	CMemoryPoolTempl<CMiniMapIcon>	m_IconPool;
	CMiniMapIconImage				m_pIconImage[eBMICON_MAX];

	CMiniMapIcon*					m_pIconForToolTip;

	cImageSelf		m_MapImage;
	WORD			m_MapNum;

	cImage			m_TooltipImage;

	// 070209 LYW --- Add VECTOR2.
	VECTOR2 m_vScale ;

	// 090909 ShinJS --- 길찾기시 Move Point 출력용 CMiniMapIcon
	CMiniMapIcon					m_MovePoint;

	BOOL							m_bUseMovePath;

public:
	CBigMapDlg();
	virtual ~CBigMapDlg();

	virtual void SetActive( BOOL val );
	virtual void Render();
	void Linking();
	void LoadMinimapImageInfo( MAPTYPE MapNum );
	void InitBigMap( MAPTYPE MapNum );

	void Process();
	void InitIconImage();

	void ReleaseMinniMapIcon( CMiniMapIcon* pIcon );
	void SetPartyIconObjectPos(DWORD PlayerID, int posX, int posZ);
	void SetPartyIconTargetPos(DWORD PlayerID, int posX, int posZ);
	
	void AddIcon( WORD Kind, DWORD dwObjectID, VECTOR3* pObjectPos, CObject* pObject, PARTY_MEMBER* pMember, CSHFarm* pFarm );

	void AddHeroIcon( CObject* pObject );
	void AddStaticNpcIcon( CObject* pObject );
	void AddStaticNpcIcon( const WORD wNpcJob, const DWORD dwObjectID, VECTOR3* pObjectPos, CObject* pObject );
	void AddPartyMemberIcon( PARTY_MEMBER* pMember );
	void AddFarmIcon(CSHFarm* pFarm);


	void RemoveIcon( DWORD ObjectID );
	void RemoveALLIcon();
	virtual DWORD ActionEvent( CMouse* mouseInfo );
	void OnActionEvent( LONG id, void* p, DWORD we );

	BOOL CanActive();

	void ShowQuestMarkIcon( CObject* pTarget, int nKind );
	void UpdateQuestMark();

	// 090909 ShinJS --- 길찾기 작업
	void ClearMovePoint() { m_MovePoint.ShowQuestMark( eQM_NONE ); }		// Move Point 제거(미출력상태로 변경)
	void SetMovePoint( VECTOR3* pDestPos );									// Move Point 설정
	void UseMovePath( const BOOL bUseMovePath ) { m_bUseMovePath = bUseMovePath; }
	void Move_UsePath();

private:
	SIZE m_mapSize;
};