#pragma once

#include "MonsterTargetDlg.h"

class CGuildMark;
class CPlayer;

class	cCharacterTargetDlg	:	public	cMonsterTargetDlg
{
public:
	cCharacterTargetDlg();
	virtual ~cCharacterTargetDlg();

public:
	virtual void Render();
	virtual void TargetDlgLink();

	virtual void SetTargetGuageInfo( CObject* pObject );

	virtual void SetMonsterUIActive( bool val );

	void SetTargetGuildInfo( char* pGuildName );

	void SelectTargetGender( CPlayer* pPlayer );

protected:
	cStatic*	m_pFaceElfMan;
	cStatic*	m_pFaceElfWoman;
	cStatic*	m_pFaceHumanMan;
	cStatic*	m_pFaceHumanWoman;
	cStatic*	m_pFaceDevilMan;
	cStatic*	m_pFaceDevilWoman;
	cStatic*	m_pCurFace;

	cStatic*	m_pGuildName;
	cCheckBox*	m_pGuildMark;

	CGuildMark*	m_pGuildImage;
};