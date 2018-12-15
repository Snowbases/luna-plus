#pragma once
#include "statusicondlg.h"

#define PETSTATUSICONDLG USINGTON(CPetStatusIconDialog)

class CPetStatusIconDialog :
	public CStatusIconDlg
{
public:
	CPetStatusIconDialog(void);
	virtual ~CPetStatusIconDialog(void);
	
	virtual void Render();
};

EXTERNGLOBALTON(CPetStatusIconDialog)
