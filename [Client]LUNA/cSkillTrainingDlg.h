#pragma once
#include ".\interface\cdialog.h"

class cListDialog;
class cStatic;

class cSkillTrainingDlg :
	public cDialog
{
	cListDialog*	m_SkillListDlg;
	cListDialog*	m_SkillTipListDlg;
	
	cStatic*		m_SkillPoint;
	cStatic*		m_Gold;

	cStatic*		m_Icon;

	cStatic*		m_NeedSp;
	cStatic*		m_NeedGold;
	cStatic*		m_AddDamageOptTooltip;

	SkillData		m_SkillList[ 100 ];

	int				m_Index;

	// 080417 LUJ, 길드 스킬 표시 상태인지 나타내는 플래그
	BOOL			mGuildMode;

public:
	cSkillTrainingDlg(void);
	virtual ~cSkillTrainingDlg(void);

	void Linking();

	// 080618 LYW : 상황에 맞게 부속 창을 표시하는 기능을 하는 함수 추가.
	void DisplayGuildSkill() ;

	// 080618 LYW : HERO의 이전 직업 들 중에 해당하는 스킬이 있는지 확인하고, 있다면,
	//			  : 그 스킬이 습득 가능한 최대 레벨을 반환하는 함수 추가.
	int GetSkillLevelFromJobTree(DWORD dwSkillIdx, WORD wCurJobLevel) ;

	// 080618 LYW : 직업에 따른 스킬종류 / 스킬명 / 색상 / 레벨등을 세팅하는 함수 추가.
	void AddSkillToSkillListDlg(BYTE byJobType, WORD wJobLevel, WORD* pPos) ;

	// 스킬 습득 창을 여는 함수.
	void OpenDialog() ;

	// 080417 LUJ, 길드 스킬 표시
	void OpenGuildSkill();
	// 080417 LUJ, 길드 스킬 학습 창이면 참을 반환한다
	BOOL IsGuildMode() const { return mGuildMode; }

	void OnActionEvent( LONG lId, void* p, DWORD we );

	void SetSkillInfo();
	void SetGold();
	void SetSkillPoint();

	void Training();
};
