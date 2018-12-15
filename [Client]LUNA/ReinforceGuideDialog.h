#pragma once


#include "interface/cTabDialog.h"


class CItemShow;
class cStatic;
class cIconDialog;


class CReinforceGuideDialog : public cTabDialog
{	
public:
	CReinforceGuideDialog(void);
	virtual ~CReinforceGuideDialog(void);

	virtual void SetActive( BOOL );
	void Linking();
	void OnActionEvent(LONG lId, void*, DWORD we);
	void Add( cWindow* );
	void Render();
	

private:
	void Release();
	cIconDialog* GetTabDialog(EWEARED_ITEM);
	cStatic* GetTitleStatic(cDialog*, DWORD iconIndex);
	cStatic* GetDescriptionStatic(cDialog*, DWORD iconIndex);

	typedef std::list< CItemShow* >			IconList;
	IconList								mIconList;
};