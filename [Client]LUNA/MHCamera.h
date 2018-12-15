// MHCamera.h: interface for the CMHCamera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MHCAMERA_H__3EE48679_085F_4E12_AF97_C06D8AB29A00__INCLUDED_)
#define AFX_MHCAMERA_H__3EE48679_085F_4E12_AF97_C06D8AB29A00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Angle.h"
#include ".\Engine\EngineCamera.h"
#include ".\Engine\EngineObject.h"

class CObject;
class CEngineObject;


struct MHCAMERASHAKE;


#define MAX_CAMERA_DESC	8
#define CAMERA_FILTER_DISTANCE	200
#define CAMERA_FILTER_FADE_INOUT_DEFAULT_SPEED	0.001f
#define CAMERA	USINGTON(CMHCamera)

enum eCAMERAMODE
{
	eCM_Free = 0,
	eCM_QuarterView,
	eCM_FollowView,
	eCM_LOGIN,
	eCM_EyeView,
};

enum eCAMERASTATE
{
	eCS_Normal,
	eCS_Sit,
	eCS_Die,
};

enum
{
	eCP_CHARMAKE = 0,
	eCP_CHARSEL,
	eCP_INTERPOLATE,

	eCP_MAX,
};

enum eCAMERA_FILTER_ACTION
{
	eCAMERA_FILTER_ACTION_NONE,
	eCAMERA_FILTER_ACTION_MOVE,
};

struct MHCAMERAPOS
{
	float m_AngleX;
	float m_AngleY;
	float m_Dist;

	VECTOR3 m_Pivot;
};


struct MHCAMERADESC
{
	CAngle m_AngleX;
	CAngle m_AngleY;

	float m_fDistance;

	VECTOR3 m_CameraPos;
	VECTOR3 m_CameraDir;
	
	VECTOR3 m_CameraDownVector;
	VECTOR3 m_CameraDownPositon;

	BOOL m_bCameraAngleMovingX;
	DWORD m_CameraAngleMoveStartX;
	DWORD m_CameraAngleMoveDurationX;
	
	BOOL m_bCameraAngleMovingY;
	DWORD m_CameraAngleMoveStartY;
	DWORD m_CameraAngleMoveDurationY;

	BOOL m_bCameraDistanceMoving;
	DWORD m_CameraDistanceMoveStart;
	DWORD m_CameraDistanceMoveDuration;
	float m_fDistanceStart;
	float m_fDistanceChange;

	float m_CharHeight;			//운기중에는 카메라가 서서히 내려갔으면.
	float m_StartCharHeight;
	float m_TargetCharHeight;
	BOOL  m_bHeightChanging;
	DWORD m_dwHeightStartTime;
	DWORD m_dwHeightChangeDuration;

	CObject* m_pPivotObject;	// 둘중에 하나는 꼭 있어야 한다.
	VECTOR3 m_PivotPos;

	BOOL m_bPivotObject;		// TRUE 면 PivotObject FALSE 면 PivotPos
};

struct stCameraFilterInfo
{
	DWORD	dwFilterIdx;
	char	szModelFileName[MAX_PATH];
	DWORD	dwDefaultWidth;
	DWORD	dwDefaultHeight;
	BOOL	bShowInterface;
	int		nMotionIdxToAction;
	std::vector<int> motionIdxVector;

	stCameraFilterInfo()
	{
		dwFilterIdx = 0;
		ZeroMemory(szModelFileName, MAX_PATH);
		dwDefaultWidth = 800;
		dwDefaultHeight = 600;
		nMotionIdxToAction = 0;
	}
};

// 100125 ShinJS --- CameraFilter Action을 위한 CObjectBase
class CCameraFilterObject : public CObjectBase
{
	DWORD m_dwObjectIndex;
	DWORD m_dwFilterIndex;
	CEngineObject m_FilterEngineObject;

	BOOL m_bCameraFilterNoRepeat;						// Camera Filter Index Repeat 여부
	BOOL m_bCameraFilterFaded;
	BOOL m_bCameraFilterFadeIn;
	BOOL m_bCameraFilterFadeOut;
	float m_fCameraFilteFadeAlpha;						// Camera Filter Index Fade Alpha 값
	float m_fCameraFilteFadeAlphaValue;					// Camera Filter Index Fade Alpha 처리시 사용될 변수
	DWORD m_dwRemainTime;								// 유지 시간

	std::deque<int> m_FilterMotionQue;					// Camera Filter Animaion Motion Index를 실행 순서대로 저장
	eCAMERA_FILTER_ACTION m_CameraFilterAction;			// Filter Motion 변경시 지정된 액션
	VECTOR3 m_FilterActionMovePos;						// eCAMERA_FILTER_ACTION_MOVE 실행시 필요 정보

public:
	CCameraFilterObject();
	~CCameraFilterObject();
	BOOL Init( DWORD dwObjectIndex, const stCameraFilterInfo& filterInfo, BOOL bNoRepeat, BOOL bFadeIn, float fFadeTime, DWORD dwRemainTime, eCAMERA_FILTER_ACTION eAction );
	void Release();
	BOOL IsInited();
	const DWORD GetObjectIndex() const { return m_dwObjectIndex; }

	void Process();
	void DoCameraFilterAction();						// Camera Filter Action 실행

	void Hide() { m_FilterEngineObject.Hide(); }
	void SetFadeOut( float fFadeTime );
	void SetFilterActionMovePos(const VECTOR3& vecPos) { m_FilterActionMovePos = vecPos; }
};

class CMHCamera  
{
	VECTOR3 m_DownVector;
	DWORD m_CurCameraMode;
	CEngineCamera m_EngineCamera;

	MHCAMERADESC m_CameraDesc[MAX_CAMERA_DESC];
	int m_CurCamera;

	float m_fAddHeight;

	// 100607 ONS 게임중 맵이동시 카메라위치 초기화를 막기위한 변수
	BOOL  m_bIsFirstTime;

//---sight distance option
	float m_fDistanceRate;	//50~100% 
	float m_fMaxSightDistance;
	float m_fFov;


	//////////////////////////////////////////////////////////////////////////
	// 진동 기능 구현을 위한 변수들
	VECTOR3 m_VibratePos;
	VECTOR3 m_VibrateAngle;
	VECTOR3 m_PushedVector;
	DWORD m_VibrateDuration;
	DWORD m_VibrateStartTime;
	BOOL m_bIsVibrating;


	void CalcVibration();
	//////////////////////////////////////////////////////////////////////////	

	
	//////////////////////////////////////////////////////////////////////////
	// 쉐이크 기능을 위한 변수들
	MHCAMERASHAKE** m_ppCameraShakeDesc;		// 쉐이킹 에 대한 Desc 들..
	VECTOR3 m_ShakingPos;
	BOOL m_bIsShaking;
	DWORD m_nCurShakingNum;						// 현재 하고 있는 쉐이킹 Desc Index (-1이면 하고 있지 않다)
	DWORD m_CurShakingPos;						// 현재 진행중인 쉐이킹 Desc내에서의 위치
	DWORD m_LastShakingPosChangedTime;			// 마지막으로 m_CurShakingPos를 바꾼 시간.
	float m_Amplitude;
	WORD  m_ShakingTimes;						// 쉐이킹 횟수

	void CalcShaking();
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	// 이동보간을 위한 변수들
	MHCAMERAPOS m_MovePos[eCP_MAX];
	int m_CurPosition;
	int m_GotoPosition;
	
	float m_MoveTime;
	float m_MoveStartTime;
	float m_MoveSpeed;
	
	BOOL m_bMoved;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 카메라자유이동을 위한 변수들
	DWORD m_LastMoveNoticeTime;			// 캐릭터가 이동했다고 서버에 알려준 시간
	// 090422 ShinJS --- FollowView를 한번만 실행하는 경우 (시점이동이 완료되면 Follow 종료후 eCM_Free로 전환)
	BOOL m_bExecuteOnceFollowView;
	
	void CalcCameraPosAngle();
	void CalcFreeCamera();
	void ProcessCameraMode();
	void ProcessCameraMoving();

	void RestrictCameraAngleDistance();
	
	void FollowCamera();

	//---KES Map GXObject Alpha 071020
	cPtrList m_AlphaHandleList;
	//-----------------------------

public:
	CMHCamera();
//	//MAKESINGLETON(CMHCamera);
	~CMHCamera();

	void Release();

	//---KES Map GXObject Alpha 071020
	void GXOAlphaProcess();
	//-----------------------------

	void Init(CObject* pObject,float angleX,float angleY,float fDistance);
	void InitCamera(int CameraNum,float angleX,float angleY,float fDistance,CObject* pObject);
	void InitCamera(int CameraNum,float angleX,float angleY,float fDistance,VECTOR3* pPivotPos);

	void SetToEngineCamera();

	void MouseRotate(int x,int y);

	void Process();

	void SetCurCamera(int Camera);
	void SetCameraPivotObject(int Camera,CObject* pObject);
	void SetCameraPivotPos(int Camera,VECTOR3* pPos);

	void SetTargetAngleX(int Camera,float fAngleX,DWORD Duration);
	void SetTargetAngleY(int Camera,float fAngleY,DWORD Duration);
	void SetDistance(int Camera,float fDistance,DWORD Duration);
	float GetDistance(int Camera) { return m_CameraDesc[Camera].m_fDistance; }

	void Rotate(int Camera,float RotateAngle,DWORD Duration);
	void ZoomDistance(int Camera,float fDistanceChange,DWORD Duration);

	void ChangeCameraSmooth(int Camera,DWORD Duration);	// 현재 카메라에서 다른 카메라로 부드럽게 이동한다.
	void ChangeToDefaultCamera();	// 현재 카메라의 설정값을 그대로 0번 카메라에 셋팅한다.
	void StopAllMoving(int Camera);	// 회전이나 줌등을 멈춘다.

	void SetMaxSightDistance( float fMaxDistance )		{ m_fMaxSightDistance = fMaxDistance; m_fDistanceRate = 0.0f;  }
	void SetCameraFov( float fFov )						{ m_fFov = fFov; }

	void SetSightDistanceRate( float fDistanceRate );


	//////////////////////////////////////////////////////////////////////////
	// 진동기능을 위한 함수들
	void SetPushedVibration(VECTOR3* PushedVector,DWORD Duration);
	//////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////////
	// 쉐이킹 기능을 위한 함수들
	void SetShaking(int ShakeNum, float amp, int Times = 1 );
	//////////////////////////////////////////////////////////////////////////
	// 이동보간을 위한 함수들
	void SetMovePos(int Index, MHCAMERAPOS* pCameraPos);
	void MoveStart(int GotoIndex);
	void Interpolation();
	BOOL IsMoved()				{	return m_bMoved;	}
	int GetGotoPosition()		{	return m_GotoPosition;	}
	void ResetInterPolation();
	//////////////////////////////////////////////////////////////////////////


	int GetCameraMode() { return m_CurCameraMode; }
	void SetCameraMode(int CameraMode);	
	void ToggleCameraViewMode();
	
	void SetCharState( int eCharState, DWORD dwDuration );
//	BOOL CanChangeCameraAngleTo( float xRad, float yRad );
	void Fly( float fFly );
	
	// 현재 카메라 정보를 가져오는 함수
	MHCAMERADESC* GetCameraDesc() { return &m_CameraDesc[m_CurCamera]; }
	// 090422 ShinJS --- FollowView를 한번만 실행(시점이동이 완료되면 Follow 종료후 eCM_Free로 전환)
	void ExecuteOnceFollowView();

	// 091005 pdy 카메라 필터 작업
private:
	CYHHashTable< stCameraFilterInfo > m_CameraFilterHash;

	// 100607 ShinJS --- 같은 필터를 여러개 생성할수 있도록 Index별 List를 관리할수 있도록 한다.
	typedef std::map< DWORD, CCameraFilterObject > CameraFilterObjectMap;
	std::map< DWORD, CameraFilterObjectMap > m_mapCameraFilter;
	CIndexGenerator m_CameraFilterIndexGenerator;

	typedef std::list< std::pair< DWORD, DWORD > > DeletedFilterContainer;
	DeletedFilterContainer m_DeleteFilterIndexList;
	DWORD m_HideInterfaceCount;
	void ProcessCameraFilter();

public:
	void LoadCameraFilterList();
	void ReleaseFilterList();
	stCameraFilterInfo* GetFilterInfo(DWORD dwKey) { return m_CameraFilterHash.GetData( dwKey ) ;}
	
	// 카메라 필터를 Attach 한다.
	CCameraFilterObject& AttachCameraFilter( DWORD dwIndex, BOOL bNoRepeat=FALSE, BOOL bFadeIn=FALSE, float fFadeRatio=1.f, DWORD dwRemainTime=0, eCAMERA_FILTER_ACTION eAction=eCAMERA_FILTER_ACTION_NONE );
	// 해당 필터Index의 필터를 모두 제거한다.
	void DetachCameraFilter( DWORD dwFilterIndex, BOOL bFadeOut=FALSE, float fFadeRatio=1.f );
	// 해당 필터Index의 필터Object만 제거한다.
	void DetachCameraFilter( DWORD dwFilterIndex, DWORD dwObjectIndex, BOOL bFadeOut=FALSE, float fFadeTime=1.f );
	void ReleaseCameraFilter();
	void AddCameraFilterDeleteList( DWORD dwFilterIndex, DWORD dwObjectIndex );
	void DeleteCameraFilterObject( DWORD dwFilterIndex, DWORD dwObjectIndex );
};
EXTERNGLOBALTON(CMHCamera)
#endif // !defined(AFX_MHCAMERA_H__3EE48679_085F_4E12_AF97_C06D8AB29A00__INCLUDED_)
