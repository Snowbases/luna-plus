#pragma once


#include "cDialog.h"

enum EICONTYPE
{
	eIconType_NotInited		= 0,
	eIconType_AllItem		= 0x00000001,
	eIconType_Skill			= 0x00000002,
//KES EXCHANGE 030920 //±³È¯Ã¢ ¹× ³ëÁ¡»ó¿¡ ¸µÅ©µÈ ¾ÆÀÌÅÛ
	eIconType_LinkedItem	= 0x00000003,
};

class cIconDialog;

class cIcon : public cDialog  
{

protected:

	WORD m_CellX;
	WORD m_CellY;

	DWORD m_IconType;
	DWORD m_dwData;
	
//KES EXCHANGE 031001
	BOOL	m_bLock;
	cImage	m_LockImage;

	BOOL	m_bZeroCount ;
	cImage	m_ZeroImage ;
	// 071126 LYW --- cIcon : 봉인 이미지 추가.
	BOOL	m_bSeal ;
	cImage  m_SealImage ;
//
	cPtrList m_LinkDialogList;
	
	BOOL	m_bState;
	cImage  m_StateImage ;

public:
	cIcon();
	virtual ~cIcon();

	virtual void Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID=0);
	virtual void Render();
	virtual void Render(VECTOR2* pPos);

	// 080225 LUJ, 툴팁을 강제로 렌더링한다
	void ToolTipForcedRender();

	virtual DWORD ActionEvent(CMouse * mouseInfo);
	
	inline void SetIconType(DWORD type){ m_IconType=type; }
	inline DWORD GetIconType(){ return m_IconType; }

	inline void SetData(DWORD dwData)	{	m_dwData = dwData;	}
	inline DWORD GetData()	{	return m_dwData;	}

	void AddLinkIconDialog(cIconDialog* pDialog);
	void RemoveLinkIconDialog(cIconDialog* pDialog);

//KES EXCHANGE 031001
	virtual void SetLock( BOOL bLock );
	BOOL IsLocked() { return m_bLock; }

	// 071126 LYW --- cIcon : 봉인 여부 세팅 함수 추가.
	void SetSeal(BOOL bSeal) { m_bSeal = bSeal ; }
	BOOL IsSeal() { return m_bSeal ; }

	virtual void SetZeroCount(BOOL bZeroCount) { m_bZeroCount = bZeroCount ; }
	BOOL IsZeroCount() { return m_bZeroCount ; }

	// LBS Ãß°¡ 
	void SetLockImage( cImage* pImage ) { m_LockImage = *pImage; }
	
	void SetStateImage( cImage* pImage ) { m_StateImage = *pImage; m_bState = TRUE; }
	void ClearStateImage( ) { m_bState = FALSE; }
//

	void SetCellPosition(WORD x,WORD y)
	{
		m_CellX = x;
		m_CellY = y;
	}
	WORD GetCellX()	{	return m_CellX;	}
	WORD GetCellY()	{	return m_CellY;	}
	

	virtual BOOL CanMoveToDialog( cDialog* targetdlg )	{	return TRUE;	}
	virtual BOOL CanDelete()							{	return TRUE;	}	


	// 070914 쿨타임 관리
public:
	void	SetCoolTime( DWORD miliSecond );
	DWORD	GetRemainedCoolTime() const;

	// 080326 NYJ --- 지정값으로 쿨타임갱신 추가
	void	SetCoolTimeAbs(DWORD dwBegin, DWORD dwEnd, DWORD miliSecond);

private:
	struct
	{
		DWORD	mBeginTick;
		DWORD	mEndTick;
		DWORD	mMax;
		cImage	mImage;
	}
	mCoolTime;
};