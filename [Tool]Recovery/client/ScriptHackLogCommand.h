/*
090123 LUJ, 스크립트 해킹 로그를 표시함
*/
#pragma once
#include "command.h"

class CScriptHackLogCommand :
	public CCommand
{
public:
	CScriptHackLogCommand( CclientApp&, const TCHAR* title );

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
};