/*
080630 LUJ, 농장 동물 로그를 처리하는 커맨드 클래스
*/
#pragma once


#include "command.h"


class CLivestockLogCommand : public CCommand
{
public:
	CLivestockLogCommand( CclientApp&, const TCHAR* title, DWORD farmIndex );

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
	DWORD mFarmIndex;
};
