#pragma once
#include ".\interface\ctabdialog.h"

enum eSkillDlgTab
{
	eSkillDlgTab_Active,
	eSkillDlgTab_Passive,
	eSkillDlgTab_Action,
	eSkillDlgTab_Emotiocon,
	eSkillDlgTab_Motion,
};

class cWindow;

class cJobSkillDlg :
	public cTabDialog
{
public:
	cJobSkillDlg(void);
	virtual ~cJobSkillDlg(void);

	void Linking();

	void SetTab( BYTE tab );

	void Render();
	void Add( cWindow *window );

	void OnActionEvnet(LONG lId, void * p, DWORD we);

	void ExcuteSkill();
};
