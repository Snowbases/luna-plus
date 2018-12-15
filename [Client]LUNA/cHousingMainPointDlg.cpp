#include "stdafx.h"
#include "cHousingMainPointDlg.h"
#include "WindowIDEnum.h"
#include "cImeEx.h"
#include "cWindowManager.h"
#include "cMsgBox.h"
#include "cHousingMgr.h"
#include "cHousingDecoPointDlg.h"
#include "GameIn.h"
#include "ChatManager.h"

//090409 pdy 하우징 하우스 검색 UI추가
cHousingMainPointDlg::cHousingMainPointDlg()
{
	m_type = WT_HOUSING_MAINPOINTDLG;
}

cHousingMainPointDlg::~cHousingMainPointDlg()
{

}

void cHousingMainPointDlg::Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID)
{
	cDialog::Init(x,y,wid,hei,basicImage,ID);
}

void cHousingMainPointDlg::Linking()
{
	 m_pHouseName_Static =  ( cStatic* )GetWindowForID( HOUSING_MAINPOINT_HOUSENAME_STATIC );
	 m_pVisitCount_Static =  ( cStatic* )GetWindowForID( HOUSING_MAINPOINT_VISIT_STATIC );
	 m_pDecoPoint_Static =  ( cStatic* )GetWindowForID( HOUSING_MAINPOINT_DECOPOINT_STATIC );
	 m_BtDecoPoint		=	( cButton* )GetWindowForID( HOUSING_DECOPOINT_BT );
}

void cHousingMainPointDlg::SetActive(BOOL val)
{
	if( m_bDisable ) return;

	cDialog::SetActive(val);
	cDialog::SetActiveRecursive(val);

	if( ! HOUSINGMGR->IsHouseOwner() )
	{
		//하우스 주인이 아닌경우 비공개되는 꾸미기 포인트 출력 UI를 닫자 
		m_pDecoPoint_Static->SetActive(FALSE);
	}
}

void cHousingMainPointDlg::SetHouseName(char* szHouseName)
{
	m_pHouseName_Static->SetStaticText(szHouseName);
}

void cHousingMainPointDlg::SetVisitCount(DWORD dwVisitCount)
{
	char Buf[32]={0};

	sprintf(Buf,"%d",dwVisitCount);
	m_pVisitCount_Static->SetStaticText(Buf);
}

void cHousingMainPointDlg::SetDecoPoint(DWORD dwDecoPoint)
{
	char Buf[32]={0};

	sprintf(Buf,"%d",dwDecoPoint);
	m_pDecoPoint_Static->SetStaticText(Buf);
}

//091012 pdy 하우징 꾸미기 포인트 버튼 기획변경 
void cHousingMainPointDlg::OnActionEvent(LONG lId, void* p, DWORD we)
{
	if(! HOUSINGMGR->IsHouseInfoLoadOk())
	{
		//하우스 로딩이 되어있지 않으면 리턴 
		return;
	}

	switch(lId)
	{
		case HOUSING_DECOPOINT_BT :
			{
				if(  HOUSINGMGR->IsHouseOwner() )
				{
					//꾸미기보너스 UI를 열자
					cHousingDecoPointDlg* pDlg = GAMEIN->GetHousingDecoPointDlg() ;

					if( pDlg )
					{
						pDlg->SetActive( TRUE ) ; 
					}
				}
				else
				{
					CHATMGR->AddMsg(CTC_SYSMSG,  CHATMGR->GetChatMsg(1887) ); //1887	"집주인이 아닐시 사용하실 수 없습니다"
				}
			}			
			break;
	}
}