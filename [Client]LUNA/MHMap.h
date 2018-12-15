// MHMap.h: interface for the CMHMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MHMAP_H__444BAED6_A7A5_4CB6_B1BF_027999B116FE__INCLUDED_)
#define AFX_MHMAP_H__444BAED6_A7A5_4CB6_B1BF_027999B116FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include ".\Engine\EngineMap.h"
#include ".\Engine\EngineSky.h"
#include ".\Engine\EngineLight.h"
#include ".\Engine\EngineObject.h"
#include "EngineCloud.h"

class CTileManager;
class CMHFile;
class CWayPointManager;

#define MAP		USINGTON(CMHMap)

struct MAPDESC
{
	MAPDESC()
	{
		DefaultSight = 8000;
		Fov = 45;

		fogdesc.bEnable = TRUE;
		fogdesc.dwColor = 0xaaaaaaaa;
		fogdesc.fDensity = 0.5f;
		fogdesc.fStart = 20000;
		fogdesc.fEnd = 80000;
		BGMSoundNum = 0;
		
		strcpy(MapFileName,"new_gae_002.map");
		strcpy(TileFileName,"");

		strcpy(SkyMod,"SkyBox.MOD");
		strcpy(SkyAnm,"SkyBox.ANM");
		
		strcpy(SunObject,"bg_login.mp3");

		bSun = 0;
		bHField = TRUE;
		bSky = 1;

		Ambient = 0xaaaaaaaa;
		
		SunPos.x = 800;
		SunPos.y = 1300;
		SunPos.z = 200;		
		
		backColor = 0x00000000;

		SunDistance = 7000;

		bFixHeight = FALSE;

		CloudNum = 0;
		CloudHMin = 1500;
		CloudHMax = 2000;
		strcpy(CloudListFile,"CloudList.txt");


		//trustpak 2005/04/15
		SkyOffset.x = 0.0f;
		SkyOffset.y = 0.0f;
		SkyOffset.z = 0.0f;
		///
	}
	BOOL bFixHeight;
	float FixHeight;
	VECTOR3 SunPos;

	DWORD Ambient;

	float DefaultSight;
	float Fov;
	
	FOG_DESC fogdesc;
	
	char MapFileName[64];
	char TileFileName[64];
	
	BOOL bSky;
	char SkyMod[64];
	char SkyAnm[64];

	SNDIDX BGMSoundNum;
	float BGMVolume;
	
	BOOL bSun;
	char SunObject[64];

	BOOL bHField;

	DWORD backColor;

	float SunDistance;

	char CloudListFile[64];
	DWORD CloudNum;
	int CloudHMin;
	int CloudHMax;

	//trustpak 2005/04/16
	VECTOR3 SkyOffset;

	///
};

class CMHMap  
{
	BOOL m_bIsInited;
	MAPTYPE m_MapNum;
	VECTOR3 m_ShadowPivotPos;

	MAPDESC* mapDesc;
	CEngineMap map;
	CEngineSky* sky;
	CTileManager* m_pTileManager;
	CEngineObject m_SunObject; 

	CEngineLight m_Shadowlight;
	
	CYHHashTable<CEngineCloud> m_CloudTable;

////
	BOOL	m_bVillage;
	VECTOR3 m_SunPosForShadow;
	
		
public:

	BOOL LoadMapDesc(MAPTYPE MapNum);
	void ApplyMapDesc();

	CMHMap();
	~CMHMap();
	DWORD GetTileWidth();
	MAPDESC* GetMapDesc()	{	return mapDesc;	}

	void LoadNpcForQuestDialog() ;

	void LoadStaticNpc(MAPTYPE MapNum);
	void LoadPreData(MAPTYPE MapNum);
	BOOL IsInited()	{	return m_bIsInited;	}
	void InitMap(MAPTYPE MapNum);

	void Release();
	void Process(DWORD tick);
	
	DWORD GetMultipleObjectWithRay(PICK_GXOBJECT_DESC* pPickDesc, DWORD dwMaxDescNum, VECTOR3* pv3Pos, VECTOR3* pv3Dir, DWORD dwFlag);
	BOOL CollisonCheck(VECTOR3* pStart,VECTOR3* pEnd,VECTOR3* pRtCollisonPos,CObject* pObj);		// 충돌하면 TRUE반환하고 pRtCollisonPos에 새로운 타겟 설정

	void SetShadowPivotPos(VECTOR3* pPivotPos);
	
	CTileManager* GetTileManager()		{	return m_pTileManager;	}
	MAPTYPE GetMapNum()	{	return m_MapNum;	}
	

	void SetMapAmbient( DWORD dwAmbient ) { map.SetAmbientColor(dwAmbient); }
	

	BOOL IsInTile(int x, int y, MAPTYPE MapNum,CObject* pObject);
	BOOL CollisionTilePos(int x, int y, MAPTYPE MapNum,CObject* pObject);
	BOOL CollisionTileCell(int x, int y, MAPTYPE MapNum,CObject* pObject);

	BOOL CollisionLine(VECTOR3* pStart,VECTOR3* pEnd, VECTOR3 * Target, MAPTYPE MapNum,CObject* pObject);
	BOOL NonCollisionLine(VECTOR3* pStart, VECTOR3* pEnd, VECTOR3* Target, MAPTYPE MapNum, CObject* pObject);

	////
	BOOL IsVillage() { return m_bVillage; }
	void SetVillage( BOOL bVillage ) { m_bVillage = bVillage; }

	// quest npc
	DWORD	m_dwQuestNpcId;
	void	AddQuestNpc( QUESTNPCINFO* pInfo );

	// 0909097 ShinJS --- 길찾기, 충돌 관련 (5팀 Source 적용)
public:
	// 타겟 지점까지 직선을 그어서 한번에 갈 수 있는가 없는가를 체크
	BOOL CollisionCheck_OneLine_New(VECTOR3* pStart,VECTOR3* pEnd);

	// 타겟 지점의 타일이 뚫려있는가만 체크
	BOOL CollisionCheck_OneTile(VECTOR3* pDestPos);

	// 길찾기를 시행하고, WayPoint를 리턴한다
	BOOL PathFind(VECTOR3* pStart,VECTOR3* pEnd, CWayPointManager* p_pWayPointManager, BOOL p_SimpleMode = TRUE);

	// 움직이지 못하는 지역일 경우 주변의 움직일 수 있는 안전한 타일의 좌표를 리턴한다
	VECTOR3 GetSafePosition(VECTOR3* p_NowPosition);


	/// Debug용. 타일정보를 보기 위해
private:
	std::vector<VECTOR3>	m_Path_Debug;
	std::deque<VECTOR3>		m_Path_Optimize_Debug;

	BOOL					m_bRenderTileData;

public:
	void RenderTileData_Debug();
	void RenderOneTile( const VECTOR3& p_TileOriPos, DWORD p_Color );		// 출력 위치를 왼쪽 위 기준으로 네모 하나
	void RenderOneWayPoint( const VECTOR3& p_WayPointPos, DWORD p_Color );	// 출력 위치 바로 그 지점에 웨이포인트 그림


	void SetPath_Debug(std::vector<VECTOR3> val) { m_Path_Debug = val; }
	void SetPath_Optimize_Debug(std::deque<VECTOR3> val) { m_Path_Optimize_Debug = val; }

	void SetRenderTileData( BOOL bRenderTileData ) { m_bRenderTileData = bRenderTileData; }
};

EXTERNGLOBALTON(CMHMap)

#endif // !defined(AFX_MHMAP_H__444BAED6_A7A5_4CB6_B1BF_027999B116FE__INCLUDED_)
