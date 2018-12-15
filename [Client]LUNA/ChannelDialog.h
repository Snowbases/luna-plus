// ChannelDialog.h: interface for the CChannelDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHANNELDIALOG_H__2799AAF3_2A8B_402D_86D4_EB2547D1888B__INCLUDED_)
#define AFX_CHANNELDIALOG_H__2799AAF3_2A8B_402D_86D4_EB2547D1888B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "./interface/cDialog.h"
#include ".\interface\cWindowHeader.h"


class cListCtrl;
class CChannelDialog  : public cDialog
{
	int m_BaseChannelIndex;
	cListCtrl * m_pChannelLCtrl;
	BOOL m_bInit;
	int m_SelectRowIdx;

	// Channel Dialog의 상태
public:
	enum eState{
		eState_CharSelect,	// 캐릭터선택모드에서 채널이동시
		eState_GameIn,		// 게임안에서 채널이동시
	};
protected:
	eState m_eState;
public:
	CChannelDialog();
	virtual ~CChannelDialog();

	// 070421 LYW --- ChannelDialog : Add function to release m_pChannelLCtrl.
	void ReleaseChannelLCtrl() ;

	virtual DWORD ActionEvent(CMouse * mouseInfo);
	void Linking();
	void SetChannelList(MSG_CHANNEL_INFO* pInfo);
	virtual void SetActive(BOOL val);

	void SelectChannel(int rowidx);
	void OnConnect();

	void SetState( CChannelDialog::eState state ) { m_eState = state; }
	const CChannelDialog::eState GetState() const { return m_eState; }
};

#endif // !defined(AFX_CHANNELDIALOG_H__2799AAF3_2A8B_402D_86D4_EB2547D1888B__INCLUDED_)
