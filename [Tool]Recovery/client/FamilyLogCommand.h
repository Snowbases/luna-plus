/*
081207 LUJ, �йи� �α׸� ó���ϴ� Ŀ�ǵ� Ŭ����
*/
#pragma once

#include "Command.h"

class CFamilyLogCommand : public CCommand
{
public:
	struct Configuration
	{
		DWORD mFamilyIndex;
		DWORD mPlayerIndex;
	};
	CFamilyLogCommand( CclientApp&, const TCHAR* title, const Configuration& );

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