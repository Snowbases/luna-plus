// EffectDescManager.cpp: implementation of the CEffectDescManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EffectDescManager.h"
#include "EffectDesc.h"
#include "..\MHFile.h"
#include "EffectManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffectDescManager::CEffectDescManager()
{
	mEffectDescHashTable.Initialize( 100 );
}

CEffectDescManager::~CEffectDescManager()
{
	Release();
}

void CEffectDescManager::Release()
{
	mEffectDescHashTable.SetPositionHead();

	for( CEffectDesc* effectDesc = mEffectDescHashTable.GetData();
		0 < effectDesc;
		effectDesc = mEffectDescHashTable.GetData())
	{
		SAFE_DELETE( effectDesc );
	}

	mEffectDescHashTable.RemoveAll();
}

void CEffectDescManager::LoadEffectList( eEffectDescKind effectKind, LPCTSTR effectListFileName, CEffectDescManager* existedEffectDescManager /*= NULL*/ )
{
	Release();

	CMHFile file;
	file.Init( effectListFileName, "rb" );
	file.GetDword();
	
	// 090623 LUJ, 이펙트 목록 파일을 읽어서 클래스에 설정한다
	while( FALSE == file.IsEOF() )
	{
		const DWORD effectIndex = file.GetDword();
		char fileName[ MAX_PATH ] = { 0 };
		sprintf(
			fileName,
			"data/script/direct/%s",
			file.GetString() );

		if( 0 == strcmpi( "data/script/direct/null", fileName ) )
		{
			continue;
		}

		CEffectDesc* const existedEffectDesc = existedEffectDescManager->GetEffectDesc( effectIndex );

		if( existedEffectDesc )
		{
			if( 0 == strcmpi( fileName, existedEffectDesc->GetFileName() ) )	//이펙트 이름이 디폴트와 같으면 로딩안하고 통과
			{
				continue;
			}
		}
		
		CEffectDesc* effectDesc = mEffectDescHashTable.GetData( effectIndex );

		if( 0 == effectDesc )
		{
			effectDesc = new CEffectDesc;
			mEffectDescHashTable.Add( effectDesc, effectIndex );
		}

		effectDesc->SetData( fileName, effectKind );
	}
}

BOOL CEffectDescManager::GetEffect(int EffectDescNum,EFFECTPARAM* pParam,CEffect* pOutEffect,BOOL bWarning)
{
	CEffectDesc* effectDesc = mEffectDescHashTable.GetData( EffectDescNum );

#ifdef _GMTOOL_
	// 091106 ShinJS --- Beff 파일 재로드
	if( EFFECTMGR->IsUpdateBeffFile() )
	{
		if( effectDesc != NULL )
		{
			// 현재 선택된 beff파일 Load 설정
			effectDesc->SetLoadFalg( TRUE );

			// 현재 실행중인 모든 이펙트 제거..
			EFFECTMGR->RemoveAllEffect();
		}
	}
#endif

	return effectDesc ? effectDesc->GetEffect( pParam, pOutEffect ) : FALSE;
}


CEffectDesc* CEffectDescManager::GetEffectDesc(int EffectDescNum)
{
	return mEffectDescHashTable.GetData( EffectDescNum );
}
