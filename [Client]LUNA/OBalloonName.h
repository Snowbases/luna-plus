#pragma once

enum EObjectKind;
class cMultiLineText;

class COBalloonName  
{
public:

	COBalloonName();
	virtual ~COBalloonName();

	void InitName( const char* );
	void SetName( const char* );
	void SetNickName( const char* );
	void SetFamilyNickName( const char* );

	BOOL IsActive()						{ return m_bActive;	}
	BOOL IsShow()						{ return m_bShow;	}
	void SetActive(BOOL val)			{ m_bActive = val;	}
	void Show( BOOL val )				{ m_bShow	= val; 	}
	BOOL Render(LONG absX, LONG absY);
	void SetTall(LONG	Tall)			{ m_lTall = Tall;	}
	void SetPosX(LONG	pos);
	void SetNickNameTall(LONG	nickTall)			{ m_lNickTall = nickTall;	}
	void SetFGColor(DWORD col)			{ m_fgColor = col;	}
	
	LONG GetPosX()			const	{ return m_lPosX; }
	LONG GetTall()			const	{ return m_lTall; }
	LONG GetFamilyNickPosX() const	{ return mFamilyNickPosX; }

	struct Data
	{
		LONG mX;
		LONG mLength;
	}
	mNickData;

	const Data& GetNickData() const;

	DWORD GetNameLength();
	void SetGTName(DWORD NameType, char* pGuildName);
	void Set_PParentsObjectKind(EObjectKind kind) { m_byPParentObjectKind = kind; }
   	EObjectKind Get_PParentsObjectKind() const { return m_byPParentObjectKind; }

protected:

	BOOL m_bActive;			//on/off option
	BOOL m_bShow;			//show/hide
	LONG m_lTall;
	LONG m_lPosX;
	LONG m_lNickTall;
	LONG mFamilyNickPosX;
	LONG m_lMasterTall;
	LONG m_lMasterPosX;
	char mObjectName[MAX_MONSTER_NAME_LENGTH +1];
	WORD m_wFontIdx;
	DWORD m_fgColor;
	char mGuildNickName[MAX_PATH];
	char mFamilyNickName[MAX_PATH];
	BOOL m_bSiegeMap;
	BOOL m_bGTournamentMap;
	DWORD m_dwGTColor;

	cMultiLineText* m_pSiegeName;
	EObjectKind m_byPParentObjectKind ;
};
