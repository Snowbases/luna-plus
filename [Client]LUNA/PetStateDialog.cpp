#include "stdafx.h"
#include "WindowIDEnum.h"
#include ".\petstatedialog.h"
#include "gamein.h"
#include "cStatic.h"
#include "ObjectManager.h"
#include "petmanager.h"
#include "./Interface/cComboBox.h"
#include "cResourceManager.h"
#include "chatmanager.h"

CPetStateDialog::CPetStateDialog(void)
{
	m_type = WT_PET_STATE_DLG;
}

CPetStateDialog::~CPetStateDialog(void)
{
}

void CPetStateDialog::Linking()
{
	mName			= ( cStatic* )GetWindowForID(PET_STATUS_NAME);
	mLevel			= ( cStatic* )GetWindowForID(PET_STATUS_LEVEL);
	mType			= ( cStatic* )GetWindowForID(PET_STATUS_TYPE);
	mExp			= ( cStatic* )GetWindowForID(PET_STATUS_EXP);
	mGrade			= ( cStatic* )GetWindowForID(PET_STATUS_GRADE);

	mStr			= ( cStatic* )GetWindowForID(PET_STATUS_STR);
	mDex			= ( cStatic* )GetWindowForID(PET_STATUS_DEX);
	mVit			= ( cStatic* )GetWindowForID(PET_STATUS_VIT);
	mInt			= ( cStatic* )GetWindowForID(PET_STATUS_INT);
	mWis			= ( cStatic* )GetWindowForID(PET_STATUS_WIS);

	mPhysicAttack	= ( cStatic* )GetWindowForID(PET_STATUS_PHY_ATT);
	mMagicAttack	= ( cStatic* )GetWindowForID(PET_STATUS_MAG_ATT);
	mPhysicDefense	= ( cStatic* )GetWindowForID(PET_STATUS_PHY_DEF);
	mMagicDefense	= ( cStatic* )GetWindowForID(PET_STATUS_MAG_DEF);
	mAccuracy		= ( cStatic* )GetWindowForID(PET_STATUS_ACCURACY);
	mAvoid			= ( cStatic* )GetWindowForID(PET_STATUS_AVOID);

	mAI				= ( cComboBox* )GetWindowForID(PET_STATUS_AI_COMBOBOX);
}

void CPetStateDialog::OnActionEvent(LONG id, void* p, DWORD event)
{
	if( id == PET_STATUS_AI_COMBOBOX && event & WE_COMBOBOXSELECT)
	{
		HEROPET->mAI = ePetAI(mAI->GetCurSelectedIdx());
		mAI->SelectComboText(
			WORD(mAI->GetCurSelectedIdx()));

		CHATMGR->AddMsg(
			CTC_SYSMSG,
			CHATMGR->GetChatMsg(1579 + mAI->GetCurSelectedIdx()));
	}
}

void CPetStateDialog::SetActive( BOOL val )
{
	if( val )	SetInfo();

	cDialog::SetActive( val );
}

void CPetStateDialog::SetInfo()
{
	float penalty = PETMGR->GetFriendlyPenalty( HEROPET->mFriendly );

	mAI->SetCurSelectedIdx(
		HEROPET->mAI);
	mAI->SelectComboText(
		WORD(HEROPET->mAI));

	mName->SetStaticText(HEROPET->GetObjectName());
	mLevel->SetStaticValue(HEROPET->mLevel);			
	if( HEROPET->mType == ePetType_Basic )
	mType->SetStaticText(RESRCMGR->GetMsg(1005));	
	else
	mType->SetStaticText(RESRCMGR->GetMsg(HEROPET->mType+996));

	// 091208 ShinJS --- 친밀도 -> 경험치로 수정
	char txt[10] = {0,};
	WORD wMaxExp = PETMGR->GetExp( HEROPET->mGrade );
	sprintf( txt, "%.1f%%", (float)HEROPET->mExp / (float)wMaxExp * 100.0f );
	mExp->SetStaticText( txt );

	mGrade->SetStaticText(RESRCMGR->GetMsg(HEROPET->mGrade+1000));			

	mStr->SetStaticValue(LONG(HEROPET->mPetStat.mStrength.mPlus));
	mDex->SetStaticValue(LONG(HEROPET->mPetStat.mDexterity.mPlus));
	mVit->SetStaticValue(LONG(HEROPET->mPetStat.mVitality.mPlus));
	mInt->SetStaticValue(LONG(HEROPET->mPetStat.mIntelligence.mPlus));
	mWis->SetStaticValue(LONG(HEROPET->mPetStat.mWisdom.mPlus));

	mPhysicAttack->SetStaticValue(LONG(HEROPET->mPetStat.mPhysicAttack.mPlus * penalty));
	mMagicAttack->SetStaticValue(LONG(HEROPET->mPetStat.mMagicAttack.mPlus * penalty));
	mPhysicDefense->SetStaticValue(LONG(HEROPET->mPetStat.mPhysicDefense.mPlus * penalty));
	mMagicDefense->SetStaticValue(LONG(HEROPET->mPetStat.mMagicDefense.mPlus * penalty));
	mAccuracy->SetStaticValue(LONG(HEROPET->mPetStat.mAccuracy.mPlus));
	mAvoid->SetStaticValue(LONG(HEROPET->mPetStat.mEvade.mPlus));
}

int CPetStateDialog::GetAISelectedIdx()
{
	return mAI->GetCurSelectedIdx();
}