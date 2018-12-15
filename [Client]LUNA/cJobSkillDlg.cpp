#include "stdafx.h"
#include ".\cjobskilldlg.h"
#include ".\interface\cIconGridDialog.h"
#include "CheatMsgParser.h"
#include "cSkillBase.h"
#include "../[cc]skill/client/manager/skillmanager.h"

// 070220 LYW --- cJobSkillDlg : Include header file of conduct part.
#include "ConductManager.h"
#include "ConductBase.h"
#include "ConductInfo.h"

#include "GameIn.h"
#include "cSkillTreeDlg.h"

cJobSkillDlg::cJobSkillDlg(void)
{
	m_type = WT_JOBSKILL_DLG;
}

cJobSkillDlg::~cJobSkillDlg(void)
{
}

void cJobSkillDlg::Linking()
{
}

void cJobSkillDlg::Render()
{
	cDialog::RenderWindow();
	cDialog::RenderComponent();
	cTabDialog::RenderTabComponent();
}

void cJobSkillDlg::Add( cWindow * window )
{
	if( window->GetType() == WT_PUSHUPBUTTON )
	{
		AddTabBtn( curIdx1++, (cPushupButton * )window );
	}
	else if(window->GetType() == WT_ICONGRIDDIALOG )
	{
		AddTabSheet( curIdx2++, window );
	}
	else 
		cDialog::Add(window);
}

void cJobSkillDlg::OnActionEvnet(LONG lId, void * p, DWORD we)
{
	if( we == WE_RBTNDBLCLICK || we == WE_RBTNCLICK ) return ;

	/// 현재 선택된 탭번호
	BYTE curtab = GetCurTabNum();
	/// 그리드 다이얼로그를 가져온다
	cIconGridDialog* pGrid = NULL ;
	pGrid = ( cIconGridDialog* )GetTabSheet( curtab );
	
	if( pGrid )
	{
		/// 선택된 위치
		WORD pos = ( WORD )( pGrid->GetCurSelCellPos() );
		/// 해당되는 스킬을 가져온다
		// 070220 LYW --- cJobSkillDlg : Modified OnActionEvent.
		//cSkillBase* pSkill = ( cSkillBase* )( pGrid->GetIconForIdx( pos ) );
		cSkillBase*		pSkill = NULL ;
		cConductBase*	pConduct = NULL ;
		int sheetNum = GAMEIN->GetSkillTreeDlg()->GetCurTabNum() ;
		if( sheetNum == eSkillDlgTab_Active || sheetNum == eSkillDlgTab_Passive )
		{
			pSkill = ( cSkillBase* )( pGrid->GetIconForIdx( pos ) );

			if( !pSkill ) return ;
		}
		else if( sheetNum == eSkillDlgTab_Action || sheetNum == eSkillDlgTab_Motion || eSkillDlgTab_Emotiocon )
		{
			pConduct = ( cConductBase* )(pGrid->GetIconForIdx( pos ) ) ;

			if( !pConduct ) return ;
		}

		switch( we )
		{
		case WE_LBTNDBLCLICK:
			{
				// 070220 LYW --- cJobSkillDlg : Modified OnActionEvent.
				/*
				SKILL_BASE* pSkillBase = pSkill->GetSkillBase();

				if( pSkillBase->Level > 0 )
				{
					SKILLMGR->OnSkillCommand( pSkillBase->wSkillIdx );
				}
				*/

				if( sheetNum == eSkillDlgTab_Active || sheetNum == eSkillDlgTab_Passive )
				{
					SKILL_BASE* pSkillBase = pSkill->GetSkillBase();

					if( pSkillBase->Level > 0 )
					{
						SKILLMGR->OnSkillCommand( pSkillBase->wSkillIdx - 1 + pSkillBase->Level );
					}
				}
				else if( sheetNum == eSkillDlgTab_Action || sheetNum == eSkillDlgTab_Motion || eSkillDlgTab_Emotiocon )
				{
					CONDUCTMGR->OnConductCommand( pConduct->m_pConductInfo->GetConductIdx() ) ;
				}

			}
			break;
		}
	}
}
