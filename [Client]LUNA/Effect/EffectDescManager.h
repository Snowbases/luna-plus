// EffectDescManager.h: interface for the CEffectDescManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EFFECTDESCMANAGER_H__267F7AC9_72BC_4867_88D6_EBFEB98350F3__INCLUDED_)
#define AFX_EFFECTDESCMANAGER_H__267F7AC9_72BC_4867_88D6_EBFEB98350F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Effect.h"

class CEffectDesc;
enum eEffectDescKind;

class CEffectDescManager 
{
	// 090623 LUJ, 안정성을 위해 해쉬 컨테이너에 저장하도록 수정
	CYHHashTable< CEffectDesc > mEffectDescHashTable;
public:
	CEffectDescManager();
	virtual ~CEffectDescManager();
	void LoadEffectList( eEffectDescKind, LPCTSTR effectListFileName, CEffectDescManager* = 0 );
	void Release();
	BOOL GetEffect( int EffectDescNum, EFFECTPARAM*, CEffect*, BOOL bWarning = TRUE );
	CEffectDesc* GetEffectDesc(int EffectDescNum);
};

#endif // !defined(AFX_EFFECTDESCMANAGER_H__267F7AC9_72BC_4867_88D6_EBFEB98350F3__INCLUDED_)
