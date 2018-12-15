/*
080403 LUJ, 퀘스트 로그를 처리하는 커맨드 클래스
*/
#pragma once


#include "Command.h"


class CMoneyLogCommand : public CCommand
{
public:
	struct Configuration
	{
		Configuration() :
		mPlayerIndex( 0 )
		{}

		DWORD mPlayerIndex;
	};

public:
	// 090122 LUJ, 생성자 변경
	CMoneyLogCommand( CclientApp&, const TCHAR* title, const Configuration& );

	virtual void SaveToExcel( DWORD serverIndex, const CListCtrl&  ) const;
	virtual void Initialize( CListCtrl& ) const;
	virtual void Stop( DWORD serverIndex ) const;
	virtual void Find( DWORD serverIndex, const TCHAR* beginTime, const TCHAR* endTime );
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