/*
080403 LUJ, 아이템 로그를 처리하는 커맨드 클래스
*/
#pragma once


#include "Command.h"


class CItemLogCommand : public CCommand
{
public:
	struct Configuration
	{
		DWORD mItemDbIndex;
		DWORD mItemIndex;
		DWORD mPlayerIndex;
	};

public:
	CItemLogCommand( CclientApp&, const TCHAR* title, const Configuration& );

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
	const Configuration mConfiguration;
};
