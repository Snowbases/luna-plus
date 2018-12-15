// WeatherManager.h: interface for the CWeatherManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WEATHERMANAGER_H__5EB34DB4_AE8E_4654_B1FB_2A4A701DAFB1__INCLUDED_)
#define AFX_WEATHERMANAGER_H__5EB34DB4_AE8E_4654_B1FB_2A4A701DAFB1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ENGINE\EngineEffect.h"

#define WEATHERMGR	USINGTON(CWeatherManager)

class CWeatherManager
{
	enum eWeatherEffectRenderState{
		eWeatherEffectRenderState_None,
		eWeatherEffectRenderState_Render,		// Effect Render 상태
		eWeatherEffectRenderState_Create,		// Effect Create 상태, 점점 뚜렷해지는 상태
		eWeatherEffectRenderState_Delete,		// Effect Delete 상태, 점점 흐려지는 상태
	};

	// Effect 설정시 Rotate Type, 0 이상은 각도로 고정값 설정
	enum eWeatherEffectRotateType{
		eWeatherEffectRotateType_Random = -1,
		eWeatherEffectRotateType_PlayerLook = -2,
		eWeatherEffectRotateType_CameraLook = -3,
	};

	// Effect 설정시 거리 Type,
	enum eWeatherEffectCreatePosType{
		eWeatherEffectCreatePosType_Camera,
		eWeatherEffectCreatePosType_Player,
	};

	struct stWeatherEffectObject{
		stWeatherEffectObject()
		{
			vecPos.x = vecPos.y = vecPos.z = 0.0f;
			eState = eWeatherEffectRenderState_None;
			dwTime = 0;
		}

		VECTOR3 vecPos;							// Effect의 위치
		eWeatherEffectRenderState eState;		// 현재 상태
		DWORD dwTime;							// 상태 변경 시각
		CEngineEffect effecObj;
	};

	struct stWeatherEffectData{

		stWeatherEffectData()
		{
			dwHashCode = 0;
			ZeroMemory( hashCodeTxt, MAX_PATH );
			ZeroMemory( effectFileName, MAX_PATH );
			wMaxObjectCnt = 0;
			fCreateTime = 200.f;
			fRenderTime = 5000.f;
			fDeleteTime = 1000.f;
			fRenderValidDist = 0.f;
			nRotate = 0;
			eCreatePosType = eWeatherEffectCreatePosType_Camera;
			fCreateDist = 0;
			fCreateRangeMin = fCreateRangeMax = 0.f;
			fScaleMin = fScaleMax = 0.f;
			dwValidCreateTime = 0;
			dwCreateDelay = 0;
			bBillboard = FALSE;
		}

		std::map<int, stWeatherEffectObject> objectMap;
		std::deque<int> usableIndex;

		DWORD dwHashCode;
		char hashCodeTxt[MAX_PATH];

		char effectFileName[MAX_PATH];	// Effect File Name
		WORD wMaxObjectCnt;				// EngineObject 개수

		float fCreateTime;				// Create State Render Time
		float fRenderTime;				// Render State Render Time
		float fDeleteTime;				// Delete State Render Time

		float fRenderValidDist;			// Render 유효 거리
		int nRotate;					// Rotation 정보

		eWeatherEffectCreatePosType eCreatePosType;			// 카메라의 좌표를 검사위치로 적용할 것인지 판단
		float fCreateDist;				// 검사위치로부터 생성할 상대적 거리
		float fCreateRangeMin;			// 생성 범위
		float fCreateRangeMax;

		float fScaleMin;				// Scale 범위
		float fScaleMax;

		DWORD dwValidCreateTime;		// 생성 가능 시각
		DWORD dwCreateDelay;			// 생성 Delay

		BOOL bBillboard;
	};


	typedef std::pair<DWORD, int> WeatherObjectKey;
	typedef std::deque<WeatherObjectKey> WeatherRenderStateQue;

	WeatherRenderStateQue m_CreateEffectQue;
	WeatherRenderStateQue m_RenderEffectQue;
	WeatherRenderStateQue m_DeleteEffectQue;

	typedef std::map<DWORD, stWeatherEffectData> WeatherEffectDataMap;
	WeatherEffectDataMap m_WeatherEffectDataMap;

	BOOL m_bEffectOn;						// Effect On/Off 여부
	DWORD m_CurEffectWeatherHashCode;		// 현재 Effect 날씨 상태
    WORD m_wIntensity;						// 강도

	void LoadScript();
	void AddCreateStateEffect( stWeatherEffectData& data, VECTOR3& vecCheckPos );

public:
	CWeatherManager();
	virtual ~CWeatherManager();
	void Init();
	void Release();
	void Process();
	BOOL EffectOn( LPCTSTR weatherTxt, WORD wIntensity=0 );
	BOOL EffectOn( DWORD dwWeatherHashCode, WORD wIntensity=0 );
	void EffectOff();
	void SetEffectCreateTime( LPCTSTR weatherTxt, float fTime );
	void SetEffectRenderTime( LPCTSTR weatherTxt, float fTime );
	void SetEffectDeleteTime( LPCTSTR weatherTxt, float fTime );
	void SetEffectCreateTime( DWORD dwWeatherHashCode, float fTime );
	void SetEffectRenderTime( DWORD dwWeatherHashCode, float fTime );
	void SetEffectDeleteTime( DWORD dwWeatherHashCode, float fTime );

#if defined(_GMTOOL_) || defined(_TESTCLIENT_)
protected:
	BOOL m_bDrawDebugInfo;
public:
	void ShowDebugState( BOOL bShow ) { m_bDrawDebugInfo = bShow; }
	BOOL IsShowDebugState() const { return m_bDrawDebugInfo; }
	void RenderDebugState();
#endif
#ifdef _GMTOOL_
	void AddGMToolMenu();
#elif defined(_TESTCLIENT_)
	void AddTSToolMenu();
#endif
};

EXTERNGLOBALTON(CWeatherManager);

#endif // !defined(AFX_WEATHERMANAGER_H__5EB34DB4_AE8E_4654_B1FB_2A4A701DAFB1__INCLUDED_)
