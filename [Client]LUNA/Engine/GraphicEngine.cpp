#include "stdafx.h"
#include "GraphicEngine.h"
#include "EngineObject.h"
#include "..\[CC]Header\GameResourceStruct.h"
#include "ObjectManager.h"

float gTickPerFrame = 0;
I4DyuchiGXExecutive* g_pExecutive = NULL;
cPtrList GarbageObjectList;
cPtrList EffectPool;
BOOL bRenderSky = FALSE;
BOOL g_bDisplayFPS = FALSE;
DWORD g_bColor;
BOOL g_bFixHeight = FALSE;
float g_fFixHeight = 0;
int g_SlowCount = 0;
int g_FastCount = 0;
HMODULE g_hExecutiveHandle = 0;

DWORD __stdcall MHErrorHandleProc(ERROR_TYPE type,DWORD dwErrorPriority,void* pCodeAddress,char* szStr)
{
	if(dwErrorPriority == 0)
	{
		ASSERTMSG(0,szStr ? szStr : "?");
	}
	return 0;
}

CGraphicEngine::CGraphicEngine(HWND hWnd,DISPLAY_INFO* pDispInfo,DWORD MaxShadowNum,DWORD ShadowMapDetail,float SightDistance,DWORD FPS)
{
	m_BackGroungColor = 0;
	ASSERT(!g_pExecutive);
	CreateExecutive(hWnd,pDispInfo,MaxShadowNum,ShadowMapDetail,SightDistance,FPS);
	g_pExecutive = m_pExecutive;
	m_bRender = TRUE;
}

void CGraphicEngine::LoadPack(LPCTSTR packedFileName)
{
	if(mPackedFileContainer.end() != mPackedFileContainer.find(packedFileName))
	{
		return;
	}

	I4DyuchiFileStorage* fileStorage = 0;
	m_pExecutive->GetFileStorage(
		&fileStorage);

	if(fileStorage)
	{
		mPackedFileContainer[packedFileName] = fileStorage->MapPackFile(
			LPTSTR(packedFileName),
			TRUE);
	}
}

void CGraphicEngine::UnLoadPack()
{
	I4DyuchiFileStorage* fileStorage = 0;
	m_pExecutive->GetFileStorage(
		&fileStorage);

	if(0 == fileStorage)
	{
		return;
	}

	while(false == mPackedFileContainer.empty())
	{
		const PackedFileContainer::iterator iterator = mPackedFileContainer.begin();
		HANDLE fileHandle = iterator->second;

		fileStorage->UnmapPackFile(
			fileHandle);

		mPackedFileContainer.erase(
			iterator);
	}
}

CGraphicEngine::~CGraphicEngine()
{
	UnLoadPack();
	ReleasePool();
	g_pExecutive = NULL;
	if(m_pExecutive)
	{
		m_pExecutive->DeleteAllGXEventTriggers();
		m_pExecutive->DeleteAllGXLights();
		m_pExecutive->DeleteAllGXObjects();
		m_pExecutive->UnloadAllPreLoadedGXObject(0);	//NULLÃß°¡
		m_pExecutive->DeleteGXMap(NULL);
		m_pExecutive->Release();
		m_pExecutive = NULL;
	}
	
	FreeLibrary(g_hExecutiveHandle);
}

BOOL CGraphicEngine::CreateExecutive(HWND hWnd,DISPLAY_INFO* pDispInfo,DWORD MaxShadowNum,DWORD ShadowMapDetail,float SightDistance,DWORD FPS)
{
	g_SlowCount = 0;
	g_FastCount = 0;
	g_hExecutiveHandle = LoadLibrary(
		"Executive.dll");

	CREATE_INSTANCE_FUNC pFunc = (CREATE_INSTANCE_FUNC)GetProcAddress(
		g_hExecutiveHandle,
		"DllCreateInstance");
	HRESULT hr = pFunc((void**)&m_pExecutive);

	if (hr != S_OK)
	{		
		MessageBox(NULL,"Executive»ý¼º ½ÇÆÐ","Error",MB_OK);
		return FALSE;
	}
	
	if(g_bUsingEnginePack)
	{
		PACKFILE_NAME_TABLE nameTable[] = {
			{"data/interface/2dImage/image.pak", 0},
			{"data/3dData/map.pak", 0},
		};

		m_pExecutive->InitializeFileStorageWithoutRegistry(
			"FileStorage.dll",
			35000,
			10000,
			MAX_PATH,
			FILE_ACCESS_METHOD_FILE_OR_PACK,
			nameTable,
			_countof(nameTable),
			TRUE);
	}
	else
	{
		// 080221 LUJ, 패킹시킬 파일 개수를 15000 -> 25000으로 수정
		m_pExecutive->InitializeFileStorageWithoutRegistry("FileStorage.dll",
			35000,10000,256,FILE_ACCESS_METHOD_ONLY_FILE,NULL,0);
	}

	pDispInfo->dwRefreshRate = 0;
	m_pExecutive->InitializeWithoutRegistry("Geometry.dll","Renderer.dll",
		hWnd, pDispInfo, 10000, 300,0,1, 0,MHErrorHandleProc);

	m_pExecutive->PreCreateLight("light.cfg",0);
	m_pExecutive->GetGeometry()->SetDrawDebugFlag(0);
	m_pExecutive->GetGeometry()->SetViewport(NULL,0);
	m_pExecutive->GetGeometry()->SetAmbientColor(0,0xaaaaaaaa);
	m_pExecutive->GetGeometry()->SetShadowFlag(ENABLE_PROJECTION_SHADOW | ENABLE_PROJECTION_TEXMAP);

	VECTOR3 pos;
	pos.x = pos.y = pos.z = 0;
	m_pExecutive->GetGeometry()->ResetCamera(&pos,100,SightDistance,gPHI/4,0);
	m_pExecutive->GetRenderer()->BeginRender(0,0,0);
	m_pExecutive->GetRenderer()->EndRender();
	m_pExecutive->SetFramePerSec(FPS);
	m_pExecutive->GetRenderer()->SetVerticalSync(FALSE);
	m_pExecutive->GetRenderer()->EnableSpecular(0);
	m_pExecutive->GetRenderer()->SetAlphaRefValue(5);
	return TRUE;
}


BOOL CGraphicEngine::BeginProcess(GX_FUNC pBeforeRenderFunc,GX_FUNC pAfterRenderFunc)
{
	if(NULL == m_pExecutive)
	{
		return FALSE;
	}

#ifdef _GMTOOL_
	if(g_bDisplayFPS)
		m_pExecutive->GetRenderer()->BeginPerformanceAnalyze();
#endif
	if(gTickTime > 100)
		++g_SlowCount;
	else if(g_SlowCount>0)
		--g_SlowCount;
	if(gTickTime < 50)
		++g_FastCount;
	else if(g_FastCount>0)
		--g_FastCount;

	// LUJ, 렌더러에서 종종 메모리 오류가 발생하고 있음. 이에 Release에서는 예외를 처리하도록 함
#ifndef _DEBUG
	try
	{
#endif
		return m_pExecutive->Run(
			m_BackGroungColor,
			pBeforeRenderFunc,
			pAfterRenderFunc,
			TRUE == bRenderSky ? BEGIN_RENDER_FLAG_DONOT_CLEAR_FRAMEBUFFER : 0);
#ifndef _DEBUG
	}
	catch(...)
	{
		return FALSE;
	}
#endif
}

void CGraphicEngine::EndProcess()
{	
#ifdef _GMTOOL_
	if(g_bDisplayFPS)
		m_pExecutive->GetRenderer()->EndPerformanceAnalyze();
#endif

	if( m_bRender )	//KES
		m_pExecutive->GetGeometry()->Present(NULL);

	ProcessGarbageObject();
}

void CGraphicEngine::SetBackGroundColor(DWORD color)
{
	m_BackGroungColor = color;
	g_bColor = color;
}

void CGraphicEngine::SetFixHeight(BOOL bFix,float height)
{
	g_bFixHeight = bFix;
	g_fFixHeight = height;
}

CObjectBase* GetSelectedObject(int MouseX,int MouseY, int PickOption , BOOL bSort )
{
	CEngineObject* pEngineObject = NULL;
	POINT pt = { MouseX, MouseY };
	const int maxPickDesc = 10;

	PICK_GXOBJECT_DESC gxoDesc[maxPickDesc] = {0,};
	DWORD dwPickNum = g_pExecutive->GXOGetMultipleObjectWithScreenCoord( gxoDesc, maxPickDesc, &pt, 0, PickOption /* PICK_TYPE_DEFAULT*/ /*PICK_TYPE_PER_FACE*//*PICK_TYPE_PER_COLLISION_MESH*/ );

	//가져온 PICK_GXOBJECT_DESC[5]를 가까운 fDist 순으로 정렬한다. 
	if( bSort )
	{
		cPtrList SortList;
		PICK_GXOBJECT_DESC TempDesc[maxPickDesc] = {0,};

		//정렬을 위해 원본을 Temp에 복사
		memcpy( TempDesc , gxoDesc , sizeof( PICK_GXOBJECT_DESC ) * maxPickDesc );
	
		for(int i=0; i<maxPickDesc; i++)
		{
			if( ! TempDesc[i].gxo ) 
				continue;

			BOOL bInsert = FALSE;
			PTRLISTPOS pos = SortList.GetHeadPosition();
			while(pos)
			{
				PTRLISTPOS OldPos = pos;
				PICK_GXOBJECT_DESC* pCurDesc = (PICK_GXOBJECT_DESC*)SortList.GetNext(pos);
				if( TempDesc[i].fDist < pCurDesc->fDist )
				{
					//작으면 그노드의 앞으로 삽입  
					SortList.InsertBefore(OldPos,&TempDesc[i]);
					bInsert = TRUE;
					break;
				}
			}
			//자신보다 큰게 없다면 끝에 넣자
			if( ! bInsert )
				SortList.AddTail(&TempDesc[i]);
		}

		//정렬된 리스트를 gxoDesc[5]에 복사 
		DWORD dwcnt=0;
		PTRLISTPOS pos = SortList.GetHeadPosition();
		while(pos)
		{
			gxoDesc[dwcnt] = *(PICK_GXOBJECT_DESC*)SortList.GetNext(pos);
			dwcnt++;
		}
		SortList.RemoveAll();
	}

	for( DWORD i = 0 ; i < dwPickNum ; ++i )
	{
		if(gxoDesc[i].gxo)
		{
			pEngineObject = (CEngineObject*)g_pExecutive->GetData(gxoDesc[i].gxo);
			if( pEngineObject != NULL )
			{
				if( HERO )
				if( HERO->GetEngineObject() != pEngineObject )
				{
					if( pEngineObject->m_pObject )
					{
						if( pEngineObject->m_pObject->GetEngineObjectType() == eEngineObjectType_Monster )
						{
							if( ((CMonster*)(pEngineObject->m_pObject))->GetSInfo()->SpecialType == 2 ) //강아지 클릭 안되도록
							{
								pEngineObject = NULL;
								continue;
							}
						}
					}

					break;	//---pick first one
				}
			}
		}
	}

	if(pEngineObject == NULL)
		return NULL;
	
	return pEngineObject->m_pObject;
}

CObjectBase* GetSelectedObject(PICK_GXOBJECT_DESC* pDesc, int MouseX,int MouseY,int PickOption, BOOL bSort)
{
	CEngineObject* pEngineObject = NULL;
	POINT pt = { MouseX, MouseY };
	DWORD dwPickNum = g_pExecutive->GXOGetMultipleObjectWithScreenCoord( pDesc, 5, &pt, 0, PickOption);

	//가져온 PICK_GXOBJECT_DESC[5]를 가까운 fDist 순으로 정렬한다. 
	if( bSort )
	{
		cPtrList SortList;
		PICK_GXOBJECT_DESC TempDesc[5] = {0,};

		//정렬을 위해 원본을 Temp에 복사
		memcpy( TempDesc , pDesc , sizeof( PICK_GXOBJECT_DESC ) * 5 );

		for(int i=0; i<5; i++)
		{
			if( ! TempDesc[i].gxo ) 
				continue;

			BOOL bInsert = FALSE;
			PTRLISTPOS pos = SortList.GetHeadPosition();
			while(pos)
			{
				PTRLISTPOS OldPos = pos;
				PICK_GXOBJECT_DESC* pCurDesc = (PICK_GXOBJECT_DESC*)SortList.GetNext(pos);
				if( TempDesc[i].fDist < pCurDesc->fDist )
				{
					//작으면 그노드의 앞으로 삽입  
					SortList.InsertBefore(OldPos,&TempDesc[i]);
					bInsert = TRUE;
					break;
				}
			}
			//자신보다 큰게 없다면 끝에 넣자
			if( ! bInsert )
				SortList.AddTail(&TempDesc[i]);
		}

		//정렬된 복사본을원본에 다시 대입 
		DWORD dwcnt=0;
		PTRLISTPOS pos = SortList.GetHeadPosition();
		while(pos)
		{
			pDesc[dwcnt] = *(PICK_GXOBJECT_DESC*)SortList.GetNext(pos);
			dwcnt++;
		}
		SortList.RemoveAll();
	}


	for( DWORD i = 0 ; i < dwPickNum ; ++i )
	{
		if(pDesc[i].gxo)
		{
			pEngineObject = (CEngineObject*)g_pExecutive->GetData(pDesc[i].gxo);

			if( pEngineObject != NULL )
			{
				if( HERO )
				if( HERO->GetEngineObject() != pEngineObject )
				{
					if( pEngineObject->m_pObject )
					{
						if( pEngineObject->m_pObject->GetEngineObjectType() == eEngineObjectType_Monster )
						{
							if( ((CMonster*)(pEngineObject->m_pObject))->GetSInfo()->SpecialType == 2 ) //강아지 클릭 안되도록
							{
								pEngineObject = NULL;
								continue;
							}
						}
					}

					break;	//---pick first one
				}
			}
		}
	}

	if(pEngineObject == NULL)
		return NULL;
	
	return pEngineObject->m_pObject;
}

CObjectBase* GetSelectedObjectBoneCheck(int MouseX, int MouseY)
{
	//static CObjectBase* pSelectedObject;
	CEngineObject* pEngineObject;
	static VECTOR3 pos;
	static float dist;
	static POINT pt;
	pt.x = MouseX;
	pt.y = MouseY;
	

	//---KES Map GXObject Alpha 071020
	//---if need, use GXOGetMultipleObjectWithScreenCoord(). now only used in character select stage.
	//--------------------------------
	DWORD modelindex,objectindex;
	GXOBJECT_HANDLE handle = g_pExecutive->GXOGetObjectWithScreenCoord(&pos,&modelindex,&objectindex,&dist,&pt,0,PICK_TYPE_PER_BONE_OBJECT);	
	if(handle == NULL)
		return NULL;

	pEngineObject = (CEngineObject*)g_pExecutive->GetData(handle);
	if(pEngineObject == NULL)
		return NULL;
	
	return pEngineObject->m_pObject;
}

VECTOR3* GetPickedPosition(int MouseX,int MouseY)
{
	static VECTOR3 TargetPos = {0};

	if(FALSE == g_bFixHeight)
	{
		float fDist = 0;
		POINT pt = {MouseX, MouseY};

		if(const BOOL isFound = g_pExecutive->GXMGetHFieldCollisionPointWithScreenCoord(&TargetPos, &fDist, &pt))
		{
			return &TargetPos;
		}

		return 0;
	}

	SHORT_RECT screenRect = {0};
	WORD value = 0;
	g_pExecutive->GetRenderer()->GetClientRect(
		&screenRect,
		&value,
		&value);

	TargetPos = GetXYZFromScreenXY2(
		g_pExecutive->GetGeometry(),
		MouseX,
		MouseY,
		screenRect.right,
		screenRect.bottom,
		g_fFixHeight);

	return &TargetPos;
}

void AddGarbageObject(GXOBJECT_HANDLE handle)
{
	if(g_pExecutive)
	{
		g_pExecutive->SetData(handle,0);
		GarbageObjectList.AddHead(handle);
	}
}

void ProcessGarbageObject()
{
	GXOBJECT_HANDLE handle;
	while((handle = (GXOBJECT_HANDLE)GarbageObjectList.RemoveTail()) != NULL)
	{
		if(g_pExecutive)
			g_pExecutive->DeleteGXObject(handle);
	}
}

void AddPool(GXOBJECT_HANDLE handle,char* filename)
{
	EffectGarbage* pGarbage = new EffectGarbage;
	strcpy(pGarbage->filename,filename);
	pGarbage->handle = handle;
	EffectPool.AddHead(pGarbage);
}
GXOBJECT_HANDLE GetObjectHandle(char* szFileName,GXSchedulePROC pProc,void* pData,DWORD dwFlag)
{
	EffectGarbage* pGarbage;
	PTRLISTPOS pos = EffectPool.GetHeadPosition();
	PTRLISTPOS beforepos;
	while(pos)
	{
		beforepos = pos;
		pGarbage = (EffectGarbage *)EffectPool.GetNext(pos);
		if(strcmp(pGarbage->filename,szFileName)==0)
		{
			EffectPool.RemoveAt(beforepos);
			GXOBJECT_HANDLE h = pGarbage->handle;
			g_pExecutive->SetData(h,pData);
			delete pGarbage;
			return h;
		}
	}

	return g_pExecutive->CreateGXObject(szFileName,pProc,pData,dwFlag);
}

void ReleasePool()
{
	EffectGarbage* pGarbage;
	while((pGarbage = (EffectGarbage*)EffectPool.RemoveTail())!=NULL)
	{
		delete pGarbage;
	}
}

BOOL IsGameSlow()
{
	return g_SlowCount > 5;
}

BOOL IsGameFast()
{
	return g_FastCount > 5;
}

void GetFieldHeight(VECTOR3* pPos)
{
	g_pExecutive->GXMGetHFieldHeight(&pPos->y,pPos->x,pPos->z);
}


BOOL GetCollisonPointWithRay(VECTOR3& From, VECTOR3& To,float height,VECTOR3& Result)
{
	if(From.y <= To.y)
		return FALSE;
	
	float	t = (-1*(height+From.y)) / (To.y - From.y);		
	float	x = From.x+t * (To.x-From.x);
	float	z = From.z+t * (To.z-From.z);
	Result.x	=	x;
	Result.y	=	height;
	Result.z	=	z;
	
	return	TRUE;
}
