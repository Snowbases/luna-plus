// GridSystem.cpp: implementation of the CGridSystem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GridSystem.h"
#include "Object.h"
#include "GridTable.h"
#include "PackedData.h"
#include "Network.h"
#include "CharMove.h"
#include "ServerTable.h"
#include "BattleSystem_Server.h"
#include "GeneralGridTable.h"
#include "MurimGridTable.h"
#include "ChannelSystem.h"
#include "Player.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGridSystem::CGridSystem()
{
//	m_pGridTable = NULL;
	m_GridTable.Initialize(10);
}

CGridSystem::~CGridSystem()
{
	Release();
}

void CGridSystem::Init()
{}

void CGridSystem::Release()
{
	CGridTable* pInfo;
	m_GridTable.SetPositionHead();
	while( (pInfo = m_GridTable.GetData()) != NULL)
	{
		pInfo->Release();
		delete pInfo;
	}
	m_GridTable.RemoveAll();
}

void CGridSystem::DeleteGridTable(DWORD GridID)
{
	CGridTable* pGridTable = (CGridTable*)m_GridTable.GetData(GridID);
	ASSERT(pGridTable);
	pGridTable->OnDelete();
	pGridTable->Release();
	m_GridTable.Remove(GridID);
	delete pGridTable;
}

void CGridSystem::ChangeGrid(CObject* pObject,VECTOR3* pPos)
{	
	if(pObject->GetInited() ==FALSE)
		return;
	if(pObject->GetInitedGrid() == FALSE)
		return;

	CGridTable* pGridTable = GetGridTable(pObject);
	if(pGridTable == NULL)
		return;

	pGridTable->ChangeGrid(pObject, pPos);
}

void CGridSystem::AddObject(CObject* pObject,VECTOR3* pPos)
{
	CGridTable* const pGridTable = GetGridTable(pObject);

	if(0 == pGridTable)
		return;

	pGridTable->AddObject(pObject, pPos);

	// 091026 LUJ, 버퍼 크기에 무관하게 전송할 수 있도록 수정
	MSGBASE* message = 0;
	const DWORD messageLength = pObject->SetAddMsg(
		0,
		TRUE,
		message);
	PACKEDDATA_OBJ->QuickSendExceptObjectSelf(
		pObject,
		message,
		messageLength);
	pGridTable->SendCurrentObjectInfo(
			pObject->GridPos.x,
			pObject->GridPos.z,
			-1,
			-1,
			pObject);
	pObject->SetInitedGrid();
}

void CGridSystem::RemoveObject(CObject* pObject)
{
	if(pObject->m_bGridInited == FALSE)
		return;
	
	CGridTable* const pGridTable = GetGridTable(pObject);

	if(0 == pGridTable)
		return;

	MSGBASE* message = 0;
	const DWORD messageLength = pObject->SetRemoveMsg(
		0,
		message);
	PACKEDDATA_OBJ->QuickSendExceptObjectSelf(
		pObject,
		message,
		messageLength);
	pGridTable->RemoveObject(pObject);
	pObject->GridPos.x = WORD( -1 );
	pObject->GridPos.z = WORD( -1 );
	pObject->GridPos.LastX = WORD( -1 );
	pObject->GridPos.LastZ = WORD( -1 );

	pObject->m_bGridInited = FALSE;
}

void CGridSystem::AddGridToPackedData(GridPosition* pGridPosition,CPackedData* pPackedData, CObject* pObject)
{
	CGridTable * pGridTable = GetGridTable(pObject);
	if(pGridTable == NULL)
		return;
	pGridTable->AddGridToPackedData(pGridPosition, pPackedData);
//	m_pGridTable[Level].AddGridToPackedData(GridNumX,GridNumZ,pPackedData);
}

void CGridSystem::AddGridToPackedDataWithoutOne(GridPosition* pGridPosition,DWORD dwID,CPackedData* pPackedData, CObject* pObject)
{
	CGridTable * pGridTable = GetGridTable(pObject);
	if(pGridTable == NULL)
		return;
	pGridTable->AddGridToPackedDataWithoutOne(pGridPosition, dwID, pPackedData);
	
//	m_pGridTable[Level].AddGridToPackedDataWithoutOne(GridNumX,GridNumZ,dwID,pPackedData);
}

void CGridSystem::AddGridToPackedDataExceptLastGrid(GridPosition* pGridPosition,DWORD ExceptID,CPackedData* pPackedData, CObject* pObject)
{	
	CGridTable * pGridTable = GetGridTable(pObject);
	if(pGridTable == NULL)
		return;
	pGridTable->AddGridToPackedDataExceptLastGrid(pGridPosition, ExceptID, pPackedData);
	
//	m_pGridTable[Level].AddGridToPackedDataExceptLastGrid(pGridPosition,ExceptID,pPackedData);
}

void CGridSystem::AddLastGridToPackedDataExceptCurrentGrid(GridPosition* pGridPosition,DWORD ExceptID,CPackedData* pPackedData, CObject* pObject)
{
	CGridTable * pGridTable = GetGridTable(pObject);
	if(pGridTable == NULL)
		return;
	pGridTable->AddLastGridToPackedDataExceptCurrentGrid(pGridPosition,ExceptID,pPackedData);

//	m_pGridTable[Level].AddLastGridToPackedDataExceptCurrentGrid(pGridPosition,ExceptID,pPackedData);
}

CObject* CGridSystem::FindPlayerInRange(VECTOR3* pPos,float Range, CObject* pObject, BYTE flag)
{
	int firstx = TransToGridCoordinate(pPos->x);
	int firstz = TransToGridCoordinate(pPos->z);

	GridPosition GridPos;
	GridPos.x = WORD( firstx >> GRID_BIT );
	GridPos.z = WORD( firstz >> GRID_BIT );
	CGridTable * pGridTable = GetGridTable(pObject);
	if(pGridTable == NULL)
		return NULL;

	return pGridTable->FindPlayerInRange(pObject,&GridPos, pPos, Range, flag);
}
CObject* CGridSystem::FindMonsterInRange(VECTOR3* pPos,float Range, CObject* pObject,BYTE flag)
{
	int firstx = TransToGridCoordinate(pPos->x);
	int firstz = TransToGridCoordinate(pPos->z);

	GridPosition GridPos;
	GridPos.x = WORD( firstx >> GRID_BIT );
	GridPos.z = WORD( firstz >> GRID_BIT );
	CGridTable * pGridTable = GetGridTable(pObject);
	if(pGridTable == NULL)
		return NULL;

	return pGridTable->FindMonsterInRange(pObject,&GridPos, pPos, Range, flag);
}

WORD CGridSystem::FindMonsterGroupInRange(DWORD* pFriendsList, VECTOR3* pPos, CObject* pObject)
{
	CGridTable * pGridTable = GetGridTable(pObject);
	if(pGridTable == NULL)
		return 0;

	int firstx = TransToGridCoordinate(pPos->x);
	int firstz = TransToGridCoordinate(pPos->z);

	GridPosition GridPos;
	GridPos.x = WORD( firstx >> GRID_BIT );
	GridPos.z = WORD( firstz >> GRID_BIT );

	return pGridTable->FindMonsterGroupInRange(pFriendsList, pObject,&GridPos, pPos);
}

CGridTable* CGridSystem::GetGridTable(CObject* pObject)
{
	DWORD GridID = pObject->GetGridID();
	CGridTable* pGridTable = (CGridTable*)m_GridTable.GetData(GridID);
	ASSERT(pGridTable);
	
	return pGridTable;
}

MAPTYPE CGridSystem::GetGridMapNum(DWORD GridID)
{
	CGridTable* pGridTable = (CGridTable*)m_GridTable.GetData(GridID);
	ASSERT(pGridTable);
	
	return pGridTable->GetMapNum();
}

void CGridSystem::CreateGridTable(DWORD GridID, BYTE Kind, int MaxWidth, int MaxHeight, MAPTYPE MapNum)
{
	CGridTable* pInfo = NULL;
	switch(Kind)
	{
	case eGridType_General:
		{
			pInfo = new CGeneralGridTable;
			pInfo->Init(GridID, MaxWidth, MaxHeight);
		}
		break;
	case eGridType_Murim:
		{
			pInfo = new CMurimGridTable;
			pInfo->Init(GridID, MaxWidth, MaxHeight);
		}
		break;
	}
	ASSERT(pInfo);
	if(pInfo == NULL)
		return;

	pInfo->SetMapNum(MapNum);

	m_GridTable.Add(pInfo, GridID);
}

BOOL CGridSystem::GetGridChangeTime(CObject* pObject,VECTOR3* pStartPos,VECTOR3* pTargetPos,float MoveSpeed,DWORD& dwChangeTime)
{
	CGridTable * pGridTable = GetGridTable(pObject);
	if(pGridTable == NULL)
		return FALSE;
	return pGridTable->GetGridChangeTime(pStartPos,pTargetPos,MoveSpeed,dwChangeTime);
}

//////////////////////////////////////////////////////////////////////////
/// 06. 08. 2차 보스 - 이영준
/// 유효그리드내 랜덤으로 타겟 설정
CObject* CGridSystem::FindPlayerRandom(CObject* pObject)
{
	CGridTable * pGridTable = GetGridTable(pObject);
	if(pGridTable == NULL)
		return FALSE;

	return pGridTable->FindPlayerRandom(pObject);
}	
//////////////////////////////////////////////////////////////////////////