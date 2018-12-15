#include "stdafx.h"
#include "QuestManager.h"
#include "Interface/GameNotifyManager.h"
#include "MHFile.h"
#include "..\[CC]Quest\QuestScriptLoader.h"
#include "..\[CC]Quest\QuestString.h"
#include "..\[CC]Quest\QuestInfo.h"
#include "..\[CC]Quest\SubQuestInfo.h"
#include "..\[CC]Quest\QuestExecute.h"
#include "..\[CC]Quest\QuestNpc.h"
#include "..\[CC]Quest\QuestNpcData.h"
#include "..\[CC]Quest\QuestNpcScript.h"
#include "Quest.h"
#include "QuestUpdater.h"
#include "ObjectManager.h"
#include "ChatManager.h"
#include "QuestItemManager.h"
#include "UserInfoManager.h"
#include "GameIn.h"
#include "ItemManager.h"
#include "InventoryExDialog.h"
#include "WindowIdEnum.h"
#include "./Interface/cWindowManager.h"
#include "cDivideBox.h"
#include "cMsgBox.h"
#include "QuickManager.h"
#include "Npc.h"
#include "MHMap.h"
#include "MiniMapDlg.h"
#include "QuestDialog.h"
#include "QuestQuickViewDialog.h"
#include "StatusIconDlg.h"
#include "NpcScriptDialog.h"
#include "NpcImageDlg.h"
#include "QuestTotalDialog.h"
#include "OptionManager.h"
#include "WorldMapDlg.h"
#include "BigMapDlg.h"

GLOBALTON(CQuestManager);																// 퀘스트 매니져를 글로벌 톤으로 함.

CQuestManager::CQuestManager()															// 생성자 함수.
{
	m_QuestInfoTable.Initialize(MAX_QUEST);												// 최대 퀘스트 수만큼(200) 퀘스트 정보 테이블을 초기화 한다.
	m_QuestStringTable.Initialize(200);													// 최대 퀘스트 수만큼(200) 퀘스트 스트링 테이블을 초기화 한다.
	m_QuestTable.Initialize(MAX_QUEST);													// 최대 퀘스트 수만큼(200) 퀘스트 테이블을 초기화 한다.
	m_QuestNpcTable.Initialize(100);													// 100개 만큼 퀘스트 npc 테이블을 초기화 한다.

	m_QuestUpdater = new CQuestUpdater;													// 퀘스트 업데이터를 새로 할당한다.

	ZeroMemory(
		m_pNpcArray,
		sizeof(m_pNpcArray));
	m_dwMaxNpc = 0;																		// 최대 npc를 담을 변수를 0으로 초기화 한다.
	
	m_dwLastMsg = 0;
	ZeroMemory(
		m_QuickViewQuestID,
		sizeof(m_QuickViewQuestID));
}

CQuestManager::~CQuestManager()															// 소멸자 함수.
{
	Release();
	SAFE_DELETE(
		m_QuestUpdater);
}

void CQuestManager::Release()															// 해제 함수.
{
	{
		m_QuestInfoTable.SetPositionHead();

		for(CQuestInfo* pQuestInfo = m_QuestInfoTable.GetData();
			0 < pQuestInfo;
			pQuestInfo = m_QuestInfoTable.GetData())
		{
			SAFE_DELETE(
				pQuestInfo);
		}

		m_QuestInfoTable.RemoveAll();
	}

	{
		m_QuestStringTable.SetPositionHead();

		for(CQuestString* pQuestString = m_QuestStringTable.GetData();
			0 < pQuestString;
			pQuestString = m_QuestStringTable.GetData())
		{
			SAFE_DELETE(
				pQuestString);
		}

		m_QuestStringTable.RemoveAll();
	}

	{
		m_QuestTable.SetPositionHead();

		for(CQuest* pQuest = m_QuestTable.GetData();
			0 < pQuest;
			pQuest = m_QuestTable.GetData())
		{
			SAFE_DELETE(
				pQuest);
		}

		m_QuestTable.RemoveAll();
	}

	{
		m_QuestNpcTable.SetPositionHead();

		for(QUESTNPCINFO* pData = m_QuestNpcTable.GetData();
			0 < pData;
			pData = m_QuestNpcTable.GetData())
		{
			SAFE_DELETE(
 				pData);
		}

		m_QuestNpcTable.RemoveAll();
	}

	m_dwLastMsg = 0;
}

BOOL CQuestManager::LoadQuestScript( char* strFileName )								// 퀘스트 스크립트를 로딩하는 함수.
{
	CMHFile file;																		// 묵향 파일을 선언한다.

	if( !file.Init( strFileName, "rb") )												// 들어온 파일이름과, 열기 모드에 따라 파일을 열고, 결과를 받는다.
	{																					// 실패했다면,
		return FALSE;																	// FALSE를 리턴한다.
	}

	char Token[256];																	// 임시 버퍼를 선언한다.

	DWORD dwQuestNum = 0;																// 퀘스트 번호를 받을 변수를 선언, 0으로 초기화 한다.

	int nMapNum = 0 ;

	DWORD dwNpcIdx = 0 ;

	while( !file.IsEOF() )																// 파일의 끝이 아닌 동안 while문을 돌린다.
	{
		file.GetString( Token );														// 스트링을 읽어 임시 버퍼에 담는다.

		if( strcmp(Token,"$QUEST") == 0 )												// 읽어들인 스트링이 $QUEST와 같다면,
		{
			dwQuestNum = file.GetDword();												// 퀘스트 번호를 읽어들인다.

			nMapNum = file.GetInt() ;													// 맵 번호를 읽어들인다.

			dwNpcIdx = file.GetDword() ;												// npc 인덱스를 읽어들인다.

			CQuestInfo* pQuestInfo = CQuestScriptLoader::LoadQuestInfo( &file, dwQuestNum ) ;

			if( pQuestInfo )
			{
				pQuestInfo->SetMapNum(nMapNum) ;
				pQuestInfo->SetNpcIdx(dwNpcIdx) ;
			}

			m_QuestInfoTable.Add( pQuestInfo, dwQuestNum );	// 퀘스트 정보 테이블에 퀘스트를 담는다.
		}
	}

	CreateAllQuestFromScript();															// 스크립트로 부터 모든 퀘스트를 생성한다.

	return TRUE;																		// TRUE를 리턴한다.
}

BOOL CQuestManager::LoadQuestString(char* strFileName)									// 퀘스트 스트링을 로딩하는 함수.
{
	CMHFile file;																		// 묵향 파일을 선언한다.

	if( !file.Init( strFileName, "rb") )												// 들어온 파일이름과, 열기 모드에 따라 파일을 열고, 결과를 받는다.
	{																					// 실패했다면,
		return FALSE;																	// FALSE를 리턴한다.
	}

	char Token[256];																	// 임시 버퍼를 선언한다.
	
	while( !file.IsEOF() )																// 파일의 끝이 아닌동안 while문을 돌린다.
	{
		file.GetString(Token);															// 스트링을 읽어 임시 버퍼에 담는다.

		if(strcmp(Token,"$SUBQUESTSTR") == 0)											// 읽어들인 스트링이 $SUBQUESTSTR과 같다면,
		{
			DWORD Key_1 = file.GetDword();												// 키1을 읽어들인다.
			DWORD Key_2 = file.GetDword();												// 키2를 읽어들인다.
			DWORD Key = 0;

			COMBINEKEY(Key_1, Key_2, Key);												// 키 변수에 키1, 키2를 결합하여 받는다.
			CQuestString* pQuestString = CQuestScriptLoader::LoadQuestString(&file);	// 퀘스트 스트링을 로딩하여 퀘스트 스트링 포인터로 받는다.

			ASSERT( pQuestString && "Why is it null?" );								// 퀘스트 스트링 정보가 유효해야 한다. assert처리.
			ASSERT( ! m_QuestStringTable.GetData( Key ) && "This memory'll be overwrite. It's reason of memory leak" );	// 퀘스트 스트링 테이블을 검사한다.
			
			m_QuestStringTable.Add(pQuestString, Key);									// 퀘스트 스트링 테이블에 퀘스트 스트링 정보를 담는다.
			pQuestString->SetIndex(Key_1, Key_2);										// 퀘스트 스트링에 키1, 키2를 인덱스로 세팅한다.
		}
	}

	//
	m_QuestItemMGR.QuestItemInfoLoad();													// 퀘스트 아이템 매니져로 퀘스트 아이템 정보를 로딩한다.

	return TRUE;																		// TRUE를 리턴한다.
}

void CQuestManager::LoadQuestNpc( char* strFileName )									// 퀘스트 npc를 로딩하는 함수.
{
	CMHFile file;																		// 묵향 파일을 선언한다.

	if( !file.Init( strFileName, "rb") )												// 들어온 파일이름과, 열기 모드에 따라 파일을 열고, 결과를 받는다.
	{																					// 실패했다면,
		return ;																		// 리턴 처리를 한다.
	}
	
	while( !file.IsEOF() )																// 파일의 끝이 아닌동안 while문을 돌린다.
	{
		QUESTNPCINFO* pData = new QUESTNPCINFO;											// 퀘스트 npc정보를 담을 메모리를 할당하고, 포인터로 받는다.
		pData->dwObjIdx = 0;															// 오브젝트 인덱스를 0으로 세팅한다.
		pData->wMapNum = file.GetWord();												// 맵 번호를 받는다.
		pData->wNpcKind = file.GetWord();												// npc 종류를 받는다.
//		file.GetString( pData->sName );
		SafeStrCpy( pData->sName, file.GetString(), MAX_NAME_LENGTH+1 );				// npc 이름을 받는다.
		pData->wNpcIdx = file.GetWord();												// npc 인덱스를 받는다.
		pData->vPos.x = file.GetFloat();												// npc 위치를 받는다.
		pData->vPos.y = 0;
		pData->vPos.z = file.GetFloat();
		pData->fDir = file.GetFloat();													// npc 방향?을 받는다.

		m_QuestNpcTable.Add( pData, pData->wNpcIdx );									// npc 테이블에 npc정보를 담는다.
	}
}

void CQuestManager::CreateAllQuestFromScript()											// 스크립트로 부터 모든 퀘스트를 생성하는 함수.
{
	m_QuestInfoTable.SetPositionHead();

	for(CQuestInfo* pQuestInfo = m_QuestInfoTable.GetData();
		0 < pQuestInfo;
		pQuestInfo = m_QuestInfoTable.GetData())
	{
		CQuest* pQuest = new CQuest( pQuestInfo );										// 퀘스트 정보에 따라 퀘스트 메모리를 할당하고, 퀘스트 포인터로 받는다.
		m_QuestTable.Add( pQuest, pQuestInfo->GetQuestIdx() );							// 퀘스트 테이블에 추가를 한다.

		// 100719 ShinJS NPC가 가진 퀘스트 목록에 추가
		const DWORD dwNpcUniqueIdx = pQuestInfo->GetNpcIdx();
		const DWORD dwQuestIdx = pQuestInfo->GetQuestIdx();

		NpcHasQuestList& hasQuestList = m_NpcHasQuestList[ dwNpcUniqueIdx ];
		hasQuestList.insert( dwQuestIdx );

		pQuest->AddNpcHasQuest();
	}
}

void CQuestManager::InitializeQuest()													// 퀘스트를 초기화 하는 함수.
{
	m_QuestTable.SetPositionHead();

	for(CQuest* pQuest = m_QuestTable.GetData();
		0 < pQuest;
		pQuest = m_QuestTable.GetData())
	{
		pQuest->Initialize();

		CQuestInfo* pQuestInfo = m_QuestInfoTable.GetData( pQuest->GetQuestIdx() );
		if( !pQuestInfo )
			continue;

		// 100405 ShinJS --- 시간제한 퀘스트 등록
		if( (WORD)pQuestInfo->GetMapNum() == MAP->GetMapNum() &&
			pQuest->HasDateLimit() )
		{
			pQuest->InitDateTime();
			m_setDateLimitQuest.insert( pQuest->GetQuestIdx() );
		}
	}

	m_dwMaxNpc = 0;
	m_dwLastMsg = 0;
}

void CQuestManager::SetMainQuestData( DWORD dwQuestIdx, DWORD dwSubQuestFlag, DWORD dwData, __time64_t regisTime, BYTE bCheckType, DWORD dwCheckTime )
{
	if( dwQuestIdx == 0 )																// 퀘스트 인덱스가 0과 같다면, // Newbieguide
	{
		//m_NewbieGuide.SetFlagData( dwSubQuestFlag, dwData, dwTime );					// 초보자 가이드의 플래그 데이터를 세팅한다.
		return;																			// 리턴 처리를 한다.
	}
	else																				// 퀘스트 인덱스가 0이 아니라면,
	{
		CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );							// 퀘스트 테이블에서 퀘스트 인덱스의 퀘스트 정보를 받는다.

		if( !pQuest ) return ;															// 퀘스트 정보가 유효하다면,

		pQuest->SetMainQuestData( dwSubQuestFlag, dwData, regisTime, bCheckType, dwCheckTime );	// 메인 퀘스트 데이터를 세팅한다.

		if( pQuest->IsQuestComplete() ) return ;
		m_QuestDialog->StartQuest(dwQuestIdx) ;
	}
}

void CQuestManager::SetSubQuestData( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwData, DWORD dwTime )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );								// 퀘스트 테이블에서 들어온 퀘스트 인덱스로 퀘스트 정보를 받는다.

	if( !pQuest ) return ;																// 퀘스트 정보가 유효한지 체크한다.

	pQuest->SetSubQuestData( dwSubQuestIdx, dwData, dwTime );						// 퀘스트의 서브 퀘스트를 세팅한다.

	AddQuestNpc( pQuest->GetSubQuestInfo( dwSubQuestIdx ) );						// 퀘스트 npc를 추가한다.

	m_QuestDialog->RefreshQuestList() ;
}

void CQuestManager::AddMapTree(int nMapNum)												// 맵 트리를 추가하는 함수.
{	
	CQuestTotalDialog * pTotalDialog = GAMEIN->GetQuestTotalDialog() ;					// 퀘스트 토탈 다이얼로그 정보를 받는다.

	if( pTotalDialog )																	// 토탈 다이얼로그 정보가 유효하면,
	{
		CQuestDialog* pQuestDlg = pTotalDialog->GetQuestDlg() ;							// 퀘스트 다이얼로그 정보를 받는다.

		if( pQuestDlg )																	// 퀘스트 다이얼로그 정보가 유효하면,
		{
			cPtrList& pTreeList = pQuestDlg->GetTreeList() ;										// 트리 리스트를 받는다.			

			BOOL bResult = FALSE ;													// 검색 결과를 담을 변수를 선언하고 FALSE 처리를 한다.

			int nTreeCount = pTreeList.GetCount() ;								// 트리 카운트를 받는다.

			QuestTree* pTree = NULL ;												// 트리 정보를 받을 포인터를 선언하고 NULL 처리를 한다.

			PTRLISTPOS pTreePos = NULL ;											// 트리의 위치 정보를 받을 포인터를 선언하고 NULL처리를 한다.

			for( int count = 0 ; count < nTreeCount ; ++count )						// 트리 카운트 만큼 FOR문을 돌린다.
			{
				pTreePos = pTreeList.FindIndex(count) ;							// 카운트에 해당하는 위치 정보를 받는다.

				if( pTreePos )														// 트리 위치 정보가 유효하다면,
				{
					pTree = (QuestTree*)pTreeList.GetAt(pTreePos) ;				// 위치에 해당하는 트리 정보를 받는다.

					if( pTree )														// 트리 정보가 유효하다면,
					{
						if( pTree->nMapNum == nMapNum )								// 현재 퀘스트의 맵 번호와 트리의 맵 번호를 비교해서 같으면,
						{
							bResult = TRUE ;										// 결과를 TRUE로 세팅한다.

							break ;													// for문을 빠져나간다.
						}
					}
				}
			}

			if( bResult == FALSE )													// 결과가 FALSE와 같으면,
			{
				pQuestDlg->AddQuestTree(nMapNum) ;
			}
		}
	}
}

void CQuestManager::DeleteMapTree(int nMapNum)
{
	CQuestTotalDialog * pTotalDialog = GAMEIN->GetQuestTotalDialog() ;					// 퀘스트 토탈 다이얼로그 정보를 받는다.

	if( pTotalDialog )																	// 토탈 다이얼로그 정보가 유효하면,
	{
		CQuestDialog* pQuestDlg = pTotalDialog->GetQuestDlg() ;							// 퀘스트 다이얼로그 정보를 받는다.

		if( pQuestDlg )																	// 퀘스트 다이얼로그 정보가 유효하면,
		{
			cPtrList& pTreeList = pQuestDlg->GetTreeList() ;										// 트리 리스트를 받는다.			

			int nTreeCount = pTreeList.GetCount() ;								// 트리 카운트를 받는다.

			QuestTree* pTree = NULL ;												// 트리 정보를 받을 포인터를 선언하고 NULL 처리를 한다.

			PTRLISTPOS pTreePos = NULL ;											// 트리의 위치 정보를 받을 포인터를 선언하고 NULL처리를 한다.

			for( int count = 0 ; count < nTreeCount ; ++count )						// 트리 카운트 만큼 FOR문을 돌린다.
			{
				pTreePos = pTreeList.FindIndex(count) ;							// 카운트에 해당하는 위치 정보를 받는다.

				if( pTreePos )														// 트리 위치 정보가 유효하다면,
				{
					pTree = (QuestTree*)pTreeList.GetAt(pTreePos) ;				// 위치에 해당하는 트리 정보를 받는다.

					if( pTree )														// 트리 정보가 유효하다면,
					{
						if( pTree->nMapNum == nMapNum )								// 현재 퀘스트의 맵 번호와 트리의 맵 번호를 비교해서 같으면,
						{
							if( pTree->list.GetCount() == 0 )						// 트리의 카운트가 0개이면,
							{
								pTreeList.RemoveAt(pTreePos) ;						// 현재 트리를 삭제한다.
							}
						}
					}
				}
			}
		}
	}
}

void CQuestManager::AddQuestTitle(int nMapNum, DWORD dwQuestIdx)						// 퀘스트 다이얼로그에 퀘스트 제목을 추가하는 함수.
{
	if( dwQuestIdx < 1 )																// 퀘스트 인덱스가 1보다 작으면,
	{
		return ;																		// 리턴 처리를 한다.
	}

	int nKey = 0 ;																		// 스트링 키 값을 담을 변수를 선언하고 0으로 세팅한다.

	COMBINEKEY(dwQuestIdx, 0, nKey) ;													// 퀘스트 인덱스와 0으로 키 값을 세팅한다.
	
	CQuestString* pQuestString = NULL ;													// 퀘스트 스트링 정보를 받을 포인터를 선언하고 null 처리를 한다.
	pQuestString = m_QuestStringTable.GetData(nKey) ;									// 컴바인한 키 값으로 퀘스트 스트링을 받는다.

	if( !pQuestString )																	// 퀘스트 스트링 정보가 유효하지 않다면,
	{
		return ;																		// 리턴처리를 한다.
	}

	CQuestDialog* pQuestDlg = NULL ;													// 퀘스트 다이얼로그 정보를 받을 포인터를 선언하고 null처리를 한다.
	pQuestDlg = GAMEIN->GetQuestDialog() ;											// 퀘스트 다이얼로그 정보를 받는다.

	if( pQuestDlg )
	{
		pQuestDlg->AddQuestTitle(pQuestString, nMapNum) ;
	}
}

void CQuestManager::DeleteQuestTitle(int nMapNum, DWORD dwQuestIdx)						// 퀘스트 리스트에서 퀘스트 제목을 삭제하는 함수.
{
	if( dwQuestIdx < 1 )																// 퀘스트 인덱스가 1보다 작으면,
	{
		ASSERTMSG(0, "CQuestManager::AddQuestTitle\n퀘스트 제목을 삭제하던 중, 퀘스트 인덱스가 1보다 작은 값이 넘어왔다.!!!") ;	// ASSERT 에러 처리.

		return ;																		// 리턴 처리를 한다.
	}

	int nKey = 0 ;																		// 스트링 키 값을 담을 변수를 선언하고 0으로 세팅한다.

	COMBINEKEY(dwQuestIdx, 0, nKey) ;													// 퀘스트 인덱스와 0으로 키 값을 세팅한다.
	
	CQuestString* pQuestString = NULL ;													// 퀘스트 스트링 정보를 받을 포인터를 선언하고 null 처리를 한다.
	pQuestString = m_QuestStringTable.GetData(nKey) ;									// 컴바인한 키 값으로 퀘스트 스트링을 받는다.

	if( !pQuestString )																	// 퀘스트 스트링 정보가 유효하지 않다면,
	{
		ASSERTMSG(0, "CQuestManager::AddQuestTitle\n퀘스트 제목을 삭제하던 중 퀘스트 스트링 정보가 유효하지 않다.") ;	// ASSERT 에러 처리.

		return ;																		// 리턴처리를 한다.
	}

	CQuestTotalDialog * pTotalDialog = NULL ;											// 퀘스트 토탈 정보를 받을 포인터를 선언하고 null 처리를 한다.
	pTotalDialog = GAMEIN->GetQuestTotalDialog() ;										// 퀘스트 토탈 다이얼로그 정보를 받는다.

	if( !pTotalDialog )
	{
		ASSERTMSG(0, "CQuestManager::AddQuestTitle\n퀘스트 제목을 삭제하던 중 퀘스트 토탈 정보가 유효하지 않다.") ;	// ASSERT 에러 처리.
		
		return ;																		// 리턴 처리를 한다.
	}

	CQuestDialog* pQuestDlg = NULL ;													// 퀘스트 다이얼로그 정보를 받을 포인터를 선언하고 null처리를 한다.
	pQuestDlg = pTotalDialog->GetQuestDlg() ;											// 퀘스트 다이얼로그 정보를 받는다.

	if( !pQuestDlg )
	{
		ASSERTMSG(0, "CQuestManager::AddQuestTitle\n퀘스트 제목을 삭제하던 중 퀘스트 다이얼로그 정보를 받지 못했다.") ;	// ASSERT 에러 처리.
		
		return ;																		// 리턴 처리를 한다.
	}

	cPtrList& pTreeList = pQuestDlg->GetTreeList() ;												// 트리 리스트를 받는다.			
	BOOL bResult = FALSE ;																// 검색 결과를 담을 변수를 선언하고 FALSE 처리를 한다.
	QuestTree* pTree = NULL ;															// 트리 정보를 받을 포인터를 선언하고 NULL 처리를 한다.
	PTRLISTPOS pTreePos = NULL ;														// 트리의 위치 정보를 받을 포인터를 선언하고 NULL처리를 한다.
	PTRLISTPOS pDeletePos = NULL ;														// 삭제할 퀘스트 스트링 위치를 나타내는 포인터를 선언하고 NULL 처리를 한다.

	for(int count = 0 ; count < pTreeList.GetCount(); ++count)
	{
		pTreePos = pTreeList.FindIndex(count) ;										// 카운트에 해당하는 위치 정보를 받는다.

		if( pTreePos )																	// 트리 위치 정보가 유효하다면,
		{
			pTree = (QuestTree*)pTreeList.GetAt(pTreePos) ;							// 위치에 해당하는 트리 정보를 받는다.

			if( pTree )																	// 트리 정보가 유효하다면,
			{
				if( pTree->nMapNum == nMapNum )											// 현재 퀘스트의 맵 번호와 트리의 맵 번호를 비교해서 같으면,
				{
					int nQuestCount = pTree->list.GetCount() ;							// 퀘스트 제목 카운트를 받는다.

					CQuestString* pDelete = NULL ;										// 삭제 할 퀘스트 스트링을 받을 포인터를 선언하고 null 처리를 한다.

					for( int count = 0 ; count < nQuestCount ; ++count )				// 퀘스트 제목 수 만큼 for문을 돌린다.
					{
						pDeletePos = pTree->list.FindIndex(count) ;						// 카운트에 해당하는 삭제 위치를 받는다.

						if( pDeletePos )												// 삭제 위치 정보가 유효하면,
						{
							pDelete = (CQuestString*)pTree->list.GetAt(pDeletePos) ;	// 삭제 할 스트링 정보를 받는다.

							if( pDelete )												// 스트링 정보가 유효하면,
							{
								if( pDelete->GetKey() == pQuestString->GetKey() )		// 트리의 스트링과, 삭제할 스트링의 키 값이 같으면,
								{
									bResult = TRUE ;									// 결과 값을 TRUE로 세팅한다.

									//pTree->list.RemoveAt(pDeletePos) ;					// 트리에서 스트링을 삭제한다.
									CQuestString* pDeleteString = (CQuestString*)pTree->list.GetAt(pDeletePos) ;

									if( pDeleteString )
									{
										pTree->list.Remove(pDeleteString) ;
										delete pDeleteString ;
									}
								}
							}
						}
					}

					break ;																// for문을 빠져나간다.
				}
			}
		}
	}

#ifdef _GMTOOL_
	if( bResult == FALSE )
	{
		OutputDebugString("CQuestManager::AddQuestTitle\n같은 맵에 퀘스트를 삭제하지 못함\n");
	}
#endif
}


void CQuestManager::StartQuest( DWORD dwQuestIdx, DWORD dwSubQuestFlag, DWORD dwData, __time64_t registTime )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );								// 퀘스트 테이블에서 들어온 퀘스트 인덱스로 퀘스트 정보를 받는다.

	pQuest->StartQuest( dwSubQuestFlag, dwData, registTime );							// 퀘스트를 시작한다.

	m_QuestDialog->StartQuest(dwQuestIdx) ;

	int Key = 0;																	// 키값을 받을 변수를 선언한고, 0으로 세팅한다.
	COMBINEKEY( dwQuestIdx, 0, Key );												// 퀘스트 인덱스와 0을 결합하여 키 값을 받는다.

	// title
	CQuestString* pQString = m_QuestStringTable.GetData(Key);						// 키 값으로 퀘스트 스트링 정보를 받는다.

	if( pQString == NULL )															// 퀘스트 스트링 정보가 유효하지 않다면,
	{
		return;																		// 리턴 처리를 한다.
	}
	cPtrList* p = pQString->GetTitle();												// 리스트 포인터를 선언하여, 제목을 받는다.
	ITEM* string = (ITEM*)p->GetHead();										// 리스트의 헤드를 퀘스트 스트링 포인터에 받는다.

	CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(221), string->string );			// 퀘스트를 시작한다는 메시지를 출력한다.
}

void CQuestManager::EndQuest( DWORD dwQuestIdx, DWORD dwSubQuestFlag, DWORD dwData, __time64_t registTime )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );								// 퀘스트 테이블에서 들어온 퀘스트 인덱스로 퀘스트 정보를 받는다.

	if( !pQuest ) return ;

	if( pQuest->IsTimeCheck() )															// 시간 체크가 필요한 퀘스트라면,
	{
		STATUSICONDLG->RemoveQuestTimeIcon( HERO, eStatusIcon_QuestTime );				// 퀘스트 시간 아이콘을 삭제한다.
	}

	pQuest->EndQuest( dwSubQuestFlag, dwData, registTime );								// 퀘스트를 종료한다.

	m_QuestDialog->EndQuest(dwQuestIdx) ;

	m_QuickView->DeleteQuest(dwQuestIdx) ;

	DWORD Key = 0;																	// 키 값을 담을 변수를 선언한다.
	COMBINEKEY(dwQuestIdx, 0, Key);													// 퀘스트 인덱스와 0을 결합하여 키값을 받는다.

	// title
	CQuestString* pQString = m_QuestStringTable.GetData(Key);						// 키 값으로 퀘스트 스트링 테이블에서 스트링 정보를 받는다.

	if( pQString == NULL )															// 퀘스트 스트링 정보가 유효한지 체크한다.
	{
		return;																		// 정보가 유효하지 않으면 리턴 처리를 한다.
	}

	cPtrList* p = pQString->GetTitle();												// 리스트 포인터로 제목을 받는다.

	ITEM* string = (ITEM*)p->GetHead();										// 리스트의 헤드를 받는다.

	CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(222), string->string );			// 퀘스트 메시지를 채팅창에 출력한다.
}

void CQuestManager::StartSubQuest( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwSubQuestFlag, DWORD dwSubData, DWORD dwSubTime )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );								// 퀘스트 테이블에서 인덱스에 해당하는 퀘스트 정보를 받는다.

	if( !pQuest ) return ;																// 퀘스트 정보가 유효한지 체크한다.

	pQuest->StartSubQuest( dwSubQuestIdx, dwSubQuestFlag, dwSubData, dwSubTime );	// 서브 퀘스트를 시작한다.
	AddQuestNpc( pQuest->GetSubQuestInfo( dwSubQuestIdx ) );						// 퀘스트 NPC를 추가한다.

	m_QuestDialog->RefreshQuestList() ;
	m_QuickView->ResetQuickViewDialog();
}

void CQuestManager::EndSubQuest( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwSubQuestFlag, DWORD dwSubData, DWORD dwSubTime )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );								// 퀘스트 인덱스로 퀘스트 테이블에서 퀘스트 정보를 받는다.

	if( !pQuest ) return ;																// 퀘스트 정보가 유효한지 체크한다.

	pQuest->EndSubQuest( dwSubQuestIdx, dwSubQuestFlag, dwSubData, dwSubTime );			// 서브 퀘스트를 종료 한다.

	m_QuestDialog->RefreshQuestList() ;
}

void CQuestManager::UpdateSubQuestData( DWORD dwQuestIdx, DWORD dwSubQuestIdx, DWORD dwMaxCount, DWORD dwSubData, DWORD dwSubTime )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );								// 퀘스트 인덱스로, 퀘스트테이블에서 퀘스트 정보를 받는다.

	if( !pQuest ) return ;																// 퀘스트 정보가 유효한지 체크한다.

	pQuest->UpdateSubQuestData( dwSubQuestIdx, dwMaxCount, dwSubData, dwSubTime );		// 서브 퀘스트 정보를 업데이트 한다.

	m_QuestDialog->RefreshQuestList() ;
	m_QuickView->ResetQuickViewDialog();
}

void CQuestManager::QuestItemAdd( DWORD dwItemIdx, DWORD dwCount )						// 퀘스트 아이템을 추가하는 함수.
{
	QUEST_ITEM_INFO* pData = m_QuestItemMGR.GetQuestItem( dwItemIdx );					// 퀘스트 아이템 매니져에서 아이템 인덱스로, 퀘스트 아이템 정보를 받는다.

	if( !pData ) return;																// 아이템 정보가 유효하지 않으면, 리턴 처리를 한다.

	m_QuestUpdater->QuestItemAdd( pData, dwCount );										// 퀘스트 업데이터에 퀘스트 아이템을 추가한다.

	CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(224), pData->ItemName );			// 채팅창에, 아이템 명을 출력한다.
}

void CQuestManager::QuestItemDelete( DWORD dwItemIdx )									// 퀘스트 아이템 삭제 함수.
{
	// itemname
	QUEST_ITEM_INFO* pData = m_QuestItemMGR.GetQuestItem( dwItemIdx );					// 아이템 인덱스로, 퀘스트 아이템 매니져에서 퀘스트 아이템 정보를 받는다.

	if( !pData ) return;																// 아이템 정보가 유효하지 않으면, 리턴 처리를 한다.

	CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(225), pData->ItemName );			// 채팅창에 아이템명과 메시지를 결합하여 추가한다.

	m_QuestUpdater->QuestItemDelete( dwItemIdx );										// 퀘스트 업데이터에서 퀘스트 아이템을 삭제한다.
}

void CQuestManager::QuestItemUpdate( DWORD dwType, DWORD dwItemIdx, DWORD dwCount )		// 퀘스트 아이템을 업데이트 하는 함수.
{
	m_QuestUpdater->QuestItemUpdate( dwType, dwItemIdx, dwCount);						// 퀘스트 업데이터에서 퀘스트 아이템을 업데이트 한다.

	// itemname
	QUEST_ITEM_INFO* pData = m_QuestItemMGR.GetQuestItem( dwItemIdx );					// 받은 아이템 인덱스로 퀘스트 아이템 매니져에서 퀘스트 아이템 정보를 받는다.

	if( !pData ) return;																// 아이템 정보가 유효하지 않으면, 리턴처리를 한다.

	m_QuestUpdater->QuestItemAdd( pData, dwCount );										// 퀘스트 업데이터에 아이템을 추가한다.

	CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(224), pData->ItemName );			// 채팅창에 아이템 이름과 메시지를 결합하여 출력한다.
}

QUEST_ITEM_INFO* CQuestManager::GetQuestItem( DWORD dwItemIdx )
{
	return m_QuestItemMGR.GetQuestItem( dwItemIdx );
}

BOOL CQuestManager::IsQuestComplete( DWORD dwQuestIdx )									// 퀘스트 완료 여부를 반환하는 함수.
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );								// 들어온 인덱스로 퀘스트 테이블에서 퀘스트 정보를 받는다.

	if( pQuest )																		// 퀘스트 정보가 유효한지 체크한다.
	{
		if( pQuest->IsQuestComplete() )													// 퀘스트가 완료 되었다면,
		{
			return TRUE;																// TRUE를 리턴한다.
		}
		else																			// 퀘스트가 완료되지 않았다면,
		{
			return FALSE;																// FALSE를 리턴한다.
		}
	}

	return FALSE;																		// FALSE를 리턴한다.
}

void CQuestManager::DeleteQuest( DWORD dwQuestIdx, BOOL bCheckTime )					// 퀘스트를 삭제하는 함수.
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );								// 들어온 인덱스로 퀘스트 테이블에서 퀘스트 정보를 받는다.

	if( !pQuest ) return ;																// 퀘스트 정보가 유효한지 체크한다.

	if( pQuest->IsQuestComplete() )
	{
		if( pQuest->HasDateLimit() )
		{
			// 퀘스트 제거/초기화
			pQuest->Initialize();
			m_QuestUpdater->DeleteQuest( dwQuestIdx );
			m_QuestDialog->EndQuest( dwQuestIdx );
			m_QuickView->DeleteQuest( dwQuestIdx );

			// 유효 시간 상태가 변경된 경우를 알려준다.
			NotifyChangedDateTime( dwQuestIdx );
		}

		return;
	}

	pQuest->Initialize();																// 퀘스트를 초기화 한다.
	m_QuestUpdater->DeleteQuest( dwQuestIdx );											// 퀘스트 업데이터에서 퀘스트를 삭제한다.

	m_QuestDialog->EndQuest(dwQuestIdx) ;
	m_QuickView->DeleteQuest(dwQuestIdx) ;

	int Key = 0;																	// 키 값을 받을 변수를 선언하고, 0으로 세팅한다.
	COMBINEKEY( dwQuestIdx, 0, Key );												// 퀘스트 인덱스와 0으로 키값을 받는다.

	CQuestString* pQString = m_QuestStringTable.GetData(Key);						// 키 값으로 퀘스트 스트링 테이블에서 퀘스트 스트링 정보를 받는다.

	if( pQString == NULL )	return;													// 퀘스트 정보가 유효하지 않으면, 리턴 처리를 한다.

	cPtrList* p = pQString->GetTitle();												// 리스트 포인터를 선언하고, 스트링의 제목을 받는다.
	ITEM* string = (ITEM*)p->GetHead();										// 리스트 포인터를 헤드로 세팅하여 스트링 포인터 변수로 받는다.

	if( pQuest->IsTimeCheck() && bCheckTime )										// 퀘스트가 시간을 체크한다면,
	{
		CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(295), string->string );		// 진행시간 초과로 종료되었다는 메시지를 출력한다.

		char msg[256] = {0, };														// 임시 버퍼를 선언한다.
		CompositString( msg, CHATMGR->GetChatMsg(295), string->string );				// 공지 메시지를 만든다.

		STATUSICONDLG->RemoveQuestTimeIcon( HERO, eStatusIcon_QuestTime );			// 시간체크 아이콘을 지운다.
	}
	else																			// 시간체크를 하지 않는다면,
	{
		CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(223), string->string );		// 퀘스트를 포기했다는 메시지를 채팅창에 출력한다.

		char msg[256] = {0, };														// 임시 버퍼를 선언한다.
		CompositString( msg, CHATMGR->GetChatMsg(223), string->string );				// 공지 메시지를 만든다.

		STATUSICONDLG->RemoveQuestTimeIcon( HERO, eStatusIcon_QuestTime );			// 퀘스트 시간 아이콘을 삭제한다.
	}
}

BOOL CQuestManager::IsNpcRelationQuest( DWORD dwNpcIdx )								// NPC와 관계된 퀘스트인지 반환하는 함수.
{
	m_QuestTable.SetPositionHead();

	for(CQuest* pQuest = m_QuestTable.GetData();
		0 < pQuest;
		pQuest = m_QuestTable.GetData())
	{
		if( pQuest->IsNpcRelationQuest( dwNpcIdx ) )									// npc 인덱스로 퀘스트가 npc와 관계된 퀘스트인지 체크한다.
		{
			return TRUE;																// npc와 관련이 있으면, TRUE를 리턴한다.
		}
	}

	return FALSE;																		// FALSE를 리턴한다.
}

DWORD CQuestManager::GetNpcScriptPage( DWORD dwNpcIdx )									// NPC 스크립트 페이지를 리턴하는 함수.
{
	m_QuestTable.SetPositionHead();

	for(CQuest* pQuest = m_QuestTable.GetData();
		0 < pQuest;
		pQuest = m_QuestTable.GetData())
	{
		const DWORD dwPage = pQuest->GetNpcScriptPage( dwNpcIdx );

		if( dwPage )																	// 페이지 정보가 유효하다면,
		{
			return dwPage;																// 페이지를 리턴한다.
		}
	}

	return 0;																			// 0을 리턴한다.
}

void CQuestManager::SendNpcTalkEvent( DWORD dwNpcIdx, DWORD dwNpcScriptIdx, DWORD dwRequitalIndex, DWORD dwRequitalCount )			// npc 대화 이벤트를 보내는 함수.
{
	// 100414 ONS 메세지에 선택보상정보를 추가한다.
	SEND_QUEST_REQUITAL_IDX Msg;																	// 퀘스트 정보를 담을 메시지 구조체를 선언한다.

	ZeroMemory( &Msg, sizeof(Msg) );
	Msg.Category = MP_QUEST;															// 카테고리를 퀘스트로 세팅한다.
	Msg.Protocol = MP_QUESTEVENT_NPCTALK;												// 프로토콜을 퀘스트 이벤트 중 npc 대화로 세팅한다.
	Msg.dwObjectID = gHeroID;
	Msg.MainQuestIdx = (WORD)dwNpcIdx;													// 메인 퀘스트 인덱스를 세팅한다.
	Msg.SubQuestIdx = (WORD)dwNpcScriptIdx;												// 서브 퀘스트 인덱스를 세팅한다.
	Msg.dwRequitalIdx = dwRequitalIndex;
	Msg.dwRequitalCount = dwRequitalCount;

	NETWORK->Send( &Msg, sizeof(Msg) );													// 세팅 된 정보로 메시지를 전송한다.
}

void CQuestManager::SendNpcTalkEventForEvent( DWORD dwNpcIdx, DWORD dwNpcScriptIdx )	// 이벤트를 위한 npc대화 이벤트를 보내는 함수.
{
	SEND_QUEST_IDX Msg;																	// 퀘스트 정보를 담을 메시지 구조체를 선언한다.

	Msg.Category = MP_QUEST;															// 카테고리를 퀘스트로 세팅한다.
	Msg.Protocol = MP_QUESTEVENT_NPCTALK_SYN;											// 프로토콜을 퀘스트 이벤트 중 npc 대화 요청으로 세팅한다.
	Msg.dwObjectID = gHeroID;
	Msg.MainQuestIdx = (WORD)dwNpcIdx;													// npc 인덱스를 세팅한다.
	Msg.SubQuestIdx = (WORD)dwNpcScriptIdx;												// npc 스크립트 인덱스를 세팅한다.

	NETWORK->Send( &Msg, sizeof(Msg) );													// 세팅 된 정보로 메시지를 전송한다.
}

void CQuestManager::SetNpcData( CNpc* pNpc )											// npc 데이터를 세팅하는 함수.
{
	if( m_dwMaxNpc > MAX_NPCARRAY-1 )													// 최대 npc 인덱스가 31보다 큰지 체크한다.
	{
		return;																			// 크다면, 리턴처리를 한다.
	}

	m_pNpcArray[m_dwMaxNpc] = pNpc;														// npc 정보를 담는 배열 인덱스에 npc 정보를 담는다.

	++m_dwMaxNpc;																		// 최대 npc 번호를 증가시킨다.
}

void CQuestManager::SetNpcIDName(WORD wIdx, stNPCIDNAME* pInfo)							// npc 이름과 아이디를 세팅하는 함수.
{
	if( wIdx > MAX_NPCIDNAME-1 )
	{
		ASSERTMSG(0, "스태틱 NPC 정보가 100개를 넘었음, MAX_NPCIDNAME 수치를 늘려주시오.") ;
		return ;
	}

	m_stNPCIDNAME[wIdx].wId = pInfo->wId ;
	strcpy(m_stNPCIDNAME[wIdx].name, pInfo->name) ;
}

stNPCIDNAME* CQuestManager::GetNpcIdName(DWORD npcIdx)
{
	if( npcIdx < 0 || npcIdx > MAX_NPCIDNAME-1 )
	{
		return NULL ;
	}

	for( int count = 0 ; count < MAX_NPCIDNAME ; ++count )
	{
		if( m_stNPCIDNAME[count].wId == (WORD)npcIdx )
		{
			return &m_stNPCIDNAME[count] ;
		}
	}

	return NULL ;
}

QUESTNPCINFO* CQuestManager::GetQusetNpcInfo(DWORD npcIdx)
{
	return m_QuestNpcTable.GetData( npcIdx );
}



void CQuestManager::ProcessNpcMark()
{
	GAMEIN->GetMiniMapDialog()->UpdateQuestMark();
	GAMEIN->GetWorldMapDialog()->GetBigMapDlg()->UpdateQuestMark();
}

eQUEST_MARK CQuestManager::GetQuestMarkState( DWORD dwUniqueNpcIdx, DWORD dwNpcObjectIdx )
{
	std::map< DWORD, NpcHasQuestList >::const_iterator questOwnedByNpcIter = m_NpcHasQuestList.find( dwUniqueNpcIdx );

	if( questOwnedByNpcIter == m_NpcHasQuestList.end() )
		return eQM_NONE;

	eQUEST_MARK mark = eQM_NONE;

	STATIC_NPCINFO* pStaticNpcInfo = GAMERESRCMNGR->GetStaticNpcInfo( (WORD)dwUniqueNpcIdx );
	if( !pStaticNpcInfo )
		return eQM_NONE;

	const WORD wNpcKind = pStaticNpcInfo->wNpcJob;
	NPC_LIST* pNpcList = GAMERESRCMNGR->GetNpcInfo( wNpcKind );
	if( !pNpcList )
		return eQM_NONE;

	const NpcHasQuestList& hasQuestList = questOwnedByNpcIter->second;

	for( NpcHasQuestList::const_iterator hasQuestListIter = hasQuestList.begin() ; hasQuestListIter != hasQuestList.end() ; ++hasQuestListIter )
	{
		const DWORD dwQuestIdx = *hasQuestListIter;
		CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );
		if( !pQuest )
			continue;

		switch( pQuest->GetNpcMarkType( dwUniqueNpcIdx ) )						// NPC의 마크 타입을 확인한다.
		{
			// 마크 타입이 !라면
		case eQM_EXCLAMATION:
			{
				if( pQuest->HasDateLimit() &&
					mark != eQM_EXCLAMATION )
				{
					mark = eQM_EXCLAMATION_HAS_TIMEQUEST;
				}
				else
				{
					mark = eQM_EXCLAMATION;
				}
			}
			break;
			// 마크 타입이 ?라면
		case eQM_QUESTION:
			{
				if( pQuest->HasDateLimit() &&
					mark != eQM_EXCLAMATION &&
					mark != eQM_EXCLAMATION_HAS_TIMEQUEST &&
					mark != eQM_QUESTION )
				{
					mark = eQM_QUESTION_HAS_TIMEQUEST;
				}
				else if( mark != eQM_EXCLAMATION &&
						 mark != eQM_EXCLAMATION_HAS_TIMEQUEST )
				{
					mark = eQM_QUESTION;
				}
			}
			break;
		}
	}

	CNpc* pNpc = (CNpc*)OBJECTMGR->GetObject( dwNpcObjectIdx );
	if( pNpc != NULL && pNpc->GetObjectKind() == eObjectKind_Npc )
	{
		OBJECTEFFECTDESC desc;
		DWORD dwEffectID = 0;
		
		switch( mark )
		{
		case eQM_EXCLAMATION:
			{
				desc.SetDesc( 17 );
				dwEffectID = NPCMARK_1_ID;
			}
			break;
		case eQM_QUESTION:
			{
				desc.SetDesc( 18 );
				dwEffectID = NPCMARK_2_ID;
			}
			break;
		case eQM_EXCLAMATION_HAS_TIMEQUEST:
			{
				desc.SetDesc( 9525 );
				dwEffectID = NPCMARK_3_ID;
			}
			break;
		case eQM_QUESTION_HAS_TIMEQUEST:
			{
				desc.SetDesc( 9526 );
				dwEffectID = NPCMARK_4_ID;
			}
			break;
		}

		if( pNpc->IsNpcMark( dwEffectID ) )
			return mark;

		pNpc->RemoveObjectEffect( NPCMARK_1_ID );	// !
		pNpc->RemoveObjectEffect( NPCMARK_2_ID );	// ?
		pNpc->RemoveObjectEffect( NPCMARK_3_ID );	// 시간제한 !
		pNpc->RemoveObjectEffect( NPCMARK_4_ID );	// 시간제한 ?

		desc.Position.x = 0.f;
		desc.Position.y = float( pNpcList->Tall ) + 50.0f;
		desc.Position.z = 0.f;
		pNpc->AddObjectEffect( dwEffectID, &desc, 1, HERO );
	}

	return mark;
}

void CQuestManager::NetworkMsgParse( BYTE Protocol, void* pMsg )									// 퀘스트 네트워크 메시지 파싱함수.
{
	if(!pMsg) return ;																				// 인자로 넘어온 메시지 정보가 유효하지 않으면, return 처리를 한다.

	switch(Protocol)																				// 프로토콜 정보를 확인한다.
	{
	case MP_QUEST_MAINDATA_LOAD:				Quest_Maindata_Load( pMsg ) ;			break;		// 메인 데이터 로드 처리를 한다.
	case MP_QUEST_SUBDATA_LOAD:					Quest_Subdata_Load( pMsg ) ;			break;		// 서브 데이터 로드 처리를 한다.
	case MP_QUEST_ITEM_LOAD:					Quest_Item_Load( pMsg ) ;				break;		// 아이템 로드 처리를 한다.
	case MP_QUEST_START_ACK:					Quest_Start_Ack( pMsg ) ;				break;		// 퀘스트 시작 처리를 한다.
	case MP_SUBQUEST_START_ACK:					SubQuest_Start_Ack( pMsg ) ;			break;		// 서브 퀘스트 시작 처리를 한다.
	case MP_QUEST_END_ACK:						Quest_End_Ack( pMsg )  ;				break;		// 퀘스트 종료 처리를 한다.
	case MP_SUBQUEST_END_ACK:					SubQuest_End_Ack( pMsg ) ;				break;		// 서브 퀘스트 종료 처리를 한다.
	case MP_SUBQUEST_UPDATE:					SubQuest_Update( pMsg ) ;				break;		// 서브 퀘스트 업데이트 처리를 한다.
	case MP_QUEST_DELETE_ACK:					Quest_Delete_Ack( pMsg ) ;				break;		// 퀘스트 삭제를 처리한다.
	case MP_QUESTITEM_GIVE:						QuestItem_Give( pMsg ) ;				break;		// 퀘스트 아이템을 주는 처리를 한다.
	case MP_QUESTITEM_TAKE:						QuestItem_Take( pMsg ) ;				break;		// 퀘스트 아이템을 수거하는 처리를 한다.
	case MP_QUESTITEM_UPDATE:					QuestItem_Update( pMsg ) ;				break;		// 퀘스트 아이템 업데이트 처리를 한다.
	case MP_QUEST_TAKEITEM_ACK:					Quest_TakeItem_Ack( pMsg ) ;			break;		// 퀘스트 아이템 수거 처리를 한다.
	case MP_QUEST_TAKEMONEY_ACK:				Quest_Takemoney_Ack( pMsg ) ;			break;		// 퀘스트 머니 획득 처리를 한다.
	case MP_QUEST_TAKEEXP_ACK:					Quest_TakeExp_Ack( pMsg ) ;				break;		// 퀘스트 경험치 획득 처리를 한다.
	case MP_QUEST_TAKESEXP_ACK:					Quest_TakesExp_Ack( pMsg ) ;			break;		// 퀘스트 경험치 획득 처리를 한다.
	case MP_QUEST_GIVEITEM_ACK:					Quest_GiveItem_Ack( pMsg ) ;			break;		// 퀘스트 아이템 분배 처리를 한다.
	// 071026 LYW --- QuestManager : Add process part of give stack item.
	case MP_QUEST_GIVEITEMS_ACK :				Quest_GiveItems_Ack( pMsg ) ;			break ;		// 퀘스트 스택 아이템 분배 처리를 한다.
	case MP_QUEST_GIVEMONEY_ACK:				Quest_GiveMoney_Ack( pMsg ) ;			break;		// 퀘스트 머니 분배 처리를 한다.
	case MP_QUEST_TIME_LIMIT:					Quest_Time_Limit( pMsg ) ;				break;		// 퀘스트 시간 제한 처리를 한다.
	case MP_QUEST_REGIST_CHECKTIME:				Quest_Regist_Checktime( pMsg ) ;		break;		// 퀘스트 시간 체크 등록 처리를 한다.
	case MP_QUEST_UNREGIST_CHECKTIME:			Quest_Unregist_Checktime( pMsg ) ;		break;		// 퀘스트 시간 체크 탈퇴 처리를 한다.
	case MP_QUESTEVENT_NPCTALK_ACK:				QuestEvent_NpcTalk_Ack( pMsg ) ;		break;		// 퀘스트 이벤트 npc 대화 수락 처리를 한다.
	case MP_QUESTEVENT_NPCTALK_NACK:			QuestEvent_NpcTalk_Nack( pMsg ) ;		break;		// 퀘스트 이벤트 npc 대화 실패 처리를 한다.
	case MP_QUEST_EXECUTE_ERROR:				Quest_Execute_Error( pMsg ) ;			break;		// 퀘스트 에러 처리를 한다.
	case MP_QUEST_ERROR_EXT :					Quest_Error_Ext( pMsg ) ;				break;		// 퀘스트 기타 에러 처리를 한다.
	case MP_QUEST_ERROR_FULLQUEST :				Quest_Error_FullQuest( pMsg ) ;			break ;		// 퀘스트 목록이 다 찼다는 에러 메시지를 처리한다.

	// 071028 LYW --- QuestManager : Add process part of error when failed give item.
	case MP_QUEST_GIVEITEM_ERR :				Quest_GiveItem_Error( pMsg ) ;			break ;		// 퀘스트 아이템 반납 에러 처리를 한다.

	default:																			break;		// 디폴트 처리.
	}

	GAMEIN->GetNpcScriptDialog()->RefreshQuestLinkPage() ;
}

void CQuestManager::QuestAbandon()																	// 퀘스트 포기 처리 함수.
{
	DWORD QuestIdx = 0 ;																			// 퀘스트 인덱스를 담을 변수를 선언하고 0으로 세팅한다.

	CQuestDialog* pDlg = NULL ;																		// 퀘스트 다이얼로그 정보를 받을 포인터를 선언하고 null처리를 한다.
	pDlg = GAMEIN->GetQuestTotalDialog()->GetQuestDlg() ;											// 퀘스트 다이얼로그 정보를 받는다.

	if( pDlg )																						// 퀘스트 다이얼로그 정보가 유효하다면,
	{
		QuestIdx = pDlg->GetSelectedQuestID() ;														// 선택 된 퀘스트 인덱스를 받는다.

		pDlg->SetQuestCount();
	}

	if( IsQuestComplete(QuestIdx) )																	// 퀘스트 인덱스로 완료한 퀘스트인지 체크한다.
	{
		return;																						// 완료한 퀘스트라면, 리턴처리를 한다.
	}

	SEND_QUEST_IDX msg;																				// 퀘스트 정보를 담을 구조체를 선언한다.

	msg.Category = MP_QUEST;																		// 카테고리를 퀘스트로 세팅한다.
	msg.Protocol = MP_QUEST_DELETE_SYN;																// 프로토콜을 퀘스트 삭세 싱크로 세팅한다.
	msg.dwObjectID = gHeroID;
	msg.MainQuestIdx = (WORD)QuestIdx;																// 퀘스트 인덱스를 세팅한다.

	NETWORK->Send(&msg, sizeof(msg));																// 세팅된 정보를 전송한다.

	CQuestInfo* pQuestInfo =NULL ;																	// 퀘스트 정보를 받을 포인터를 선언하고 null 처리를 한다.
	pQuestInfo = GetQuestInfo(QuestIdx) ;															// 퀘스트 인덱스로 퀘스트 정보를 받는다.

	if(!pQuestInfo)																					// 퀘스트 정보가 유효하지 않으면,
	{
		ASSERTMSG(0, "퀘스트 포기하는 시점에서 퀘스트 정보를 로딩하지 못하였습니다.!") ;			// ASSERT 에러 체크.
		return  ;																					// 리턴 처리를 한다.
	}

	int nMapNum = pQuestInfo->GetMapNum() ;															// 퀘스트 정보에 있는 맵 번호를 받는다.

	DeleteQuestTitle(nMapNum, QuestIdx);
	DeleteMapTree(nMapNum);
}

void CQuestManager::QuestAbandon( DWORD dwQuestIdx )												// 퀘스트 포기함수( 퀘스트 인덱스 인자 )
{
	if( IsQuestComplete(dwQuestIdx) )																// 들어온 퀘스트 인덱스의 퀘스트를 완료 했는지 체크한다.
	{
		return;																						// 완료한 퀘스트라면, 리턴 처리를 한다.
	}

	SEND_QUEST_IDX msg;																				// 퀘스트 정보를 담을 구조체를 선언한다.

	msg.Category = MP_QUEST;																		// 카테고리를 퀘스트로 세팅한다.
	msg.Protocol = MP_QUEST_DELETE_SYN;																// 프로토콜을 퀘스트 삭제 싱크로 세팅한다.
	msg.dwObjectID = gHeroID;
	msg.MainQuestIdx = (WORD)dwQuestIdx;															// 퀘스트 인덱스를 세팅한다.

	NETWORK->Send(&msg, sizeof(msg));																// 세팅된 정보를 전송한다.

	//test
	m_QuestUpdater->DeleteQuest( dwQuestIdx );														// 퀘스트 업데이터의 퀘스트 삭제 함수를 호출한다.
}

void CQuestManager::AddQuestNpc( CSubQuestInfo* pSubQuestInfo )										// 퀘스트 npc를 추가하는 함수.
{
	if( !pSubQuestInfo ) return;																	// 서브 퀘스트 정보가 없으면, 리턴처리를 한다.

	cPtrList& pNpcList = pSubQuestInfo->GetQuestNpcList();
	PTRLISTPOS npos = pNpcList.GetHeadPosition();

	while( npos )																					// npc 리스트의 위치 정보를 담는 포인터가 유효한 동안 while문을 돌린다.
	{
		CQuestNpc* pNpc = (CQuestNpc*)pNpcList.GetNext( npos );

		if( pNpc )																					// npc 정보가 유효하다면,
		{
			cPtrList& pList = pNpc->GetNpcDataList();
			PTRLISTPOS pos = pList.GetHeadPosition();

			while( pos )																		// 위치 정보가 유효한 동안 while문을 돌린다.
			{
				CQuestNpcData* pData = (CQuestNpcData*)pList.GetNext( pos );					// 위치에 해당하는 퀘스트 npc 데이터 정보를 받는다.

				if( pData )																		// 데이터 정보가 유효한지 체크한다.
				{
					QUESTNPCINFO* pInfo = m_QuestNpcTable.GetData( pData->GetNpcIdx() );		// 데이터 정보의 npc 인덱스를 참조하여 퀘스트 npc 정보를 받는다.

					if( pInfo )																	// npc 정보가 유효한지 체크한다.
					{
						MAP->AddQuestNpc( pInfo );												// 맵에 퀘스트 npc를 추가한다.
					}
				}
			}
		}
	}
}

BOOL CQuestManager::IsQuestStarted( DWORD dwQuestIdx )												// 퀘스트가 시작되었는지 여부를 리턴하는 함수.
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );											// 들어온 퀘스트 인덱스로, 퀘스트 정보를 받는다.

	if( pQuest )																					// 퀘스트 정보가 유효한지 체크한다.
	{
		if( pQuest->IsSubQuestComplete( 0 ) )														// 퀘스트의 서브 퀘스트가 완료 되었다면,
		{
			return FALSE;																			// FALSE를 리턴한다.
		}
		else																						// 그렇지 않으면,
		{
			return pQuest->CanItemUse( dwQuestIdx );												// 아이템을 사용할 수 있는지 여부를 리턴한다.
		}
	}
	return FALSE;																					// FALSE를 리턴한다.
}

char* CQuestManager::GetQuestTitle( DWORD dwQuestIdx )												// 퀘스트 타이틀을 리턴하는 함수.
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );											// 들어온 퀘스트 인덱스로 퀘스트 정보를 받는다.

	if( !pQuest )	return NULL;																	// 퀘스트 정보가 유효하지 않으면, NULL을 리턴한다.

	//
	int Key = 0;																					// 키 값을 받을 변수를 선언하고 0으로 세팅한다.

	COMBINEKEY( dwQuestIdx, 0, Key );																// 퀘스트 인덱스와 0을 결합하여 키값을 받는다.

	CQuestString* pQString = m_QuestStringTable.GetData(Key);										// 키 값으로 퀘스트 스트링 정보를 받는다.

	if( pQString == NULL )	return NULL;															// 퀘스트 스트링 정보가 유효하지 않으면, NULL을 리턴한다.

	cPtrList* p = pQString->GetTitle();																// 퀘스트 스트링의 타이틀을 리스트 포인터로 받는다.

	QString* string = (QString*)p->GetHead();														// 리스트의 해드를 스트링 포인터로 받는다.

	return string->Str;																				// 스트링 포인터의 스트링을 리턴한다.
}

CQuestString* CQuestManager::GetQuestString(DWORD dwMainQuestIdx, DWORD dwSubQuestIdx)				// 퀘스트 번호와 서브 퀘 번호로 퀘스트 스트링을 반환하는 함수.
{
	int nCombineKey = 0 ;																			// 컴바인 키 값을 담을 변수를 선언하고 0으로 세팅한다.

	COMBINEKEY( dwMainQuestIdx, dwSubQuestIdx, nCombineKey) ;										// 메인퀘 번호와 서브퀘 번호로 컴바인 키를 만든다.

	CQuestString* pQuestString = m_QuestStringTable.GetData(nCombineKey) ;							// 컴바인 키로 퀘스트 스트링 테이블에서 퀘스트 스트링을 받는다.

	return pQuestString ;																			// 퀘스트 스트링을 반환한다.
}

void CQuestManager::QuestTimeLimit( DWORD dwQuestIdx, DWORD dwTime )								// 퀘스트의 시간을 제한하는 함수.
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );											// 들어온 인덱스로 퀘스트 정보를 받는다.

	if( !pQuest )	return;																			// 퀘스트 정보가 유효하지 않으면 리턴 처리를 한다.

	//
	stTIME time;																					// 시간 정보를 담을 구조체를 선언한다.
	time.value = dwTime;																			// 들어온 시간값을 세팅한다.

	int Key = 0;																					// 키 값을 받을 변수를 선언하고 0으로 세팅한다.

	COMBINEKEY( dwQuestIdx, 0, Key );																// 퀘스트 인덱스와 0을 결합하여 키 값을 받는다.

	CQuestString* pQString = m_QuestStringTable.GetData(Key);										// 받은 키 값을 참조해 퀘스트 스트링 정보를 받는다.

	if( pQString == NULL )	return;																	// 퀘스트 스트링 정보가 유효하지 않으면 리턴 처리를 한다.

	cPtrList* p = pQString->GetTitle();																// 스트링의 타이틀을 리스트 포인터로 받는다.

	QString* string = (QString*)p->GetHead();														// 리스트의 헤드를 스트링 포인터로 받는다.

	CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(299), string->Str,								// 시간 정보와 메시지를 결합하여, 채팅창으로 출력한다.
					 time.GetYear()+2000, time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute() );	

	char msg[256] = {0, };																			// 임시 버퍼를 선언한다.

	CompositString( msg, CHATMGR->GetChatMsg(299), string->Str,										// 시간 정보와 메시지를 결합하여, 출력할 메시지를 완성한다.
					 time.GetYear()+2000, time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute() );	

	STATUSICONDLG->RemoveQuestTimeIcon( HERO, eStatusIcon_QuestTime );								// 퀘스트의 시간제한 아이콘을 삭제한다.
}

void CQuestManager::RegistCheckTime( DWORD dwQuestIdx, BYTE bCheckType, DWORD dwCheckTime )			// 시간 체크 등록을 하는 함수.
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );											// 들어온 퀘스트 인덱스로 퀘스트 정보를 받는다.

	if( !pQuest )	return;																			// 퀘스트 정보가 유효하지 않으면 리턴 처리를 한다.

	pQuest->RegistCheckTime( bCheckType, dwCheckTime );												// 퀘스트 정보에 시간 체크 여부 정보를 세팅한다.

	STATUSICONDLG->AddQuestTimeIcon( HERO, eStatusIcon_QuestTime );									// 퀘스트 시간 아이콘을 추가한다.
}

void CQuestManager::UnRegistCheckTime( DWORD dwQuestIdx )											// 시간 체크 등록을 해제하는 함수.
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );											// 들어온 퀘스트 인덱스로 퀘스트 정보를 받는다.

	if( !pQuest )	return;																			// 퀘스트 정보가 유효하지 않으면 리턴 처리를 한다.

	pQuest->UnRegistCheckTime();																	// 퀘스트 정보에서 UnRegistCheckTime함수를 호출한다.

	STATUSICONDLG->RemoveQuestTimeIcon( HERO, eStatusIcon_QuestTime );								// 퀘스트 시간 아이콘을 삭제한다.
}

void CQuestManager::QuestEventAck( DWORD dwQuestIdx )												// 퀘스트 이벤트 요청 함수.
{
	WINDOWMGR->MsgBox( MBI_QUESTMSG, MBT_OK, "출석체크가 되었습니다." );							// 출석 체크가 되었다는 메시지를 출력한다.
}

void CQuestManager::QuestEventNack( DWORD dwQuestIdx, DWORD dwError )								// 퀘스트 이벤트 실패 함수.
{
	switch( dwError )																				// 에러를 확인한다.
	{
	case 1:																							// 에러가 1번과 같으면,
		{
			WINDOWMGR->MsgBox( MBI_QUESTMSG, MBT_OK, "이미 출석체크가 되어 협행을 진행할 수 없습니다." );	// 메시지를 출력한다.
		}
		break;
	case 2:																							// 에러가 2번과 같으면,
		{
			WINDOWMGR->MsgBox( MBI_QUESTMSG, MBT_OK, "홈페이지 출석체크가 되어 있지 않아 협행을 진행할 수 없습니다." );	// 메시지를 출력한다.
		}
		break;
	}
}
void CQuestManager::Quest_Maindata_Load( void* pMsg )												// 메인 데이터를 로딩하는 함수.
{
	SEND_MAINQUEST_DATA* pmsg = (SEND_MAINQUEST_DATA*)pMsg;											// 메인 퀘스트 데이터를 받는 포인터 변수를 선언하고 메시지를 받는다.

	if(pmsg->wCount==0)		return;																	// 카운트가 0과 같으면 리턴 처리를 한다.
	
	for(int i=0; i<pmsg->wCount; i++)																// 카운트 수 만큼 for문을 돌린다.
	{
		SetMainQuestData( pmsg->QuestList[i].QuestIdx, pmsg->QuestList[i].state.value,				// 메인 퀘스트 데이터를 세팅한다.
			pmsg->QuestList[i].EndParam, pmsg->QuestList[i].registTime, pmsg->QuestList[i].CheckType, 
			pmsg->QuestList[i].CheckTime );
	}

	ProcessNpcMark();																				// npc 마크 프로세스를 실행한다.
}

void CQuestManager::Quest_Subdata_Load( void* pMsg )												// 서브 데이터를 로딩하는 함수.
{
	SEND_SUBQUEST_DATA* pmsg = (SEND_SUBQUEST_DATA*)pMsg;											// 서브 퀘스트 데이터를 받는 포인터 변수를 선언하고 메시지를 받는다.

	if(pmsg->wCount==0)		return;																	// 카운트가 0과 같으면 리턴 처리를 한다.
	
	for(int i=0; i<pmsg->wCount; i++)																// 카운트 수 만큼 for문을 돌린다.
	{
		SetSubQuestData( pmsg->QuestList[i].QuestIdx, pmsg->QuestList[i].SubQuestIdx,				// 서브 퀘스트 데이터를 세팅한다.
							pmsg->QuestList[i].state, pmsg->QuestList[i].time );
	}

	ProcessNpcMark();																				// npc 마크 프로세스를 실행한다.
}

void CQuestManager::Quest_Item_Load( void* pMsg )													// 퀘스트 아이템을 로딩하는 함수.
{
	SEND_QUESTITEM* pmsg = (SEND_QUESTITEM*)pMsg;													// 퀘스트 아이템 정보를 받는 포인터 변수를 선언하고 메시지를 받는다.

	if(pmsg->wCount==0)		return;																	// 카운트가 0과 같으면 리턴 처리를 한다.

	for(int i=0; i<pmsg->wCount; i++)																// 카운트 수만큼 for문을 돌린다.
	{			
		m_QuestUpdater->QuestItemAdd(m_QuestItemMGR.GetQuestItem(pmsg->ItemList[i].ItemIdx),		// 퀘스트 업데이터에 퀘스트 아이템을 추가한다.	
			pmsg->ItemList[i].Count);			
	}
}

void CQuestManager::Quest_Start_Ack( void* pMsg )													// 퀘스트 시작 수락 함수.
{
	SEND_QUEST_IDX* pmsg = (SEND_QUEST_IDX*)pMsg;													// 퀘스트 정보를 받을 포인터 변수를 선언하고 메시지를 받는다.

	StartQuest( pmsg->MainQuestIdx, pmsg->dwFlag, pmsg->dwData, pmsg->registTime );					// 받은 정보를 참조로 퀘스트를 시작한다.

	// 100317 ONS 퀘스트 자동 알리미 기능 추가 
	if( OPTIONMGR->GetGameOption()->bAutoQuestNotice )
	{
		m_QuickView->RegistQuest( pmsg->MainQuestIdx );
	}
}

void CQuestManager::SubQuest_Start_Ack( void* pMsg )												// 서브 퀘스트 시작 수락 함수.
{
	SEND_QUEST_IDX* pmsg = (SEND_QUEST_IDX*)pMsg;													// 퀘스트 정보를 받을 포인터 변수를 선언하고 메시지를 받는다.

	StartSubQuest( pmsg->MainQuestIdx, pmsg->SubQuestIdx, pmsg->dwFlag, pmsg->dwData, (DWORD)pmsg->registTime );// 서브 퀘스트를 시작한다.

	ProcessNpcMark();																				// npc 마크 프로세스를 시작한다.
}

void CQuestManager::Quest_End_Ack( void* pMsg )														// 퀘스트 종료 수락 함수.
{
	SEND_QUEST_IDX* msg = (SEND_QUEST_IDX*)pMsg;													// 퀘스트 정보를 받을 포인터를 선언하고, 메시지를 받는다.
		
	EndQuest( msg->MainQuestIdx, msg->dwFlag, msg->dwData, msg->registTime );						// 퀘스트를 종료한다.
	ProcessNpcMark();	
}

void CQuestManager::SubQuest_End_Ack( void* pMsg )													// 서브 퀘스트 종료 수락 함수.
{
	SEND_QUEST_IDX* msg = (SEND_QUEST_IDX*)pMsg;													// 퀘스트 정보를 받을 포인터를 선언하고, 메시지를 받는다.

	EndSubQuest( msg->MainQuestIdx, msg->SubQuestIdx, msg->dwFlag, msg->dwData, (DWORD)msg->registTime );		// 서부 퀘스트를 종료한다.
	ProcessNpcMark();																				// npc 마크 프로세스를 실행한다.

	GAMENOTIFYMGR->EndSubQuest(msg) ;																// 서브 퀘스트 종료 공지를 추가한다.
}

void CQuestManager::SubQuest_Update( void* pMsg )													// 서브 퀘스트 업데이트 함수.
{
	SEND_SUBQUEST_UPDATE* msg = (SEND_SUBQUEST_UPDATE*)pMsg;										// 서브 퀘스트 업데이트 정보를 받을 포인터를 선언하고 메시지를 받는다.

	UpdateSubQuestData( msg->wQuestIdx, msg->wSubQuestIdx, msg->dwMaxCount, msg->dwData, msg->dwTime );	// 서브 퀘스트 데이터를 업데이트 한다.

	GAMENOTIFYMGR->UpdateSubQuest(msg) ;															// 퀘스트 업데이트 공지를 추가한다.
}	

void CQuestManager::QuestItem_Give( void* pMsg )													// 퀘슽 아이템 반환 함수.
{
	SEND_QUESTITEM_IDX* pmsg = (SEND_QUESTITEM_IDX*)pMsg;											// 퀘스트 아이템 정보를 받을 포인터를 선언하고 메시지를 받는다.

	QuestItemDelete( pmsg->dwItemIdx );																// 퀘스트 아이템을 삭제한다.
}

void CQuestManager::QuestItem_Take( void* pMsg )													// 퀘스트 아이템 수락 함수.
{
	SEND_QUESTITEM_IDX* pmsg = (SEND_QUESTITEM_IDX*)pMsg;											// 퀘스트 아이템 정보를 받을 포인터를 선언하고 메시지를 받는다.

	QuestItemAdd( pmsg->dwItemIdx, pmsg->dwItemNum );												// 퀘스트 아이템을 추가한다.

	EFFECTMGR->StartHeroEffectProcess(eEffect_GetItem_Quest);										// 아이템을 수락하는 이펙트를 시작한다.

//			ITEMMGR->ItemDropEffect( pmsg->dwItemIdx );
}

void CQuestManager::QuestItem_Update( void* pMsg )													// 퀘스트 아이템 업데이트 함수.
{
	SEND_QUESTITEM_IDX* pmsg = (SEND_QUESTITEM_IDX*)pMsg;											// 퀘스트 아이템 정보를 받을 포인터를 선언하고 메시지를 받는다.
		
	QuestItemUpdate( eQItem_SetCount, pmsg->dwItemIdx, pmsg->dwItemNum );							// 퀘스트 아이템을 업데이트 한다.

	EFFECTMGR->StartHeroEffectProcess(eEffect_GetItem_Quest);										// 아이템을 수락하는 이펙트를 시작한다.
//			ITEMMGR->ItemDropEffect( pmsg->dwItemIdx );
}

void CQuestManager::Quest_Delete_Ack( void* pMsg )													// 퀘스트 삭제를 수락하는 함수.
{
	SEND_QUEST_IDX* pmsg = (SEND_QUEST_IDX*)pMsg;													// 퀘스트 정보를 담을 포인터를 선언하고 메시지를 받는다.

	DeleteQuest( pmsg->MainQuestIdx, (BOOL)pmsg->dwFlag );											// 퀘스트를 삭제한다.

	ProcessNpcMark();																				// npc 마크 프로세스를 실행한다.
}

void CQuestManager::Quest_TakeItem_Ack( void* pMsg )												// 퀘스트 아이템 획득 수락 함수.
{
	ITEMOBTAINARRAY * pmsg = (ITEMOBTAINARRAY *)pMsg;												// 아이템 획득 정보를 담을 포인터를 선언하고 메시지를 받는다.

	CItem* pItem = NULL;																			// 아이템 정보를 담을 포인터를 선언하고 null처리를 한다.

	for(WORD i = 0; i < pmsg->ItemNum; ++i )
	{
		pItem = GAMEIN->GetInventoryDialog()->GetItemForPos( pmsg->GetItem(i)->Position);

		if(pItem)
		{
			pItem->SetItemBaseInfo( *( pmsg->GetItem(i) ) );												// 아이템 기본 정보를 세팅한다.
		}
		else																						// 아이템 정보가 유효하지 않으면,
		{
			pItem = ITEMMGR->MakeNewItem(pmsg->GetItem(i),"MP_QUEST_TAKEITEM_ACK");					// 새로 아이템을 생성해 포인터로 받는다.

			GAMEIN->GetInventoryDialog()->AddItem(pItem);
		}
	}
	// 
	cDialog* pDlg = WINDOWMGR->GetWindowForID( DBOX_DIVIDE_INV );									// 디바이드 박스 정보를 받는다.

	if( pDlg )																						// 정보가 유효한지 체크한다.
	{
		((cDivideBox*)pDlg)->ExcuteDBFunc( 0 );														// 박스의 함수를 실행한다.
	}

	QUICKMGR->RefreshQickItem();																	// 퀵 매니져의 퀵 아이템 새로고침을 실행한다.

	if( !pItem ) return;																			// 아이템 정보가 유효하지 않으면 리턴 처리를 한다.

	{
		CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(235), pItem->GetItemInfo()->ItemName );		// 퀘스트 보상으로 아이템을 얻었다는 메시지를 출력한다.
	}

	EFFECTMGR->StartHeroEffectProcess(eEffect_GetItem_Quest);										// 아이템 획득 이펙트를 실행한다.

	//GAMENOTIFYMGR->TakeItem(pItem) ;																// 퀘스트 아이템 획득 공지를 띄운다.
}

void CQuestManager::Quest_Takemoney_Ack( void* pMsg )												// 퀘스트 머니 획득 수락 함수.
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;																// 메시지를 받을 포인터를 선언하고 메시지를 받는다.

	CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(236), AddComma(pmsg->dwData) );					// 골드를 얻었다는 메시지를 출력한다.
	
	EFFECTMGR->StartHeroEffectProcess(eEffect_GetMoney);											// 아이템을 얻었다는 이펙트를 실행한다.

	//GAMENOTIFYMGR->TakeMoney(AddComma(pmsg->dwData)) ;												// 퀘스트 보상 골드 획득 공지를 띄운다.
}

void CQuestManager::Quest_TakeExp_Ack( void* pMsg )													// 퀘스트 경험치를 획득 수락 함수.
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;																// 메시지를 받을 포인터를 선언하고 메시지를 받는다.

	CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(237), AddComma(pmsg->dwData) );					// 경험치를 얻었다는 메시지를 출력한다.

	//GAMENOTIFYMGR->TakeExp(pmsg->dwData) ;															// 퀘스트 보상 경험치 획득 공지를 띄운다.
	
//	HERO->SetExpPoint(HERO->GetExpPoint() + pmsg->dwData) ;											// HERO의 경험치를 세팅한다.
}

void CQuestManager::Quest_TakesExp_Ack( void* pMsg )												// 퀘스트 sp 획득 수락 함수.
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;																// 메시지를 받을 포인터를 선언하고 메시지를 받는다.

	CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(238), pmsg->dwData );							// sp를 얻었다는 메시지를 출력한다.

	HERO->SetExpPoint(HERO->GetExpPoint() + pmsg->dwData) ;											// HERO의 경험치를 세팅한다.
}

void CQuestManager::Quest_GiveItem_Ack( void* pMsg )												// 퀘스트 아이템을 반환하는 함수.
{
	MSG_EXCHANGE_REMOVEITEM* pmsg = (MSG_EXCHANGE_REMOVEITEM*)pMsg;
	CItem* pItem = GAMEIN->GetInventoryDialog()->GetItemForPos( pmsg->wAbsPosition );
	const eITEMTABLE wTableIdx = ITEMMGR->GetTableIdxForAbsPos( pmsg->wAbsPosition );

	ITEMMGR->DeleteItemofTable( wTableIdx, pmsg->wAbsPosition, FALSE );								// 아이템 매니져로 테이블의 아이템을 삭제한다.

	QUICKMGR->RefreshQickItem();																	// 퀵 매니져의 퀵 아이템 새로고침을 실행한다.

	if( !pItem ) return;																			// 아이템 정보가 유효하지 않으면 리턴 처리를 한다.

	CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(297), pItem->GetItemInfo()->ItemName );			// 아이템이 사라졌다는 메시지를 출력한다.
}

// 071026 LYW --- QuestManager : Add function to decrease item.
void CQuestManager::Quest_GiveItems_Ack( void* pMsg )
{
	MSG_EXCHANGE_DECREASEITEM* pmsg = (MSG_EXCHANGE_DECREASEITEM*)pMsg;								// 교환 삭제 아이템 정보를 받는 포인터를 선언하고 메시지를 받는다.

	CItem* pItem = GAMEIN->GetInventoryDialog()->GetItemForPos( pmsg->wAbsPosition );				// 아이템 위치로 아이템 정보를 받는다.

	pItem->SetDurability(pmsg->nItemCount) ;

	QUICKMGR->RefreshQickItem();																	// 퀵 매니져의 퀵 아이템 새로고침을 실행한다.

	if( !pItem ) return;																			// 아이템 정보가 유효하지 않으면 리턴 처리를 한다.

	CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(297), pItem->GetItemInfo()->ItemName );			// 아이템이 사라졌다는 메시지를 출력한다.
}

// 071028 LYW --- CQuestManager : Add function to process give item error.
void CQuestManager::Quest_GiveItem_Error( void* pMsg )						// 퀘스트 아이템 반납에 실패한 에러 처리를 하는 함수.
{
	ASSERT(pMsg);

	CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg( 1285 ) );

	MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg ;									// 에러 처리에 맞는 데이터 형번환을 한다.

	sUSERINFO_ERR_GIVEITEM EGI ;											// 에러 내용을 담을 구조체를 선언한다.
	memset(&EGI, 0, sizeof(sUSERINFO_ERR_GIVEITEM)) ;						// 구조체를 초기화 한다.

	if( HERO == NULL ) return ;												// 히어로 정보가 없으면 리턴 처리를 한다.

	strcpy(EGI.UserName, HERO->GetObjectName()) ;							// 유저 캐릭터의 이름을 복사한다.
	EGI.bCaption = (BYTE)pmsg->dwData1 ;									// 에러 발생 함수를 세팅한다.
	EGI.bErrNum	 = (BYTE)pmsg->dwData2 ;									// 에러 번호를 세팅한다.
	EGI.dwQuestIdx = pmsg->dwData3 ;										// 퀘스트 인덱스를 세팅한다.
	EGI.dwItemIdx  = pmsg->dwData4 ;										// 아이템 인덱스를 세팅한다.

	char strFilePath[MAX_PATH] = {0, } ;									// 파일을 기록할 경로를 담을 버퍼를 선언하고 초기화 한다.

	wsprintf( strFilePath, "%s\\data\\interface\\log\\%d.EGI", 
		DIRECTORYMGR->GetFullDirectoryName(eLM_Root),gHeroID);

	HANDLE hFile = CreateFile( strFilePath, GENERIC_WRITE, 0, NULL, 
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ) ;						// 파일을 생성한다.

	if( hFile == INVALID_HANDLE_VALUE )										// 결과가 실패 했으면,
	{
		return ;															// return.
	}

	DWORD dwSize ;															// 사이즈를 담을 변수를 선언한다.

	WriteFile( hFile, &EGI, sizeof(EGI), &dwSize, NULL ) ;					// 에러 내용을 기록한다.

	CloseHandle( hFile ) ;
}

void CQuestManager::Quest_GiveMoney_Ack( void* pMsg )												// 머니를 주는 함수.
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;																// 메시지를 받을 포인터를 선언하고 메시지를 받는다.

	CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg(236), AddComma(pmsg->dwData) );					// 퀘스트 보상으로 골드를 얻었다는 메시지를 출력한다.
}

void CQuestManager::Quest_Time_Limit( void* pMsg )													// 퀘스트 시간 제한 함수.
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;															// 메시지를 받을 포인터를 선언하고 메시지를 받는다.

	QuestTimeLimit( pmsg->dwData1, pmsg->dwData2 );													// 들어온 정보로 퀘스트 시간 제한을 한다.
}

void CQuestManager::Quest_Regist_Checktime( void* pMsg )											// 시간 체크 등록을 하는 함수.
{
	MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;															// 메시지를 받을 포인터를 선언하고 메시지를 받는다.

	RegistCheckTime( pmsg->dwData1, (BYTE)pmsg->dwData2, pmsg->dwData3 );							// 시간 체크를 등록한다.
}

void CQuestManager::Quest_Unregist_Checktime( void* pMsg )											// 시간 체크 등록을 취소하는 함수.
{
	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;																// 메시지를 받을 포인터를 선언하고 메시지를 받는다.

	UnRegistCheckTime( pmsg->dwData );																// 시간 체크를 취소한다.
}

void CQuestManager::QuestEvent_NpcTalk_Ack( void* pMsg )											// npc 대화 요청 이벤트 함수.
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;															// 메시지를 받을 포인터를 선언하고 메시지를 받는다.

	QuestEventAck( pmsg->dwData1 );																	// 퀘스트 이벤트를 수락한다.
}

void CQuestManager::QuestEvent_NpcTalk_Nack( void* pMsg )											// npc 대화요청 실패 이벤트 함수.
{
	MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;															// 메시지를 받을 포인터를 선언하고 메시지를 받는다.

	QuestEventNack( pmsg->dwData1, pmsg->dwData2 );													// 퀘스트 이벤트 실패 처리를 한다.
}

void CQuestManager::Quest_Execute_Error( void* pMsg )												// 퀘스트 실행 에러 처리 함수.
{
	MSG_INT* pmsg = (MSG_INT*)pMsg;																	// 메시지를 받을 포인터를 선언하고 메시지를 받는다.

	int nMsgNum = 0 ;																				// 에러 메시지 번호를 세팅할 변수를 선언하고 0으로 세팅한다.

	switch(pmsg->nData)																				// 메시지로 넘어온 에러 코드를 확인한다.
	{
	case e_EXC_ERROR_NO_ERROR :					nMsgNum = 990 ;		break ;							// 에러 코드에 맞는 메시지 번호를 지정한다.
	case e_EXC_ERROR_NO_PLAYERINFO :			nMsgNum = 991 ;		break ;
	case e_EXC_ERROR_NO_QUESTGROUP :			nMsgNum = 992 ;		break ;
	case e_EXC_ERROR_NO_QUEST :					nMsgNum = 993 ;		break ;
	case e_EXC_ERROR_NO_INVENTORYINFO :			nMsgNum = 994 ;		break ;
	case e_EXC_ERROR_NO_EXTRASLOT :				nMsgNum = 995 ;		break ;
	case e_EXC_ERROR_NO_EXTRASLOTQUEST :		nMsgNum = 996 ;		break ;
	case e_EXC_ERROR_NO_QUESTITEM :				nMsgNum = 997 ;		break ;
	case e_EXC_ERROR_NOT_ENOUGH_QUESTITEM :		nMsgNum = 998 ;		break ;
	case e_EXC_ERROR_NO_GIVEITEM :				nMsgNum = 999 ;		break ;
	case e_EXC_ERROR_NOT_ENOUGH_GIVEITEM :		nMsgNum = 1000 ;	break ;
	case e_EXC_ERROR_NOT_ENOUGH_GIVEMONEY :		nMsgNum = 1001 ;	break ;
	//case e_EXC_ERROR_NOT_SAME_WEAPONKIND :		nMsgNum = 1002 ;	break ;
	// 071223 LYW --- QuestManager : 퀘스트 에러 메시지 출력을 알림창에서 채팅창 출력으로 바꾼다.
	case e_EXC_ERROR_NOT_SAME_WEAPONKIND :
		{
			CHATMGR->AddMsg( CTC_CHEAT_1, CHATMGR->GetChatMsg(1002) ) ;
			return ;
		}
		break ;
	case e_EXC_ERROR_NOT_SAME_WEAPONKIND_COUNT :nMsgNum = 1003 ;	break ;
	case e_EXC_ERROR_CANT_RECEIVE_MONEY :		nMsgNum = 1004 ;	break ;
	// 100414 ONS 보상이 선택되지 않았을 경우 메세지 출력
	case e_EXC_ERROR_NOT_EXIST_REQUITAL:		nMsgNum = 2214 ;	break;
	}

	if( pmsg->nData != e_EXC_ERROR_NOT_EXIST_REQUITAL)
	{
		GAMEIN->GetNpcScriptDialog()->EndDialog();														// npc 스크립트 다이얼로그로 end 다이얼로그를 호출한다.
		GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);;												// npc 스크립트 다이얼로그를 비활성화 한다.
	}
	WINDOWMGR->MsgBox( MBI_QUESTMSG, MBT_OK, CHATMGR->GetChatMsg(nMsgNum) );						// 소지품에 빈 공간이 없어 진행 불가라는 메시지를 출력한다.
}

void CQuestManager::Quest_Error_Ext( void* pMsg )													// 퀘스트 기타 에러 처리 함수.
{
	if( !pMsg ) return ;																			// 메시지 정보가 유효하지 않으면, 리턴처리한다.

	MSG_DWORD* pmsg = (MSG_DWORD*)pMsg ;															// 메시지를 받을 포인터를 선언하고 메시지를 받는다.

	ITEM_INFO* pItemInfo = ITEMMGR->GetItemInfo(pmsg->dwData);										// 메시지로 넘어온 아이템 인덱스로 아이템 정보를 받는다.

	if( !pItemInfo ) return ;																		// 아이템 정보가 유요하지 않으면, return 처리 한다.

	char tempBuf[128] = {0,} ;																		// 메시지 출력을 위한 임시 버퍼를 선언한다.
	//sprintf(tempBuf, "[%s] 아이템의 개수가 부족하여 퀘스트를 완료할 수 없습니다.", pItemInfo->ItemName) ;	// 에러 메시지를 조합한다.
	sprintf(tempBuf, CHATMGR->GetChatMsg(986), pItemInfo->ItemName) ;	// 에러 메시지를 조합한다.

	GAMEIN->GetNpcScriptDialog()->EndDialog();														// npc 스크립트 다이얼로그로 end 다이얼로그를 호출한다.
	GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);;												// npc 스크립트 다이얼로그를 비활성화 한다.
	WINDOWMGR->MsgBox( MBI_QUESTMSG, MBT_OK, tempBuf );												// 소지품에 빈 공간이 없어 진행 불가라는 메시지를 출력한다.
}

void CQuestManager::Quest_Error_FullQuest( void* pMsg )
{
	if( !pMsg ) return ;

	char tempBuf[128] = {0,} ;																		// 메시지 출력을 위한 임시 버퍼를 선언한다.
	sprintf(tempBuf, CHATMGR->GetChatMsg(1474)) ;

	GAMEIN->GetNpcScriptDialog()->EndDialog();														// npc 스크립트 다이얼로그로 end 다이얼로그를 호출한다.
	GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);;												// npc 스크립트 다이얼로그를 비활성화 한다.
	WINDOWMGR->MsgBox( MBI_QUESTMSG, MBT_OK, tempBuf );												// 소지품에 빈 공간이 없어 진행 불가라는 메시지를 출력한다.
}

// 070329 LYW --- QuestManager : Add function to make string.
void CQuestManager::CompositString( char* inputStr, char* str, ... )								// 출력용 메시지를 완성하는 함수.
{
	if( inputStr == NULL ) return ;																	// 들어온 스트링 정보가 유효하지 않으면 리턴 처리를 한다.
	if( str == NULL ) return;																		// 스트링 정보가 유효하지 않으면 리턴 처리를 한다.

	va_list argList;																				// 가변 인자 변수를 선언한다.

	va_start(argList, str);																			// 가변 인자 해석을 시작한다.
	vsprintf(inputStr,str,argList);																	// 메시지를 완성한다.
	va_end(argList);																				// 가변 인자 사용 완료.
}

void CQuestManager::LoadQuickViewInfoFromFile(DWORD* pIdx)
{
	if( !pIdx )
	{
		return ;
	}

	CQuestQuickViewDialog* pQuickViewDlg = NULL ;
	pQuickViewDlg = GAMEIN->GetQuestQuickViewDialog() ;

	if( pQuickViewDlg )
	{
		for( int index = 0 ; index < MAX_QUICKVIEW_QUEST ; ++index )
		{
			pQuickViewDlg->RegistQuest(pIdx[index]) ;
		}
		// 100601 ONS 퀘스트 알리미 등록창을 업데이트한다.
		pQuickViewDlg->ResetQuickViewDialog();
	}
}

int CQuestManager::CheckQuestType(DWORD wQuestIdx, DWORD wSubQuestIdx)							
{
	CQuest* pQuest = QUESTMGR->GetQuest( wQuestIdx );							// 퀘스트 정보를 받는다.
	CSubQuestInfo* pDescSubQuestInfo ;												// 서브 퀘스트 정보를 받을 포인터를 선언한다.

	pDescSubQuestInfo = NULL ;
	pDescSubQuestInfo = pQuest->GetSubQuestInfo(wSubQuestIdx) ;

	if( !pDescSubQuestInfo ) return e_QT_ERROR_TYPE ;

	CQuestNpcScript* pQuestNpcScript = pDescSubQuestInfo->GetQuestNpcScript();
	if( !pQuestNpcScript )
		return e_QT_ERROR_TYPE;

	return pQuestNpcScript->GetDialogType();
}

void CQuestManager::NotifyChangedDateTime( const DWORD dwQuestIdx )
{
	CQuestInfo* pQuestInfo = m_QuestInfoTable.GetData( dwQuestIdx );
	CQuest* pQuest = GetQuest( dwQuestIdx );
	if( !pQuestInfo || !pQuest )
		return;

	// 100621 ShinJS 퀘스트가 수행가능인 경우만 메세지를 띄운다
	if( pQuest->IsQuestComplete() == 1 ||
		pQuest->IsQuestState( pQuestInfo->GetNpcIdx() ) != eQuestState_Possible )
		return;

	const BOOL bIsValid = pQuest->IsValidDateTime();

	int Key = 0;
	COMBINEKEY( dwQuestIdx, 0, Key );
	CQuestString* pQString = m_QuestStringTable.GetData(Key);
	if( pQString == NULL )	return;
	cPtrList* p = pQString->GetTitle();
	ITEM* string = (ITEM*)p->GetHead();

	// 비활성->활성, 활성->다음 활성 상태로 변경된 경우
	if( bIsValid )
	{
		// 퀘스트가 완료되었거나 맵이 다른 경우 알리지 않는다
		int nMapNum = pQuestInfo->GetMapNum();
		if( pQuest->IsQuestComplete() ||
			MAP->GetMapNum() != nMapNum )
			return;

		// 수행가능한 경우
		CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg( 2217 ), string->string );			// "[%s] 퀘스트를 수행할 수 있습니다."

	}
	// 활성->비활성, 비활성->다음 비활성
	else
	{
		// 퀘스트가 수행중이고 완료되지 않은 경우
		if( pQuest->IsPerforming() && 
			pQuest->IsQuestComplete() == 0 )
		{
			CHATMGR->AddMsg( CTC_QUEST, CHATMGR->GetChatMsg( 2218 ), string->string );		// "[%s] 퀘스트를 수행할 수 없습니다."
		}
	}
}

void CQuestManager::Process()
{
	// 100405 ShinJS --- 시간제한 퀘스트 검사
	static DWORD dwDateLimitCheckTime;
	if( dwDateLimitCheckTime < gCurTime )
	{
		dwDateLimitCheckTime = gCurTime + 1000;

		BOOL bChanged = FALSE;
		for( std::set< DWORD >::const_iterator iterDateLimitQuest = m_setDateLimitQuest.begin() ; iterDateLimitQuest != m_setDateLimitQuest.end() ; ++iterDateLimitQuest )
		{
			const DWORD dwQuestIdx = *iterDateLimitQuest;

			CQuestInfo* pQuestInfo = m_QuestInfoTable.GetData( dwQuestIdx );
			CQuest* pQuest = GetQuest( dwQuestIdx );
			if( !pQuestInfo || !pQuest )
				continue;

			__time64_t nextDateTime = pQuest->GetNextDateTime();
			pQuest->IsValidDateTime();
			// 상태가 변경된 경우
			if( pQuest->GetNextDateTime() != nextDateTime )
			{
				// 유효 시간 상태가 변경된 경우를 알려준다.
				NotifyChangedDateTime( dwQuestIdx );
				bChanged = TRUE;
			}
		}

		if( bChanged )
		{
			ProcessNpcMark();
			m_QuestDialog->RefreshQuestList() ;
		}
	}
}

void CQuestManager::InitDateTime()
{
	BOOL bChanged = FALSE;
	for( std::set< DWORD >::const_iterator iterDateLimitQuest = m_setDateLimitQuest.begin() ; iterDateLimitQuest != m_setDateLimitQuest.end() ; ++iterDateLimitQuest )
	{
		const DWORD dwQuestIdx = *iterDateLimitQuest;
		
		CQuest* pQuest = GetQuest( dwQuestIdx );
		if( !pQuest )
			continue;

		// 상태가 변경된 경우
		__time64_t nextDateTime = pQuest->GetNextDateTime();
		pQuest->InitDateTime();
		if( pQuest->GetNextDateTime() != nextDateTime )
		{
			// 유효 시간 상태가 변경된 경우를 알려준다.
			NotifyChangedDateTime( dwQuestIdx );
			bChanged = TRUE;
		}
	}

	if( bChanged )
	{
		ProcessNpcMark();
		m_QuestDialog->RefreshQuestList() ;
	}
}

BOOL CQuestManager::IsValidDateTime( DWORD dwQuestIdx )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );
	if( !pQuest )
		return FALSE;

	return pQuest->IsValidDateTime();
}

BOOL CQuestManager::IsCompletedDemandQuestExecute( DWORD dwQuestIdx )
{
	CQuest* pQuest = m_QuestTable.GetData( dwQuestIdx );
	if( !pQuest )
		return FALSE;

	return pQuest->IsCompletedDemandQuestExecute();
}

// 100414 ONS 퀘스트 마지막트리거 정보를 가져온다.
cPtrList* CQuestManager::GetLastSubQuestTrigger( DWORD dwQuestIdx )
{
	CQuestInfo* pQuestInfo = GetQuestInfo(dwQuestIdx) ;	
	if( !pQuestInfo ) return NULL;

	int nSubQuestCount = pQuestInfo->GetSubQuestCount();
	if( nSubQuestCount < 1  )	return NULL;

	CSubQuestInfo* pSubQuestInfo = pQuestInfo->GetSubQuestInfo(nSubQuestCount-1) ;
	if( !pSubQuestInfo ) return NULL;

	return &(pSubQuestInfo->GetTriggerList());
}

void CQuestManager::AddNpcHasQuest( DWORD dwUniqueNpcIdx, DWORD dwQuestIdx )
{
	NpcHasQuestList& hasQuestList = m_NpcHasQuestList[ dwUniqueNpcIdx ];
	hasQuestList.insert( dwQuestIdx );
}