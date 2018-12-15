#include "stdafx.h"
#include "CharMakeManager.h"
#include "windowidenum.h"
#include "ObjectManager.h"
#include "interface/cStatic.h"

// 061215 LYW --- Include dialog to create character.
#include "CharMakeNewDlg.h"

// 061215 LYW --- Include window manager.
#include "./interface/cWindowManager.h"


GLOBALTON(cCharMakeManager)
cCharMakeManager::cCharMakeManager()
{
	m_bInit = FALSE;
	ZeroMemory( &mWearedItem, sizeof( mWearedItem ) );
}

cCharMakeManager::~cCharMakeManager()
{
}

void cCharMakeManager::Init()
{
	m_pNewPlayer = NULL;
	// desc_hseos_성별선택01
	// S 성별선택 추가 added by hseos 2007.06.16
	// ..여기 오기전에 DB에서 성별의 정보를 읽어와서 설정했기 때문에
	// ..초기화 하기 전과 후에 처리해줘야 함.
	BYTE nTmpSexType = m_CharMakeInfo.SexType;
	memset(&m_CharMakeInfo, 0, sizeof(CHARACTERMAKEINFO));
	// E 성별선택 추가 added by hseos 2007.06.16
	m_CharMakeInfo.SexType = nTmpSexType;
	m_pCharMakeNewDlg = ( CCharMakeNewDlg* )WINDOWMGR->GetWindowForID( CM_MAKEDLG ) ;

	// 061217 LYW --- Load and setting option list.
	LoadOptionList() ;
	SetOptionList( e_MFACE, 0 ) ;
	SetOptionList( e_MHAIR, 0 ) ;
	SetOptionList( e_WMFACE, 0 ) ;
	SetOptionList( e_WMHAIR, 0 ) ;

	SetOptionList( e_EMFACE, 0 ) ;
	SetOptionList( e_EMHAIR, 0 ) ;
	SetOptionList( e_EWMFACE, 0 ) ;
	SetOptionList( e_EWMHAIR, 0 ) ;

	// 090424 ONS 신규종족 관련 처리 추가 
	SetOptionList( e_DMFACE, 0 ) ;
	SetOptionList( e_DMHAIR, 0 ) ;
	SetOptionList( e_DWMFACE, 0 ) ;
	SetOptionList( e_DWMHAIR, 0 ) ;

	m_bInit = TRUE;

	// 061215 --- Delete this code for a moment.
	/*
	m_pHeight = (cGuageBar*)CHARMAKE->GetCharMakeDialog()->GetWindowForID(CMID_Height);
	m_pWidth = (cGuageBar*)CHARMAKE->GetCharMakeDialog()->GetWindowForID(CMID_Width);
	*/
}

void cCharMakeManager::Release()
{
	for(int i=0;i<CM_MAX;++i)
	{
		if( !m_UserSelectOption[i].IsEmpty() )
		{
			PTRLISTSEARCHSTART(m_UserSelectOption[i],CM_OPTION*,Value)
				delete Value;
			PTRLISTSEARCHEND

			m_UserSelectOption[i].RemoveAll();
		}
	}
}
/*SW050812
void cCharMakeManager::ComboBoxResetAll()
{

	cComboBoxEx* pCombo = NULL;

	for(int i=0;i<CE_MAX;++i)
	{
		if( !m_UserSelectOption[i].IsEmpty() && i != CE_SEX  && i != CE_AREA)
		{
			PTRLISTPOS pos = m_UserSelectOption[i].GetHeadPosition();
			CM_OPTION* Value = (CM_OPTION*)m_UserSelectOption[i].GetAt(pos);
			pCombo = (cComboBoxEx*)Value->pDescCombo;
			pCombo->SelectComboText(0);
		}
	}
}*/

void cCharMakeManager::ListBoxResetAll()
{
	cStatic* pStatic = NULL;

	for(int i=0;i<CE_MAX;++i)
	{
		if( !m_UserSelectOption[i].IsEmpty() && i != CE_SEX  && i != CE_AREA && i != CE_WEAPON )
		{
			PTRLISTPOS pos = m_UserSelectOption[i].GetHeadPosition();
			CM_OPTION* Value = (CM_OPTION*)m_UserSelectOption[i].GetAt(pos);
			pStatic = (cStatic*)Value->pDescStatic;
			pStatic->SetStaticText(Value->strName);
		}
	}	
}

void cCharMakeManager::LoadOptionList()
{
	int idx = 0;
	int num = 0;
	char StaticId[64];

	CMHFile file;
	if( FALSE == file.Init( "System/Resource/CharMake_List.bin", "rb" ) ) return;

	char temp[256];
	file.GetLine(temp, 256);

	while(1)
	{
		if( idx >= CM_MAX || file.IsEOF()) break;

		num = file.GetInt(); 
		strcpy(StaticId, file.GetString());

		for( int i=0;i<num;++i)
		{
			CM_OPTION*	option = new CM_OPTION;
			strcpy(option->strName, file.GetString());
			option->dwData = file.GetDword();
			option->dwHelperData = file.GetDword();
			//option->pDescStatic = (cStatic*)CHARMAKE->GetCharMakeNewDlg()->GetWindowForID( IDSEARCH(StaticId) );
			CCharMakeNewDlg* pDlg = ( CCharMakeNewDlg* )WINDOWMGR->GetWindowForID( CM_MAKEDLG ) ;
			option->pDescStatic = pDlg->GetStatic(idx) ;
			if(option->pDescStatic != NULL)
			{
				option->pDescStatic->SetAlign(TXT_MIDDLE);
			}

			m_UserSelectOption[idx].AddTail(option);
		}
		++idx;
	}
}

// 061214 LYW ---- End add.
//=================================================================================================
// NAME			: SetOptionList()
// PURPOSE		: Att fucntion to setting selection value.
// ATTENTION	: num ==> 0 = mface, 1 = mhair, 2 = wmface, 3 = wmhair
//=================================================================================================
void cCharMakeManager::SetOptionList( int curStatic, int curIdx )
{
	PTRLISTPOS pos = m_UserSelectOption[curStatic].FindIndex(curIdx) ;

	if(pos)
	{
		CM_OPTION* Value = (CM_OPTION*)m_UserSelectOption[curStatic].GetAt(pos) ;
		cStatic* pStatic = ( cStatic* )WINDOWMGR->GetWindowForIDEx(CM_ST_MFACE + curStatic) ;
		if(pStatic)
		{
			pStatic->SetStaticText(Value->strName) ;	
			pStatic->SetFontIdx(0) ;
			// 070122 LYW --- Setting color of font.
			pStatic->SetFGColor( RGBA_MAKE( 10, 10, 10, 255 ) ) ;
		}
	}
}

void cCharMakeManager::ReplaceCharMakeInfo()
{
	if( m_pNewPlayer ) 
	{
		OBJECTMGR->AddGarbageObject( (CObject*)m_pNewPlayer );
		m_pNewPlayer = NULL;
	}
}

void cCharMakeManager::CreateNewCharacter(VECTOR3* pv3Pos)
{
	BASEOBJECT_INFO sBaseInfo;
	CHARACTER_TOTALINFO sTotalInfo;

	memset(&sBaseInfo, 0, sizeof(BASEOBJECT_INFO));
	memset(&sTotalInfo, 0, sizeof(CHARACTER_TOTALINFO));

	sBaseInfo.dwObjectID = 1;
	strcpy( sBaseInfo.ObjectName, "NewCharacter" );
	sBaseInfo.ObjectState = eObjectState_Enter;

//	sTotalInfo.Job = m_CharMakeInfo.JobType;
	sTotalInfo.HairType = m_CharMakeInfo.HairType;
	sTotalInfo.FaceType = m_CharMakeInfo.FaceType;
	sTotalInfo.Race = m_CharMakeInfo.RaceType;
	sTotalInfo.Gender = m_CharMakeInfo.SexType;
	sTotalInfo.Height = 1;
	sTotalInfo.Width = 1;
	sTotalInfo.bVisible = TRUE;

	// 090622 ONS 종족에 따라 기본 아이템을 설정한다.
	BYTE byRace = m_CharMakeInfo.RaceType;
	if( byRace == e_DEVIL)
	{
		sTotalInfo.WearedItemIdx[ eWearedItem_Weapon ]	= 11007554;
		sTotalInfo.WearedItemIdx[ eWearedItem_Dress ]	= 12009347;
	}
	else
	{
		// 090529 LUJ, 직업에 따라 기본 아이템을 설정한다
		sTotalInfo.WearedItemIdx[ eWearedItem_Weapon ] = GetWeaponIndex( ENUM_CM_CLASS( m_CharMakeInfo.JobType ) );
		sTotalInfo.WearedItemIdx[ eWearedItem_Dress ] = GetDressIndex( ENUM_CM_CLASS( m_CharMakeInfo.JobType ) );
	}
	m_pNewPlayer = OBJECTMGR->AddPlayer(&sBaseInfo, 0, &sTotalInfo);
	m_pNewPlayer->GetEngineObject()->ApplyHeightField(FALSE);
	m_pNewPlayer->SetOverInfoOption(0);
	m_pNewPlayer->SetPosition( pv3Pos );
	// 091009 ONS 캐릭터 생성화면 마족 기본모션 수정
	m_pNewPlayer->ChangeMotion( CHARACTER_MOTION[ eCharacterMotion_Standard ][ (byRace==e_DEVIL)?WP_DUALDAGGER:WP_ONEHANDED ] ,1);
	m_pNewPlayer->ChangeBaseMotion( CHARACTER_MOTION[ eCharacterMotion_Standard ][ WP_NONE ] );	
}

DWORD cCharMakeManager::GetWeaponIndex( ENUM_CM_CLASS jobType ) const
{
	switch( jobType )
	{
	case e_FIGHTER:
		{
			return 11000001;
		}
	case e_ROGUE:
		{
			return 11000187;
		}
	case e_MAGE:
		{
			return 11000249;
		}
	}

	return 0;
}

DWORD cCharMakeManager::GetDressIndex( ENUM_CM_CLASS jobType ) const
{
	switch( jobType )
	{
	case e_FIGHTER:
		{
			return 12000032;
		}
	case e_ROGUE:
		{
			return 12000032;
		}
	case e_MAGE:
		{
			return 12000001;
		}
	}

	return 0;
}

cPtrList* cCharMakeManager::GetOptionList(int idx)
{
	if( idx < 0 || CM_MAX <= idx ) return NULL ; 
	
	return &m_UserSelectOption[idx] ; 
}

