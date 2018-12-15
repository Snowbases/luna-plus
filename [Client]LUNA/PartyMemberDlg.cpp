// PartyMemberDlg.cpp: implementation of the CPartyMemberDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PartyMemberDlg.h"
#include "PartyManager.h"
#include "GameIn.h"
#include "WindowIDEnum.h"
#include "Interface/cPushupButton.h"
#include "Interface/cStatic.h"
#include "Interface/cScriptManager.h"
#include "interface\cCheckBox.h"
#include "./ObjectGuagen.h"
#include "PartyBtnDlg.h"
#include "cResourceManager.h"
#include "Player.h"
#include "ObjectManager.h"
#include "MHMap.h"
#include "StatusIconDlg.h"

CPartyMemberDlg::CPartyMemberDlg() :
// 080318 LUJ, 파티원 버프 스킬 창 초기화
mStatusIconDialog( new CStatusIconDlg ),
// 080318 LUJ, 체크 박스 초기화
mCheckBox(0),
m_MemberID(0)
{
	ZeroMemory(
		&mMasterMarkPosition,
		sizeof(mMasterMarkPosition));
}

CPartyMemberDlg::~CPartyMemberDlg()
{
	SAFE_DELETE( mStatusIconDialog );
}

void CPartyMemberDlg::SetActive(BOOL val)
{
	cDialog::SetActive(val);

	// 080318 LUJ, 회원 창이 꺼지면 스킬 아이콘도 함께 안 보이도록 해야한다
	if( mStatusIconDialog )
	{
		mStatusIconDialog->SetObject( val ? HERO : 0 );
	}
}

void CPartyMemberDlg::Linking(int i)
{
	m_pName = (cPushupButton *)GetWindowForID(PA_MEMBER1NAME+i);
	m_pLife = (CObjectGuagen *)GetWindowForID(PA_GUAGEMEMBER1LIFE+i);
	m_pMana = (CObjectGuagen *)GetWindowForID(PA_GUAGEMEMBER1MANA+i);
	m_pLevel = (cStatic *)GetWindowForID(PA_MEMBER1LEVEL+i);

	// 080318 LUJ, 파티원 버프 스킬 창 초기화
	if( mStatusIconDialog )
	{
		const float defaultSize = 14.0f;
		VECTOR2 position = {
			GetAbsX() + GetWidth(),
			GetAbsY() + 20.0f};
		mStatusIconDialog->SetPosition( position );
		mStatusIconDialog->SetIconSize( defaultSize, defaultSize );
		mStatusIconDialog->SetMaxIconQuantity( 36 );
		mStatusIconDialog->Init( HERO, &position, 12 );
	}

	// 080318 LUJ, 체크 박스 초기화
	{
		switch( GetID() )
		{
		case PA_MEMBER1DLG:
			{
				mCheckBox = ( cCheckBox* )GetWindowForID( PA_MEMBER1_SKILL_ICON_BUTTON );
				break;
			}
		case PA_MEMBER2DLG:
			{
				mCheckBox = ( cCheckBox* )GetWindowForID( PA_MEMBER2_SKILL_ICON_BUTTON );
				break;
			}
		case PA_MEMBER3DLG:
			{
				mCheckBox = ( cCheckBox* )GetWindowForID( PA_MEMBER3_SKILL_ICON_BUTTON );
				break;
			}
		case PA_MEMBER4DLG:
			{
				mCheckBox = ( cCheckBox* )GetWindowForID( PA_MEMBER4_SKILL_ICON_BUTTON );
				break;
			}
		case PA_MEMBER5DLG:
			{
				mCheckBox = ( cCheckBox* )GetWindowForID( PA_MEMBER5_SKILL_ICON_BUTTON );
				break;
			}
		case PA_MEMBER6DLG:
			{
				mCheckBox = ( cCheckBox* )GetWindowForID( PA_MEMBER6_SKILL_ICON_BUTTON );
				break;
			}
		}

		if( mCheckBox && 
			mCheckBox->GetType() != WT_CHECKBOX )
		{
			mCheckBox = 0;
		}
	}

	// 091127 LUJ, 파티 마스터 아이콘 표시 위치
	{
		const float iconSize = 14.0f;
		const float space = 10.0f;
		mMasterMarkPosition.x = float(GetWidth()) - iconSize - space;
		mMasterMarkPosition.y = GetAbsY() + iconSize + space;
	}
}


void CPartyMemberDlg::SetMemberData(PARTY_MEMBER* pInfo)
{
	if(pInfo == NULL)
	{
		m_MemberID = 0;
		SetActive(FALSE);

		m_pLife->SetValue(0, 0);
		m_pMana->SetValue(0, 0);
		return;
	}
	
	m_MemberID = pInfo->dwMemberID;

	// 071003 LYW --- PartyMemberDlg : Setting master mark part.
	if( m_MemberID == PARTYMGR->GetMasterID() )
	{
		const DWORD masterMarkImageIndex = 118;
		SCRIPTMGR->GetImage(
			masterMarkImageIndex,
			&m_MasterMark,
			PFT_HARDPATH );
	}

	if(pInfo->bLogged)
	{
		const DWORD PMD_LOGIN_BASICCOLOR = RGBA_MAKE(255,255,255,255);
		const DWORD PMD_LOGIN_OVERCOLOR = RGBA_MAKE(255,255,255,255);
		const DWORD PMD_LOGIN_PRESSCOLOR = RGBA_MAKE(255,234,0,255);

		SetTip(*pInfo);
		m_pName->SetText(pInfo->Name, PMD_LOGIN_BASICCOLOR, PMD_LOGIN_OVERCOLOR, PMD_LOGIN_PRESSCOLOR);

		if(pInfo->mMapType==MAP->GetMapNum())
		{
			m_pLife->SetValue(pInfo->LifePercent*0.01f, 0);
			m_pMana->SetValue(pInfo->ManaPercent*0.01f, 0);
		}
		m_pLevel->SetStaticValue(pInfo->Level);
		m_pLevel->SetFGColor(0x0a0a0aff);
	}
	else
	{
		const DWORD PMD_LOGOUT_BASICCOLOR = RGBA_MAKE(172,182,199,255);
		const DWORD PMD_LOGOUT_OVERCOLOR = RGBA_MAKE(172,182,199,255);
		const DWORD PMD_LOGOUT_PRESSCOLOR = RGBA_MAKE(255,234,0,255);

		// ToolTip Clear
		cImage toolTipBackground;
		SCRIPTMGR->GetImage(
			0,
			&toolTipBackground,
			PFT_HARDPATH);
		SetToolTip(
			"",
			RGBA_MAKE(255, 255, 255, 255),
			&toolTipBackground,
			TTCLR_ITEM_CANEQUIP);

		m_pName->SetText(pInfo->Name, PMD_LOGOUT_BASICCOLOR, PMD_LOGOUT_OVERCOLOR, PMD_LOGOUT_PRESSCOLOR);
		m_pLife->SetValue(0, 0);
		m_pMana->SetValue(0, 0);
		m_pLevel->SetStaticText("");
		m_pLevel->SetFGColor(0x0a0a0aff);
		m_pLevel->SetStaticText("");
	}

	SetActive(TRUE);
}

DWORD CPartyMemberDlg::ActionEvent(CMouse * mouseInfo)
{
	DWORD we = WE_NULL;
	if( !m_bActive ) return we;
	we = cDialog::ActionEvent(mouseInfo);
	
	if(we & WE_LBTNCLICK)
	{
		GAMEIN->GetPartyDialog()->SetClickedMemberID(m_MemberID);
	}
	return we;
}

void CPartyMemberDlg::SetNameBtnPushUp(BOOL val)
{
	m_pName->SetPush(val);
}

void CPartyMemberDlg::Render()
{
	if( !IsActive() ) return ;

	// 080318 LUJ, 버프 스킬 표시
	// 080613 LUJ, 액티브 상태에서만 렌더링되도록 위치 이동
	if( mStatusIconDialog	&&
		mCheckBox			&&
		mCheckBox->IsChecked() )
	{
		mStatusIconDialog->Render();
	}

	cDialog::RenderWindow();
	cDialog::RenderComponent();

	if(m_MemberID == PARTYMGR->GetMasterID())
	{
		VECTOR2 scale = {1.0f, 1.0f};
		m_MasterMark.RenderSprite(
			&scale,
			0,
			0,
			&mMasterMarkPosition,
			0xffffffff) ;
	}
}

// 080318 LUJ, 버프 아이콘 표시 위치 갱신 위해 오버라이딩
void CPartyMemberDlg::SetAbsXY( LONG x, LONG y )
{
	cDialog::SetAbsXY( x, y );

	if( mStatusIconDialog )
	{
		VECTOR2 position = { 0 };
		position.x	= float( x ) + GetHeight();
		position.y	= float( y ) + 20.0f;

		mStatusIconDialog->SetPosition( position );
	}
}


// 080318 LUJ, 버프 스킬을 표시하거나 끈다
void CPartyMemberDlg::SetVisibleSkill( BOOL isVisible )
{
	if( mStatusIconDialog )
	{
		mStatusIconDialog->SetObject( isVisible ? HERO : 0 );
	}

	if( mCheckBox )
	{
		mCheckBox->SetChecked( isVisible );
	}
}


// 080318 LUJ, 버프 스킬 표시 여부를 알린다
BOOL CPartyMemberDlg::IsVisibleSkill() const
{
	if( mStatusIconDialog )
	{
		return 0 != mStatusIconDialog->GetObject();
	}

	return FALSE;	
}

void CPartyMemberDlg::SetTip(const PARTY_MEMBER& member)
{
	cWindow* window = 0;

	switch(GetID())
	{
	case PA_MEMBER1DLG:
		{
			window = GetWindowForID(PA_MEMBER1NAME);
			break;
		}
	case PA_MEMBER2DLG:
		{
			window = GetWindowForID(PA_MEMBER2NAME);
			break;
		}
	case PA_MEMBER3DLG:
		{
			window = GetWindowForID(PA_MEMBER3NAME);
			break;
		}
	case PA_MEMBER4DLG:
		{
			window = GetWindowForID(PA_MEMBER4NAME);
			break;
		}
	case PA_MEMBER5DLG:
		{
			window = GetWindowForID(PA_MEMBER5NAME);
			break;
		}
	case PA_MEMBER6DLG:
		{
			window = GetWindowForID(PA_MEMBER6NAME);
			break;
		}
	}

	if(0 == window)
	{
		return;
	}
	else if(0 == member.mMapType)
	{
		return;
	}

	cImage toolTipBackground;
	SCRIPTMGR->GetImage(
		0,
		&toolTipBackground,
		PFT_HARDPATH);
	SetToolTip(
		"",
		RGBA_MAKE(255, 255, 255, 255),
		&toolTipBackground,
		TTCLR_ITEM_CANEQUIP);

	const WORD index = ( 1 < member.mJobGrade ? member.mJob[ member.mJobGrade - 1 ] : 1 );
	const WORD job = ( member.mJob[ 0 ] * 1000 ) + ( ( member.mRace + 1 ) * 100 ) + ( member.mJobGrade * 10 ) + index;
	char line[MAX_PATH] = {0};
	sprintf(
		line,
		"%s, %s",
		RESRCMGR->GetMsg(RESRCMGR->GetClassNameNum(job)),
		GetMapName(member.mMapType));
	AddToolTipLine(line);
}

void CPartyMemberDlg::SetLife(float value)
{
	m_pLife->SetValue(
		value / 100.0f,
		1);
}

void CPartyMemberDlg::SetMana(float value)
{
	m_pMana->SetValue(
		value / 100.0f,
		1);
}

void CPartyMemberDlg::SetLevel(LONG value)
{
	if(m_pLevel)
	{
		m_pLevel->SetStaticValue(value);
	}
}
