#pragma once


#include "GameState.h"

#define CHARMAKE USINGTON(CCharMake)


class CCharMakeNewDlg ;
class CCameraConfiguration;
class cImageSelf;

// 061218 LYW --- Add enum for scene.
class CCharMake : public CGameState  
{
	CCameraConfiguration* mCameraConfiguration;
	// 091221 LUJ, 하단 막대
	cImageSelf* mImageBottomLeftBar;
	VECTOR2 mPositionBottomLeftBar;
	VECTOR2 mScaleBottomLeftBar;
	cImageSelf* mImageBottomRightBar;
	VECTOR2 mPositionBottomRightBar;
	VECTOR2	mScaleBottomRightBar;
	BOOL m_bEnterGame;

	CCharMakeNewDlg* m_pCharMakeNewDlg;

	friend class cWindowManager;
	friend extern void	CM_CharMakeBtnFunc(LONG lId, void* p, DWORD we);
	friend extern void	CM_CharCancelBtnFunc(LONG lId, void* p, DWORD we);

public:
	CCharMake();
	virtual ~CCharMake();

	BOOL Init(void* pInitParam);
	void Release(CGameState* pNextGameState);

	void Process();
	void BeforeRender();
	void AfterRender();
	
	void NetworkMsgParse(BYTE Category,BYTE Protocol,void* pMsg, DWORD dwMsgSize);
	void SetCharMakeNewDlg( CCharMakeNewDlg* pDlg ) { m_pCharMakeNewDlg = pDlg ; }
	CCharMakeNewDlg* GetCharMakeNewDlg() { return m_pCharMakeNewDlg ; }


	void DisplayNotice( int MsgNum );
	void SetDisableDlg( BOOL val );

private:
	void UserConn_Character_NameCheck_Nack( void* pMsg ) ;
	void UserConn_Character_NameCheck_Ack() ;
	void UserConn_Character_Make_Nack() ;
	void UserConn_Character_Make_Ack( void* pMsg ) ;
	void UserConn_CharacterList_Ack( void* pMsg ) ;
	void Cheat_EventNotify_On( void* pMsg ) ;
	void Cheat_EventNotify_Off( void* pMsg ) ;
	void AdjustBar();

	// 070918 웅주, 마우스를 드래그하여 캐릭터를 회전시킴
private:
	void ProcessDragRotation();

	struct
	{
		float mRadian;		// 회전시킬 값
		float mSpeed;		// 회전 속도
	}
	mRotation;

	struct
	{
		int		mX;
		int		mY;
		DWORD	mIsDrag;
	}
	mPosition;
};


EXTERNGLOBALTON(CCharMake)