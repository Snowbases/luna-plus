// PartyInviteDlg.h: interface for the CPartyInviteDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTYINVITEDLG_H__A9592DEA_F0A6_4DC6_8A34_44D935029C9F__INCLUDED_)
#define AFX_PARTYINVITEDLG_H__A9592DEA_F0A6_4DC6_8A34_44D935029C9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "INTERFACE\cDialog.h"

class cStatic;
class cTextArea;

class CPartyInviteDlg : public cDialog  
{
	DWORD mInviterPlayerIndex;
	cTextArea* m_pInviter;
	cStatic* m_pDistribute;
public:
	CPartyInviteDlg();
	virtual ~CPartyInviteDlg();
	DWORD GetInviterPlayerIndex() const { return mInviterPlayerIndex; }
	void Linking();
	void Update(const MSG_NAME_DWORD3&);
};

#endif // !defined(AFX_PARTYINVITEDLG_H__A9592DEA_F0A6_4DC6_8A34_44D935029C9F__INCLUDED_)
