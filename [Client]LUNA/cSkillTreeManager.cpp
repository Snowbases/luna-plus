#include "stdafx.h"
#include "cSkillTreeManager.h"
#include "cJobSkillDlg.h"
#include "../[cc]skill/client/info/ActiveSkillInfo.h"
#include "../[cc]skill/client/manager/Skillmanager.h"
#include "cSkillbase.h"
#include "cSkillTrainingDlg.h"
#include "WindowIDEnum.h"
#include "Interface/cWindowManager.h"
#include "Interface/cScriptManager.h"
#include "interface/cIconGridDialog.h"
#include "interface/cTabDialog.h"
#include "ItemManager.h"
#include "chatmanager.h"
#include "QuickManager.h"
#include "ObjectManager.h"
#include "GameIn.h"
#include "InventoryExDialog.h"
#include "CharacterCalcManager.h"
#include "CharacterDialog.h"

cSkillTreeManager::cSkillTreeManager(void)
{
	mSkillBaseTable.Initialize(
		10);
}

cSkillTreeManager::~cSkillTreeManager(void)
{

}

void cSkillTreeManager::Init()
{
//	100531 IconIndexCreator 초기화	
	mIconIndexCreator.Init(
		MAX_ITEMICON_NUM,
		1);
}

void cSkillTreeManager::Release()
{
	mSkillBaseTable.SetPositionHead();

	for(cSkillBase* pSkill = mSkillBaseTable.GetData();
		0 < pSkill;
		pSkill = mSkillBaseTable.GetData())
	{
		mSkillBaseTable.Remove( pSkill->GetSkillIdx() );
		mIconIndexCreator.ReleaseIndex( pSkill->GetID() );
		WINDOWMGR->AddListDestroyWindow( pSkill );
	}

	mSkillBaseTable.RemoveAll();
	mStoredSkill.clear();
	mStoredSetSkill.clear();
	//	100531 IconIndexCreator Release() 처리
	mIconIndexCreator.Release();
}

void cSkillTreeManager::NetworkMsgParse(BYTE Protocol,void* pMsg)
{
	switch(Protocol) 
	{
	case MP_SKILLTREE_UPDATE_ACK:
		{
			MSG_SKILL_UPDATE* const pmsg = (MSG_SKILL_UPDATE*)pMsg;
			const DWORD index = pmsg->SkillBase.wSkillIdx;
			const LEVELTYPE	level = pmsg->SkillBase.Level;

			Update(
				pmsg->SkillBase);
			cActiveSkillInfo* pInfo = ( cActiveSkillInfo* )SKILLMGR->GetSkillInfo( index + level - 1 );

			if( pInfo )
			{
				DWORD point = HERO->GetHeroTotalInfo()->SkillPoint -= pInfo->GetSkillInfo()->TrainPoint;
				HERO->GetHeroTotalInfo()->SkillPoint = 0;
				HERO->SetSkillPoint( point );
			}

			// 070510 LYW --- cSkillTreeManager : Update skill info.
			{
				cSkillTrainingDlg* dialog = ( cSkillTrainingDlg* )WINDOWMGR->GetWindowForID( SKILL_TRAINING );
				// 090909 ONS 스킬습득창이 활성화된 상태에서만 업데이트한다.
				if(dialog->IsActive())
				{
					dialog->SetDisable( FALSE );
					dialog->OpenDialog();
				}
			}

			QUICKMGR->RefreshSkillItem(
				index);
			GAMEIN->GetInventoryDialog()->RefreshArmorItem();
			GAMEIN->GetCharacterDialog()->RefreshInfo();
		}
		break;
	case MP_SKILLTREE_UPDATE_NACK:
		{
			// 070613 LYW --- cSkillTreeManager : Add msg to chatting dlg.
			char szSkillInfo[128] = {0, } ;
			sprintf(szSkillInfo, CHATMGR->GetChatMsg( 1444 )) ;
			CHATMGR->AddMsg(CTC_TIPS, szSkillInfo) ;

			{
				cSkillTrainingDlg* dialog = ( cSkillTrainingDlg* )WINDOWMGR->GetWindowForID( SKILL_TRAINING );

				dialog->SetDisable( FALSE );
			}
		}
		break;
	case MP_SKILLTREE_POINT_NOTIFY:
		{
			MSG_DWORD* pmsg = ( MSG_DWORD* )pMsg;

			HERO->SetSkillPoint( pmsg->dwData );
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1164 ), pmsg->dwData );
		}
		break;
	}
}

void cSkillTreeManager::SetToolTipIcon(cIcon* pSkill, DWORD skillIndex)
{
	cSkillInfo* const pCommSkill = SKILLMGR->GetSkillInfo(
		skillIndex);

	if( ! pCommSkill )
	{
		return;
	}

	const ACTIVE_SKILL_INFO* pActiveSkillInfo = ((cActiveSkillInfo*)pCommSkill)->GetSkillInfo();
	if( ! pActiveSkillInfo )
	{
		return;
	}

	cImage imgToolTip;
	SCRIPTMGR->GetImage( 0, &imgToolTip, PFT_HARDPATH );
	pSkill->SetToolTip( "", RGBA_MAKE(255, 255, 255, 255), &imgToolTip, TTCLR_ITEM_CANEQUIP );

	// 100315 ShinJS --- 스킬툴팁 정보표시 변경
	TCHAR line[MAX_PATH] = {0};
	wsprintf( line, _T("[ %s  LV. %d ]"), pCommSkill->GetName(), pCommSkill->GetLevel() );

	// 스킬 종류(스탯/무기)에 대한 스킬이름 색상 변경
	DWORD dwColor = TTTC_SKILL_DEFAULT;
	if( pActiveSkillInfo->Unit == UNITKIND_PHYSIC_ATTCK ||
		pActiveSkillInfo->Unit == UNITKIND_MAGIC_ATTCK )
	{
		switch( pActiveSkillInfo->UnitDataType )
		{
		case 1:			dwColor = TTTC_SKILL_TYPE1;			break;
		case 2:			dwColor = TTTC_SKILL_TYPE2;			break;
		}
	}

	pSkill->AddToolTipLine(	line, dwColor );
	pSkill->AddToolTipLine(	_T("") );

	if( pCommSkill->GetKind() != SKILLKIND_PASSIVE )
	{
		char typeTxt[MAX_PATH]={0,};
		// 필요장비
		if( SKILLMGR->GetSkillEquipTypeText( pActiveSkillInfo, typeTxt, MAX_PATH ) )
		{
			sprintf(line, CHATMGR->GetChatMsg( 2049 ), SKILLMGR->GetSkillEquipTypeText( pActiveSkillInfo, typeTxt, MAX_PATH ) );
			pSkill->AddToolTipLine( line, TTTC_SKILLLEVEL );
		}

		// Life 표시
		if( pActiveSkillInfo->mLife.mPercent )
		{
			sprintf( line, "%s%0.0f%%", CHATMGR->GetChatMsg( 2050 ), pActiveSkillInfo->mLife.mPercent * 100.0f );
			pSkill->AddToolTipLine( line, TTTC_SKILLLEVEL );
		}
		else if( pActiveSkillInfo->mLife.mPlus )
		{
			sprintf( line, "%s%0.0f", CHATMGR->GetChatMsg( 2050 ), pActiveSkillInfo->mLife.mPlus );
			pSkill->AddToolTipLine( line, TTTC_SKILLLEVEL );
		}

		// 080616 LUJ, 상대적으로 소요되는 마나 양 표시
		if( pActiveSkillInfo->mMana.mPercent )
		{
			sprintf( line, "%s%0.0f%%", CHATMGR->GetChatMsg( 768 ), pActiveSkillInfo->mMana.mPercent * 100.0f );
			pSkill->AddToolTipLine( line, TTTC_SKILLLEVEL );
		}
		// 080616 LUJ, 절대적으로 표시되는 마나 양 표시
		else if( pActiveSkillInfo->mMana.mPlus )
		{
			sprintf( line, "%s%0.0f", CHATMGR->GetChatMsg( 768 ), pActiveSkillInfo->mMana.mPlus );
			pSkill->AddToolTipLine( line, TTTC_SKILLLEVEL );
		}

		// 필요 아이템
		if( pActiveSkillInfo->mConsumeItem.wIconIdx )
		{
			const ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo( pActiveSkillInfo->mConsumeItem.wIconIdx );
			if( pItemInfo )
			{
				sprintf( line, CHATMGR->GetChatMsg( 2051 ), pItemInfo->ItemName, pActiveSkillInfo->mConsumeItem.Durability );
				pSkill->AddToolTipLine( line, TTTC_SKILLLEVEL );
			}
		}

		// 소비 비용
		if( pActiveSkillInfo->Money )
		{
			sprintf( line, CHATMGR->GetChatMsg( 2052 ), AddComma( pActiveSkillInfo->Money ) );
			pSkill->AddToolTipLine( line, TTTC_SKILLLEVEL );
		}

		// 소비 포인트정보
		if( pActiveSkillInfo->mConsumePoint.mType != ACTIVE_SKILL_INFO::ConsumePoint::TypeNone )
		{
			pSkill->AddToolTipLine( SKILLMGR->GetConsumePointTypeText( pActiveSkillInfo, typeTxt, MAX_PATH ), TTTC_SKILLLEVEL );
		}

		// 시/분/초로 표현
		char timeMsg[MAX_PATH]={0,};
		int coolHour=0, coolMin=0;
		float coolSec = pActiveSkillInfo->CoolTime / 1000.f;
		if( coolSec > 0.1f )
		{
			sprintf( line, CHATMGR->GetChatMsg( 2054 ) );		// "재사용시간: "
		}
		else if( coolSec >= 60.f )
		{
			coolMin = (int)(coolSec / 60.f);
			coolSec = coolSec - coolMin * 60.f;
			if( coolMin >= 60.f )
			{
				coolHour = (int)(coolMin / 60.f);
				coolMin = coolMin - coolHour * 60;
			}
		}

		if( coolHour )
		{
			sprintf( timeMsg, CHATMGR->GetChatMsg( 1410 ), coolHour, coolMin, coolSec );
			strcat( line, timeMsg );
		}
		if( coolMin )
		{
			sprintf( timeMsg, CHATMGR->GetChatMsg( 1411 ), coolMin, coolSec );
			strcat( line, timeMsg );
		}
		if( coolSec )
		{
			sprintf( timeMsg, CHATMGR->GetChatMsg( 2053 ), coolSec );
			strcat( line, timeMsg );
		}

		pSkill->AddToolTipLine( line, TTTC_SKILLLEVEL );

		// 091105 ONS 툴팁에 "필요 버프" 추가
		DWORD dwBuffSkillIdx = pActiveSkillInfo->RequiredBuff;
		if(dwBuffSkillIdx)
		{
			cSkillInfo* pSkillInfo = SKILLMGR->GetSkillInfo(dwBuffSkillIdx) ;
			sprintf(line, CHATMGR->GetChatMsg( 1973 ), pSkillInfo->GetName());
			pSkill->AddToolTipLine( line, TTTC_SKILLLEVEL );
		}
	}
	ITEMMGR->AddToolTip( pSkill, pCommSkill->GetTooltip() );

	if(pCommSkill->GetCoolTime() > gCurTime)
	{
		TCHAR text[MAX_PATH] = {0};
		_stprintf(
			text,
			_T("%0.1f%s"),
			float(pCommSkill->GetCoolTime() - gCurTime) / 1000,
			CHATMGR->GetChatMsg(675));
		pSkill->AddToolTipLine(
			text,
			TTTC_QUESTITEM);
	}
}

cSkillBase* cSkillTreeManager::GetSkill(DWORD SkillIdx)
{
	return mSkillBaseTable.GetData(SkillIdx / 100 * 100 + 1);
}

void cSkillTreeManager::Update(const SKILL_BASE& updatedSkill)
{
	const cSkillInfo* const skillInfo = SKILLMGR->GetSkillInfo(
		updatedSkill.wSkillIdx);

	if(0 == skillInfo)
	{
		return;
	}

	cTabDialog*	const treeDialog = (cTabDialog*)WINDOWMGR->GetWindowForID(
		STD_SKILLTREEDLG);
	cTabDialog* const tabDialog = (cTabDialog*)treeDialog->GetTabSheet(
		BYTE((SKILLKIND_PASSIVE == skillInfo->GetKind() ? eSkillDlgTab_Passive : eSkillDlgTab_Active)));

	for(BYTE tabIndex = 0; tabIndex < tabDialog->GetTabNum(); ++tabIndex)
	{
		cIconGridDialog* const iconDialog = (cIconGridDialog*)tabDialog->GetTabSheet(
			tabIndex);

		for(POSTYPE position = 0; position < iconDialog->GetCellNum(); ++position)
		{
			if(cIcon* const icon = iconDialog->GetIconForIdx(position))
			{
				if(icon->GetData() != updatedSkill.wSkillIdx)
				{
					continue;
				}
				else if(0 == updatedSkill.Level)
				{
					SKILLMGR->RemovePassive(
						updatedSkill.wSkillIdx);

					mIconIndexCreator.ReleaseIndex(
						icon->GetID());
					iconDialog->DeleteIcon(
						icon);
					WINDOWMGR->AddListDestroyWindow(
						icon);
					mSkillBaseTable.Remove(
						updatedSkill.wSkillIdx);
				}
				else
				{
					cSkillBase* const skillBase = (cSkillBase*)icon;

					SKILLMGR->RemovePassive(
						skillBase->GetSkillIdx());

					skillBase->SetSkillBase(
						updatedSkill);
					SKILLMGR->AddPassive(
						skillBase->GetSkillIdx());
				}

				tabIndex = tabDialog->GetTabNum();
				break;
			}
			else
			{
				cImage skillHighImage;
				SCRIPTMGR->GetImage(
					skillInfo->GetImage(),
					&skillHighImage,
					PFT_SKILLPATH );

				cImage skillLowImage;
				cSkillBase* const skillBase = new cSkillBase;
				skillBase->Init(
					0,
					0,
					DEFAULT_ICONSIZE, DEFAULT_ICONSIZE,
					&skillLowImage,
					&skillHighImage,
					mIconIndexCreator.GenerateIndex());
				skillBase->SetData(
					updatedSkill.wSkillIdx);
				skillBase->SetSkillBase(
					updatedSkill);

				WINDOWMGR->AddWindow(
					skillBase);
				mSkillBaseTable.Add(
					skillBase,
					updatedSkill.wSkillIdx);

				SKILLMGR->AddPassive(
					updatedSkill.wSkillIdx);

				iconDialog->AddIcon(
					position,
					skillBase);

				tabIndex = tabDialog->GetTabNum();
				break;
			}
		}
	}

	for(BYTE tabIndex = 0; tabIndex < tabDialog->GetTabNum(); ++tabIndex)
	{
		std::queue< cIcon* > iconQueue;
		cIconGridDialog* const iconDialog = (cIconGridDialog*)tabDialog->GetTabSheet(
			tabIndex);

		// 아이콘을 빼서 큐에 담는다
		for(POSTYPE position = 0; position < iconDialog->GetCellNum(); ++position)
		{
			if(cIcon* const icon = iconDialog->GetIconForIdx(position))
			{
				iconQueue.push(
					icon);

				iconDialog->DeleteIcon(
					position,
					0);
			}
		}

		// 아이콘을 채워넣는다
		for(POSTYPE position = 0; position < iconDialog->GetCellNum(); ++position)
		{
			if(iconQueue.empty())
			{
				break;
			}

			iconDialog->AddIcon(
				position,
				iconQueue.front());

			iconQueue.pop();
		}
	}
}

void cSkillTreeManager::AddSetSkill(DWORD skillIndex, LEVELTYPE level)
{
	SKILL_BASE skill = {0};
	skill.wSkillIdx = skillIndex;
	skill.Level = level;

	if(cSkillBase* const oldSkill = GetSkill(skillIndex))
	{
		skill = *oldSkill->GetSkillBase();
		skill.Level = skill.Level + level;
	}

	Update(
		skill);
}


void cSkillTreeManager::RemoveSetSkill(DWORD skillIndex, LEVELTYPE level)
{
	cSkillBase* const oldSkill = GetSkill(
		skillIndex);

	if(0 == oldSkill)
	{
		return;
	}

	SKILL_BASE skill = *oldSkill->GetSkillBase();
	skill.Level = skill.Level - level;

	Update(
		skill);
}

void cSkillTreeManager::ResetSetSkill()
{
	mStoredSkill.clear();
	mStoredSetSkill.clear();
}


cSkillTreeManager* cSkillTreeManager::GetInstance()
{
	static cSkillTreeManager instance;

	return &instance;
}


const SKILL_BASE* cSkillTreeManager::GetSkillBase( DWORD skillIndex )
{
	cSkillBase* skill = GetSkill( skillIndex );

	return skill ? skill->GetSkillBase() : 0;
}

void cSkillTreeManager::SetPositionHead()
{
	mSkillBaseTable.SetPositionHead();
}

SKILL_BASE* cSkillTreeManager::GetSkillBase()
{
	cSkillBase* pSkillbase = mSkillBaseTable.GetData();

	if( pSkillbase )
		return pSkillbase->GetSkillBase();
	else
		return NULL;
}

void cSkillTreeManager::AddJobSkill( DWORD skillIndex, BYTE level )
{
	SKILL_BASE SkillBase;
	SkillBase.dwDBIdx = 0;
	SkillBase.wSkillIdx = skillIndex;
	SkillBase.Level = level;
	m_JobSkillList.push_back(SkillBase);

	AddSetSkill(skillIndex, level);
	QUICKMGR->RefreshSkillItem( skillIndex );
}

void cSkillTreeManager::ClearJobSkill()
{
	std::list<SKILL_BASE>::iterator	BeginIter = m_JobSkillList.begin();

	while( BeginIter != m_JobSkillList.end() )
	{
		//SKILL_BASE* skill = m_SkillTree.GetSkillBase(BeginIter->wSkillIdx);
		cSkillInfo*	skillInfo	= SKILLMGR->GetSkillInfo( BeginIter->wSkillIdx );

		if( skillInfo )
		{
			RemoveSetSkill(
				BeginIter->wSkillIdx,
				BeginIter->Level);
			QUICKMGR->RefreshSkillItem( BeginIter->wSkillIdx );
			/*m_SkillTree.RemoveSkillBase(skill);*/
		}

		BeginIter++;
	}

	CHARCALCMGR->CalcCharStats( HERO );

	m_JobSkillList.clear();
}
