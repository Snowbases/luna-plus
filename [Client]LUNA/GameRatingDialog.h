#pragma once
#include "interface/cDialog.h"

class CGameRatingDialog :
	public cDialog
{
	DWORD	mTime;

public:
	CGameRatingDialog(void);
	~CGameRatingDialog(void);

	virtual void Render();
};
