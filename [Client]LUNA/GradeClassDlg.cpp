#include "stdafx.h"
#include "GradeClassDlg.h"
#include "WindowIDEnum.h"
#include "./Interface/cStatic.h"
#include "./Interface/cPushupButton.h"
#include "./Interface/cButton.h"
#include "./Interface/cResourceManager.h"
#include "ObjectManager.h"
#include "./Tutorialmanager.h"			


CGradeClassDlg::CGradeClassDlg()
{
}


CGradeClassDlg::~CGradeClassDlg()
{
}


void CGradeClassDlg::Add( cWindow* window )
{
	WORD wWindowType = window->GetType() ;																		// 윈도우 타입을 받아온다.

	if( wWindowType == WT_PUSHUPBUTTON )																		// 윈도우 타입이 푸쉬업 버튼이면,
	{
		AddTabBtn( curIdx1++, (cPushupButton*)window ) ;														// 현재 인덱스1로 탭 버튼을 추가한다.
	}
	else if( wWindowType == WT_DIALOG )																			// 윈도우 타입이 다이얼로그이면,
	{
		AddTabSheet( curIdx2++, window ) ;																		// 현재 인덱스2로 쉬트를 추가한다.
	}
	else																										// 이도 저도 아니면,
	{
		cTabDialog::Add( window ) ;																				// 윈도우를 추가한다.
	}
}

void CGradeClassDlg::Init()
{
	for(int i=0; i<MAX_FIGHT_CLASS_BTN; i++)
	{
		m_pFighterClassBtn[i]->SetPush(TRUE);
		m_pFighterClassBtn[i]->SetDisable(TRUE);
	}
	for(i=0; i<MAX_ROGUE_CLASS_BTN; i++)
	{
		m_pRogueClassBtn[i]->SetPush(TRUE);
		m_pRogueClassBtn[i]->SetDisable(TRUE);
	}
	for(i=0; i<MAX_MAGE_CLASS_BTN; i++)
	{
		m_pMageClassBtn[i]->SetPush(TRUE);
		m_pMageClassBtn[i]->SetDisable(TRUE);
	}

	for(i=0; i<MAX_DEVIL_CLASS_BTN; i++)
	{
		m_pDevilClassBtn[i]->SetPush(TRUE);
		m_pDevilClassBtn[i]->SetDisable(TRUE);
	}

	SelectTab( eClass_FIGHTER );
}

void CGradeClassDlg::Linking()
{
	// 전직 단계 버튼 로드
	for(int i=0; i<6; i++)
	{
		m_pStaticGradeBtn[i] = (cButton*)GetWindowForID(GRADECLASS_1 + i);
		m_pStaticGradeBtn[i]->SetActive(TRUE);
		m_pStaticGradeBtn[i]->SetDisable(TRUE);
	}
	// 파이터 직업 버튼 로드
	for(int i=0; i<MAX_FIGHT_CLASS_BTN; i++)
	{
		m_pFighterClassBtn[i] = (cPushupButton*)((cTabDialog*)GetTabSheet(eClass_FIGHTER))->GetWindowForID(CLASS_1 + i);
		m_pFighterClassBtn[i]->SetActive(TRUE);
	}
	// 로그 직업 버튼 로드
	for(i=0; i<MAX_ROGUE_CLASS_BTN; i++)
	{
		m_pRogueClassBtn[i] = (cPushupButton*)((cTabDialog*)GetTabSheet(eClass_ROGUE))->GetWindowForID(CLASS_2 + i);
		m_pRogueClassBtn[i]->SetActive(TRUE);
	}
	// 메이지 직업 버튼 로드
	for(i=0; i<MAX_MAGE_CLASS_BTN; i++)
	{
		m_pMageClassBtn[i] = (cPushupButton*)((cTabDialog*)GetTabSheet(eClass_MAGE))->GetWindowForID(CLASS_3 + i);
		m_pMageClassBtn[i]->SetActive(TRUE);
	}
	// 마족 직업 버튼 로드
	for(i=0; i<MAX_DEVIL_CLASS_BTN; i++)
	{
		m_pDevilClassBtn[i] = (cPushupButton*)((cTabDialog*)GetTabSheet(eClass_DEVIL))->GetWindowForID(CLASS_4 + i);
		m_pDevilClassBtn[i]->SetActive(TRUE);
	}
}

void CGradeClassDlg::Render()
{
	cDialog::RenderWindow();
	cDialog::RenderComponent();
	cTabDialog::RenderTabComponent();
}


void CGradeClassDlg::SetActive( BOOL val )
{
	if(val)
	{
		Init();
		if( !TUTORIALMGR->CheckTutorialByIndex(GLADECLASS_TUTORIAL_NO) )
		{
			CHARACTER_TOTALINFO ChaInfo;
			OBJECTMGR->GetHero()->GetCharacterTotalInfo(&ChaInfo);

			SelectTab( ChaInfo.Job[0]-1 );	
			// 캐릭터 전직 트리 설정
			switch(ChaInfo.Job[0]-1)
			{
				case eClass_FIGHTER:	
					SetClassTree(m_pFighterClassBtn, eClass_FIGHTER);
					break;
				case eClass_ROGUE: 
					SetClassTree(m_pRogueClassBtn, eClass_ROGUE);
					break;
				case eClass_MAGE: 
					SetClassTree(m_pMageClassBtn, eClass_MAGE);
					break;
				case eClass_DEVIL:
					SetClassTree(m_pDevilClassBtn, eClass_DEVIL);
					break;
			}
		}
	}

	cTabDialog::SetActive( val );
	TUTORIALMGR->Check_OpenDialog(this->GetID(), val) ;
}

// 각 전직 레벨을 판단하여 캐릭터의 직업정보와 비교후 해당 직업버튼을 활성화한다.
void CGradeClassDlg::SetClassTree(cPushupButton** pPushBtn, ENUM_CLASS eClass)
{
    if(pPushBtn == NULL) return;

	// 각 클래스의 전직레벨의 수를 정의한다.
	const BYTE ClassLevelArray[eClass_MAXCLASS][MAX_CLASS_LEVEL] = 
		{	{1,2,3,4,6,3}, 
			{1,2,3,4,5,3}, 
			{1,2,3,4,5,3}, 
			{1,1,1,1,1,0} 
		};

	// 전직정보를 얻기위해 캐릭터 정보를 얻는다.
	CHARACTER_TOTALINFO ChaInfo;
	OBJECTMGR->GetHero()->GetCharacterTotalInfo(&ChaInfo);

    // 1차전직 - 무조건 버튼 활성화
	pPushBtn[0]->SetPush(FALSE);
	// 각 전직레벨을 캐릭터의 전직 정보와 비교하여 해당 버튼을 활성화시킨다.
	int nIdx = 1;
	int nLevel = 1;
	while(nLevel < ChaInfo.JobGrade+1)
	{
		for(int i = 0; i < ClassLevelArray[eClass][nLevel]; i++, nIdx++)
		{
			if(ChaInfo.Job[nLevel] == i+1)
			{
				pPushBtn[nIdx]->SetPush(FALSE);
				pPushBtn[nIdx]->SetDisable(TRUE);
			}
		}
		nLevel++;
	};
}
