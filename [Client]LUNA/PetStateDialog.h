#pragma once
#include "./interface/cdialog.h"

class cStatic;
class cComboBox;

class CPetStateDialog :
	public cDialog
{
	cStatic*	mName;
	cStatic*	mLevel;
	cStatic*	mType;
	cStatic*	mExp;
	cStatic*	mGrade;

	cStatic*	mStr;
	cStatic*	mDex;
	cStatic*	mVit;
	cStatic*	mInt;
	cStatic*	mWis;

	cStatic*	mPhysicAttack;
	cStatic*	mMagicAttack;
	cStatic*	mPhysicDefense;
	cStatic*	mMagicDefense;
	cStatic*	mAccuracy;
	cStatic*	mAvoid;

	cComboBox*	mAI;

public:
	CPetStateDialog(void);
	virtual ~CPetStateDialog(void);

	void Linking();
	
	virtual void OnActionEvent(LONG id, void* p, DWORD event);
	virtual void SetActive( BOOL val );

	void SetInfo();

	// 091215 ShinJS --- AI 선택 상태를 반환하는 함수 추가
	int GetAISelectedIdx();
};
