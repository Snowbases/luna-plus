//=================================================================================================
//	FILE		: CCharMakeNewDlg.cpp
//	PURPOSE		: For new dialog for create character.
//	PROGRAMMER	: Yongs Lee
//	DATE		: December 14, 2006
//	ADDITION	:
//	ATTENTION	:
//=================================================================================================


//=================================================================================================
//	INCLUDE PART
//=================================================================================================
#include "stdafx.h"
#include ".\charmakenewdlg.h"
#include "WindowIDEnum.h"

#include "./interface/cPushupButton.h"
#include "./interface/cStatic.h"
#include "./interface/cButton.h"
#include "./interface/cTextArea.h"
#include "./interface/cEditBox.h"
#include "./interface/cResourceManager.h"
#include "./interface/cWindowManager.h"

#include "Player.h"
#include "CharMake.h"

#include "ObjectManager.h"
#include "CharMakeManager.h"
#include "AppearanceManager.h"

// 090504 ONS 신규종족 생성시 50레벨 이상의 캐릭터 존재여부 파악을 위해 추가
#include "CharSelect.h"

//=================================================================================================
// NAME			: CCharMakeNewDlg()
// PURPOSE		: The Function Constructor.
// ATTENTION	:
//=================================================================================================
CCharMakeNewDlg::CCharMakeNewDlg(void)
{
	int count = 0 ;

	for( count = 0 ; count < MAX_PUSHUPBTN_COUNT ; ++count )
	{
		m_pPushupBtn[ count ] = NULL ;
	}

	for( count = 0 ; count < MAX_STATIC_COUNT ; ++count )
	{
		m_pStatic[ count ] = NULL ;
	}

	for( count = 0 ; count < MAX_CMBTN_COUNT ; ++count )
	{
		m_pButton[ count ] = NULL ;
	}

	for( count = 0 ; count < MAX_TEXAREA_COUNT ; ++count )
	{
		m_pTextArea[ count ] = NULL ;
	}

	m_pBtnMake		= NULL ;
	m_pBtnCancel	= NULL ;

	m_byCurRacial	= CHARMAKEMGR->GetCharacterMakeInfo()->RaceType;
	m_byCurClass	= e_FIGHTER ;
	// desc_hseos_성별선택01
	// S 성별선택 추가 added by hseos 2007.06.16
	// ..DB에서 읽어온 유저의 성별로 설정
	// m_byCurGender	= e_MAN ;
	m_byCurGender = CHARMAKEMGR->GetCharacterMakeInfo()->SexType;
	// E 성별선택 추가 added by hseos 2007.06.16

	m_byCurFace		= 0x00 ;
	m_byCurHair		= 0x00 ;

	// 061218 LYW --- Add Alpha static window.
	m_pAlphaWindow  = NULL ;

	// 061227 LYW --- Add stitc for racial and class.
	m_pST_Racial	= NULL ;
	m_pST_Class		= NULL ;

	// 090603 ONS 신규종족 생성가능 식별변수.
	m_bMakeDevil	= NULL;
}


//=================================================================================================
// NAME			: ~v()
// PURPOSE		: The Function Destructor.
// ATTENTION	:
//=================================================================================================
CCharMakeNewDlg::~CCharMakeNewDlg(void)
{
}


//=================================================================================================
// NAME			: Linking()
// PURPOSE		: For linking components to window manager.
// ATTENTION	:
//=================================================================================================
void CCharMakeNewDlg::Linking()
{
	int count = 0 ;

	for( count = 0 ; count < MAX_PUSHUPBTN_COUNT ; ++count )
	{
		m_pPushupBtn[ count ] = ( cPushupButton* )GetWindowForID( CM_PB_HUMAN + count ) ;
		m_pPushupBtn[ count ]->SetPush( FALSE ) ;
	}
#ifndef _SELECT_GENDER_
	m_pPushupBtn[e_PB_MAN]->SetDisable(TRUE);
	m_pPushupBtn[e_PB_WOMAN]->SetDisable(TRUE);
#endif

	for( count = 0 ; count < MAX_STATIC_COUNT ; ++count )
	{
		m_pStatic[ count ] = ( cStatic* )GetWindowForID( CM_ST_MFACE + count ) ;
	}



	for( count = 0 ; count < MAX_CMBTN_COUNT ; ++count )
	{
		m_pButton[ count ] = ( cButton* )GetWindowForID( CM_SPB_FACE_LEFT + count ) ;
	}

	for( count = 0 ; count < MAX_TEXAREA_COUNT ; ++count )
	{
		m_pTextArea[ count ] = ( cTextArea* )GetWindowForID( CM_TA_RACIAL + count ) ;
	}

	m_pBtnMake		= ( cButton* )GetWindowForID( CM_BTN_MAKE ) ;
	m_pBtnCancel	= ( cButton* )GetWindowForID( CM_BTN_CANCEL ) ;

	m_pAlphaWindow  = ( cStatic* )GetWindowForID( CM_ALPHAWINDOW ) ;
	
	m_pST_Racial	= ( cStatic* )GetWindowForID( CM_ST_RACIAL ) ;
	m_pST_Class		= ( cStatic* )GetWindowForID( CM_ST_CLASS ) ;

	RefreshPushUpBtn() ;
}


//=================================================================================================
// NAME			: RefreshPushUpBtn()
// PURPOSE		: // 061215 LYW --- Refresh pushup buttons.
// ATTENTION	:
//=================================================================================================
void CCharMakeNewDlg::RefreshPushUpBtn()
{	
	m_pPushupBtn[ e_PB_HUMAN ]->SetPush( TRUE ) ;
	m_pPushupBtn[ e_PB_HUMAN ]->SetDisable( TRUE ) ;

	m_pPushupBtn[ e_PB_FIGHTER ]->SetPush( TRUE ) ;
	m_pPushupBtn[ e_PB_FIGHTER ]->SetDisable( TRUE ) ;
#ifndef _SELECT_GENDER_
	m_pPushupBtn[ e_PB_MAN ]->SetPush( TRUE ) ;
	m_pPushupBtn[ e_PB_MAN ]->SetDisable( TRUE ) ;
#endif
	m_byCurRacial	= RaceType_Human ;
	m_byCurClass	= e_FIGHTER ;
	// desc_hseos_성별선택01
	// S 성별선택 추가 added by hseos 2007.06.16
	// ..DB에서 읽어온 유저의 성별로 설정
	// m_byCurGender	= e_MAN ;
 	m_byCurGender = CHARMAKEMGR->GetCharacterMakeInfo()->SexType;
	if (m_byCurGender == e_MAN)
	{
		m_pPushupBtn[ e_PB_MAN ]->SetPush( TRUE ) ;
		m_pPushupBtn[ e_PB_WOMAN ]->SetPush( FALSE ) ;
	}
	else
	{
		m_pPushupBtn[ e_PB_MAN ]->SetPush( FALSE ) ;
		m_pPushupBtn[ e_PB_WOMAN ]->SetPush( TRUE ) ;
	}
	// E 성별선택 추가 added by hseos 2007.06.16
	m_byCurFace		= 0x00 ;
	m_byCurHair		= 0x00 ;

	if( m_byCurGender ==  e_MAN )
	{
		m_pStatic[ e_MFACE ]->SetActive( TRUE ) ;
		m_pStatic[ e_MHAIR ]->SetActive( TRUE ) ;

		m_pStatic[ e_WMFACE ]->SetActive( FALSE ) ;
		m_pStatic[ e_WMHAIR ]->SetActive( FALSE ) ;
	}
	else
	{
		m_pStatic[ e_MFACE ]->SetActive( FALSE ) ;
		m_pStatic[ e_MHAIR ]->SetActive( FALSE ) ;

		m_pStatic[ e_WMFACE ]->SetActive( TRUE ) ;
		m_pStatic[ e_WMHAIR ]->SetActive( TRUE ) ;
	}

	m_pStatic[ e_EMFACE ]->SetActive( FALSE ) ;
	m_pStatic[ e_EMHAIR ]->SetActive( FALSE ) ;

	m_pStatic[ e_EWMFACE ]->SetActive( FALSE ) ;
	m_pStatic[ e_EWMHAIR ]->SetActive( FALSE ) ;

	// 090519 ONS 신규종족 캐릭터 생성 처리
	m_pStatic[ e_DMFACE ]->SetActive( FALSE ) ;
	m_pStatic[ e_DMHAIR ]->SetActive( FALSE ) ;

	m_pStatic[ e_DWMFACE ]->SetActive( FALSE ) ;
	m_pStatic[ e_DWMHAIR ]->SetActive( FALSE ) ;

	m_pTextArea[ e_RACIAL ]->SetScriptText(RESRCMGR->GetMsg(250)) ;
	m_pTextArea[ e_CLASS ]->SetScriptText(RESRCMGR->GetMsg(251)) ;

	m_pAlphaWindow->SetOptionAlpha( (DWORD)0 ) ;
	m_pAlphaWindow->SetAlpha( (BYTE)0 ) ;

	// 090423 ONS 신규종족명 생성처리
	int nMsgIndex = 0;
	if(m_byCurRacial == e_HUMAN || m_byCurRacial == e_ELF)
		nMsgIndex = 247 + m_byCurRacial;
	else
		nMsgIndex = 1129;

	m_pST_Racial->SetStaticText( RESRCMGR->GetMsg(nMsgIndex) ) ;

	char str[128] ;
	char str2[128] ;
	// 090423 ONS 신규종족명 생성처리
	sprintf( str, "%s", RESRCMGR->GetMsg(nMsgIndex) ) ;
	switch( m_byCurClass )
		{
		case 0 : 
			{
				sprintf( str2, " %s", RESRCMGR->GetMsg( 33 ) ) ;
			}
			break ;

		case 1 :
			{
				sprintf( str2, " %s", RESRCMGR->GetMsg( 34 ) ) ;
			}
			break ;

		case 2 :
			{
				sprintf( str2, " %s", RESRCMGR->GetMsg( 35 ) ) ;
			}
			break ;
		}
	strcat(str, str2 ) ;
	m_pST_Class->SetStaticText( str ) ;

	// 090504 ONS 신규종족 생성 가능 확인(특정(50)레벨 이상)
	m_bMakeDevil = CHARSELECT->HasLevelOfChar( MAKE_DEVIL_LIMIT_LEVEL );
	DisableDevilRace( !m_bMakeDevil );
}


//=================================================================================================
// NAME			: ApplyOption()
// PURPOSE		: // 061215 LYW --- Apply option selected by user.
// ATTENTION	:
//=================================================================================================	
void CCharMakeNewDlg::ApplyOption(CHARACTERMAKEINFO* makeInfo) 
{
	if( !makeInfo )		return;

	BYTE jobType = 0x00 ;

	// 090423 ONS 신규종족 직업번호 추가
	if( m_byCurRacial == RaceType_Human || m_byCurRacial == RaceType_Elf )
	{
		switch( m_byCurClass )
		{
		case e_FIGHTER :
			{
				jobType = 0x01 ;
			}
			break ;

		case e_ROGUE :
			{
				jobType = 0x02 ;
			}
			break ;

		case e_MAGE :
			{
				jobType = 0x03 ;
			}
			break ;
		}
	}
	else
	{
		jobType = 0x04;		
	}
	cEditBox * editboxName = (cEditBox *)WINDOWMGR->GetWindowForIDEx(CM_IDEDITBOX);
	char* pName = editboxName->GetEditText(); 

	strcpy( makeInfo->Name, pName ) ;
	makeInfo->RaceType	= m_byCurRacial ;
	makeInfo->SexType	= m_byCurGender ;
	makeInfo->HairType	= m_byCurHair ;
	makeInfo->FaceType	= m_byCurFace ;
	makeInfo->JobType	= jobType ;
}


//=================================================================================================
// NAME			: ApplyOption()
// PURPOSE		: // 061216 LYW --- Add function to process events.
// ATTENTION	:
//=================================================================================================	
void CCharMakeNewDlg::OnActionEvent( LONG lId, void* p, DWORD we )
{
#ifndef _SELECT_GENDER_
	if( lId == CM_PB_MAN || lId == CM_PB_WOMAN )
	{
		m_pPushupBtn[e_MAN]->SetDisable(TRUE) ;
		m_pPushupBtn[e_WOMAN]->SetDisable(TRUE) ;

		return ;
	}
#endif
	if( we & WE_BTNCLICK )
	{
		switch( lId )
		{
			case CM_SPB_FACE_LEFT :		ChangeFace( CM_SPB_FACE_LEFT) ;					break ;
			case CM_SPB_FACE_RIGHT :	ChangeFace( CM_SPB_FACE_RIGHT) ;				break ;

			case CM_SPB_HAIR_LEFT :		ChangeHair( CM_SPB_HAIR_LEFT) ;					break ;
			case CM_SPB_HAIR_RIGHT :	ChangeHair( CM_SPB_HAIR_RIGHT) ;				break ;
		}
	}
	else if( we & WE_PUSHDOWN )
	{
		switch( lId )
		{
			case CM_PB_HUMAN :			ChangeRace( CM_PB_HUMAN, RaceType_Human ) ;		break ;
			case CM_PB_ELF :			ChangeRace( CM_PB_ELF,	 RaceType_Elf ) ;		break ;
			// 090423 ONS 신규종족 선택 처리
			case CM_PB_DEVIL :			ChangeRace( CM_PB_DEVIL, RaceType_Devil ) ;		break ;	

			case CM_PB_FIGHTER :		ChangeJob( CM_PB_FIGHTER, e_FIGHTER ) ;			break ;
			case CM_PB_MAGE :			ChangeJob( CM_PB_MAGE, e_MAGE ) ;				break ;
			case CM_PB_ROGUE :			ChangeJob( CM_PB_ROGUE, e_ROGUE ) ;				break ;

#ifdef _SELECT_GENDER_
			//// 070616 LYW --- CharMakeNewDlg : Deactive change gender part.
			case CM_PB_MAN :			ChangeGender( CM_PB_MAN, e_MAN ) ;				break ;
			case CM_PB_WOMAN :			ChangeGender( CM_PB_WOMAN, e_WOMAN ) ;			break ;
#endif
		}
	}

	ChangeText() ;
}

cPtrList* CCharMakeNewDlg ::GetCurOptionList(int nItem)
{
	int nListIndex = 0 ;

	switch(nItem)
	{
	case e_OPTION_HAIR :
		{
			switch(m_byCurRacial)
			{
			case e_HUMAN :
				{
					switch(m_byCurGender)
					{
					case e_MAN :
						{
							nListIndex = 1 ;
						}
						break ;

					case e_WOMAN :
						{
							nListIndex = 3 ;
						}
						break ;
					}
				}
				break ;

			case e_ELF :
				{
					switch(m_byCurGender)
					{
					case e_MAN :
						{
							nListIndex = 5 ;
						}
						break ;

					case e_WOMAN :
						{
							nListIndex = 7 ;
						}
						break ;
					}
				}
				break ;
			// 090423 ONS 신규종족 헤어스타일 설정
			case e_DEVIL :
				{
					switch(m_byCurGender)
					{
					case e_MAN :
						{
							nListIndex = 9 ;
						}
						break ;

					case e_WOMAN :
						{
							nListIndex = 11 ;
						}
						break ;
					}
				}
				break ;
			}
		}
		break ;

	case e_OPTION_FACE :
		{
			switch(m_byCurRacial)
			{
			case e_HUMAN :
				{
					switch(m_byCurGender)
					{
					case e_MAN :
						{
							nListIndex = 0 ;
						}
						break ;

					case e_WOMAN :
						{
							nListIndex = 2 ;
						}
						break ;
					}
				}
				break ;

			case e_ELF :
				{
					switch(m_byCurGender)
					{
					case e_MAN :
						{
							nListIndex = 4 ;
						}
						break ;

					case e_WOMAN :
						{
							nListIndex = 6 ;
						}
						break ;
					}
				}
				break ;
			// 090423 ONS 신규종족 얼굴모양 설정
			case e_DEVIL :
				{
					switch(m_byCurGender)
					{
					case e_MAN :
						{
							nListIndex = 8 ;
						}
						break ;

					case e_WOMAN :
						{
							nListIndex = 10 ;
						}
						break ;
					}
				}
				break ;
			}
		}
		break ;
	}

	return CHARMAKEMGR->GetOptionList(nListIndex) ;
}

// 070522 LYW --- Add function to change face of character.
void CCharMakeNewDlg::ChangeFace( LONG lID)
{
	cPtrList* pOptionList = GetCurOptionList(e_OPTION_FACE) ;

	int nOptionCount = pOptionList->GetCount() ;

	if( nOptionCount <= 0 ) return ;

	switch( lID )
	{
	case CM_SPB_FACE_LEFT :
		{
			if( m_byCurFace > 0 )
			{
				--m_byCurFace;
			}
			else
			{
				m_byCurFace = BYTE(nOptionCount - 1);
			}
		}
		break ;

	case CM_SPB_FACE_RIGHT :
		{
			if( m_byCurFace < nOptionCount -1 )
			{
				++m_byCurFace;
			}
			else
			{
				m_byCurFace = 0 ;
			}
		}
		break ;
	}

	CHARMAKEMGR->SetOptionList( e_MFACE, m_byCurFace ) ;
	CHARMAKEMGR->SetOptionList( e_WMFACE, m_byCurFace ) ;
	CHARMAKEMGR->SetOptionList( e_EMFACE, m_byCurFace ) ;
	CHARMAKEMGR->SetOptionList( e_EWMFACE, m_byCurFace ) ;
	// 090424 ONS 신규종족 얼굴 설정
	CHARMAKEMGR->SetOptionList( e_DMFACE, m_byCurFace ) ;
	CHARMAKEMGR->SetOptionList( e_DWMFACE, m_byCurFace ) ;

	if( CHARMAKEMGR->GetNewPlayer() )
	{
		CHARMAKEMGR->GetCharacterMakeInfo()->FaceType = m_byCurFace ;
	}

	CHARMAKEMGR->ReplaceCharMakeInfo() ;
}

// 070522 LYW --- Add function to change hair stype of character.
void CCharMakeNewDlg::ChangeHair( LONG lID )
{
	cPtrList* pOptionList = GetCurOptionList(e_OPTION_HAIR) ;

	int nOptionCount = pOptionList->GetCount() ;

	if( nOptionCount <= 0 ) return ;

	switch( lID )
	{
	case CM_SPB_HAIR_LEFT :
		{
			if( m_byCurHair > 0 )
				{
					--m_byCurHair;
				}
				else
				{
					m_byCurHair = BYTE(nOptionCount - 1);
				}
		}
		break ;

	case CM_SPB_HAIR_RIGHT :
		{
			if( m_byCurHair < nOptionCount - 1 )
			{
				++m_byCurHair;
			}
			else
			{
				m_byCurHair = 0 ;
			}
		}
		break ;
	}

	CHARMAKEMGR->SetOptionList( e_MHAIR, m_byCurHair ) ;
	CHARMAKEMGR->SetOptionList( e_WMHAIR, m_byCurHair ) ;
	CHARMAKEMGR->SetOptionList( e_EMHAIR, m_byCurHair ) ;
	CHARMAKEMGR->SetOptionList( e_EWMHAIR, m_byCurHair ) ;
	// 090424 ONS 신규종족 헤어스타일 설정
	CHARMAKEMGR->SetOptionList( e_DMHAIR, m_byCurHair ) ;
	CHARMAKEMGR->SetOptionList( e_DWMHAIR, m_byCurHair ) ;

	if( CHARMAKEMGR->GetNewPlayer() )
	{
		CHARMAKEMGR->GetCharacterMakeInfo()->HairType = m_byCurHair ;
	}

	CHARMAKEMGR->ReplaceCharMakeInfo() ;
}

// 070522 LYW --- Add function to change race of character.
void CCharMakeNewDlg::ChangeRace( LONG lID, BYTE race )
{
	switch(lID)
	{
	case CM_PB_HUMAN :
		{
			m_pPushupBtn[ e_PB_HUMAN ]->SetPush( TRUE ) ;
			m_pPushupBtn[ e_PB_HUMAN ]->SetDisable( TRUE ) ;

			m_pPushupBtn[ e_PB_ELF ]->SetPush( FALSE ) ;
			m_pPushupBtn[ e_PB_ELF ]->SetDisable( FALSE ) ;

			// 090423 ONS 신규종족 버튼 / 직업버튼 활성화
			ChangeDevilButton(FALSE);

			m_byCurRacial = RaceType_Human ;
			m_pST_Racial->SetStaticText(RESRCMGR->GetMsg(247)) ;

			if( m_byCurGender == e_MAN )
			{
				m_pStatic[ e_EMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_EMHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_EWMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_EWMHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_MFACE ]->SetActive( TRUE ) ;
				m_pStatic[ e_MHAIR ]->SetActive( TRUE ) ;

				m_pStatic[ e_WMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_WMHAIR ]->SetActive( FALSE ) ;

				// 090423 ONS 신규종족 관련 버튼 비활성화
				m_pStatic[ e_DMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_DMHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_DWMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_DWMHAIR ]->SetActive( FALSE ) ;
			}
			else
			{
				m_pStatic[ e_EMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_EMHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_EWMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_EWMHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_MFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_MHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_WMFACE ]->SetActive( TRUE ) ;
				m_pStatic[ e_WMHAIR ]->SetActive( TRUE ) ;

				// 090423 ONS 신규종족 관련 버튼 비활성화
				m_pStatic[ e_DMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_DMHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_DWMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_DWMHAIR ]->SetActive( FALSE ) ;
			}
		}
		break ;

	case CM_PB_ELF :
		{
			m_pPushupBtn[ e_PB_ELF ]->SetPush( TRUE ) ;
			m_pPushupBtn[ e_PB_ELF ]->SetDisable( TRUE ) ;

			m_pPushupBtn[ e_PB_HUMAN ]->SetPush( FALSE ) ;
			m_pPushupBtn[ e_PB_HUMAN ]->SetDisable( FALSE ) ;

			// 090423 ONS 신규종족 버튼 / 직업버튼 활성화
			ChangeDevilButton(FALSE);

			m_byCurRacial = RaceType_Elf ;
			m_pST_Racial->SetStaticText(RESRCMGR->GetMsg(248)) ;

			if( m_byCurGender == e_MAN )
			{
				m_pStatic[ e_EMFACE ]->SetActive( TRUE ) ;
				m_pStatic[ e_EMHAIR ]->SetActive( TRUE ) ;

				m_pStatic[ e_MFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_MHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_WMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_WMHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_EWMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_EWMHAIR ]->SetActive( FALSE ) ;

				// 090423 ONS 신규종족 관련 버튼 비활성화
				m_pStatic[ e_DMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_DMHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_DWMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_DWMHAIR ]->SetActive( FALSE ) ;
			}
			else
			{
				m_pStatic[ e_EMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_EMHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_EWMFACE ]->SetActive( TRUE ) ;
				m_pStatic[ e_EWMHAIR ]->SetActive( TRUE ) ;

				m_pStatic[ e_MFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_MHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_WMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_WMHAIR ]->SetActive( FALSE ) ;

				// 090423 ONS 신규종족 관련 버튼 비활성화
				m_pStatic[ e_DMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_DMHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_DWMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_DWMHAIR ]->SetActive( FALSE ) ;
			}
		}
		break ;
	// 090423 ONS 신규종족 관련 버튼 처리
	case CM_PB_DEVIL :
		{
			m_pPushupBtn[ e_PB_ELF ]->SetPush( FALSE ) ;
			m_pPushupBtn[ e_PB_ELF ]->SetDisable( FALSE ) ;

			m_pPushupBtn[ e_PB_HUMAN ]->SetPush( FALSE ) ;
			m_pPushupBtn[ e_PB_HUMAN ]->SetDisable( FALSE ) ;

			m_pPushupBtn[ e_PB_DEVIL ]->SetPush( TRUE ) ;	
			m_pPushupBtn[ e_PB_DEVIL ]->SetDisable( TRUE ) ;

			m_byCurRacial = RaceType_Devil ;
			m_pST_Racial->SetStaticText(RESRCMGR->GetMsg(1129)) ;
			DisableJob( TRUE );


			if( m_byCurGender == e_MAN )
			{
				m_pStatic[ e_EMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_EMHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_MFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_MHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_WMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_WMHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_EWMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_EWMHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_DMFACE ]->SetActive( TRUE ) ;
				m_pStatic[ e_DMHAIR ]->SetActive( TRUE ) ;

				m_pStatic[ e_DWMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_DWMHAIR ]->SetActive( FALSE ) ;
			}
			else
			{
				m_pStatic[ e_EMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_EMHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_EWMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_EWMHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_MFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_MHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_WMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_WMHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_DMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_DMHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_DWMFACE ]->SetActive( TRUE ) ;
				m_pStatic[ e_DWMHAIR ]->SetActive( TRUE ) ;
			}
		}
		break ;
	}

	if( CHARMAKEMGR->GetNewPlayer() )
	{
		CHARMAKEMGR->GetCharacterMakeInfo()->RaceType = race ;
	}

	CHARMAKEMGR->ReplaceCharMakeInfo() ;
}

// 070522 LYW --- Add function to change gender of character.
void CCharMakeNewDlg::ChangeGender( LONG lID, BYTE gender )
{
	switch( lID )
	{
	case CM_PB_MAN :
		{
			m_pPushupBtn[ e_PB_MAN ]->SetPush( TRUE ) ;
			m_pPushupBtn[ e_PB_MAN ]->SetDisable( TRUE ) ;

			m_pPushupBtn[ e_PB_WOMAN ]->SetPush( FALSE ) ;
			m_pPushupBtn[ e_PB_WOMAN ]->SetDisable( FALSE ) ;

			if( m_byCurRacial == RaceType_Human )
			{
				m_pStatic[ e_EMFACE ]->SetActive( TRUE ) ;
				m_pStatic[ e_EMHAIR ]->SetActive( TRUE ) ;

				m_pStatic[ e_EWMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_EWMHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_MFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_MHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_WMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_WMHAIR ]->SetActive( FALSE ) ;

			}
			else
			{
				m_pStatic[ e_EMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_EMHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_EWMFACE ]->SetActive( TRUE ) ;
				m_pStatic[ e_EWMHAIR ]->SetActive( TRUE ) ;

				m_pStatic[ e_MFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_MHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_WMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_WMHAIR ]->SetActive( FALSE ) ;

			}
			m_byCurGender = e_MAN ;
		}
		break ;

	case CM_PB_WOMAN :
		{
			m_pPushupBtn[ e_PB_WOMAN ]->SetPush( TRUE ) ;
			m_pPushupBtn[ e_PB_WOMAN ]->SetDisable( TRUE ) ;

			m_pPushupBtn[ e_PB_MAN ]->SetPush( FALSE ) ;
			m_pPushupBtn[ e_PB_MAN ]->SetDisable( FALSE ) ;

			if( m_byCurRacial == RaceType_Human )
			{
				m_pStatic[ e_EMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_EMHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_EWMFACE ]->SetActive( TRUE ) ;
				m_pStatic[ e_EWMHAIR ]->SetActive( TRUE ) ;

				m_pStatic[ e_MFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_MHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_WMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_WMHAIR ]->SetActive( FALSE ) ;

			}
			else
			{
				m_pStatic[ e_EMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_EMHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_EWMFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_EWMHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_MFACE ]->SetActive( FALSE ) ;
				m_pStatic[ e_MHAIR ]->SetActive( FALSE ) ;

				m_pStatic[ e_WMFACE ]->SetActive( TRUE ) ;
				m_pStatic[ e_WMHAIR ]->SetActive( TRUE ) ;
			}

			m_byCurGender = e_WOMAN ;
		}
		break ;
	}

	if( CHARMAKEMGR->GetNewPlayer() )
	{
		CHARMAKEMGR->GetCharacterMakeInfo()->SexType = gender ;
	}

	CHARMAKEMGR->ReplaceCharMakeInfo() ;
}

// 070522 LYW --- Add function to change job of character.
void CCharMakeNewDlg::ChangeJob( LONG lID, BYTE job )
{
	switch( lID )
	{
	case CM_PB_FIGHTER :
		{
			m_pPushupBtn[ e_PB_FIGHTER ]->SetPush( TRUE ) ;
			m_pPushupBtn[ e_PB_FIGHTER ]->SetDisable( TRUE ) ;

			m_pPushupBtn[ e_PB_MAGE ]->SetPush( FALSE ) ;
			m_pPushupBtn[ e_PB_MAGE ]->SetDisable( FALSE ) ;
			m_pPushupBtn[ e_PB_ROGUE ]->SetPush( FALSE ) ;
			m_pPushupBtn[ e_PB_ROGUE ]->SetDisable( FALSE ) ;

			m_byCurClass = e_FIGHTER ;
		}
		break ;

	case CM_PB_MAGE :
		{
			m_pPushupBtn[ e_PB_MAGE ]->SetPush( TRUE ) ;
			m_pPushupBtn[ e_PB_MAGE ]->SetDisable( TRUE ) ;

			m_pPushupBtn[ e_PB_FIGHTER ]->SetPush( FALSE ) ;
			m_pPushupBtn[ e_PB_FIGHTER ]->SetDisable( FALSE ) ;
			m_pPushupBtn[ e_PB_ROGUE ]->SetPush( FALSE ) ;
			m_pPushupBtn[ e_PB_ROGUE ]->SetDisable( FALSE ) ;

			m_byCurClass = e_MAGE ;
		}
		break ;

	case CM_PB_ROGUE :
		{
			m_pPushupBtn[ e_PB_ROGUE ]->SetPush( TRUE ) ;
			m_pPushupBtn[ e_PB_ROGUE ]->SetDisable( TRUE ) ;

			m_pPushupBtn[ e_PB_FIGHTER ]->SetPush( FALSE ) ;
			m_pPushupBtn[ e_PB_FIGHTER ]->SetDisable( FALSE ) ;
			m_pPushupBtn[ e_PB_MAGE ]->SetPush( FALSE ) ;
			m_pPushupBtn[ e_PB_MAGE ]->SetDisable( FALSE ) ;

			m_byCurClass = e_ROGUE ;
		}
		break ;
	}

	// 090529 LUJ, 선택된 직업을 저장한 후, 캐릭터를 재생성하도록 한다
	CHARMAKEMGR->GetCharacterMakeInfo()->JobType = job;
	CHARMAKEMGR->ReplaceCharMakeInfo();
}

// 070522 LYW --- Add function to change text part.
void CCharMakeNewDlg::ChangeText()
{
	if( m_byCurRacial == RaceType_Human )
	{
		if( m_byCurClass == e_FIGHTER )
		{
			m_pTextArea[ e_CLASS ]->SetScriptText(RESRCMGR->GetMsg(251)) ;
		}
		else if( m_byCurClass == e_ROGUE )
		{
			m_pTextArea[ e_CLASS ]->SetScriptText(RESRCMGR->GetMsg(252)) ;
		}
		else
		{
			m_pTextArea[ e_CLASS ]->SetScriptText(RESRCMGR->GetMsg(253)) ;
		}

		m_pTextArea[ e_RACIAL ]->SetScriptText(RESRCMGR->GetMsg(250)) ;
	}
	else if( m_byCurRacial == RaceType_Elf )
	{
		if( m_byCurClass == e_FIGHTER )
		{
			m_pTextArea[ e_CLASS ]->SetScriptText(RESRCMGR->GetMsg(255)) ;
		}
		else if( m_byCurClass == e_ROGUE )
		{
			m_pTextArea[ e_CLASS ]->SetScriptText(RESRCMGR->GetMsg(256)) ;
		}
		else
		{
			m_pTextArea[ e_CLASS ]->SetScriptText(RESRCMGR->GetMsg(257)) ;
		}

		m_pTextArea[ e_RACIAL ]->SetScriptText(RESRCMGR->GetMsg(254)) ;
	}
	else
	{
		// 090424 ONS 신규종족 클래스와 종족 설명처리 추가
		m_pTextArea[ e_CLASS ]->SetScriptText(RESRCMGR->GetMsg(1163)) ;
		m_pTextArea[ e_RACIAL ]->SetScriptText(RESRCMGR->GetMsg(1131)) ;
	}

	char str[128] ;
	// 090424 ONS 신규종족+직업 버튼 처리
	if(m_byCurRacial == RaceType_Devil)
	{
		sprintf( str, "%s", RESRCMGR->GetMsg(1132) ) ;
	}
	else
	{
		char str2[128] ;
		sprintf( str, "%s", RESRCMGR->GetMsg( 247 + m_byCurRacial ) ) ;

		switch( m_byCurClass )
		{
		case 0 : 
			{
				sprintf( str2, " %s", RESRCMGR->GetMsg( 33 ) ) ;
			}
			break ;

		case 1 :
			{
				sprintf( str2, " %s", RESRCMGR->GetMsg( 34 ) ) ;
			}
			break ;

		case 2 :
			{
				sprintf( str2, " %s", RESRCMGR->GetMsg( 35 ) ) ;
			}
			break ;
		}
		strcat(str, str2 ) ;
	}
	m_pST_Class->SetStaticText( str ) ;
}

// 061215 LYW --- Return pointer that is member variable in this class.
// num ==> 0 = return mface, 1 = return mhair, 2 = return wmface, 3 = return wmhair
// 070525 LYW --- CharMakeNewDlg : Modified function GetStatic.
//cStatic* GetStatic( LONG num ) { return m_pStatic[ num ] ; }
cStatic* CCharMakeNewDlg::GetStatic( LONG num )
{
	if( num >= MAX_STATIC_COUNT ) _asm int 3 ;

	return m_pStatic[ num ] ;
}

void CCharMakeNewDlg::DisableDevilRace( BOOL bDisable )
{
	DWORD FullColor = 0xffffffff;
	DWORD HalfColor = RGBA_MAKE(200,200,200,255);

	DWORD StaticColor = RGB_HALF(10,10,10);

	cWindow* pWnd = NULL;

	if( bDisable )
	{
		FullColor = HalfColor;
		StaticColor = HalfColor ;
	}
	
	// 신규종족 Text 활성/비활성
	pWnd = GetWindowForID( CM_ST_DEVIL );
	if( pWnd )
		((cStatic*)pWnd)->SetFGColor(StaticColor);

	// 신규종족 PushBtn 활성/비활성
	pWnd = GetWindowForID( CM_PB_DEVIL );
	if( pWnd )
	{		
		pWnd->SetDisable(bDisable);
		pWnd->SetImageRGB(FullColor);
	}
}

// 090504 ONS 직업선택 PushButn을 활성/비활성화 시킨다 (신규종족 선택시 비활성화)
void CCharMakeNewDlg::DisableJob( BOOL bDisable )
{
	DWORD FullColor = 0xffffffff;
	DWORD HalfColor = RGBA_MAKE(200,200,200,255);

	DWORD StaticColor = RGB_HALF(10,10,10);

	cWindow* pWnd = NULL;

	if( bDisable )
	{
		FullColor = HalfColor;
		StaticColor = HalfColor ;

		m_pPushupBtn[ e_PB_FIGHTER ]->SetPush( FALSE ) ;
		m_pPushupBtn[ e_PB_ROGUE ]->SetPush( FALSE ) ;
		m_pPushupBtn[ e_PB_MAGE ]->SetPush( FALSE ) ;
	}

	// 직업 Text 활성/비활성
	pWnd = GetWindowForID( CM_ST_FIGHTER );
	if( pWnd )
		((cStatic*)pWnd)->SetFGColor(StaticColor);
	pWnd = GetWindowForID( CM_ST_ROGUE );
	if( pWnd )
		((cStatic*)pWnd)->SetFGColor(StaticColor);
	pWnd = GetWindowForID( CM_ST_MAGE );
	if( pWnd )
		((cStatic*)pWnd)->SetFGColor(StaticColor);

	// 직업 PushBtn 활성/비활성
	pWnd = GetWindowForID( CM_PB_FIGHTER );
	if( pWnd )
	{
		pWnd->SetDisable(bDisable);
		pWnd->SetImageRGB(FullColor);
	}
	pWnd = GetWindowForID( CM_PB_ROGUE );
	if( pWnd )
	{
		pWnd->SetDisable(bDisable);
		pWnd->SetImageRGB(FullColor);
	}
	pWnd = GetWindowForID( CM_PB_MAGE );
	if( pWnd )
	{
		pWnd->SetDisable(bDisable);
		pWnd->SetImageRGB(FullColor);
	}
}



void CCharMakeNewDlg::ChangeDevilButton( BOOL bFlag )
{
	// 090504 ONS 신규종족 생성이 가능한 경우 (특정레벨 이상의 캐릭터 보유한 경우)
	if(m_bMakeDevil)
	{
		m_pPushupBtn[ e_PB_DEVIL ]->SetPush( bFlag ) ;
		m_pPushupBtn[ e_PB_DEVIL ]->SetDisable( bFlag ) ;

		m_pPushupBtn[ e_PB_FIGHTER + m_byCurClass ]->SetPush( TRUE ) ;

		// 직업 버튼(파이터,로그,메이지)들을 활성화한다
		DisableJob( FALSE );
	}

}