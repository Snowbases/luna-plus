#include "stdafx.h"
#include "UiSettingManager.h"
#include "./Interface/cWindowManager.h"
#include "../[CC]Header/GameResourceManager.h"
#include "interface/cDialog.h"
#include "WindowIDEnum.h"
#include "QuickSlotDlg.h"
#include "MHAudioManager.h"

GLOBALTON(cUiSettingManager)

cUiSettingManager::cUiSettingManager(void)
{
	LoadSettingInfo();
}

cUiSettingManager::~cUiSettingManager(void)
{}

// 091207 ONS 활성화 상태정보 처리 추가
void cUiSettingManager::SetSettingList( DWORD dwWindowID, BOOL bIsSavePos, BOOL bIsSaveActivity )
{
	if( !dwWindowID )
		return;

	// UI세팅상태 테이블을 설정한다.
	stUiSettingState pState;
	ZeroMemory(&pState, sizeof(stUiSettingState));
	pState.dwID = dwWindowID;
	pState.bSavePosition = bIsSavePos;
	pState.bSaveActivity = bIsSaveActivity;
	m_UiSetting.push_back(pState);

}

void cUiSettingManager::ResetUiSettingInfo()
{
	if(m_UiSetting.size() > m_UiSettingMap.size())
	{
		// 새로운 UI세팅이 추가될 경우 세팅정보를 추가한다.
		std::vector<stUiSettingState>::iterator iter;
		for( iter  = m_UiSetting.begin(); iter != m_UiSetting.end(); ++iter )
		{
			stUiSettingState pState = *iter;
			
			const stSettingProperty* pProperty = GetUiSettingInfo( pState.dwID );
			if( pProperty )
				continue;

			cDialog* dlg = WINDOWMGR->GetWindowForID( pState.dwID );
			if( !dlg )
				continue;
			
			// 기존의 세팅정보에 포함되지 않으면 추가한다.
			stUiSettingInfo stInfo;
			ZeroMemory(&stInfo, sizeof(stUiSettingInfo));

			stInfo.dwID = pState.dwID;
			if(pState.bSavePosition)
			{
				stInfo.property.vPos.x = dlg->GetAbsX();
				stInfo.property.vPos.y = dlg->GetAbsY();
			}
			if(pState.bSaveActivity)
			{
				stInfo.property.bActivity = dlg->IsActive();
			}
			AddSettingInfo( &stInfo );
		}		
	}
	else
	{
		// UI세팅을 제거했을경우 세팅정보를 삭제한다.
		std::vector<stUiSettingInfo> vTempInfo;
		std::vector<stUiSettingState>::iterator iter;
		for( iter  = m_UiSetting.begin(); iter != m_UiSetting.end(); ++iter )
		{
			stUiSettingState pState = *iter;
			const stSettingProperty* pProperty = GetUiSettingInfo( pState.dwID );
			if( pProperty )
			{
				// 해당 세팅정보가 존재하면 복사한다.
				stUiSettingInfo stInfo;
				ZeroMemory(&stInfo, sizeof(stUiSettingInfo));

				stInfo.dwID = pState.dwID;
				stInfo.property.bActivity = pProperty->bActivity;
				stInfo.property.vPos.x = pProperty->vPos.x;
				stInfo.property.vPos.y = pProperty->vPos.y;

				vTempInfo.push_back(stInfo);
			}
		}
		m_UiSettingMap.clear();

		for(int i = 0; i< (int)vTempInfo.size(); i++)
		{
			m_UiSettingMap[vTempInfo[i].dwID] = vTempInfo[i].property;
		}
		vTempInfo.clear();
	}
}

void cUiSettingManager::ApplySettingInfo()
{
	// UI세팅정보에 변화가 있다면 세팅정보를 갱신한다.
	if(m_UiSetting.size() != m_UiSettingMap.size())
	{
		ResetUiSettingInfo();
	}

	// LUJ, 인터페이스를 복구하는 동안, 효과음이 나오지 않도록 한다.
	//		복구 후 원래 음량으로 되돌린다.
	const float soundVolume = AUDIOMGR->GetSoundVolume(
		1);
	AUDIOMGR->SetSoundVolume(
		0);

	for(std::vector<stUiSettingState>::iterator iter  = m_UiSetting.begin();
		iter != m_UiSetting.end();
		++iter)
	{
		stUiSettingState pState = *iter;
		cDialog* dlg = WINDOWMGR->GetWindowForID( pState.dwID );
		if( NULL == dlg )
			continue;
		
		// 091207 ONS 활성화 상태정보 처리 추가
		const stSettingProperty* pProperty = GetUiSettingInfo( pState.dwID );
		if( NULL == pProperty )
			continue;
		
		if(pState.bSavePosition)
		{
			VECTOR2 vPos = {0};
			// 100510 ONS 확장 퀵슬롯의 경우 비율로 위치를 계산할경우 해상도가 변경됨에따라 기본퀵슬롯과 거리가 벌어지게
			// 되므로 기본퀵슬롯 위치를 기준으로 절대위치를 지정한다.
			if( pState.dwID == QI2_QUICKSLOTDLG )
			{
				cQuickSlotDlg* dialog = ( cQuickSlotDlg* )WINDOWMGR->GetWindowForID( QI1_QUICKSLOTDLG );
				if( !dialog )
				{
					continue;
				}

				DWORD dwQuickSlotHeight = dlg->GetHeight();
				vPos.x = dialog->GetAbsX();
				vPos.y = dialog->GetAbsY() - dwQuickSlotHeight;
			}
			else
			{
				// 100127 ONS 해상도변경에 따라 다이얼로그의 위치를 변경시킨다.
				// 해상도가 줄어들었을 경우 화면밖에 벗어난 다이얼로그의 위치를 보정한다.
				const DISPLAY_INFO& dispInfo = GAMERESRCMNGR->GetResolution();
				// 저장된 다이얼로그 X좌표가 해상도보다 클경우 보정한다.
				if(pProperty->vPos.x + dlg->GetWidth() > dispInfo.dwWidth)
				{
					vPos.x = float(dispInfo.dwWidth - dlg->GetWidth());
				}
				else
				{
					vPos.x = pProperty->vPos.x;
				}

				// 저장된 다이얼로그 Y좌표가 해상도보다 클경우 보정한다.
				if(pProperty->vPos.y + dlg->GetHeight() > dispInfo.dwHeight)
				{
					vPos.y = float(dispInfo.dwHeight - dlg->GetHeight());
				}
				else
				{
					vPos.y = pProperty->vPos.y;
				}
			}

			dlg->SetAbsXY((LONG)vPos.x, (LONG)vPos.y);
		}
		if(pState.bSaveActivity)
		{
			dlg->SetActive(pProperty->bActivity);
		}
	}

	AUDIOMGR->SetSoundVolume(
		soundVolume);
}

void cUiSettingManager::UpdateSettingInfo()
{
	// 맵에서 나갈때 변경된 UI정보를 갱신한다.
	std::vector<stUiSettingState>::iterator iter;
	for( iter  = m_UiSetting.begin(); iter != m_UiSetting.end(); ++iter )
	{
		stUiSettingState pState = *iter;
		
		cDialog* dlg = WINDOWMGR->GetWindowForID( pState.dwID );
		if( NULL == dlg )
			continue;
		
		if(pState.bSavePosition)
		{
			SetUiPosition( pState.dwID, dlg->GetAbsX(), dlg->GetAbsY() );
		}
		// 091207 ONS 활성화 상태정보 처리 추가
		if(pState.bSaveActivity)
		{
			SetUiCurActivity(pState.dwID, dlg->IsActive());
		}
	}
	if( !m_UiSetting.empty() )
	{
		m_UiSetting.clear();
	}
}

// 100304 ONS 해상도를 변경함에따라 다이얼로그의 위치가 비정상적으로 표시되는 문제를 해결하기위해
// 게임화면을 4분면으로 나누어 각각의 모서리를 기준으로 다이얼로그를 위치시키도록 수정.
void cUiSettingManager::LoadSettingInfo()
{
	// 화면을 4등분한 영역을 설정한다.
	SetScreenRect();

	// 게임 기동시 UI세팅정보를 로드한다.
	FILE* const file = fopen( "./system/UiSetting.txt", "rt");
	if( !file )	return;

	const char* delimit = " \n\t=,~";
	const char* markBegin = "{";
	const char* markEnd = "}";
	const BYTE	LOAD_SUCCESS = 5;
	    	
	DWORD		dwOpenCnt = 0;
	DWORD		dwWindowID = 0;
	DWORD		dwWidth = 0;
	DWORD		dwHeight = 0;
	BOOL		bIsComment = FALSE;
	BOOL		bIsDlgBlock = FALSE;
	BYTE		byActivity = FALSE;
	BYTE		byLoadCount = 0;
	VECTOR2		posRate = {0};
	eDispSector sector = eDispSector_None;

	while(0 == feof( file ))
	{
		char buff[ MAX_PATH ] = {0,};
		fgets( buff, sizeof( buff ) / sizeof( *buff ), file );

		char* token = strtok( buff, delimit );
		if( ! token )
		{
			continue;
		}
		else if( bIsComment )
		{
			continue;
		}
		else if( ! stricmp( "DLG", token ) )
		{
			bIsDlgBlock = TRUE;
			byLoadCount = 0;
		}
		else if( ! strnicmp( markBegin, token, strlen( markBegin ) ) )
		{
			++dwOpenCnt;
		}
		else if( ! strnicmp( markEnd, token, strlen( markEnd ) ) )
		{
			--dwOpenCnt;
			bIsDlgBlock = FALSE;
		}
		else if( !bIsDlgBlock )
		{
			continue;
		}

		// 다이얼로그 세팅정보를 읽어들인다.
        if( stricmp( token, "ID" ) == 0 )
		{
			token = strtok( 0, delimit );
			if( token )
			{
				dwWindowID = (DWORD)atoi( token );
				byLoadCount++;
			}
		}
		else if( stricmp( token, "Sector" ) == 0 )
		{
			token = strtok( 0, delimit );
			if( token )
			{
				sector = (eDispSector)atoi( token );
				byLoadCount++;
			}
		}
		else if( stricmp( token, "Rate" ) == 0 )
		{
			token = strtok( 0, delimit );
			if( token )
			{
				posRate.x = (float)atof( token );
				if( posRate.x < 0.05f )	
					posRate.x = 0.0f;

				token = strtok( 0, delimit );
				if( token )
				{
					posRate.y = (float)atof( token );
					if( posRate.y < 0.05f )	
						posRate.y = 0.0f;

					byLoadCount++;
				}
            }
		}
		else if( stricmp( token, "Size" ) == 0 )
		{
			token = strtok( 0, delimit );
			if( token )
			{
				dwWidth = (DWORD)atoi( token );

				token = strtok( 0, delimit );
				if( token )
				{
					dwHeight = (DWORD)atoi( token );
					byLoadCount++;
				}
			}
		}
		else if( stricmp( token, "Activity" ) == 0 )
		{
			token = strtok( 0, delimit );
			if( token )
			{
				byActivity = (BYTE)atoi( token );
				byLoadCount++;
			}
		}

		// 로드항목의 갯수를 체크한 후 저장한다.
		if( LOAD_SUCCESS == byLoadCount )
		{
			stUiSettingInfo stInfo;
			ZeroMemory(&stInfo, sizeof(stUiSettingInfo));

			stInfo.dwID = dwWindowID;
			stInfo.property.bActivity = byActivity;

			// 화면의 4꼭지점을 기준으로 다이얼로그의 위치 절대좌표로 설정한다.	
			const DISPLAY_INFO& dispInfo = GAMERESRCMNGR->GetResolution();
			switch(sector)
			{
			case eDispSector_1:
				{
					stInfo.property.vPos.x = m_screenRect[sector].left + ( dispInfo.dwWidth/2 * posRate.x );
					stInfo.property.vPos.y = m_screenRect[sector].top + ( dispInfo.dwHeight/2 * posRate.y );
				}
				break;
			case eDispSector_2:
				{
					stInfo.property.vPos.x = m_screenRect[sector].right - ( dispInfo.dwWidth/2 * posRate.x ) - dwWidth;
					stInfo.property.vPos.y = m_screenRect[sector].top + ( dispInfo.dwHeight/2 * posRate.y );
				}
				break;
			case eDispSector_3:
				{
					stInfo.property.vPos.x = m_screenRect[sector].left + ( dispInfo.dwWidth/2 * posRate.x );
					stInfo.property.vPos.y = m_screenRect[sector].bottom - ( dispInfo.dwHeight/2 * posRate.y ) - dwHeight;
				}
				break;
			case eDispSector_4:
				{
					stInfo.property.vPos.x = m_screenRect[sector].right - ( dispInfo.dwWidth/2 * posRate.x ) - dwWidth;
					stInfo.property.vPos.y = m_screenRect[sector].bottom - ( dispInfo.dwHeight/2 * posRate.y ) - dwHeight;
				}
				break;
			}

			AddSettingInfo(&stInfo);
		}
	}
	fclose(file);
}

// 100304 ONS 해상도를 변경함에따라 다이얼로그의 위치가 비정상적으로 표시되는 문제를 해결하기위해
// 게임화면을 4분면으로 나누어 각각의 모서리를 기준으로 다이얼로그를 위치시키도록 수정.
void cUiSettingManager::SaveSettingInfo()
{
	// 게임 종료시 UI세팅정보를 저장한다.
	FILE* const file = fopen( "./system/UiSetting.txt", "wt");
	if( !file )		return;
    
	stdext::hash_map< DWORD, stSettingProperty >::iterator it;
	for( it  = m_UiSettingMap.begin(); it != m_UiSettingMap.end(); ++it )
	{
		DWORD dwWindowID = (it->first);
		const stSettingProperty* pProperty = GetUiSettingInfo( dwWindowID );
		if( NULL == pProperty )
		{
			continue;
		}

		stUiSettingInfo stInfo;
		ZeroMemory(&stInfo, sizeof(stUiSettingInfo));

		stInfo.dwID = dwWindowID;
		memcpy(&stInfo.property, pProperty, sizeof(stSettingProperty));

		// 다이얼로그가 위치한 섹터와 해당 섹터에서의 위치를 비율로 구한다.
		const eDispSector Sector = (eDispSector)GetDisplaySector( pProperty );
		if( eDispSector_None != Sector )
		{
			const VECTOR2&	  PosRate = SetPositionRate( pProperty, Sector );
			fprintf(file, "DLG\n{");
			fprintf(file, "\n\tID = %d", dwWindowID);
			fprintf(file, "\n\tSector = %d", int(Sector));
			fprintf(file, "\n\tRate = %f, %f", PosRate.x, PosRate.y);
			fprintf(file, "\n\tSize = %d, %d", pProperty->dwWidth, pProperty->dwHeight);
			fprintf(file, "\n\tActivity = %d", pProperty->bActivity);
			fprintf(file, "\n}\n");
		}
	}
	fclose(file);
}

void cUiSettingManager::AddSettingInfo( stUiSettingInfo* pInfo )
{
	// UI설정 정보(위치, 활성화 속성)를 추가한다.
	stSettingProperty* pProperty = NULL;
	pProperty = &( m_UiSettingMap[ pInfo->dwID ] );
	pProperty->vPos.x = pInfo->property.vPos.x;
	pProperty->vPos.y = pInfo->property.vPos.y;
	pProperty->bActivity = 	pInfo->property.bActivity;
}

const stSettingProperty* cUiSettingManager::GetUiSettingInfo(DWORD dwWindowID)
{
	// 해당 윈도우의 UI설정 정보(위치, 활성화 속성)를 가져온다.
	UiSettingMap::const_iterator it = m_UiSettingMap.find(dwWindowID);
	return m_UiSettingMap.end() == it ? 0 : &( it->second );
}

void cUiSettingManager::SetUiPosition( DWORD dwWindowID, float fPosX, float fPosY )
{
	// 변경된 UI위치정보를 저장한다.
	UiSettingMap::iterator it = m_UiSettingMap.find(dwWindowID);
	stSettingProperty* pProperty = &(it->second);
	if( NULL == pProperty )
		return;

	pProperty->vPos.x = fPosX;
	pProperty->vPos.y = fPosY;

	cDialog* dlg = WINDOWMGR->GetWindowForID( dwWindowID );
	if( dlg )
	{
		pProperty->dwWidth = dlg->GetWidth();
		pProperty->dwHeight = dlg->GetHeight();
	}
}

void cUiSettingManager::SetUiCurActivity( DWORD dwWindowID, BOOL bCurActivity )
{
	// 변경된 UI활성화상태 정보를 저장한다.
	UiSettingMap::iterator it = m_UiSettingMap.find(dwWindowID);
	stSettingProperty* pProperty = &(it->second);
	if( NULL == pProperty )
		return;
	pProperty->bActivity = bCurActivity;
}

// 100304 ONS 해당 다이얼로그가 위치하는 부분을 구한다.(1/4, 2/4, 3/4, 4/4분면)
int cUiSettingManager::GetDisplaySector( const stSettingProperty* pProperty )
{
	for( BYTE sector = eDispSector_1; sector < eDispSector_None; sector++ )
	{
		if( m_screenRect[sector].left	<= pProperty->vPos.x	&& 
			m_screenRect[sector].right	>= pProperty->vPos.x	&&
			m_screenRect[sector].top	<= pProperty->vPos.y	&&
			m_screenRect[sector].bottom >= pProperty->vPos.y )
				return sector;
	}
	return eDispSector_None;
}

// 100304 ONS 다이얼로그의 위치를 각 모서리를 기준으로 비율로 설정한다.
VECTOR2 cUiSettingManager::SetPositionRate( const stSettingProperty* pProperty, eDispSector Sector )
{
	VECTOR2 posRate = {0};

	switch( Sector )
	{
	case eDispSector_1:
		{
			posRate.x = (pProperty->vPos.x - m_screenRect[Sector].left) / (m_screenRect[Sector].right - m_screenRect[Sector].left);
			posRate.y = (pProperty->vPos.y - m_screenRect[Sector].top) / (m_screenRect[Sector].bottom - m_screenRect[Sector].top);
		}
		break;
	case eDispSector_2:
		{
			posRate.x = (m_screenRect[Sector].right - (pProperty->vPos.x + pProperty->dwWidth)) / (m_screenRect[Sector].right - m_screenRect[Sector].left);
			posRate.y = (pProperty->vPos.y - m_screenRect[Sector].top) / (m_screenRect[Sector].bottom - m_screenRect[Sector].top);
		}
		break;
	case eDispSector_3:
		{
			posRate.x = (pProperty->vPos.x - m_screenRect[Sector].left) / (m_screenRect[Sector].right - m_screenRect[Sector].left);
			posRate.y = (m_screenRect[Sector].bottom - (pProperty->vPos.y + pProperty->dwHeight)) / (m_screenRect[Sector].bottom - m_screenRect[Sector].top);
		}
		break;
	case eDispSector_4:
		{
			posRate.x = (m_screenRect[Sector].right - (pProperty->vPos.x + pProperty->dwWidth)) / (m_screenRect[Sector].right - m_screenRect[Sector].left);
			posRate.y = (m_screenRect[Sector].bottom - (pProperty->vPos.y + pProperty->dwHeight)) / (m_screenRect[Sector].bottom - m_screenRect[Sector].top);
		}
		break;
	}

	// 화면을 벗어나는 경우 위치를 보정한다.
	if(posRate.x < 0)
	{
		posRate.x = 0.0f;
	}
	if(posRate.y < 0)
	{
		posRate.y = 0.0f;
	}

	return posRate;
}

// 100304 ONS 화면을 4등분한 영역을 설정한다.
void cUiSettingManager::SetScreenRect()
{
	const DISPLAY_INFO& dispInfo = GAMERESRCMNGR->GetResolution();	

	m_screenRect[eDispSector_1].left = 0;
	m_screenRect[eDispSector_1].top = 0;
	m_screenRect[eDispSector_1].right = dispInfo.dwWidth/2;
	m_screenRect[eDispSector_1].bottom = dispInfo.dwHeight/2;
	
	m_screenRect[eDispSector_2].left = dispInfo.dwWidth/2+1;
	m_screenRect[eDispSector_2].top = 0;
	m_screenRect[eDispSector_2].right = dispInfo.dwWidth;
	m_screenRect[eDispSector_2].bottom = dispInfo.dwHeight/2;

	m_screenRect[eDispSector_3].left = 0;
	m_screenRect[eDispSector_3].top = dispInfo.dwHeight/2+1;
	m_screenRect[eDispSector_3].right = dispInfo.dwWidth/2;
	m_screenRect[eDispSector_3].bottom = dispInfo.dwHeight;	

	m_screenRect[eDispSector_4].left = dispInfo.dwWidth/2+1;
	m_screenRect[eDispSector_4].top = dispInfo.dwHeight/2+1;
	m_screenRect[eDispSector_4].right = dispInfo.dwWidth;
	m_screenRect[eDispSector_4].bottom = dispInfo.dwHeight;	
}