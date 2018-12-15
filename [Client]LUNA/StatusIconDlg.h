// StatusIconDlg.h: interface for the CStatusIconDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STATUSICONDLG_H__C0B71631_2257_49D3_B0A0_DC28AA71B614__INCLUDED_)
#define AFX_STATUSICONDLG_H__C0B71631_2257_49D3_B0A0_DC28AA71B614__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./Interface/cImage.h"
#include "./Interface/cMultiLineText.h"
class CObject;
class CMouse;
#define STATUSICONDLG USINGTON(CStatusIconDlg)



struct ICONRENDERINFO
{
	DWORD	ItemIndex;
	BOOL	bPlus;
	BOOL	bAlpha;
	int		Alpha;
};

struct StatusInfo
{
	cImage* Image;
	DWORD	Index;
	DWORD	Time;
	DWORD	ID;
	// 090204 LUJ, 수치 범위를 확장함
	int		Count;
	float	fAlpha;
};

class CStatusIconDlg  
{
	int m_MaxDesc;
	StaticString* m_pDescriptionArray;

	WORD m_IconCount[eStatusIcon_Max];
	cImage m_StatusIcon[eStatusIcon_Max];
	ICONRENDERINFO m_IconInfo[eStatusIcon_Max];
	
	DWORD m_dwRemainTime[eStatusIcon_Max];
	DWORD m_dwStartTime[eStatusIcon_Max];

	int m_MaxIconPerLine;		// 한줄에 아이콘 몇개?

	cMultiLineText m_toolTip;
	int m_CurIconNum;
//	LONG m_ttX, m_ttY;

//	BOOL m_bInit;

	CYHHashTable< StatusInfo > mStatusInfoTable;
	int		m_nQuestIconCount;
	VECTOR2	Scale;
	
	void LoadDescription();
protected:
	VECTOR2 m_DrawPosition;		// 그릴 위치
	CObject* m_pObject;

public:
	CStatusIconDlg();
	virtual ~CStatusIconDlg();

	void AddIcon(CObject* pObject,WORD StatusIconNum,DWORD ItemIdx=0, DWORD dwRemainTime = 0);
	void AddQuestTimeIcon(CObject* pObject,WORD StatusIconNum);
	void RemoveIcon(CObject* pObject,WORD StatusIconNum, DWORD ItemIdx=0);
	void RemoveQuestTimeIcon(CObject* pObject,WORD StatusIconNum);
	void RemoveAllQuestTimeIcon();
	// 090204 LUJ, 수치 범위를 확장함
	void AddSkillIcon( DWORD ID, DWORD Index, DWORD time, int count );
	void RemoveSkillIcon( DWORD ID, DWORD Index );
	void RemoveAllSkillIcon( );
	void UpdateSkillTime( DWORD ID, DWORD Index, DWORD time );
	BOOL IsHasBuff(DWORD skillIndex);
	void UpdateSkillCount( DWORD ID, DWORD Index, int count );

	virtual void Render();

	void Init(CObject* pObject,VECTOR2* pDrawPosition,int MaxIconPerLine);
	void Release();
	void AddQuestIconCount()	{ ++m_nQuestIconCount; }
	// 080318 LUJ, 파티원 버프 스킬 표시위해 오브젝트 설정
	inline void SetObject( CObject* object ) { m_pObject = object; }

	// 08318 LUJ, 현재 표시 상태를 얻어온다
	inline const CObject* GetObject() const { return m_pObject; };

	// 080318 LUJ, 현재 표시 중인 스킬을 자료구조에 복사한다
	void Copy( std::list< StatusInfo >& );

	// 080318 LUJ, 상태창 위치 설정
	inline void SetPosition( const VECTOR2& position ) { m_DrawPosition = position; }

	// 080318 LUJ, 아이콘 크기 설정
	void SetIconSize( float width, float height );	
private:
	VECTOR2	mIconSize;
	

	// 080318 LUJ, 최대 아이콘 표시 개수
public:
	inline void SetMaxIconQuantity( DWORD quantity ) { mMaxIconQuantity = quantity; }	
private:
	DWORD	mMaxIconQuantity;

	// 080318 LUJ, 최대 표시 개수를 초과한 아이콘은 여기에 저장해두었다가, 공간이 남을때 표시된다
	typedef std::list< StatusInfo* >	ReservedIconList;
	ReservedIconList					mReservedIconList;

	// 080318 LUJ, 경과 시간 표시
	DWORD mElaspedTime;

	// 080318 LUJ, 스킬을 표시하고 있는 타겟 플레이어 인덱스를 반환한다
	DWORD GetPlayerIndex();	
};

EXTERNGLOBALTON(CStatusIconDlg)

#endif // !defined(AFX_STATUSICONDLG_H__C0B71631_2257_49D3_B0A0_DC28AA71B614__INCLUDED_)
