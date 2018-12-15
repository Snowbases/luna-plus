// 091116 ONS UiSettingManager
// 게임로드 / 맵이동시 기존에 저장된 UI의 위치를 관리하는 클래스 
#pragma once
#define UISETTINGMGR USINGTON(cUiSettingManager)

struct stUiSettingState
{
	DWORD		dwID;
	BOOL		bSavePosition;
	BOOL		bSaveActivity;
};

// UI위치, 활성화 상태정보 구조체 추가
struct stSettingProperty
{
	VECTOR2		vPos;
	DWORD		dwWidth;
	DWORD		dwHeight;
	BOOL		bActivity;
};

struct stUiSettingInfo
{
	DWORD				dwID;
	stSettingProperty	property;
};


class cUiSettingManager
{
	// 100304 ONS 해상도변경에 따른 다이얼로그 위치 수정
	enum eDispSector
	{
		eDispSector_1 = 0,
		eDispSector_2,
		eDispSector_3,
		eDispSector_4,
		eDispSector_None
	};

	RECT		m_screenRect[4];

	typedef std::vector<stUiSettingState>					UiSettingVector;
	UiSettingVector											m_UiSetting;
	
	typedef stdext::hash_map< DWORD, stSettingProperty >	UiSettingMap;
	UiSettingMap											m_UiSettingMap;

public:
	cUiSettingManager(void);
	~cUiSettingManager(void);

	void AddSettingInfo( stUiSettingInfo* pInfo );

	// 스크립트 파일내의 세팅속성에 변화가 있을경우 세팅정보를 갱신한다.
	void ResetUiSettingInfo(void);

	// 세팅 대상이 되는 다이얼로그 리스트를 설정한다.
	void SetSettingList( DWORD dwWindowID, BOOL bIsSavePos, BOOL bIsSaveActivity );

	// m_UiSettingList에 저장된 상태값을 통해서 현재위치와 활성화상태값을 파일에 저장한다.
	void SaveSettingInfo(void);
	
	// 현재위치와 활성화상태값을 로드한다.(UiSetting.opt)
	void LoadSettingInfo(void);

	// 맵이동시 m_UiSettingList에 등록된 윈도우의 위치와 활성화상태를 적용한다.
	void ApplySettingInfo(void);

	// 맵이동 및 로그아웃 전에 SettingInfo를 갱신한다.
	void UpdateSettingInfo(void);

	const stSettingProperty* GetUiSettingInfo(DWORD dwWindowID);
	void SetUiPosition( DWORD dwWindowID, float fPosX, float fPosY );
	void SetUiCurActivity( DWORD dwWindowID, BOOL bCurActivity );

	// 100304 ONS 해상도변경에 따른 다이얼로그 위치 수정
	void SetScreenRect();
	int GetDisplaySector( const stSettingProperty* pProperty );
	VECTOR2 SetPositionRate( const stSettingProperty* pProperty, eDispSector Sector );
};

EXTERNGLOBALTON(cUiSettingManager)