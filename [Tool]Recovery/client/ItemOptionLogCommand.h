/*
080403 LUJ, 퀘스트 로그를 처리하는 커맨드 클래스
*/
#pragma once


#include "Command.h"


class CItemOptionLogCommand : public CCommand
{
public:
	CItemOptionLogCommand( CclientApp&, const TCHAR* title, DWORD itemDbIndex );

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
	DWORD mItemDbIndex;
};
