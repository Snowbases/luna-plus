#pragma once


#include "interface/cDialog.h"


class cTextArea;


class CGuildInviteDialog : public cDialog  
{	
public:
	CGuildInviteDialog();
	virtual ~CGuildInviteDialog();

	virtual void OnActionEvent( LONG id, void* p, DWORD event );
	
	void Linking();
	//void SetInfo(char* GuildName, char* MasterName);
	//SW060713 ¹®ÇÏ»ý
	//void SetInfo( const char* GuildName, const char* MasterName, int FlgKind);

	void SetData( const SEND_GUILD_INVITE* );


private:
	cTextArea*	m_pInviteMsg;
	DWORD		mInvitedPlayerIndex;
	

	enum eKind
	{
		eKindNone,
		eKindMember,
		eKindStudent,
	}
	mInvitedKind;
};