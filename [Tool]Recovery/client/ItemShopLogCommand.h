/*
080403 LUJ, 유료 아이템 구입 로그를 처리하는 커맨드 클래스
*/
#pragma once


#include "Command.h"


class CItemShopLogCommand : public CCommand
{
public:
	CItemShopLogCommand( CclientApp&, const TCHAR* title, DWORD userIndex = 0 );

	virtual void SaveToExcel( DWORD serverIndex, const CListCtrl&  )	const;
	virtual void Initialize	( CListCtrl& )								const;
	virtual void Stop		( DWORD serverIndex )						const;

	virtual void Find(
		DWORD			serverIndex,
		const TCHAR*	beginTime,
		const TCHAR*	endTime );

	virtual void Parse(
		const MSGROOT*,
		CListCtrl&,
		CProgressCtrl&,
		CStatic&,
		CButton&	findButton,
		CButton&	stopButton ) const;

private:
	DWORD	mUserIndex;
};
