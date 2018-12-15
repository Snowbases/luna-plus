// PartyInviteDlg.cpp: implementation of the CPartyInviteDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PartyInviteDlg.h"
#include "WindowIDEnum.h"
#include "./Interface/cTextArea.h"
#include "./Interface/cStatic.h"
#include "ChatManager.h"
#include "cResourceManager.h"

CPartyInviteDlg::CPartyInviteDlg() :
mInviterPlayerIndex(0)
{
	m_type = WT_PARTYINVITEDLG;
}

CPartyInviteDlg::~CPartyInviteDlg()
{

}

void CPartyInviteDlg::Linking()
{
	m_pDistribute = (cStatic*)GetWindowForID(PA_INVITEDISTRIBUTE);
	m_pInviter = (cTextArea*)GetWindowForID(PA_INVITER);
}

void CPartyInviteDlg::Update(const MSG_NAME_DWORD3& message)
{
	mInviterPlayerIndex = message.dwData3;
	
	switch(PARTY_OPTION(message.dwData2))
	{
	case ePartyOpt_Damage :
		{
			m_pDistribute->SetStaticText(
				RESRCMGR->GetMsg(163));
		}
		break ;

	case ePartyOpt_Sequence :
		{
			m_pDistribute->SetStaticText(
				RESRCMGR->GetMsg(161));
		}
		break ;
	}

	TCHAR text[MAX_PATH] = {0};
	_sntprintf(
		text,
		_countof(text),
		CHATMGR->GetChatMsg(765),
		message.Name);
	m_pInviter->SetScriptText(
		text);
}