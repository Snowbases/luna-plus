#include "stdafx.h"
#include "cHouseSearchDlg.h"
#include "WindowIDEnum.h"
#include "cImeEx.h"
#include "cWindowManager.h"
#include "cMsgBox.h"
#include "../ChatManager.h"
#include "InventoryExDialog.h"
#include "GameIn.h"
#include "Item.h"
#include "ObjectStateManager.h"
#include "ObjectManager.h"
#include "FilteringTable.h"

//090409 pdy 하우징 하우스 검색 UI추가
cHouseSearchDlg::cHouseSearchDlg()
{
	m_type = WT_HOUSE_SEARCH_DLG;
}

cHouseSearchDlg::~cHouseSearchDlg()
{

}

void cHouseSearchDlg::Init(LONG x, LONG y, WORD wid, WORD hei, cImage * basicImage, LONG ID)
{
	cDialog::Init(x,y,wid,hei,basicImage,ID);
	m_type = WT_MINIFRIENDDLG;

	m_VisitList.clear();
}

void cHouseSearchDlg::Linking()
{
	m_pSearchName = (cStatic*)GetWindowForID(HOUSE_SEARCH_NAME);
	m_pSearchNameEdit = (cEditBox*)GetWindowForID(HOUSE_SEARCH_NAMEEDIT);
	m_pSearchNameEdit->SetValidCheck(VCM_CHARNAME/*VCM_DEFAULT*/);
	m_pSearchNameEdit->SetEditText("");
	m_pSearchBtn = (cButton*)GetWindowForID(HOUSE_SEARCH_SEARCHBTN);
	m_pSearchRandomBtn = (cButton*)GetWindowForID(HOUSE_SEARCH_RANDOMBTN);
	m_pVisitList = (cComboBox*)GetWindowForID(HOUSE_SEARCH_VISITLIST);

	LoadVisitList();
}

void cHouseSearchDlg::SetActive(BOOL val)
{
	if( m_bDisable ) return;

	cDialog::SetActive(val);
	cDialog::SetActiveRecursive(val);

	m_pSearchNameEdit->SetFocusEdit(val);

	if(val)
		m_pSearchNameEdit->SetEditText("");
	else
	{	
		if(eHouseVisitByItem == m_VisitInfo.m_dwVisitKind )
		{
			CInventoryExDialog* pDlg = GAMEIN->GetInventoryDialog();
			if(pDlg)
			{
				pDlg->SetDisable( FALSE );

				CItem* pItem = pDlg->GetItemForPos(m_VisitInfo.m_dwData2);

				if( pItem )
					pItem->SetLock(FALSE);
			}
		}			

		m_VisitInfo.Clear();
	}
}

void cHouseSearchDlg::SetName(char* Name)
{
	m_pSearchNameEdit->SetEditText(Name);
}

char* cHouseSearchDlg::GetName()
{
	return m_pSearchNameEdit->GetEditText();
}

void cHouseSearchDlg::OnActionEvent(LONG lId, void* p, DWORD we)
{
	switch(lId)
	{
		case HOUSE_SEARCH_NAMEEDIT:
		{
			if(we == WE_RETURN)
			{
				char* name = "";
				name = m_pSearchNameEdit->GetEditText();
				int len = strlen(name);

				//090605 하우스 검색시 유효하지않는 캐릭터이름 검사 추가 
				BOOL bCheck = FALSE;

				if( len == 0 )
				{
					return;
				}
				else if( len < 4 )
				{
					bCheck = TRUE;
				}
				else if( len > MAX_NAME_LENGTH )
				{
					bCheck = TRUE;
				}
				
				if( FILTERTABLE->IsInvalidCharInclude((unsigned char*)name) )
				{
					bCheck = TRUE;	
				}

				if( !FILTERTABLE->IsUsableName(name) )
				{
					bCheck = TRUE;	
				}

				if( bCheck == TRUE )
				{
					//090605 pdy 하우징 시스템메세지 하우스 검색시 사용할수없는 이름 [하우스 검색방문]  
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 551 ) );		//551	"사용할 수 없는 이름입니다."
					return;
				}

				char MsgBuf[128] = {0,};
				sprintf(MsgBuf,CHATMGR->GetChatMsg(1890), name); //1890	"%s의 집으로 이동 하시겠습니까?"

				//090527 pdy 하우징 팝업창 [이름검색하우스방문]
				WINDOWMGR->MsgBox( MBI_HOUSESEACH_AREYOUSURE, MBT_YESNO, MsgBuf ) ;	//1890	"%s의 집으로 이동 하시겠습니까?"
				SetDisableState(TRUE) ;
			}
		}
		break;

		case HOUSE_SEARCH_SEARCHBTN :
			{
				char* name = "";
				name = m_pSearchNameEdit->GetEditText();
				int len = strlen(name);

				//090605 하우스 검색시 유효하지않는 캐릭터이름 검사 추가 
				BOOL bCheck = FALSE;

				if( len == 0 )
				{
					return;
				}
				else if( len < 4 )
				{
					bCheck = TRUE;
				}
				else if( len > MAX_NAME_LENGTH )
				{
					bCheck = TRUE;
				}
				
				if( FILTERTABLE->IsInvalidCharInclude((unsigned char*)name) )
				{
					bCheck = TRUE;	
				}

				if( !FILTERTABLE->IsUsableName(name) )
				{
					bCheck = TRUE;	
				}

				if( bCheck == TRUE )
				{
					//090605 pdy 하우징 시스템메세지 하우스 검색시 사용할수없는 이름 [하우스 검색방문]  
					CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 551 ) );		//551	"사용할 수 없는 이름입니다."
					return;
				}

				char MsgBuf[128] = {0,};
				sprintf(MsgBuf,CHATMGR->GetChatMsg(1890), name); //1890	"%s의 집으로 이동 하시겠습니까?"

				//090527 pdy 하우징 팝업창 [이름검색하우스방문]
				WINDOWMGR->MsgBox( MBI_HOUSESEACH_AREYOUSURE, MBT_YESNO, MsgBuf ) ;	//1890	"%s의 집으로 이동 하시겠습니까?"
				SetDisableState(TRUE) ;
			}
			break;
		//090618 pdy 하우징 기능추가 랜덤입장 
		case HOUSE_SEARCH_RANDOMBTN :
			{
				//090618 pdy 하우징 팝업창 [하우스랜점입장]
				WINDOWMGR->MsgBox( MBI_VISIT_RANDOM_AREYOUSURE, MBT_YESNO, CHATMGR->GetChatMsg(1925) ) ;	//1890	"%s의 집으로 이동 하시겠습니까?"
				SetDisableState(TRUE) ;
			}
			break;

		// 091214 pdy 콤보리스트와 Edit박스의 문자가 겹치는 현상 수정
		case HOUSE_SEARCH_VISITLIST:
			{
				if( we != WE_COMBOBOXSELECT )
					break;

				char name[128] = {0,};
				strcpy(name, m_pVisitList->GetComboText());
				m_pSearchNameEdit->SetEditText(name);
				m_pVisitList->DeleteComboText();
			}
			break;
	}
}


void cHouseSearchDlg::SetDisableState(BOOL val)
{
	if( val )
	{
		OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Deal) ;
	}
	else
	{
		OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal) ;
	}

	SetDisable(val) ;
}

void cHouseSearchDlg::LoadVisitList()
{
	FILE* fp;
	fp = fopen("./System/HouseVisitList.txt", "rt");
	if(! fp)
		return;

	char name[MAX_NAME_LENGTH] = {0,};

	while(0 == feof(fp))
	{
		fscanf(fp, "%s\n", name);
		stVisitName visit;
		strcpy(visit.name, name);
		m_VisitList.push_back(visit);
	}
	m_VisitList.reverse();
	fclose(fp);

	if(m_VisitList.size() > 10)
	{
		// 비정상 목록이 초과했다.
		// 파일지우고 종료
		m_VisitList.clear();	
		_unlink("./System/HouseVisitList.txt");
		return;
	};

	std::list<stVisitName>::reverse_iterator rIter;
	int i;
	for(rIter=m_VisitList.rbegin(), i=0; rIter!=m_VisitList.rend(); rIter++, i++)
	{
		char* name = rIter->name;
		ITEM* item = m_pVisitList->GetItem(i);
		strcpy(item->string, name);
	}
}

void cHouseSearchDlg::AddVisitList()
{
	// 091124 NYJ 방문목록 추가
	int num = m_VisitList.size();
	if(num > 9)
		m_VisitList.pop_front();

	stVisitName visit;
	strcpy(visit.name, GetName());
	
	// 중복검사
	std::list<stVisitName>::iterator iter;
	for(iter=m_VisitList.begin(); iter!=m_VisitList.end(); iter++)
	{
		if(0 == strcmp(visit.name, iter->name))
		{
			m_VisitList.erase(iter);
			break;
		}
	}

	// 뒤에 추가
	m_VisitList.push_back(visit);

	FILE* fp;
	fp = fopen("./System/HouseVisitList.txt", "wt");
	if(! fp)
		return;

	std::list<stVisitName>::reverse_iterator rIter;
	int i;
	for(rIter=m_VisitList.rbegin(), i=0; rIter!=m_VisitList.rend(); rIter++, i++)
	{
		ITEM* item = m_pVisitList->GetItem(i);
		strcpy(item->string, rIter->name);
		fprintf(fp, "%s\n", rIter->name);
	}
	fflush(fp);
	fclose(fp);
}