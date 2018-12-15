#pragma once

#include "interface/cDialog.h"


class cStatic;


class CGuildLevelUpDialog : public cDialog  
{
	//cStatic * m_pLevelupNotComplete[4];
	//cStatic * m_pLevelupComplete[4];
	//cStatic * m_pLevel[5];
public:
	CGuildLevelUpDialog();
	virtual ~CGuildLevelUpDialog();
	void Linking();
	virtual void SetActive( BOOL );
	virtual void OnActionEvent( LONG id, void* p, DWORD event );

	void SetLevel( DWORD level );

private:
	typedef std::list< cStatic* >	StaticList;

	StaticList						mCompleteStaticList;
	StaticList						mIncompleteStaticList;
	StaticList						mLevelStaticList;

	cWindow*	mSubmitButton;
};