// FamilyDialog.cpp: implementation of the CFamilyDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "../Input/Mouse.h"															// 마우스 헤더 파일을 불러온다.

#include "WindowIDEnum.h"															// 윈도우 아이디 헤더파일을 불러온다.

#include "./Interface/cWindowManager.h"												// 윈도우 매니져 헤더파일을 불러온다.
#include "../Interface/cResourceManager.h"											// 리소스 매니져 헤더파일을 불러온다.

#include "ObjectManager.h"															// 오브젝트 매니져 헤더파일을 불러온다.
//#include "FamilyManager.h"															// 패밀리 매니져 헤더파일을 불러온다.
#include "ChatManager.h"															// 채팅 매니져 헤더파일을 불러온다.

#include "../hseos/family/shfamilymanager.h"										// 패밀리 매니져 해더파일을 불러온다.

#include "./Interface/cScriptManager.h"												// 스크립트 매니져 헤더파일을 불러온다.

#include "cFont.h"	// 테스트를 위해 폰트를 불러온다.


#include "./interface/cStatic.h"													// 스태틱 인터페이스 헤더파일을 불러온다.
#include "./interface/cComboBox.h"													// 콤보박스 인터페이스 헤더파일을 불러온다.
#include "./interface/cListDialog.h"												// 리스트 다이얼로그 인터페이스 헤더파일을 불러온다.
#include "./interface/cPushupButton.h"												// 푸쉬업 버튼 인터페이스 헤더파일을 불러온다.
#include "cMsgBox.h"																// 메시지 박스 인터페이스 헤더파일을 불러온다.

#include "GameIn.h"																	// 게임인 헤더파일을 불러온다.
//#include "Family.h"																	// 패밀리 헤더파일을 불러온다.

#include "FamilyDialog.h"															// 패밀리 다이얼로그 헤더파일을 불러온다.
#include "FamilyRankDialog.h"														// 패밀리 랭킹 다이얼로그 헤더파일을 불러온다.
#include "FamilyNickNameDialog.h"													// 패밀리 호칭 변경 다이얼로그 헤더파일을 불러온다.
//#include "FamilyUnion.h"	

#include "cImageSelf.h"
#include "FamilyMarkDialog.h"
// desc_hseos_농장시스템_01
// S 농장시스템 추가 added by hseos 2007.10.11
#include "../hseos/Farm/SHFarmManageDlg.h"
// E 농장시스템 추가 added by hseos 2007.10.11

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFamilyDialog::CFamilyDialog()														// 생성자 함수.
{
	m_pFamilyName = NULL;
	m_pMasterName = NULL;
	m_pHonorPoint = NULL;
	m_pLvListDlg = NULL;
	m_pIDListDlg = NULL;
	m_pMemberID	= NULL;
	m_pMemberLevel = NULL;
	m_pMemberClass = NULL;
	m_pMemberLogin = NULL;
	m_pMemberNickName = NULL;
	m_pMemberGuildName = NULL;
	m_pChangeNickName = NULL;
	m_pInvite = NULL;
	m_pRegistMark = NULL;
	m_pLeaveBtn	= NULL;
	m_pExpelBtn	= NULL;
	m_pFarmManageBtn = NULL;
	m_pPayTaxBtn = NULL;
	m_pTransferBtn = NULL;
	m_nCurSelectIdx = 0;
	ZeroMemory(
		m_pClassImg,
		sizeof(m_pClassImg));
	ZeroMemory(
		m_pLongInImg,
		sizeof(m_pLongInImg));
	ZeroMemory(
		m_pLogOutImg,
		sizeof(m_pLogOutImg));
}

CFamilyDialog::~CFamilyDialog()														// 소멸자 함수.
{
	//ReleaseIconList() ;																// 클래스 아이콘 리스트를 해제한다.
}

void CFamilyDialog::Linking()														// 컨트롤들을 링크하는 함수.
{
	m_pFamilyName		= (cStatic*)GetWindowForID(FMY_NAME) ;						// 패밀리 이름를 링크한다.
	m_pMasterName		= (cStatic*)GetWindowForID(FMY_MASTERNAME) ;				// 마스터 이름를 링크한다.
	m_pHonorPoint		= (cStatic*)GetWindowForID(FMY_HONORPOINT) ;				// 명예 포인트를 링크한다.

	m_pLvListDlg		= (cListDialog*)GetWindowForID(FMY_MEMBER_LV_INFO_LIST) ;	// 멤버 Lv 정보 리스트 다이얼로그를 링크한다.
	m_pIDListDlg		= (cListDialog*)GetWindowForID(FMY_MEMBER_ID_INFO_LIST) ;	// 멤버 ID 정보 리스트 다이얼로그를 링크한다.

	m_pMemberID			= (cStatic*)GetWindowForID(FMY_MEMBERID) ;					// 멤버 아이디를 링크한다.
	m_pMemberLevel		= (cStatic*)GetWindowForID(FMY_MEMBERLEVEL) ;				// 멤버 레벨을 링크한다.
	m_pMemberClass		= (cStatic*)GetWindowForID(FMY_MEMBERCLASS) ;				// 멤버 클래스를 링크한다.
	m_pMemberLogin		= (cStatic*)GetWindowForID(FMY_MEMBERLOGIN) ;				// 멤버 로그인을 링크한다.
	m_pMemberNickName	= (cStatic*)GetWindowForID(FMY_MEMBERNICKNAME) ;			// 멤버 호칭을 링크한다.
	m_pMemberGuildName  = (cStatic*)GetWindowForID(FMY_MEMBERGUILDNAME) ;			// 멤버 길드명을 링크한다.

	m_pChangeNickName	= (cButton*)GetWindowForID(FMY_CHANGENICKNAME) ;			// 호칭변경 버튼을 링크한다.
	m_pInvite			= (cButton*)GetWindowForID(FMY_INVITE) ;					// 초대신청 버튼을 링크한다.
	m_pRegistMark		= (cButton*)GetWindowForID(FMY_REGISTMARK) ;				// 문장등록 버튼을 링크한다.

	// desc_hseos_패밀리01
	// S 패밀리 추가 added by hseos 2007.10.22
	m_pLeaveBtn			= (cButton*)GetWindowForID(FMY_LEAVE);
	m_pExpelBtn			= (cButton*)GetWindowForID(FMY_EXPEL);
	m_pFarmManageBtn	= (cButton*)GetWindowForID(FMY_FARM_MANAGE);
	// 091204 ONS 패밀리 다이얼로그에 패밀리장 이양 및 농장세금 납부 기능 버튼 추가
	m_pPayTaxBtn		= (cButton*)GetWindowForID(FMY_PAYTAX);
	m_pTransferBtn		= (cButton*)GetWindowForID(FMY_TRANSFER);

	// E 패밀리 추가 added by hseos 2007.10.22
}

void CFamilyDialog::UpdateAllInfo() 												// 패밀리 창에 보이는 모든 정보를 업데이트 한다.
{
	CSHFamily* pFamily = HERO->GetFamily() ;										// 패밀리 정보를 받아온다.

	if( pFamily )																	// 패밀리 정보가 유효한지 체크한다.
	{
		m_pFamilyName->SetStaticText(pFamily->Get()->szName) ;						// 패밀리명을 세팅한다.

		m_pMasterName->SetStaticText(pFamily->Get()->szMasterName) ;				// 마스터의 이름을 세팅한다.

		SetMemberList(pFamily) ;													// 멤버 리스트를 세팅한다.

		SetHonorPoint(0) ;															// 명예 포인트를 설정한다.

		CSHFamilyMember* pMember = pFamily->GetMember(0) ;							// 멤버 정보를 받아온다.

		if( pMember )																// 멤버 정보가 유효한지 체크한다.
		{
			SetMemberInfo( pMember) ;												// 멤버 정보를 세팅한다.
		}

		// 첫번째 멤버를 선택한다.
		SelectMemberOfList( 0 );

  		if (pFamily->Get()->nMasterID == gHeroID)
		{
			m_pLeaveBtn->SetDisable(TRUE);
			m_pLeaveBtn->SetImageRGB(RGB( 190, 190, 190 ));
			m_pInvite->SetDisable(FALSE);	
			m_pInvite->SetImageRGB(RGB( 255, 255, 255 ));
			m_pExpelBtn->SetDisable(FALSE);
			m_pExpelBtn->SetImageRGB(RGB( 255, 255, 255 ));
			m_pTransferBtn->SetDisable(FALSE);
			m_pTransferBtn->SetImageRGB(RGB( 255, 255, 255 ));
			m_pRegistMark->SetDisable(FALSE);
			m_pRegistMark->SetImageRGB(RGB( 255, 255, 255 ));
		}
		else
		{
			m_pExpelBtn->SetDisable(TRUE);
			m_pExpelBtn->SetImageRGB(RGB( 190, 190, 190 ));
			m_pInvite->SetDisable(TRUE);	
			m_pInvite->SetImageRGB(RGB( 190, 190, 190 ));
			m_pRegistMark->SetDisable(TRUE);
			m_pRegistMark->SetImageRGB(RGB( 190, 190, 190 ));
			m_pTransferBtn->SetDisable(TRUE);
			m_pTransferBtn->SetImageRGB(RGB( 190, 190, 190 ));
			m_pLeaveBtn->SetDisable(FALSE);
			m_pLeaveBtn->SetImageRGB(RGB( 255, 255, 255 ));


		}
	}
}

void CFamilyDialog::SetHonorPoint( DWORD dwPoint )									// 명예 포인트를 설정하는 함수.
{
	CSHFamily* pFamily = HERO->GetFamily() ;										// 패밀리 정보를 받아온다.

	if( pFamily )																	// 패밀리 정보가 유효한지 체크한다.
	{
 		char tempBuf[10+5] = {0, } ;												// 패밀리 포인트를 문자열로 바꾸어 줄 임시 버퍼 선언.

		itoa(pFamily->GetEx()->nHonorPoint, tempBuf, 10) ;							// 패밀리 포인트를 문자로 변환한다.

		m_pHonorPoint->SetStaticText(tempBuf) ;										// 패밀리 포인트를 세팅한다.
	}

	// 명예 포인트가 절감되는 상황에는 dwPoint 에 - int 값을 넣기 때문에  
	// 반드시 int 로 캐스팅 해서 양수인지 확인해야 함. by hseos
	//if( (int)dwPoint > 0 )															// 넘어온 명예 포인트가 0보다 크면,
	//{
	//	char tempBuf[128] = {0, } ;													// 명예 포인트를 문자열로 바꾸어 줄 임시 버퍼 선언.								

	//	sprintf(tempBuf, "명예 포인트가 %d 포인트 상승하였습니다.", dwPoint) ;		// 명예 포인트를 문자로 변환한다.

	//	CHATMGR->AddMsg(CTC_SYSMSG, tempBuf) ;										// 채팅창에 포인트 업 공지를 한다.
	//}
}

void CFamilyDialog::SetMemberInfo( CSHFamilyMember* pMember )						// 패밀리 창 하단부, 멤버의 상세 정보를 나타내는 함수.
{
	m_pMemberID->SetStaticText( pMember->Get()->szName ) ;							// 멤버 이름을 세팅한다.

	char tempBuf[10] = {0, } ;														// 멤버 레벨을 문자로 변환 할 임시 버퍼를 선언한다.
	itoa(pMember->Get()->nLV, tempBuf, 10) ;										// 멤버 레벨을 문자로 변환한다.
	m_pMemberLevel->SetStaticText( tempBuf ) ;										// 멤버 레벨을 세팅한다.

	int nClass		= pMember->Get()->nJobFirst ;									// 멤버의 클래스 계열 값을 받는다.
	int nRace		= pMember->Get()->nRace ;										// 멤버의 종족 번호를 받는다.
	int nJobGrade	= pMember->Get()->nJobGrade ;									// 멤버의 클래스 단계를 받는다.
	int nJobNum		= pMember->Get()->nJobCur ;										// 멤버의 클래스 번호를 받는다.

	if( nJobGrade == 1 )													// 직업 단계가 1일때는, 계열마다 한가지 직업밖에 없기 때문에
	{
		nJobNum = 1 ;														// 직업 번호를 1로 세팅한다.
	}

	int nClassIdx = ((nClass)*1000) + ((nRace+1)*100) + (nJobGrade*10) + nJobNum ;	// 멤버의 클래스 번호를 받는다.

	m_pMemberClass->SetStaticText(RESRCMGR->GetMsg(RESRCMGR->GetClassNameNum(nClassIdx))) ;	// 멤버 클래스를 세팅한다.


	ZeroMemory(tempBuf, sizeof(tempBuf)) ;											// 임시 버퍼를 초기화 한다.

	int nIsLogIn = pMember->Get()->eConState ;										// 멤버의 로그인 여부를 받는다.
	char* pChatMsg = NULL;

	switch(nIsLogIn)																// 로그인 여부를 확인한다.
	{
	//case MEMBER_CONSTATE_LOGOFF :	strcpy(tempBuf, "접속안함") ;					// 접속 안한 상태로 세팅한다.
	//case MEMBER_CONSTATE_LOGIN :	strcpy(tempBuf, "접속중") ;						// 접속 중인 상태로 세팅한다.

	case CSHGroupMember::MEMBER_CONSTATE_LOGOFF:
		{
			// 접속 안한 상태로 세팅한다.
			//strcpy(tempBuf, CHATMGR->GetChatMsg( 1443 )) ;
			pChatMsg = CHATMGR->GetChatMsg( 1443 );// 080306 NYJ --- Buffer Overflow를 방지하기 위해 포인터로 받아오는게 안전할듯.
		}
		break ;
	case CSHGroupMember::MEMBER_CONSTATE_LOGIN:
		{
			// 접속 중인 상태로 세팅한다.
			//strcpy(tempBuf, CHATMGR->GetChatMsg( 1442 )) ;
			pChatMsg = CHATMGR->GetChatMsg( 1442 );// 080306 NYJ --- Buffer Overflow를 방지하기 위해 포인터로 받아오는게 안전할듯.
		}
		break ;
	}
	
	m_pMemberLogin->SetStaticText(/*tempBuf*/pChatMsg) ;										// 멤버의 로그인 여부를 세팅한다.
	m_pMemberNickName->SetStaticText( pMember->Get()->szNickname ) ;				// 멤버의 호칭을 세팅한다.
	m_pMemberGuildName->SetStaticText( pMember->GetEx()->szGuild ) ;				// 멤버의 길드명을 세팅한다.
}

void CFamilyDialog::SetMemberList(CSHFamily* pFamily)
{
	DWORD dwColor = 0xffffffff;
	int nLevelBlank = 0;
	int nClass		= 0;
	int nRace		= 0;
	int nJobGrade	= 0;
	int nJobNum		= 0;
	int nLogIn		= 0;
	char levelBuf[8] = {0};
	CSHFamilyMember* pMember = 0;

	m_pLvListDlg->RemoveAll();
	m_pIDListDlg->RemoveAll();

	int nMemberCount = pFamily->Get()->nMemberNum ;									// 멤버 수를 구한다.

	for( int bigCount = 0 ; bigCount < nMemberCount ; ++bigCount )					// 멤버 수만큼 포문을 돌린다.
	{
		pMember = pFamily->GetMember(bigCount) ;									// 카운트에 해당하는 멤버 정보를 받는다.

		if( pMember && pMember->Get()->nID != 0 )									// 멤버 정보가 유효한지 체크한다.
		{

			nClass		= pMember->Get()->nJobFirst ;								// 멤버의 클래스 계열 값을 받는다.
			nRace		= pMember->Get()->nRace ;									// 멤버의 종족 번호를 받는다.
			nJobGrade	= pMember->Get()->nJobGrade ;								// 멤버의 클래스 단계를 받는다.
			nJobNum		= pMember->Get()->nJobCur ;

			if( nJobGrade == 1 )													// 직업 단계가 1일때는, 계열마다 한가지 직업밖에 없기 때문에
			{
				nJobNum = 1 ;														// 직업 번호를 1로 세팅한다.
			}

			int nClassIdx = ((nClass)*1000) + ((nRace+1)*100) + (nJobGrade*10) + nJobNum ;	// 멤버의 클래스 번호를 받는다.

			//int nIconNum = GetClassIconNum(nClassIdx) ;								// 클래스에 해당하는 이미지 번호를 받는다.
			//if( nIconNum >= 0 && m_ClassIconList.GetCount() >= nIconNum )			// 아이콘 이미지 번호의 영역을 확인한다.
			// 080320 LYW --- FamilyDialog : 클래스 아이콘 기능을 리소스 매니져로 옮김
 			int nIconNum = RESRCMGR->GetClassIconNum(nClassIdx) ;
 			if( nIconNum >= 0 && RESRCMGR->GetClassIconCount() >= nIconNum )
			{
				SCRIPTMGR->GetImage( nIconNum, &m_pClassImg[bigCount], PFT_CLASS_ICON_PATH ) ;	// 클래스 아이콘 이미지를 할당받는다.
			}

			if(pMember->Get()->eConState==CSHGroupMember::MEMBER_CONSTATE_LOGOFF)	// 멤버가 로그인 중이라면,
			{
				dwColor = RGB_HALF(200, 200, 200) ;									// 회색으로 색상을 설정한다.
			}
			else																	// 멤버가 로그아웃 상태라면,
			{
				dwColor = RGB_HALF(10, 10, 10) ;									// 검은색으로 색상을 설정한다.				
			}

			int nLevel = pMember->Get()->nLV ;

			itoa(nLevel, levelBuf, 10) ;											// 멤버 레벨을 레벨 버퍼에 문자로 변환하여 담는다.

			//itoa(pMember->Get()->nLV, levelBuf, 12) ;								// 멤버 레벨을 레벨 버퍼에 문자로 변환하여 담는다.

			nLevelBlank = strlen(levelBuf) ;										// 레벨 문자의 길이를 구한다.

			m_pLvListDlg->AddItem( levelBuf, dwColor );
			
			m_pIDListDlg->AddItem( pMember->Get()->szName, dwColor ) ;				// 멤버 리스트에 정보를 세팅한다.

			nLogIn = pMember->Get()->eConState ;									// 멤버의 로그인 여부를 받는다.

			if( nLogIn )															// 멤버가 로그인 중이라면,
			{
				SCRIPTMGR->GetImage( 102, &m_pLongInImg[bigCount], PFT_HARDPATH ) ;	// 로그인 이미지로 세팅한다.
			}
			else																	// 멤버가 로그아웃 중이라면,
			{
				SCRIPTMGR->GetImage( 103, &m_pLongInImg[bigCount], PFT_HARDPATH ) ;	// 로그아웃 이미지로 세팅한다.
			}
		}
	}
}

void CFamilyDialog::Render()														// 클래스 이미지와 로그인 이미지를 출력할 렌더 함수.
{
	if( !IsActive() ) return ;														// 패밀리 창이 활성화 중이 아니라면, 리턴한다.

	cDialog::RenderWindow();														// 윈도우를 렌더한다.

	cDialog::RenderComponent();														// 윈도우 컴퍼넌트들을 렌더한다.


	CSHFamily* pFamily = HERO->GetFamily() ;										// 패밀리 정보를 받아온다.

	if( pFamily && pFamily->Get()->nMasterID != 0 )									// 패밀리 정보가 유효하고, 마스터 아이디가 0이 아니면,
	{
		VECTOR2 scale = {1.0f, 1.0f};
		VECTOR2 vPos = {0};
		DWORD dwColor = 0xffffffff;

		for( UINT count = 0 ; count < pFamily->Get()->nMemberNum ; ++count )		// 멤버 수만큼 포문을 돌린다.
		{
			vPos.y = (float)(GetAbsY() + 156) + (16*count) ;						// 이미지 출력위치 Y를 세팅한다.

			vPos.x = (float)(GetAbsX() + 30) ;										// 이미지 출력위치 X를 세팅한다.

			if( !m_pClassImg[count].IsNull() )										// 멤버 클래스 이미지가 널이 아니라면,
			{
				m_pClassImg[count].RenderSprite(&scale, NULL, 0, &vPos, dwColor) ;	// 멤버 클래스 이미지를 출력한다.
			}

			vPos.x += 190  ;														// 로그인 위치를 출력할 세팅한다.

			if( !m_pLongInImg[count].IsNull() )										// 멤버 로그인 여부 이미지가 널이 아니라면,
			{
				m_pLongInImg[count].RenderSprite(&scale, NULL, 0, &vPos, dwColor) ;	// 멤버 로그인 여부 이미지를 출력한다.
			}
		}
	}

	cImageSelf* pcsFamilyEmblem = HERO->GetFamilyEmblem();
	if (pcsFamilyEmblem)
	{
		VECTOR2 scale, pos2;
		scale.x = 1; scale.y = 1;
		pos2.x = GetAbsX()+210;
		pos2.y = GetAbsY()+66;
		pcsFamilyEmblem->RenderSprite(&scale, NULL, 0 , &pos2, 0xffffffff);	
	}
}

DWORD CFamilyDialog::ActionEvent( CMouse* mouseInfo )								// 패밀리 창의 이벤트를 처리하는 함수.
{
	DWORD we = WE_NULL;																// 리턴할 윈도우 이벤트 변수를 선언한다.

	if( !m_bActive )		return we;												// 윈도우가 활성화 상태가 아니면, 이벤트를 리턴한다.

	we = cDialog::ActionEvent(mouseInfo);											// 기본 다이얼로그의 이벤트를 받아온다.

	return we;																		// 윈도우 이벤트를 리턴 처리 한다.
}

void CFamilyDialog::OnActionEvent( LONG lId, void* p, DWORD we )																		// 패밀리 창의 버튼의 이벤트 처리를 하는 함수.
{
	CSHFamily* pFamily = HERO->GetFamily() ;																							// 패밀리 정보를 받아온다.

	if( we & (WE_BTNCLICK | WE_ROWCLICK) )
	{
		switch( lId )																													// 넘어온 버튼의 아이디를 확인한다.
		{
		case FMY_CHANGENICKNAME :																										// 호칭변경을 클릭하였다면, (호칭 변경은 모든 멤버가 사용할 수 있다.)
			{
				if( pFamily )																											// 패밀리 정보가 유효한지 체크한다.
				{
					CSHFamilyMember* pMember = pFamily->GetMember(m_nCurSelectIdx) ;													// 선택한 대상 정보를 받아온다.

					if( pMember )																										// 멤버 정보가 유효한지 체크한다.
					{
						CFamilyNickNameDialog* pDlg = GAMEIN->GetFamilyNickNameDlg() ;													// 호칭 변경창 정보를 받는다.

						if( pDlg )																										// 호칭 변경창 정보가 유효한지 체크한다.
						{
							pDlg->SetActive(TRUE) ;																						// 호칭 변경창을 활성화 시킨다.
						}
					}
				}
			}
			break ;

		case FMY_INVITE :																												// 초대신청을 클릭하였다면,
			{
				if( pFamily && pFamily->Get()->nMasterID == gHeroID )																// 패밀리 정보가 유효하고, HERO가 마스터 이면,
				{
					CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetSelectedObject() ;														// 선택한 플레이어 정보를 받는다.

					if( pPlayer )																										// 플레이어 정보가 유효한지 체크한다.
					{
						if( pPlayer->GetObjectKind() == eObjectKind_Player )															// 플레이어 타입이 유저인지 체크한다.
						{
							if( pPlayer->GetID() != gHeroID )																		// 본인을 선택한 상태인지 체크한다.
							{
								CSHFamilyMember* pMember = NULL ;																		// 패밀리 멤버 정보를 담을 포인터를 선언한다.			

								for( int count = 0 ; count < MAX_MEMBER_COUNT ; ++count )												// 멤버의 아이디와 비교하기 위해 for문을 돌린다.
								{
									pMember = pFamily->GetMember(count) ;																// 멤버의 정보를 받는다.

									if( pMember )																						// 멤버 정보가 유효한지 체크한다.
									{
										if( pMember->Get()->nID != pPlayer->GetID() )													// 멤버의 아이디와 선택한 대상의 아이디가 같지 않으면,
										{
											g_csFamilyManager.CLI_RequestInvite(pPlayer) ;												// 초대 신청을 한다.
											return ;																					// 리턴 처리를 한다.
										}
										else																							// 선택한 대상의 아이디와 멤버의 아이디가 같으면,
										{
											WINDOWMGR->MsgBox( MBI_FAMILY_ERROR_NOTARGET, MBT_OK, CHATMGR->GetChatMsg( 1316) );
										}
									}

									pMember = NULL ;																					// 멤버 정보를 담을 포인터를 NULL 처리를 한다.
								}
							}
							else
							{
								WINDOWMGR->MsgBox( MBI_FAMILY_ERROR_NOTARGET, MBT_OK, CHATMGR->GetChatMsg( 1317 ) );
							}							
						}
						else
						{
							WINDOWMGR->MsgBox( MBI_FAMILY_ERROR_NOTARGET, MBT_OK, CHATMGR->GetChatMsg( 1318 ) );
						}
					}
					else																												// 플레이어 정보가 유효하지 않다면,
					{
						WINDOWMGR->MsgBox( MBI_FAMILY_ERROR_NOTARGET, MBT_OK, CHATMGR->GetChatMsg( 1319 ) );
					}
				}
				else
				{
					WINDOWMGR->MsgBox( MBI_FAMILY_ABLE_MASTER, MBT_OK, CHATMGR->GetChatMsg( 732 ) );									// 예외 처리를 한다.
				}
			}
			break ;

		case FMY_REGISTMARK :																											// 문장 등록을 클릭하였다면,
			{
 				GAMEIN->GetFamilyMarkDlg()->SetActive(TRUE);			
//				WINDOWMGR->MsgBox( MBI_FAMILY_ERROR_NOTARGET, MBT_OK, CHATMGR->GetChatMsg(528) );										// 예외 처리를 한다.
			}
			break ;

		// desc_hseos_패밀리01
		// S 패밀리 추가 added by hseos 2007.10.22
		case FMY_LEAVE:
			{
		 		WINDOWMGR->MsgBox( MBI_FAMILY_LEAVE, MBT_YESNO, CHATMGR->GetChatMsg( 1142 ), HERO->GetFamily()->Get()->szName, int((CSHFamilyManager::LEAVE_PENALTY_EXP_DOWN_RATE[CSHFamilyManager::FLK_LEAVE]+0.001)*100), CSHFamilyManager::LEAVE_PENALTY_REJOIN_TIME[CSHFamilyManager::FLK_LEAVE] );
			}
			break;
		case FMY_EXPEL:
			{
				if (gHeroID == HERO->GetFamily()->GetMember(GAMEIN->GetFamilyDlg()->GetSelectedMemberIdx())->Get()->nID)
				{
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1163 ));
				}
				else
				{
	 		   		WINDOWMGR->MsgBox( MBI_FAMILY_EXPEL, MBT_YESNO, CHATMGR->GetChatMsg( 1143 ), HERO->GetFamily()->GetMember(GAMEIN->GetFamilyDlg()->GetSelectedMemberIdx())->Get()->szName, int((CSHFamilyManager::LEAVE_PENALTY_EXP_DOWN_RATE[CSHFamilyManager::FLK_EXPEL]+0.001)*100), CSHFamilyManager::LEAVE_PENALTY_REJOIN_TIME[CSHFamilyManager::FLK_EXPEL] );
				}
			}
			break;
		case FMY_FARM_MANAGE:
			{
				GAMEIN->GetFarmManageDlg()->SetActive( TRUE );
			}
			break;
		// 091125 ONS 패밀리 다이얼로그에 패밀리장 이양 기능 추가
		case FMY_TRANSFER:
			{
				if (gHeroID == HERO->GetFamily()->GetMember(GAMEIN->GetFamilyDlg()->GetSelectedMemberIdx())->Get()->nID)
				{
					// 마스터 자신에게는 이양할 수 없다.
					CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg( 1996 ));
				}
				else
				{
	 		   		WINDOWMGR->MsgBox( MBI_FAMILY_TRANSFER, MBT_YESNO, CHATMGR->GetChatMsg( 1990 ), HERO->GetFamily()->GetMember(GAMEIN->GetFamilyDlg()->GetSelectedMemberIdx())->Get()->szName);
				}
			}
			break;
		// 091204 ONS 패밀리 다이얼로그에 농장세금 납부 기능 버튼 추가
		case FMY_PAYTAX:
			{
				g_csFamilyManager.CLI_RequestPayTax(2);
			}
			break;
		// E 패밀리 추가 added by hseos 2007.10.22

		case FMY_MEMBER_LV_INFO_LIST:
			{
				int nClickedIndex = m_pLvListDlg->GetClickedRowIdx();
				if( nClickedIndex < 0 )
					break;

				CSHFamily* pFamily = HERO->GetFamily();
				if( !pFamily )
					break;

				CSHFamilyMember* pMember = pFamily->GetMember( nClickedIndex );
				if( !pMember )
					break;
				
				SetMemberList( pFamily );
				SetMemberInfo( pMember);
				SelectMemberOfList( nClickedIndex );
			}
			break;
		}
	}
}

void CFamilyDialog::SelectMemberOfList( int nIndex )
{
	// 색 초기화
	DWORD dwBaseColor = RGB_HALF(100, 100, 100);

	int nListCnt = m_pLvListDlg->GetListItem()->GetCount();
	for( int idx = 0 ; idx < nListCnt ; ++idx )
	{
		PTRLISTPOS pos = m_pLvListDlg->GetListItem()->FindIndex( idx );
		if( !pos )			continue;

		ITEM* pItem = (ITEM*)m_pLvListDlg->GetListItem()->GetAt( pos );
		if( !pItem )			continue;

		pItem->rgb = dwBaseColor;
	}

	nListCnt = m_pIDListDlg->GetListItem()->GetCount();
	for( int idx = 0 ; idx < nListCnt ; ++idx )
	{
		PTRLISTPOS pos = m_pIDListDlg->GetListItem()->FindIndex( idx );
		if( !pos )			continue;

		ITEM* pItem = (ITEM*)m_pIDListDlg->GetListItem()->GetAt( pos );
		if( !pItem )			continue;

		pItem->rgb = dwBaseColor;
	}

	ITEM* pItem = m_pLvListDlg->GetItem( nIndex );
	if( !pItem )		return;
	pItem->rgb = RGB_HALF(255, 255, 0); 									// 아이템의 색상을 다시 설정한다.

	pItem = m_pIDListDlg->GetItem( nIndex );
	if( !pItem )		return;
	pItem->rgb = RGB_HALF(255, 255, 0) ; 									// 아이템의 색상을 다시 설정한다.

	m_nCurSelectIdx = nIndex;

	if( m_pIDListDlg->GetClickedRowIdx() != nIndex )
		m_pIDListDlg->SetCurSelectedRowIdxWithoutRowMove( nIndex );

	if( m_pLvListDlg->GetClickedRowIdx() != nIndex )
		m_pLvListDlg->SetCurSelectedRowIdxWithoutRowMove( nIndex );
}