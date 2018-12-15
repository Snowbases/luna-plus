#pragma once
#include "History.h"

class CReplaceTileHistory : public CHistory
{
public:
	CReplaceTileHistory();
	virtual ~CReplaceTileHistory(void);
	virtual void Undo();
	virtual void Redo();
	void SetValue(DWORD index, LPCTSTR oldPath, LPCTSTR newPath);

private:
	struct Parameter
	{
		DWORD mIndex;
		CString mOldPath;
		CString mNewPath;

		Parameter() :
		mIndex(0)
		{}
	}
	mParameter;
};
