#include "StdAfx.h"
#include ".\gameratingdialog.h"

static DWORD	ToggleTime	= 0;
static BOOL		IsShow		= FALSE;

CGameRatingDialog::CGameRatingDialog(void)
:mTime(0)
{
	if( !ToggleTime )
	{
		ToggleTime = gCurTime + 3000;
	}
}

CGameRatingDialog::~CGameRatingDialog(void)
{
}

void CGameRatingDialog::Render()
{
	if( gCurTime >= ToggleTime )
	{
		IsShow = !IsShow;

		if( IsShow )
		{
			ToggleTime = gCurTime + 3000;
		}
		else
		{
			ToggleTime = gCurTime + ( 60 * 60 * 1000 );
		}
	}

	if( IsShow )
	{
		cDialog::Render();
	}
}