// WeatherManager.cpp: implementation of the CWeatherManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WeatherManager.h"
#include "Hero.h"
#include "ObjectManager.h"
#include "MHfile.h"
#include "StatusIconDlg.h"
#include "MHCamera.h"
#include "ChatManager.h"
#include "WeatherDialog.h"
#include "GameIn.h"
#include "./interface/cFont.h"

#ifdef _TESTCLIENT_
#include "TSToolManager.h"
#endif

GLOBALTON(CWeatherManager);

CWeatherManager::CWeatherManager()
{
	m_bEffectOn = FALSE;
	m_CurEffectWeatherHashCode = 0;
	m_wIntensity = 0;

	LoadScript();
}

CWeatherManager::~CWeatherManager()
{
	Release();
}

void CWeatherManager::Init()
{
#ifdef _GMTOOL_
	// GM-Tool 만 맵이동시 스크립트를 다시 읽을수 있도록 한다.
	LoadScript();

	m_bDrawDebugInfo = FALSE;
#endif	
}

void CWeatherManager::LoadScript()
{
	Release();

	enum Block
	{
		BlockNone,
		BlockWeather,
	}
	blockType = BlockNone;
	
	DWORD dwOpenCnt = 0;
	BOOL bIsComment = FALSE;

	DWORD dwCurWeatherHashCode = 0;

	CMHFile file;
	file.Init( "./System/Resource/WeatherEffect.bin", "rb" );
	while( ! file.IsEOF() )
	{
		char txt[ MAX_PATH ] = {0,};
		file.GetLine( txt, MAX_PATH );

		int txtLen = _tcslen( txt );

		// 중간 주석 제거
		for( int i=0 ; i<txtLen-1 ; ++i )
		{
			if( txt[i] == '/' && txt[i+1] == '/' )
			{
				txt[i] = 0;
				break;
			}
			else if( txt[i] == '/' && txt[i+1] == '*' )
			{
				txt[i] = 0;
				bIsComment = TRUE;
			}
			else if( txt[i] == '*' && txt[i+1] == '/' )
			{
				txt[i] = ' ';
				txt[i+1] = ' ';
				bIsComment = FALSE;
			}
			else if( bIsComment )
			{
				txt[i] = ' ';
			}
		}

		char buff[ MAX_PATH ] = {0,};
		SafeStrCpy( buff, txt, MAX_PATH );
		const char* delimit = " \n\t=,~()";
		const char* markBegin = "{";
		const char* markEnd = "}";
		char* token = strtok( buff, delimit );

		if( ! token )
		{
			continue;
		}
		else if( bIsComment )
		{
			continue;
		}
		else if( ! stricmp( "Weather", token ) )
		{
			blockType = BlockWeather;
			token = strtok( 0, delimit );
			if( !token )		break;
			dwCurWeatherHashCode = GetHashCodeFromTxt( token );
			stWeatherEffectData& data = m_WeatherEffectDataMap[ dwCurWeatherHashCode ];
			SafeStrCpy( data.hashCodeTxt, token, sizeof(data.hashCodeTxt) );
			data.dwHashCode = dwCurWeatherHashCode;
		}
		else if( ! strnicmp( markBegin, token, strlen( markBegin ) ) )
		{
			++dwOpenCnt;
		}
		else if( ! strnicmp( markEnd, token, strlen( markEnd ) ) )
		{
			--dwOpenCnt;

			switch( blockType )
			{
			case BlockWeather:
				{
					blockType = BlockNone;
					dwCurWeatherHashCode = 0;
				}
				break;
			}
		}

		switch( blockType )
		{
		case BlockWeather:
			{
				if( stricmp( token, "distance" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )		break;
					stWeatherEffectData& data = m_WeatherEffectDataMap[ dwCurWeatherHashCode ];
					data.fRenderValidDist = float( atof( token ) );
				}
				else if( stricmp( token, "createTime" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )		break;
					stWeatherEffectData& data = m_WeatherEffectDataMap[ dwCurWeatherHashCode ];
					data.fCreateTime = float( atof( token ) );
				}
				else if( stricmp( token, "renderTime" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )		break;
					stWeatherEffectData& data = m_WeatherEffectDataMap[ dwCurWeatherHashCode ];
					data.fRenderTime = float( atof( token ) );
				}
				else if( stricmp( token, "deleteTime" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )		break;
					stWeatherEffectData& data = m_WeatherEffectDataMap[ dwCurWeatherHashCode ];
					data.fDeleteTime = float( atof( token ) );
				}
				else if( stricmp( token, "file" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )		break;
					stWeatherEffectData& data = m_WeatherEffectDataMap[ dwCurWeatherHashCode ];
					SafeStrCpy( data.effectFileName, token, MAX_PATH );
				}
				else if( stricmp( token, "maxEffectCnt" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )		break;
					stWeatherEffectData& data = m_WeatherEffectDataMap[ dwCurWeatherHashCode ];
					data.wMaxObjectCnt = WORD( atoi( token ) );
				}
				else if( stricmp( token, "rotate" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )		break;

					stWeatherEffectData& data = m_WeatherEffectDataMap[ dwCurWeatherHashCode ];

					if( stricmp( token, "Random" ) == 0 )
					{
						data.nRotate = eWeatherEffectRotateType_Random;
					}
					else if( stricmp( token, "Player" ) == 0 )
					{
						data.nRotate = eWeatherEffectRotateType_PlayerLook;
					}
					else if( stricmp( token, "Camera" ) == 0 )
					{
						data.nRotate = eWeatherEffectRotateType_CameraLook;
					}
					else
					{
						data.nRotate = atoi( token );
					}
				}
				else if( stricmp( token, "createPosition" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )		break;

					stWeatherEffectData& data = m_WeatherEffectDataMap[ dwCurWeatherHashCode ];

					if( stricmp( token, "Camera" ) == 0 )
					{
						data.eCreatePosType = eWeatherEffectCreatePosType_Camera;
					}
					else if( stricmp( token, "Player" ) == 0 )
					{
						data.eCreatePosType = eWeatherEffectCreatePosType_Player;
					}
				}
				else if( stricmp( token, "createDistance" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )		break;

					stWeatherEffectData& data = m_WeatherEffectDataMap[ dwCurWeatherHashCode ];
					data.fCreateDist = float( atof( token ) );
				}
				else if( stricmp( token, "createRange" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )		break;

					stWeatherEffectData& data = m_WeatherEffectDataMap[ dwCurWeatherHashCode ];
					data.fCreateRangeMin = float( atof( token ) );

					token = strtok( 0, delimit );
					if( !token )		break;
					data.fCreateRangeMax = float( atof( token ) );
				}
				else if( stricmp( token, "scaleRange" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )		break;

					stWeatherEffectData& data = m_WeatherEffectDataMap[ dwCurWeatherHashCode ];
					data.fScaleMin = float( atof( token ) );

					token = strtok( 0, delimit );
					if( !token )		break;
					data.fScaleMax = float( atof( token ) );
				}
				else if( stricmp( token, "createDelay" ) == 0 )
				{
					token = strtok( 0, delimit );
					if( !token )		break;

					stWeatherEffectData& data = m_WeatherEffectDataMap[ dwCurWeatherHashCode ];
					data.dwCreateDelay = DWORD( atoi( token ) );
				}
			}
			break;
		}
	}

#ifdef _GMTOOL_
	if( dwOpenCnt )
	{
		MessageBox( NULL, "{} Error! Check WeatherEffect.bin", NULL, NULL );
	}
#endif

	// 현재 날씨가 설정된 경우 실행시킨다.
	if( m_CurEffectWeatherHashCode )
	{
		EffectOn( m_CurEffectWeatherHashCode, m_wIntensity );
	}
}

void CWeatherManager::Release()
{ 
	WeatherEffectDataMap::iterator iterData;
	for( iterData = m_WeatherEffectDataMap.begin() ; iterData != m_WeatherEffectDataMap.end() ; ++iterData )
	{
		stWeatherEffectData& data = iterData->second;
		
		std::map<int, stWeatherEffectObject>::iterator iterObject;
		for( iterObject = data.objectMap.begin() ; iterObject != data.objectMap.end() ; ++iterObject )
		{
			stWeatherEffectObject& object = iterObject->second;
			object.effecObj.Release();
		}

		data.objectMap.clear();
	}

	m_WeatherEffectDataMap.clear();

	m_CreateEffectQue.clear();
	m_RenderEffectQue.clear();
	m_DeleteEffectQue.clear();
}

void CWeatherManager::Process()
{
	MHCAMERADESC* pCameraDesc = CAMERA->GetCameraDesc();
	VECTOR3 vecCheckPos = {0,};

	WeatherEffectDataMap::iterator iterData = m_WeatherEffectDataMap.find( m_CurEffectWeatherHashCode );
	if( iterData != m_WeatherEffectDataMap.end() )
	{
		stWeatherEffectData& data = iterData->second;

		switch( data.eCreatePosType )
		{
		case eWeatherEffectCreatePosType_Camera:
			{
				vecCheckPos = pCameraDesc->m_CameraPos + (pCameraDesc->m_CameraDir * data.fCreateDist);
			}
			break;
		case eWeatherEffectCreatePosType_Player:
			{
				if( !HERO )
					break;

				MATRIX4 matRot;
				SetRotationXMatrix( &matRot, HERO->GetAngle() );
				VECTOR3 vecDir = {0,};
				vecDir.z = 1.f;
				TransformVector3_VPTR2( &vecDir, &vecDir, &matRot, 1 );

				VECTOR3 vecHeroPos;
				HERO->GetPosition( &vecHeroPos );
				vecCheckPos = vecHeroPos + (vecDir * data.fCreateDist);
			}
			break;
		}
	}


	WeatherRenderStateQue::iterator iterQue = m_CreateEffectQue.begin();
	while( iterQue != m_CreateEffectQue.end() )
	{
		WeatherObjectKey& dataKey = *iterQue;
		WeatherEffectDataMap::iterator iterData = m_WeatherEffectDataMap.find( dataKey.first );
		if( iterData == m_WeatherEffectDataMap.end() )
		{
			++iterQue;
			continue;
		}

		stWeatherEffectData& data = iterData->second;
		stWeatherEffectObject& object = data.objectMap[ dataKey.second ];

		// 시간 경과에 대하여 Alpha 값 변경
		float fAlpha = (gCurTime - object.dwTime) / data.fCreateTime;

		// Create -> Render 상태로 변경
		if( fAlpha > 1.f )
		{
			object.eState = eWeatherEffectRenderState_Render;
			object.dwTime = DWORD( gCurTime + data.fRenderTime );
			object.effecObj.SetAlpha( 1.f );

			// Render Que에 저장
			m_RenderEffectQue.push_back( dataKey );

			// Create Que 제거
			iterQue = m_CreateEffectQue.erase( iterQue );

			continue;
		}

		// 변경된 Alpha 적용
		object.effecObj.SetAlpha( fAlpha );

		++iterQue;
	}

	iterQue = m_RenderEffectQue.begin();
	while( iterQue != m_RenderEffectQue.end() )
	{
		WeatherObjectKey& dataKey = *iterQue;
		WeatherEffectDataMap::iterator iterData = m_WeatherEffectDataMap.find( dataKey.first );
		if( iterData == m_WeatherEffectDataMap.end() )
		{
			++iterQue;
			continue;
		}

		stWeatherEffectData& data = iterData->second;
		stWeatherEffectObject& object = data.objectMap[ dataKey.second ];

		float fDist = CalcDistance( &vecCheckPos, &object.vecPos );

		// Render -> Delete 상태로 변경
		if( gCurTime > object.dwTime ||
			fDist > data.fRenderValidDist )
		{
			object.eState = eWeatherEffectRenderState_Delete;
			object.dwTime = gCurTime;

			// Delete Que에 저장
			m_DeleteEffectQue.push_back( dataKey );

			// Render Que 제거
			iterQue = m_RenderEffectQue.erase( iterQue );

			continue;
		}

		++iterQue;
	}

	iterQue = m_DeleteEffectQue.begin();
	while( iterQue != m_DeleteEffectQue.end() )
	{
		WeatherObjectKey& dataKey = *iterQue;
		WeatherEffectDataMap::iterator iterData = m_WeatherEffectDataMap.find( dataKey.first );
		if( iterData == m_WeatherEffectDataMap.end() )
		{
			++iterQue;
			continue;
		}

		stWeatherEffectData& data = iterData->second;
		stWeatherEffectObject& object = data.objectMap[ dataKey.second ];

		// 시간 경과에 대하여 Alpha 값 변경
		float fAlpha = 1.f - (gCurTime - object.dwTime) / data.fDeleteTime;

		// Delete -> None 상태로 변경
		if( fAlpha < 0.f )
		{
			object.eState = eWeatherEffectRenderState_None;
			object.effecObj.Hide();

			// UsableIndex 추가
			data.usableIndex.push_back( dataKey.second );

			// Delete Que 제거
			iterQue = m_DeleteEffectQue.erase( iterQue );

			continue;
		}

		// 변경된 Alpha 적용
		object.effecObj.SetAlpha( fAlpha );

		++iterQue;
	}

	if( ! m_bEffectOn || iterData == m_WeatherEffectDataMap.end() )
		return;

	// Effect 추가
	AddCreateStateEffect( iterData->second, vecCheckPos );
}

void CWeatherManager::AddCreateStateEffect( stWeatherEffectData& data, VECTOR3& vecCheckPos )
{
	int nRenderEffectCnt = m_CreateEffectQue.size() + m_RenderEffectQue.size();

	if( data.usableIndex.empty() ||
		data.dwValidCreateTime > gCurTime ||
		nRenderEffectCnt > m_wIntensity )
		return;

	int nUsableIndex = data.usableIndex.back();
	data.usableIndex.pop_back();

	stWeatherEffectObject& object = data.objectMap[ nUsableIndex ];

	// 크기 처리(소수점 두자리까지 유효하게 계산)
	VECTOR3 vecScale = {1.0f, 1.0f, 1.0f};
	if( data.fScaleMin < data.fScaleMax )
	{
		float fRandRange = data.fScaleMax - data.fScaleMin;
		float fScale = (rand() % (int)(fRandRange*100))/100.f + data.fScaleMin;
		vecScale.x = vecScale.y = vecScale.z = fScale;
	}

	// 회전 처리
	float fRad = 0.f;
	switch( data.nRotate )
	{
		// Random
	case eWeatherEffectRotateType_Random:
		{
			fRad = DEGTORAD( rand() % 360 );
		}
		break;

		// Player(HERO)가 바라보는 방향
	case eWeatherEffectRotateType_PlayerLook:
		{
			if( HERO )
				fRad = HERO->GetAngle();
		}
		break;

		// Camera 가 바라보는 방향
	case eWeatherEffectRotateType_CameraLook:
		{
			MHCAMERADESC* pCameraDesc = CAMERA->GetCameraDesc();
			fRad = pCameraDesc->m_AngleY.ToRad() + PI;
		}
		break;

		// 값이 설정된 경우
	default:
		{
			fRad = DEGTORAD( data.nRotate );
		}
		break;
	}

	// 위치 처리
	object.vecPos = vecCheckPos;
	if( data.fCreateRangeMin || data.fCreateRangeMax )
	{
		VECTOR3 vecDir={0,};
        vecDir.x = 1.f - ((float)(rand()%2000)/1000.f);
		vecDir.y = 1.f - ((float)(rand()%2000)/1000.f);
		vecDir.z = 1.f - ((float)(rand()%2000)/1000.f);
		Normalize( &vecDir, &vecDir );

		float fRandRange = data.fCreateRangeMax - data.fCreateRangeMin;
		float fScale = (rand() % (int)(fRandRange*100))/100.f + data.fCreateRangeMax;
		vecDir = vecDir * fScale;

		object.vecPos = object.vecPos + vecDir;
	}

	object.eState = eWeatherEffectRenderState_Create;
	object.dwTime = gCurTime;
	object.effecObj.Show();
	object.effecObj.SetAlpha( 0.f );
	object.effecObj.SetEngineToCurmotion();
	object.effecObj.SetEngObjPosition( &object.vecPos );
	object.effecObj.SetEngObjAngle( fRad );
	object.effecObj.SetScale( &vecScale );

	data.dwValidCreateTime = gCurTime + data.dwCreateDelay;

	m_CreateEffectQue.push_back( std::make_pair( m_CurEffectWeatherHashCode, nUsableIndex ) );
}

BOOL CWeatherManager::EffectOn( LPCTSTR weatherTxt, WORD wIntensity )
{
	DWORD dwHashCode = GetHashCodeFromTxt( weatherTxt );

	return EffectOn( dwHashCode, wIntensity );
}

BOOL CWeatherManager::EffectOn( DWORD dwWeatherHashCode, WORD wIntensity )
{
	WeatherEffectDataMap::iterator iterData = m_WeatherEffectDataMap.find( dwWeatherHashCode );
	if( iterData == m_WeatherEffectDataMap.end() )
		return FALSE;

	m_CurEffectWeatherHashCode = dwWeatherHashCode;
	m_bEffectOn = TRUE;
	m_wIntensity = wIntensity;

	// Engine Object가 생성되지 않은 경우
	stWeatherEffectData& data = iterData->second;
	if( data.wMaxObjectCnt > 0 && data.objectMap.size() == 0 )
	{
		for( int i=0 ; i < (int)data.wMaxObjectCnt ; ++i )
		{
			data.usableIndex.push_back( i );

			stWeatherEffectObject& object = data.objectMap[i];
			if( object.effecObj.Init( data.effectFileName, NULL, eEngineObjectType_GameSystemObject, (WORD)0, (WORD)-1 ) )
			{
				object.effecObj.SetRepeat( TRUE );
			}
		}
	}

#ifdef _GMTOOL_
	if( wIntensity > data.wMaxObjectCnt )
	{
		MessageBox( NULL, "Weather Effect Intensity must be lower than MaxObjectCount!", NULL, NULL );
	}
#endif

	return TRUE;
}

void CWeatherManager::EffectOff()
{
	m_CurEffectWeatherHashCode = 0;
	m_bEffectOn = FALSE;
}

void CWeatherManager::SetEffectCreateTime( LPCTSTR weatherTxt, float fTime )
{
	DWORD dwHashCode = GetHashCodeFromTxt( weatherTxt );
	SetEffectCreateTime( dwHashCode, fTime );
}
void CWeatherManager::SetEffectRenderTime( LPCTSTR weatherTxt, float fTime )
{
	DWORD dwHashCode = GetHashCodeFromTxt( weatherTxt );
	SetEffectRenderTime( dwHashCode, fTime );
}
void CWeatherManager::SetEffectDeleteTime( LPCTSTR weatherTxt, float fTime )
{
	DWORD dwHashCode = GetHashCodeFromTxt( weatherTxt );
	SetEffectDeleteTime( dwHashCode, fTime );
}

void CWeatherManager::SetEffectCreateTime( DWORD dwWeatherHashCode, float fTime )
{
	WeatherEffectDataMap::iterator iterData = m_WeatherEffectDataMap.find( m_CurEffectWeatherHashCode );
	if( iterData == m_WeatherEffectDataMap.end() )
		return;

	stWeatherEffectData& data = iterData->second;
	data.fCreateTime = fTime;
}
void CWeatherManager::SetEffectRenderTime( DWORD dwWeatherHashCode, float fTime )
{
	WeatherEffectDataMap::iterator iterData = m_WeatherEffectDataMap.find( m_CurEffectWeatherHashCode );
	if( iterData == m_WeatherEffectDataMap.end() )
		return;

	stWeatherEffectData& data = iterData->second;
	data.fRenderTime = fTime;
}
void CWeatherManager::SetEffectDeleteTime( DWORD dwWeatherHashCode, float fTime )
{
	WeatherEffectDataMap::iterator iterData = m_WeatherEffectDataMap.find( m_CurEffectWeatherHashCode );
	if( iterData == m_WeatherEffectDataMap.end() )
		return;

	stWeatherEffectData& data = iterData->second;
	data.fDeleteTime = fTime;
}

#if defined(_GMTOOL_) || defined(_TESTCLIENT_)
#include "GMToolManager.h"
void CWeatherManager::RenderDebugState()
{
	if( !m_bDrawDebugInfo )
		return;

	// 현재 Weather 정보를 표시한다.
	char txt[ 512 ]={0,};
	RECT rc;
	rc.left = 220;
	rc.right = 1040;

	int nRow = 5;
	rc.top = 20*nRow++;	rc.bottom = rc.top + 20;
	sprintf( txt, "--------------------------------------------" );
	CFONT_OBJ->RenderFont( 0, txt, strlen(txt), &rc, RGBA_MAKE(255,255,255,200));

	rc.top = 20*nRow++;	rc.bottom = rc.top + 20;
	sprintf( txt, "               날씨 상태 정보" );
	CFONT_OBJ->RenderFont( 0, txt, strlen(txt), &rc, RGBA_MAKE(255,255,255,200));

	rc.top = 20*nRow++;	rc.bottom = rc.top + 20;
	sprintf( txt, "--------------------------------------------" );
	CFONT_OBJ->RenderFont( 0, txt, strlen(txt), &rc, RGBA_MAKE(255,255,255,200));

	WeatherEffectDataMap::iterator iterData = m_WeatherEffectDataMap.find( m_CurEffectWeatherHashCode );
	if( iterData != m_WeatherEffectDataMap.end() )
	{
		stWeatherEffectData& data = iterData->second;

		rc.top = 20*nRow++;	rc.bottom = rc.top + 20;
		sprintf( txt, "현재 설정된 날씨 : %s", data.hashCodeTxt );
		CFONT_OBJ->RenderFont( 0, txt, strlen(txt), &rc, RGBA_MAKE(255,255,255,200));

		rc.top = 20*nRow++;	rc.bottom = rc.top + 20;
		sprintf( txt, "현재 설정된 강도 : %d", m_wIntensity );
		CFONT_OBJ->RenderFont( 0, txt, strlen(txt), &rc, RGBA_MAKE(255,255,255,200));

		rc.top = 20*nRow++;	rc.bottom = rc.top + 20;
		sprintf( txt, "사용가능한 남은 Object 개수 : %d\n", data.usableIndex.size() );
		CFONT_OBJ->RenderFont( 0, txt, strlen(txt), &rc, RGBA_MAKE(255,255,255,200));

		rc.top = 20*nRow++;	rc.bottom = rc.top + 20;
		sprintf( txt, "Create State Object 개수 : %d\n", m_CreateEffectQue.size() );
		CFONT_OBJ->RenderFont( 0, txt, strlen(txt), &rc, RGBA_MAKE(255,255,255,200));

		rc.top = 20*nRow++;	rc.bottom = rc.top + 20;
		sprintf( txt, "Render State Object 개수 : %d\n", m_RenderEffectQue.size() );
		CFONT_OBJ->RenderFont( 0, txt, strlen(txt), &rc, RGBA_MAKE(255,255,255,200));

		rc.top = 20*nRow++;	rc.bottom = rc.top + 20;
		sprintf( txt, "Delete State Object 개수 : %d\n", m_DeleteEffectQue.size() );
		CFONT_OBJ->RenderFont( 0, txt, strlen(txt), &rc, RGBA_MAKE(255,255,255,200));
	}

	rc.top = 20*nRow++;	rc.bottom = rc.top + 20;
	sprintf( txt, "--------------------------------------------" );
	CFONT_OBJ->RenderFont( 0, txt, strlen(txt), &rc, RGBA_MAKE(255,255,255,200));
}
#endif

#ifdef _GMTOOL_
void CWeatherManager::AddGMToolMenu()
{
	WeatherEffectDataMap::iterator iterData;
	for( iterData = m_WeatherEffectDataMap.begin() ; iterData != m_WeatherEffectDataMap.end() ; ++iterData )
	{
		stWeatherEffectData& data = iterData->second;
		GMTOOLMGR->AddWeatherType( data.hashCodeTxt );
	}
}
#elif defined _TESTCLIENT_
void CWeatherManager::AddTSToolMenu()
{
	WeatherEffectDataMap::iterator iterData;
	for( iterData = m_WeatherEffectDataMap.begin() ; iterData != m_WeatherEffectDataMap.end() ; ++iterData )
	{
		stWeatherEffectData& data = iterData->second;
		TSTOOLMGR->AddWeatherType( data.hashCodeTxt );
	}
}
#endif