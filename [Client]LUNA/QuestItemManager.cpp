// QuestItemManager.cpp: implementation of the CQuestItemManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "QuestItemManager.h"
#include "mhFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CQuestItemManager::CQuestItemManager()																// 생성자 함수.
{
	m_QuestItemList.Initialize(200);																// 퀘스트 아이템 리스트를 200개로 초기화 한다.
}

CQuestItemManager::~CQuestItemManager()																// 소멸자 함수.
{	
	Release();																						// 해제 함수를 호출한다.
}

void CQuestItemManager::Release()																	// 해제 함수.
{
	m_QuestItemList.SetPositionHead();

	for(QUEST_ITEM_INFO* pData = m_QuestItemList.GetData();
		0 < pData;
		pData = m_QuestItemList.GetData())
	{
		SAFE_DELETE(
			pData);
	}

	m_QuestItemList.RemoveAll();																	// 퀘스트 아이템 리스트를 모두 비운다.
}

void CQuestItemManager::QuestItemInfoLoad()															// 퀘스트 아이템 정보를 로딩하는 함수.
{
	CMHFile file;																					// 묵향 파일을 선언한다.

	if(!file.Init("./system/Resource/QuestItemList.bin", "rb"))										// 퀘스트 아이템 리스트 파일을 읽기 모드로 연다.
	{
		return;																						// 읽기에 실패하면,
	}

	int Key_1, Key_2;																				// 키값 1,2를 선언한다.
	int Key = 0;																					// 키 값을 받을 변수를 선언하고 0으로 세팅한다.
	QUEST_ITEM_INFO * pInfo = NULL;	

	int max = 0;																					// 최대 값을 담을 변수를 선언하고 0으로 세팅한다.

	while(1)																						// while문을 돌린다.
	{
		if(file.IsEOF())																			// 파일의 끝이라면,
			break;																					// break로 빠져나간다.

		ASSERT(!pInfo);																				// 퀘스트 아이템 정보게 assert를 건다.

		pInfo = new QUEST_ITEM_INFO;																// 퀘스트 아이템 정보에 해당하는 메모리를 할당한다.

		pInfo->ItemIdx = file.GetDword();															// 퀘스트 아이템 인덱스를 읽어들인다.

		SafeStrCpyEmptyChange(pInfo->ItemName, file.GetString(), MAX_ITEMNAME_LENGTH+1);			// 퀘스트 스트링을 읽어들여 복사한다.

		Key_1 = file.GetDword();																	// 키 값 1을 읽어들인다.
		Key_2 = file.GetDword();																	// 키 값 2를 읽어들인다.

		COMBINEKEY(Key_1, Key_2, Key);																// 키 값 1,2를 결합하여 키 값을 받는다.

		pInfo->QuestKey = Key;																		// 퀘스트 키값을 세팅한다.

		pInfo->Image2DNum = file.GetWord();															// 2D 이미지 번호를 받는다.
		pInfo->SellPrice = file.GetWord();															// 판매 가격을 받는다.

		SafeStrCpyEmptyChange(pInfo->ItemDesc, file.GetString(), MAX_ITEMDESC_LENGTH+1);			// 퀘스트 스트링을 읽어들여 아이템 desc에 복사한다.

		ASSERT(m_QuestItemList.GetData(pInfo->ItemIdx) == FALSE);									// 퀘스트 아이템 리스트에 assert를 건다.

		m_QuestItemList.Add(pInfo, pInfo->ItemIdx);													// 퀘스트 아이템 리스트에, 아이템 정보를 추가한다.

		max = pInfo->ItemIdx;																		// 최대 아이템 인덱스를 받는다.

		pInfo = NULL;																				// 퀘스트 정보를 NULL 처리 한다.
	}

	file.Release();																					// 파일을 해제 한다.
}


QUEST_ITEM_INFO* CQuestItemManager::GetQuestItem(DWORD ItemIdx)										// 퀘스트 아이템을 리턴 하는 함수.
{
	return m_QuestItemList.GetData(ItemIdx);														// 퀘스트 아이템 데이터를 받는다.
}
