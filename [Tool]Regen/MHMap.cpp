// MHMap.cpp: implementation of the CMHMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MHMap.h"

//#include "GameResourceManager.h"
#include "TileManager.h"
#include "MHFile.h"
#include "DefineStruct.h"
#include "resource.h"
#include "MainFrm.h"
#include "RegenToolDoc.h"
#include "RegenToolView.h"

//#include "Mp3/BGMLoader.h"
//#include "ObjectManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMHMap::CMHMap()
{
	sky = NULL;
	m_pTileManager = NULL;
	mapDesc = NULL;
}

CMHMap::~CMHMap()
{
	SAFE_DELETE(m_pTileManager);
	SAFE_DELETE(mapDesc);
}

void CMHMap::InitMap(int MapNum)
{
	Release();

	m_MapNum = MapNum;
	char mapdescfile[256];
	sprintf(mapdescfile,"data\\script\\mapset\\MAP%d.bmhm",MapNum);
	CMHFile file;
	file.Init(mapdescfile,"rb");
	LoadMapDesc(&file);
	file.Release();
	
	g_bColor = mapDesc->backColor;
	g_fFixHeight = mapDesc->FixHeight;
	g_bFixHeight = mapDesc->bFixHeight;
	bRenderSky = mapDesc->bSky;
	g_pExecutive->GetGeometry()->SetAmbientColor(
		0,
		mapDesc->Ambient);

	TCHAR fileName[MAX_PATH] = {0};
	_stprintf(
		fileName,
		_T("data\\3dData\\map\\%s"),
		mapDesc->MapFileName);
	map.Init(
		fileName);

	if(mapDesc->bSky)
	{
		SAFE_DELETE(sky);
		sky = new CEngineSky;
		sky->CreateSky(
			mapDesc->SkyMod,
			mapDesc->SkyAnm);
	}
		
	SAFE_DELETE(m_pTileManager);
	m_pTileManager = new CTileManager;
	_stprintf(
		fileName,
		_T("system\\map\\%s"),
		mapDesc->TileFileName);
	m_pTileManager->LoadTileInfo(
		fileName);
	
	if(mapDesc->fogdesc.bEnable)
	{
		g_pExecutive->GetGeometry()->DisableFog(0);
		g_pExecutive->GetGeometry()->SetFog(&mapDesc->fogdesc,0);
	}

	g_pExecutive->GetGeometry()->EnableDirectionalLight(0);
	
	DIRECTIONAL_LIGHT_DESC LightDesc;
	LightDesc.dwAmbient = 0;
	LightDesc.dwDiffuse = 0x66666666;
	LightDesc.dwSpecular = 0;
	LightDesc.v3Dir = mapDesc->SunPos * -1.f;
	LightDesc.bEnable = FALSE;
	g_pExecutive->GetGeometry()->SetDirectionalLight(&LightDesc,0);

	//////////////////////////////////////////////////////////////////////////
	// 해 와 달
	if(mapDesc->bSun)
	{
		DIRECTORYMGR->SetLoadMode(eLM_Map);
		m_SunObject.Init(mapDesc->SunObject,NULL,eEngineObjectType_Effect);
		DIRECTORYMGR->SetLoadMode(eLM_Root);
	}
	
	LoadStaticNpc(MapNum);
}

void CMHMap::InitTile(char* TileName, CTile* pViewTile)
{
	SAFE_DELETE(m_pTileManager);
	m_pTileManager = new CTileManager;
	m_pTileManager->LoadTileInfo(TileName);
}

void CMHMap::Release()
{
	ReleasePool();
	ProcessGarbageObject();
	m_Shadowlight.Release();
//	if(MP3)
//		MP3->Stop();
	SAFE_DELETE(mapDesc);
	map.Release();
	SAFE_DELETE(sky);
	SAFE_DELETE(m_pTileManager);
	m_SunObject.Release();
}

void CMHMap::Process(DWORD CurTime)
{
	if(sky)
	{
		const DWORD oneFrameTick = 50;
		const DWORD unitFrameTick = oneFrameTick * 2;

		if(oneFrameTick < (GetTickCount() % unitFrameTick))
		{
			sky->IncreaseSkyAniFrame(1);
		}

		sky->RenderSky();
	}

	//if(mapDesc->bSun)
	//{
	//	VECTOR3 pos;
	//	Normalize(&pos,&mapDesc->SunPos);
	//	pos = pos * mapDesc->SunDistance;
	//	pos = m_ShadowPivotPos;// + pos;
	//	pos.x += 10000;
	//	pos.y -= 2000;
	//	m_SunObject.SetEngObjPosition(&pos);
	//}
}
BOOL CMHMap::CollisonCheck(VECTOR3* pStart,VECTOR3* pEnd,VECTOR3* pRtCollisonPos)
{
	if(m_pTileManager == NULL)
		return FALSE;
	if(m_pTileManager->CollisonCheck(pStart,pEnd,pRtCollisonPos) == TRUE)
		return TRUE;


	return FALSE;
}

void CMHMap::SetShadowPivotPos(VECTOR3* pPivotPos)
{
	m_ShadowPivotPos = *pPivotPos;
}

void CMHMap::LoadMapDesc(CMHFile* file)
{
	char value[64] = {0,};
	SAFE_DELETE(mapDesc);
	mapDesc = new MAPDESC;

	while(1)
	{
		if(file->IsEOF())
			break;

		strcpy(value,strupr(file->GetString()));

		if(strcmp(value,"*SIGHT") == 0)
		{
			mapDesc->DefaultSight = file->GetFloat();
		}
		else if(strcmp(value,"*FOV") == 0)
		{
			mapDesc->Fov = file->GetFloat();
		}
		else if(strcmp(value,"*FOG") == 0)
		{
			mapDesc->fogdesc.bEnable = file->GetBool();
		}
		else if(strcmp(value,"*FOGCOLOR") == 0)
		{
			DWORD r,g,b,a;
			r = file->GetDword();
			g = file->GetDword();
			b = file->GetDword();
			a = file->GetDword();
			mapDesc->fogdesc.dwColor = RGBA_MAKE(r,g,b,a);
		}
		else if(strcmp(value,"*FOGDENSITY") == 0)
		{
			mapDesc->fogdesc.fDensity = file->GetFloat();
		}
		else if(strcmp(value,"*FOGSTART") == 0)
		{
			mapDesc->fogdesc.fStart = file->GetFloat();
		}
		else if(strcmp(value,"*FOGEND") == 0)
		{
			mapDesc->fogdesc.fEnd = file->GetFloat();
		}
		else if(strcmp(value,"*MAP") == 0)
		{
			file->GetString(mapDesc->MapFileName);
		}
		else if(strcmp(value,"*TILE") == 0)
		{
			file->GetString(mapDesc->TileFileName);
		}
		else if(strcmp(value,"*SKYMOD") == 0)
		{
			file->GetString(mapDesc->SkyMod);
		}
		else if(strcmp(value,"*SKYANM") == 0)
		{
			file->GetString(mapDesc->SkyAnm);
		}
		else if(strcmp(value,"*SKYBOX") == 0)
		{
			mapDesc->bSky = file->GetBool();
		}
		else if(strcmp(value,"*BGM") == 0)
		{
			file->GetString(mapDesc->BGMSound);
		}
		else if(strcmp(value,"*BRIGHT") == 0)
		{
			BYTE dd = file->GetByte();
			mapDesc->Ambient = RGBA_MAKE(dd,dd,dd,dd);
		}
		else if(strcmp(value,"*SUNPOS") == 0)
		{
			mapDesc->SunPos.x = file->GetFloat();
			mapDesc->SunPos.y = file->GetFloat();
			mapDesc->SunPos.z = file->GetFloat();
		}
		else if(strcmp(value,"*SUNOBJECT") == 0)
		{
			file->GetString(mapDesc->SunObject);
		}
		else if(strcmp(value,"*SUN") == 0)
		{
			mapDesc->bSun = file->GetBool();
		}
		else if(strcmp(value,"*SUNDISTANCE") == 0)
		{
			mapDesc->SunDistance = file->GetFloat();
		}
		else if(strcmp(value,"*BACKCOLOR") == 0)
		{
			DWORD r,g,b,a;
			r = file->GetDword();
			g = file->GetDword();
			b = file->GetDword();
			a = file->GetDword();
			mapDesc->backColor = RGBA_MAKE(r,g,b,a);
		}
		else if(strcmp(value,"*FIXHEIGHT") == 0)
		{
			mapDesc->bFixHeight = TRUE;
			mapDesc->FixHeight = file->GetFloat();			
		}
	}
	
	file->Release();
}

void CMHMap::LoadStaticNpc(DWORD MapNum)
{
	// 웅주, 나중에 NPC도 불러와보자
}