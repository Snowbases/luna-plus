/*
080403 LUJ, 퀘스트 로그를 처리하는 커맨드 클래스
*/
#pragma once


#include "Command.h"


class CGuildWarehouseLogCommand : public CCommand
{
public:
	CGuildWarehouseLogCommand( CclientApp&, const TCHAR* title, DWORD guildIndex );

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
	DWORD mGuildIndex;
};
