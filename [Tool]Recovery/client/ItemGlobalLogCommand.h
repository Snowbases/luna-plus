/*
080716 LUJ, 일별로 분리된 아이템 로그 테이블을 일괄로 검색할 수 있는 커맨드 클래스
*/
#pragma once


#include "command.h"


class CItemGlobalLogCommand : public CCommand
{
public:
	struct Configuration
	{
		eLogitemmoney	mLogType;
		DWORD			mPlayerIndex;
		DWORD			mItemDbIndex;
		DWORD			mItemIndex;
	};

public:
	CItemGlobalLogCommand( CclientApp&, const TCHAR* title, const Configuration& );
	
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
