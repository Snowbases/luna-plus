#pragma once

#include "Command.h"

class CCookLogCommand : public CCommand
{
public:
	struct Configuration
	{
		DWORD mPlayerIndex;
	};

private:
	const Configuration mConfiguration;

public:
	CCookLogCommand(CclientApp&, LPCTSTR title, const Configuration&);
	virtual void SaveToExcel(DWORD serverIndex, const CListCtrl&) const;
	virtual void Initialize	(CListCtrl&) const;
	virtual void Stop(DWORD serverIndex) const;
	virtual void Find(DWORD	serverIndex, LPCTSTR beginTime, LPCTSTR	endTime);
	virtual void Parse(const MSGROOT*, CListCtrl&, CProgressCtrl&, CStatic&, CButton& findButton, CButton& stopButton) const;
};