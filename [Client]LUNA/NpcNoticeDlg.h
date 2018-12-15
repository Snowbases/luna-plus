#ifndef _NPCNOTICEDLG_
#define _NPCNOTICEDLG_

#include "cListDialogEx.h"

class cNpcNoticeDlg : public cDialog
{
	cListDialogEx*	m_pListDlg;						// NPC 대사 리스트 다이얼로그.

public:
	cNpcNoticeDlg();
	virtual ~cNpcNoticeDlg();

	void Linking();

	BOOL OpenDialog( DWORD dwNpcId, WORD npcChxNum );

	virtual void SetActive( BOOL val );
};

#endif