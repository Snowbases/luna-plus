/**********************************************************************

이름: ProgressDialog
작성: 2007/09/18 17:35:59, 이웅주

목적:	조합/분해/강화/인챈트 때 프로그레스바를 표시한다.
		각각의 작업은 요청 내역을 저장해두었다가, 설정된 시간 이후에
		자동으로 요청된다.
                                                                     
***********************************************************************/
#pragma once


#include "interface/cDialog.h"

// 071213 KTH -- 딜레이 2초
#define WAITMILISECOND	2000
#define WAITMILISECOND_FOR_COOK	2800

class cStatic;
class CObjectGuagen;
class cTextArea;

class CProgressDialog : public cDialog
{
public:
	CProgressDialog();
	virtual ~CProgressDialog();

	virtual void Render();
	virtual void SetActive( BOOL );
	virtual void OnActionEvent( LONG id, void* p, DWORD event );

	void Linking();
	void SetText( const char* );

	enum eAction
	{
		eActionNone,
		eActionEnchant,		// 인챈트
		eActionMix,			// 조합
		eActionReinforce,	// 강화
		eActionApplyOption,	// 옵션 적용
		eActionPetSummon,
		eActionPetGradeUp,
		eActionPetResurrection,
		// 080916 LUJ, 아이템 합성 추가
		eActionCompose,

		// 080826 LYW --- ProgressDialog : 워터시드 사용.
		eActionUseWaterSeed,

		eActionChangeSize,	// 키변경
		eActionSummonMonster, // 몬스터소환
		
		eActionExSummonEffect,	// 이펙트소환
		eActionExCooking,	// 요리
		// 090422 ShinJS --- 탈것 소환/봉인
		eActionVehicleSummon,
		eActionVehicleSeal,
	};

	void Wait(eAction);
	void Wait(DWORD second, LPCTSTR, cbFUNC);
	void Cancel();
	eAction	GetActionType() const { return	mAction.mType; }

private:
	void Restore();

	cWindow* mTextImage;
   	cTextArea* mTextArea;
   	cWindow* mCancelBtn;
	cStatic* mText;
	CObjectGuagen* mGauge;

	struct Action
	{
		DWORD mBeginTick;
		DWORD mEndTick;
		eAction	mType;
		BOOL mIsWaiting;
		cbFUNC mFunctionPointer;
	}
	mAction;

	BOOL  m_bHideDlg;
	DWORD m_dwExtra;
	// 090422 ShinJS --- 탈것 소환 위치를 저장, Progress 종료후 소환패킷전송시 사용
	VECTOR3	m_vecVehicleSummonPos;

public:
	void SetVehicleSummonPos( const VECTOR3& vecPos ) { m_vecVehicleSummonPos = vecPos; }
};
