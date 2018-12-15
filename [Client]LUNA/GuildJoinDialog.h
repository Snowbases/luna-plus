/**********************************************************************

이름: GuildJoinDialog
작성: 2007/08/09 16:55:58, 이웅주

목적:	길드원을 어떤 분류로 초대할지 결정한다
                                                                     
***********************************************************************/

#pragma once


#include "interface\cdialog.h"


class CGuildJoinDialog :	public cDialog
{
public:
	CGuildJoinDialog(void);
	virtual ~CGuildJoinDialog(void);

	void Linking();
	void OnActionEvent(LONG lId, void* p, DWORD we);
	void SetEnableStudent( BOOL );
	void SetEnableMember( BOOL );
};
