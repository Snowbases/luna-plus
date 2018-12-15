//=================================================================================================
//	FILE		: CProfileDlg.h
//	PURPOSE		: Profile for character.
//	PROGRAMMER	: Yongs Lee
//	DATE		: December 11, 2006
//	ADDITION	:
//	ATTENTION	:
//=================================================================================================
#pragma once
#ifndef __PROFILE_H__
#define __PROFILE_H__


//=================================================================================================
//	INCLUDE PART
//=================================================================================================
#include ".\interface\cdialog.h"


//=================================================================================================
//	DEFINE PART
//=================================================================================================
#define MAX_PRINTCOUNT	6


//=================================================================================================
//	THE OTHERS PART
//=================================================================================================
class cStatic ;


//=================================================================================================
// NAME			: CProfileDlg Class.
// PURPOSE		: The class for character profile.
// ATTENTION	:
//=================================================================================================
class CProfileDlg : public cDialog
{
public :
	char m_szClass[5][12][24] ;
	// 090504 ONS 신규종족 추가에 따른 변경(m_szRacial[2][16] -> m_szRacial[3][16])
	char m_szRacial[3][16] ;
	char m_szMapName[24] ;

	cStatic*	m_pStatic[ MAX_PRINTCOUNT ] ;

public:
	CProfileDlg(void);
	virtual ~CProfileDlg(void);

	void Linking() ;

	void UpdateProfile( char* strName, CHARACTER_TOTALINFO* charInfo ) ;

	// 061227 LYW --- Add function to refresh pofile.
	void RefreshProfile() ;
};
#endif //__PROFILE_H__