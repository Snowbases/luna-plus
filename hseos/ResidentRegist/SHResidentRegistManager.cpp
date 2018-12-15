/*********************************************************************

	 파일		: SHResidentRegist.cpp
	 작성자		: hseos
	 작성일		: 2007/06/04

	 파일설명	: 주민등록 클래스의 소스

 *********************************************************************/

#include "stdafx.h"
#include "../../hseos/SHMain.h"
#include "../../hseos/Common/SHMath.h"
#include "SHResidentRegistManager.h"
#include "MHFile.h"

#if defined(_AGENTSERVER)
	#include "Network.h"
	#include "AgentDBMsgParser.h"
	#include "AgentNetworkMsgParser.h"
	#include "FilteringTable.h"
	#include "../../hseos/Date/SHDateManager.h"
#elif defined(_MAPSERVER_)
	#include "MapDBMsgParser.h"
	#include "NetWork.h"
	#include "UserTable.h"
	#include "Player.h"
	#include "PackedData.h"
	#include "Party.h"
	#include "PartyManager.h"
	#include "Itemmanager.h"
#else
	#include "ObjectManager.h"
	#include "ObjectStateManager.h"

	#include "../GameIn.h"
	#include "../DateMatchingDlg.h"
	#include "../ChatManager.h"
	#include "../WindowIDEnum.h"	
	#include "../cMsgBox.h"

	#include "../Interface/cTextArea.h"
	#include "../Interface/cWindowManager.h"
	#include "../Interface/cListDialog.h"
	#include "PartyManager.h"

	#include "NpcScriptDialog.h"
	#include "../../hseos/Date/SHChallengeZoneListDlg.h"
#endif

CSHResidentRegistManager g_csResidentRegistManager;

const int CSHResidentRegistManager::MAX_SERCH_NUM_PER_AGENT			= 30;
const int CSHResidentRegistManager::MAX_SERCH_NUM_CLIENT			= CSHResidentRegistManager::MAX_SERCH_NUM_PER_AGENT	*4;
const int CSHResidentRegistManager::DATE_MATCHING_RESERCH_TIME		= 4000;

// -------------------------------------------------------------------------------------------------------------------------------------
// CSHResidentRegistManager Method																								  생성자
//
CSHResidentRegistManager::CSHResidentRegistManager()
{
	m_pstSerchResult = NULL;
	m_stAddInfo.m_pTblPropensity = NULL;
	m_stAddInfo.m_pTblLocation = NULL;
	m_nSerchResultNum = 0;
	LoadScriptFileData();

	#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
		m_pstSerchResult = new stSerchResult[MAX_SERCH_NUM_CLIENT];
	#endif

	ZeroMemory(&m_stChatPartner, sizeof(m_stChatPartner));
}

// -------------------------------------------------------------------------------------------------------------------------------------
// ~CSHResidentRegist Method																									  파괴자
//
CSHResidentRegistManager::~CSHResidentRegistManager()
{
	SAFE_DELETE_ARRAY(m_stAddInfo.m_pTblPropensity);
	SAFE_DELETE_ARRAY(m_stAddInfo.m_pTblLocation);
	SAFE_DELETE_ARRAY(m_stAddInfo.m_pTblSerchAgeRange);
	SAFE_DELETE_ARRAY(m_pstSerchResult);
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  LoadScriptData Method																							  스크립트 파일 로드
//
BOOL CSHResidentRegistManager::LoadScriptFileData()
{
	char szLine[MAX_PATH], szTxt[MAX_PATH];
	int	 nID = 0, nKind = 0, nIndex = 0, nMin = 0, nMax = 0;
	CMHFile fp;

	// 최대개수를 얻기 위해 처음 읽는 부분
	fp.Init("./System/Resource/ResidentRegist.bin", "rb");
	if(!fp.IsInited())
	{
		return FALSE;
	}

	while(TRUE)
	{
		fp.GetLine(szLine, sizeof(szLine));
		if (strstr(szLine, "//")) continue;			
		else if (strstr(szLine, "END_KIND")) 
		{
			nKind++;
			continue;
		}

		switch(nKind)
		{
		// 성향
		case 0: m_stAddInfo.m_nMaxPropensity++;			break;
		// 접속 지역
		case 1:	m_stAddInfo.m_nMaxLocation++;			break;
		// 검색 나이범위
		case 2:	m_stAddInfo.m_nMaxSerchAgeRange++;		break;
		}

		if (fp.IsEOF()) break;
	}

	fp.Release();

	m_stAddInfo.m_pTblPropensity	= new WORD[m_stAddInfo.m_nMaxPropensity];
	m_stAddInfo.m_pTblLocation		= new stLocation[m_stAddInfo.m_nMaxLocation];
	m_stAddInfo.m_pTblSerchAgeRange	= new stSerchAgeRange[m_stAddInfo.m_nMaxSerchAgeRange];

	// 데이터를 읽기 위해 두 번째..
	nKind = 0;
	fp.Init("./System/Resource/ResidentRegist.bin", "rb");
	if(!fp.IsInited())
	{
		return FALSE;
	}
	while(TRUE)
	{
		fp.GetLine(szLine, sizeof(szLine));
		if (strstr(szLine, "//")) continue;			
		else if (strstr(szLine, "END_KIND"))
		{
			nKind++;
			continue;
		}

		switch(nKind)
		{
		// 성향
		case 0:
			sscanf(szLine, "%d %s", &nID, szTxt);
			if		(strcmp(szTxt, "HAIR_BLACK") == 0)		nIndex = PROPENSITY_KIND_HAIR_BLACK;
			else if (strcmp(szTxt, "HAIR_DYEING") == 0)		nIndex = PROPENSITY_KIND_HAIR_DYEING;
			else if (strcmp(szTxt, "CLOTHES_SUIT") == 0)	nIndex = PROPENSITY_KIND_CLOTHES_SUIT;
			else if (strcmp(szTxt, "CLOTHES_CASUAL") == 0)	nIndex = PROPENSITY_KIND_CLOTHES_CASUAL;
			else if (strcmp(szTxt, "MAKEUP") == 0)			nIndex = PROPENSITY_KIND_MAKEUP;
			else if (strcmp(szTxt, "PERFUME") == 0)			nIndex = PROPENSITY_KIND_PERFUME;
			else if (strcmp(szTxt, "BODY_THIN") == 0)		nIndex = PROPENSITY_KIND_BODY_THIN;
			else if (strcmp(szTxt, "BODY_FAT") == 0)		nIndex = PROPENSITY_KIND_BODY_FAT;
			else if (strcmp(szTxt, "BEARD") == 0)			nIndex = PROPENSITY_KIND_BEARD;
			else if (strcmp(szTxt, "GLASSES") == 0)			nIndex = PROPENSITY_KIND_GLASSES;
			else if (strcmp(szTxt, "HAT") == 0)				nIndex = PROPENSITY_KIND_HAT;
			else if (strcmp(szTxt, "ACCESSORY") == 0)		nIndex = PROPENSITY_KIND_ACCESSORY;
			else if (strcmp(szTxt, "SUMMER") == 0)			nIndex = PROPENSITY_KIND_SUMMER;
			else if (strcmp(szTxt, "WINTER") == 0)			nIndex = PROPENSITY_KIND_WINTER;
			else if (strcmp(szTxt, "BLOOD_A") == 0)			nIndex = PROPENSITY_KIND_BLOOD_A;
			else if (strcmp(szTxt, "BLOOD_B") == 0)			nIndex = PROPENSITY_KIND_BLOOD_B;
			else if (strcmp(szTxt, "BLOOD_O") == 0)			nIndex = PROPENSITY_KIND_BLOOD_O;
			else if (strcmp(szTxt, "BLOOD_AB") == 0)		nIndex = PROPENSITY_KIND_BLOOD_AB;
			else if (strcmp(szTxt, "BOTTLE_STAR") == 0)		nIndex = PROPENSITY_KIND_BOTTLE_STAR;
			else if (strcmp(szTxt, "FISH_STAR") == 0)		nIndex = PROPENSITY_KIND_FISH_STAR;
			else if (strcmp(szTxt, "SHEEP_STAR") == 0)		nIndex = PROPENSITY_KIND_SHEEP_STAR;
			else if (strcmp(szTxt, "OX_STAR") == 0)			nIndex = PROPENSITY_KIND_OX_STAR;
			else if (strcmp(szTxt, "TWINS_STAR") == 0)		nIndex = PROPENSITY_KIND_TWINS_STAR;
			else if (strcmp(szTxt, "CRAB_STAR") == 0)		nIndex = PROPENSITY_KIND_CRAB_STAR;
			else if (strcmp(szTxt, "LION_STAR") == 0)		nIndex = PROPENSITY_KIND_LION_STAR;
			else if (strcmp(szTxt, "VIRGIN_STAR") == 0)		nIndex = PROPENSITY_KIND_VIRGIN_STAR;
			else if (strcmp(szTxt, "WEIGHT_STAR") == 0)		nIndex = PROPENSITY_KIND_WEIGHT_STAR;
			else if (strcmp(szTxt, "SCORPION_STAR") == 0)	nIndex = PROPENSITY_KIND_SCORPION_STAR;
			else if (strcmp(szTxt, "ARCHER_STAR") == 0)		nIndex = PROPENSITY_KIND_ARCHER_STAR;
			else if (strcmp(szTxt, "GOAT_STAR") == 0)		nIndex = PROPENSITY_KIND_GOAT_STAR;
			else if (strcmp(szTxt, "CLEAN") == 0)			nIndex = PROPENSITY_KIND_CLEAN;
			else if (strcmp(szTxt, "COOKING") == 0)			nIndex = PROPENSITY_KIND_COOKING;
			else if (strcmp(szTxt, "PARTY") == 0)			nIndex = PROPENSITY_KIND_PARTY;
			else if (strcmp(szTxt, "LONELY") == 0)			nIndex = PROPENSITY_KIND_LONELY;
			else if (strcmp(szTxt, "SPORTS") == 0)			nIndex = PROPENSITY_KIND_SPORTS;
			else if (strcmp(szTxt, "MUSIC") == 0)			nIndex = PROPENSITY_KIND_MUSIC;
		#if defined(_DEBUG)
			else 
			{
				char szErr[2048];

				sprintf(szErr, "%s %s() %d\n존재하지 않는 키워드입니다.(키워드:%s, 파일:ResidentRegist.bin)", __FILE__, __FUNCTION__, __LINE__, szTxt);
				MessageBox(NULL, szErr, NULL, NULL);
			}
		#endif

			m_stAddInfo.m_pTblPropensity[nIndex] = (WORD)nID;
			break;
		// 접속 지역
		case 1:
			sscanf(szLine, "%d %s", &nID, szTxt);

			m_stAddInfo.m_pTblLocation[nID].nID = (WORD)nID;
			SafeStrCpy(m_stAddInfo.m_pTblLocation[nID].szName, szTxt, MAX_PATH);
		#if defined(_DEBUG)
			// 바로 전의 데이터와 비교해서 ID값이 +1 된 것이 아니라면 오류 메세지 출력
			if (nID > 0)
			{
				if (m_stAddInfo.m_pTblLocation[nID-1].nID != m_stAddInfo.m_pTblLocation[nID].nID-1)
				{
					char szErr[2048];

					sprintf(szErr, "%s %s() %d\n접속지역 ID가 중복되었거나 순차적이지 않습니다.(파일:ResidentRegist.bin)", __FILE__, __FUNCTION__, __LINE__);
					MessageBox(NULL, szErr, NULL, NULL);
				}
			}
		#endif
			break;
		// 검색 나이 범위
		case 2:
			sscanf(szLine, "%d %d %d", &nID, &nMin, &nMax);

			m_stAddInfo.m_pTblSerchAgeRange[nID].nMin = (WORD)nMin;
			m_stAddInfo.m_pTblSerchAgeRange[nID].nMax = (WORD)nMax;
			break;
		}

		if (fp.IsEOF()) break;
	}
	
	fp.Release();

	return TRUE;
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_CalcMatchingPoint Method																		
//
/*
UINT CSHResidentRegistManager::ASRV_CalcMatchingPoint(USERINFO* pUserInfo, USERINFO* pSerchInfo)
{
	int nMatchingPoint = 100;
	int nMinusPoint = 0;

	// 차이나는 나이만큼 -, 최대 -값은 30
	nMinusPoint += abs((int)pUserInfo->DateMatchingInfo.dwAge - (int)pSerchInfo->DateMatchingInfo.dwAge);
	if (nMinusPoint > 30) nMinusPoint = 30;

	// 접속 지역이 다르면 -10
	if (pUserInfo->DateMatchingInfo.dwRegion != pSerchInfo->DateMatchingInfo.dwRegion)
	{
		nMinusPoint += 10;
	}

	// 차이나는 좋아하는 성향 개수만큼 -10
	int nMaxPro = sizeof(pUserInfo->DateMatchingInfo.pdwGoodFeeling)/sizeof(pUserInfo->DateMatchingInfo.pdwGoodFeeling[0]);
	for(int i=0; i<nMaxPro; i++)
	{
		for(int j=0; j<nMaxPro; j++)
		{
			if (pUserInfo->DateMatchingInfo.pdwGoodFeeling[i] == pSerchInfo->DateMatchingInfo.pdwGoodFeeling[j])
			{
				goto NEXT1;
			}
		}
		nMinusPoint += 10;
	NEXT1:;
	}

	// 차이나는 싫어하는 성향 개수만큼 -10
	nMaxPro = sizeof(pUserInfo->DateMatchingInfo.pdwBadFeeling)/sizeof(pSerchInfo->DateMatchingInfo.pdwBadFeeling[0]);
	for(int i=0; i<nMaxPro; i++)
	{
		for(int j=0; j<nMaxPro; j++)
		{
			if (pUserInfo->DateMatchingInfo.pdwBadFeeling[i] == pSerchInfo->DateMatchingInfo.pdwBadFeeling[j])
			{
				goto NEXT2;
			}
		}
		nMinusPoint += 10;
	NEXT2:;
	}

	nMatchingPoint -= nMinusPoint;
	if (nMatchingPoint < 0)	nMatchingPoint = 0;

	return nMatchingPoint;
}
*/
// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_ParseRequestFromClient Method																 클라이언트 요청 분석/처리(에이전트)
//
VOID CSHResidentRegistManager::ASRV_ParseRequestFromClient(DWORD dwConnectionID, char* pMsg, DWORD dwLength)
{
#if defined(_AGENTSERVER)
	MSGBASE* pTempMsg = (MSGBASE*)pMsg;
	
	// 클라이언트에서 패킷 조작으로 남의 캐릭터 ID 를 보낼 수 있기 때문에
	// g_pUserTableForObjectID->FindUser 로 UserInfo 를 구하면 안 됨!!
	USERINFO* pUserInfo = g_pUserTable->FindUser( dwConnectionID );
	if( pUserInfo == NULL ) return;
	pTempMsg->dwObjectID = pUserInfo->dwCharacterID;

	switch( pTempMsg->Protocol )
	{
	case MP_RESIDENTREGIST_DATEMATCHING_REQUEST_CHAT:
		{
			MSG_DWORD2* pPacket = (MSG_DWORD2*)pMsg;

			// 플레이어가 주민등록을 했나?
			if (pUserInfo->DateMatchingInfo.bIsValid == FALSE)
			{
				return;
			}
			// 플레이어가 대화 가능한가?
			// ..해킹 또는 오류의 경우
			if (pUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_NONE)
			{
				return;
			}

			// 대상 플레이어 찾기
			USERINFO* pTargetUserInfo = g_pUserTableForObjectID->FindUser(pPacket->dwData1);
			if (pTargetUserInfo == NULL)
			{
				// 현재 접속되어 있지 않다.
				pPacket->Category			= MP_RESIDENTREGIST;
				pPacket->Protocol			= MP_RESIDENTREGIST_DATEMATCHING_REQUEST_CHAT;
				pPacket->dwObjectID			= pPacket->dwObjectID;
				pPacket->dwData1			= DATE_MATCHING_CHAT_RESULT_LOGOFF;
				g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)pPacket, sizeof(*pPacket) );
				return;
			}

			// 대상 플레이어가 주민등록을 했나?
			if (pTargetUserInfo->DateMatchingInfo.bIsValid == FALSE)
			{
				return;
			}

			// 100629 ONS 데이트매칭 거부옵션 설정 상태 체크
			if( pTargetUserInfo->GameOption.bNoDateMatching == TRUE )
			{
				pPacket->Category			= MP_RESIDENTREGIST;
				pPacket->Protocol			= MP_RESIDENTREGIST_DATEMATCHING_REQUEST_CHAT;
				pPacket->dwObjectID			= pPacket->dwObjectID;
				pPacket->dwData1			= DATE_MATCHING_CHAT_RESULT_REJECTION;
				g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)pPacket, sizeof(*pPacket) );
				return;
			}

			// 대상 플레이어가 대화 가능한가?
			if (pTargetUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_NONE)
			{
				// 채팅 중이거나 채팅 신청(받음) 중이다.
				pPacket->Category			= MP_RESIDENTREGIST;
				pPacket->Protocol			= MP_RESIDENTREGIST_DATEMATCHING_REQUEST_CHAT;
				pPacket->dwObjectID			= pPacket->dwObjectID;
				pPacket->dwData1			= DATE_MATCHING_CHAT_RESULT_BUSY;
				g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)pPacket, sizeof(*pPacket) );
				return;
			}

			// 100107 ONS 대상자가 챌린지존에 있는지 여부를 검사한다.
			if(g_csDateManager.IsChallengeZone( pTargetUserInfo->wUserMapNum ))
			{
				// 신청 대상자가 챌린지존에 있다면 대화신청을 막는다.
				pPacket->Category			= MP_RESIDENTREGIST;
				pPacket->Protocol			= MP_RESIDENTREGIST_DATEMATCHING_REQUEST_CHAT;
				pPacket->dwObjectID			= pPacket->dwObjectID;
				pPacket->dwData1			= DATE_MATCHING_CHAT_RESULT_START_CHALLENGEZONE;
				g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)pPacket, sizeof(*pPacket) );			
				return;
			}
			
			pUserInfo->DateMatchingInfo.nRequestChatState			= DATE_MATCHING_CHAT_REQUEST_STATE_REQUEST;
			pUserInfo->DateMatchingInfo.nChatPlayerID				= pTargetUserInfo->dwCharacterID;
			pUserInfo->DateMatchingInfo.nRequestChatTimeTick		= gCurTime;
			pTargetUserInfo->DateMatchingInfo.nRequestChatState		= DATE_MATCHING_CHAT_REQUEST_STATE_RECV_REQUEST;
			pTargetUserInfo->DateMatchingInfo.nChatPlayerID			= pUserInfo->dwCharacterID;

			// 클라이언트에 알리기
			// ..신청자에게 채팅 신청을 했음을 알리기
			pPacket->Category			= MP_RESIDENTREGIST;
			pPacket->Protocol			= MP_RESIDENTREGIST_DATEMATCHING_REQUEST_CHAT;
			pPacket->dwObjectID			= pUserInfo->dwCharacterID;
			pPacket->dwData1			= DATE_MATCHING_CHAT_RESULT_REQUEST_AND_WAIT;
			g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)pPacket, sizeof(*pPacket) );
			// ..신청자의 맵서버에 신청상태 알리기
			pPacket->Category			= MP_RESIDENTREGIST;
			pPacket->Protocol			= MP_RESIDENTREGIST_DATEMATCHING_REQUEST_CHAT;
			pPacket->dwObjectID			= pUserInfo->dwCharacterID;
 			pPacket->dwData1			= pUserInfo->DateMatchingInfo.nRequestChatState;
			pPacket->dwData2			= gCurTime;
			g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, (char*)pPacket, sizeof(*pPacket) );
			// ..대상자에게 누군가가 채팅 신청을 했음을 알리기
			MSG_DWORD_NAME stPacket;

 			stPacket.Category			= MP_RESIDENTREGIST;
			stPacket.Protocol			= MP_RESIDENTREGIST_DATEMATCHING_REQUEST_CHAT;
			stPacket.dwObjectID			= pTargetUserInfo->dwCharacterID;
			stPacket.dwData				= DATE_MATCHING_CHAT_RESULT_REQUESTED;
			SafeStrCpy(stPacket.Name, pUserInfo->DateMatchingInfo.szName, MAX_NAME_LENGTH+1);
			g_Network.Send2User( pTargetUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );

			// ..대상자에게 신청자의 정보 보내기
			{
				PACKET_SERCH_DATE_MATCHING stPacket;
				ZeroMemory(&stPacket, sizeof(stPacket));
				stPacket.Category											= MP_RESIDENTREGIST;
				stPacket.Protocol											= MP_RESIDENTREGIST_DATEMATCHING_SOMEBODY_INFO;
				stPacket.dwObjectID											= pTargetUserInfo->dwCharacterID;		
				SafeStrCpy(stPacket.stSerchResult.stRegistInfo.szNickName, pUserInfo->DateMatchingInfo.szName, MAX_NAME_LEN+1);
				stPacket.stSerchResult.stRegistInfo.nRace					= (WORD)pUserInfo->DateMatchingInfo.nRace;
				stPacket.stSerchResult.stRegistInfo.nSex					= (WORD)pUserInfo->DateMatchingInfo.byGender;			
				stPacket.stSerchResult.stRegistInfo.nAge					= (WORD)pUserInfo->DateMatchingInfo.dwAge;				
				stPacket.stSerchResult.stRegistInfo.nLocation				= (WORD)pUserInfo->DateMatchingInfo.dwRegion;			
				stPacket.stSerchResult.stRegistInfo.nFavor					= (WORD)pUserInfo->DateMatchingInfo.dwGoodFeelingDegree;	
				stPacket.stSerchResult.stRegistInfo.nPropensityLike01		= (WORD)pUserInfo->DateMatchingInfo.pdwGoodFeeling[0];	
				stPacket.stSerchResult.stRegistInfo.nPropensityLike02		= (WORD)pUserInfo->DateMatchingInfo.pdwGoodFeeling[1];	
				stPacket.stSerchResult.stRegistInfo.nPropensityLike03		= (WORD)pUserInfo->DateMatchingInfo.pdwGoodFeeling[2];	
				stPacket.stSerchResult.stRegistInfo.nPropensityDisLike01	= (WORD)pUserInfo->DateMatchingInfo.pdwBadFeeling[0];	
				stPacket.stSerchResult.stRegistInfo.nPropensityDisLike02	= (WORD)pUserInfo->DateMatchingInfo.pdwBadFeeling[1];	
				stPacket.stSerchResult.nID									= pUserInfo->dwCharacterID;
				stPacket.stSerchResult.nMatchingPoint						= (WORD)CalcMatchingPoint(&pTargetUserInfo->DateMatchingInfo, &pUserInfo->DateMatchingInfo);
				SafeStrCpy(stPacket.stSerchResult.szIntroduction, pUserInfo->DateMatchingInfo.szIntroduce, MAX_INTRODUCE_LEN+1);
				g_Network.Send2User( pTargetUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
			}
		}
		break;
	// 다른 에이전트에 있는 플레이어에게 대화 신청
	case MP_RESIDENTREGIST_DATEMATCHING_REQUEST_CHAT_DIFF_AGENT:
		{
			MSG_DWORD2* pPacket = (MSG_DWORD2*)pMsg;

			// 플레이어가 주민등록을 했나?
			if (pUserInfo->DateMatchingInfo.bIsValid == FALSE)
			{
				return;
			}
			// 플레이어가 대화 가능한가?
			// ..해킹 또는 오류의 경우
			if (pUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_NONE)
			{
				return;
			}

			// 대상 플레이어 에이전트에 확인하러 간다.
			MSG_DWORD4 stPacket;

			stPacket.Category	= pPacket->Category;
			stPacket.Protocol	= MP_RESIDENTREGIST_DATEMATCHING_REQUEST_CHAT_DIFF_AGENT_TARGET_CHECK;
			stPacket.dwObjectID = pPacket->dwData2;					// 대상 플레이어의 에이전트 ID
			stPacket.dwData1	= pPacket->dwData1;					// 대상 플레이어 ID
			stPacket.dwData2	= g_pServerSystem->GetServerNum();	// 요청 에이전트 ID - 결과를 받을 떄 필요
			stPacket.dwData3	= pPacket->dwObjectID;				// 요청 플레이어 ID
			g_Network.Send2SpecificAgentServer((char*)&stPacket, sizeof(stPacket) );
		}
		break;
	case MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT:
		{
			MSG_DWORD2* pPacket = (MSG_DWORD2*)pMsg;
			// 대상 플레이어 찾기
			USERINFO* pTargetUserInfo = g_pUserTableForObjectID->FindUser(pUserInfo->DateMatchingInfo.nChatPlayerID);
			if (pTargetUserInfo == NULL)
			{
				// 대상 플레이어가 같은 에이전트에 있어야 하는 데 없다.
				// ..상태 초기화
				ASRV_InitDateMatchingChatState(pUserInfo);
				// ..신청자의 맵서버에 신청상태 알리기
				pPacket->Category			= MP_RESIDENTREGIST;
				pPacket->Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
				pPacket->dwObjectID			= pUserInfo->dwCharacterID;
 				pPacket->dwData1			= pUserInfo->DateMatchingInfo.nRequestChatState;
				pPacket->dwData2			= gCurTime;
				g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, (char*)pPacket, sizeof(*pPacket) );
				return;
			}

			// 플레이어가 주민등록을 했나?
			if (pUserInfo->DateMatchingInfo.bIsValid == FALSE)
			{
				return;
			}
			// 대상 플레이어가 주민등록을 했나?
			if (pTargetUserInfo->DateMatchingInfo.bIsValid == FALSE)
			{
				return;
			}
			
			int nResult = pPacket->dwData1;
			switch(nResult)
			{
			// 신청자가 취소
			case DATE_MATCHING_CHAT_RESULT_CANCEL:
				{
					// 신청자인가?
					if (pUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_REQUEST)
					{
						return;
					}

					ASRV_InitDateMatchingChatState(pUserInfo);
					ASRV_InitDateMatchingChatState(pTargetUserInfo);
					// 클라이언트에 알리기
					// ..대상자에게 취소되었음을 알리기
					pPacket->Category			= MP_RESIDENTREGIST;
					pPacket->Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
					pPacket->dwObjectID			= pTargetUserInfo->dwCharacterID;
					pPacket->dwData1			= DATE_MATCHING_CHAT_RESULT_CANCEL;
					g_Network.Send2User( pTargetUserInfo->dwConnectionIndex, (char*)pPacket, sizeof(*pPacket) );
					// ..신청자의 맵서버에 신청상태 알리기
					pPacket->Category			= MP_RESIDENTREGIST;
					pPacket->Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
					pPacket->dwObjectID			= pUserInfo->dwCharacterID;
 					pPacket->dwData1			= pUserInfo->DateMatchingInfo.nRequestChatState;
					pPacket->dwData2			= gCurTime;
					g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, (char*)pPacket, sizeof(*pPacket) );
				}
				break;
			// 거절함
			case DATE_MATCHING_CHAT_RESULT_REFUSE:
				{
					// 대상자인가?
					if (pUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_RECV_REQUEST)
					{
						return;
					}

					ASRV_InitDateMatchingChatState(pUserInfo);
					ASRV_InitDateMatchingChatState(pTargetUserInfo);
					// 클라이언트에 알리기
					// ..신청자에게 거절당했음을 알리기
					pPacket->Category			= MP_RESIDENTREGIST;
					pPacket->Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
					pPacket->dwObjectID			= pTargetUserInfo->dwCharacterID;
					pPacket->dwData1			= DATE_MATCHING_CHAT_RESULT_REFUSE;
					g_Network.Send2User( pTargetUserInfo->dwConnectionIndex, (char*)pPacket, sizeof(*pPacket) );
					// ..신청자의 맵서버에 신청상태 알리기
					pPacket->Category			= MP_RESIDENTREGIST;
					pPacket->Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
					pPacket->dwObjectID			= pTargetUserInfo->dwCharacterID;
 					pPacket->dwData1			= pTargetUserInfo->DateMatchingInfo.nRequestChatState;
					pPacket->dwData2			= gCurTime;
					g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, (char*)pPacket, sizeof(*pPacket) );
				}
				break;
			// 채팅시작
			case DATE_MATCHING_CHAT_RESULT_START_CHAT:
				{
					// 대상자인가?
					if (pUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_RECV_REQUEST)
					{
						return;
					}

					pUserInfo->DateMatchingInfo.nRequestChatState		= DATE_MATCHING_CHAT_REQUEST_STATE_CHATTING;
					pTargetUserInfo->DateMatchingInfo.nRequestChatState	= DATE_MATCHING_CHAT_REQUEST_STATE_CHATTING;

					PACKET_SERCH_DATE_MATCHING stPacket;
					ZeroMemory(&stPacket, sizeof(stPacket));
					// 클라이언트에 알리기
					// ..신청자에게 채팅시작 알리기
					stPacket.Category											= MP_RESIDENTREGIST;
					stPacket.Protocol											= MP_RESIDENTREGIST_DATEMATCHING_RESULT_START_CHAT;
					stPacket.dwObjectID											= pTargetUserInfo->dwCharacterID;
					stPacket.bMaster											= TRUE;
					SafeStrCpy(stPacket.stSerchResult.stRegistInfo.szNickName, pUserInfo->DateMatchingInfo.szName, MAX_NAME_LEN+1);
					stPacket.stSerchResult.stRegistInfo.nRace					= (WORD)pUserInfo->DateMatchingInfo.nRace;
					stPacket.stSerchResult.stRegistInfo.nSex					= (WORD)pUserInfo->DateMatchingInfo.byGender;			
					stPacket.stSerchResult.stRegistInfo.nAge					= (WORD)pUserInfo->DateMatchingInfo.dwAge;				
					stPacket.stSerchResult.stRegistInfo.nLocation				= (WORD)pUserInfo->DateMatchingInfo.dwRegion;			
					stPacket.stSerchResult.stRegistInfo.nFavor					= (WORD)pUserInfo->DateMatchingInfo.dwGoodFeelingDegree;	
					stPacket.stSerchResult.stRegistInfo.nPropensityLike01		= (WORD)pUserInfo->DateMatchingInfo.pdwGoodFeeling[0];	
					stPacket.stSerchResult.stRegistInfo.nPropensityLike02		= (WORD)pUserInfo->DateMatchingInfo.pdwGoodFeeling[1];	
					stPacket.stSerchResult.stRegistInfo.nPropensityLike03		= (WORD)pUserInfo->DateMatchingInfo.pdwGoodFeeling[2];	
					stPacket.stSerchResult.stRegistInfo.nPropensityDisLike01	= (WORD)pUserInfo->DateMatchingInfo.pdwBadFeeling[0];	
					stPacket.stSerchResult.stRegistInfo.nPropensityDisLike02	= (WORD)pUserInfo->DateMatchingInfo.pdwBadFeeling[1];	
					stPacket.stSerchResult.nID									= pUserInfo->dwCharacterID;
					stPacket.stSerchResult.nMatchingPoint						= (WORD)CalcMatchingPoint(&pTargetUserInfo->DateMatchingInfo, &pUserInfo->DateMatchingInfo);
					SafeStrCpy(stPacket.stSerchResult.szIntroduction, pUserInfo->DateMatchingInfo.szIntroduce, MAX_INTRODUCE_LEN+1);
					g_Network.Send2User( pTargetUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
					// ..신청자의 맵서버에 신청상태 알리기
					pPacket->Category											= MP_RESIDENTREGIST;
					pPacket->Protocol											= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
					pPacket->dwObjectID											= pTargetUserInfo->dwCharacterID;
 					pPacket->dwData1											= pTargetUserInfo->DateMatchingInfo.nRequestChatState;
					pPacket->dwData2											= gCurTime;
					g_Network.Send2Server( pTargetUserInfo->dwMapServerConnectionIndex, (char*)pPacket, sizeof(*pPacket) );
					// ..대상자에게 채팅시작 알리기
					ZeroMemory(&stPacket, sizeof(stPacket));
					stPacket.Category											= MP_RESIDENTREGIST;
					stPacket.Protocol											= MP_RESIDENTREGIST_DATEMATCHING_RESULT_START_CHAT;
					stPacket.dwObjectID											= pUserInfo->dwCharacterID;		
					SafeStrCpy(stPacket.stSerchResult.stRegistInfo.szNickName, pTargetUserInfo->DateMatchingInfo.szName, MAX_NAME_LEN+1);
					stPacket.stSerchResult.stRegistInfo.nRace					= (WORD)pTargetUserInfo->DateMatchingInfo.nRace;
					stPacket.stSerchResult.stRegistInfo.nSex					= (WORD)pTargetUserInfo->DateMatchingInfo.byGender;			
					stPacket.stSerchResult.stRegistInfo.nAge					= (WORD)pTargetUserInfo->DateMatchingInfo.dwAge;				
					stPacket.stSerchResult.stRegistInfo.nLocation				= (WORD)pTargetUserInfo->DateMatchingInfo.dwRegion;			
					stPacket.stSerchResult.stRegistInfo.nFavor					= (WORD)pTargetUserInfo->DateMatchingInfo.dwGoodFeelingDegree;	
					stPacket.stSerchResult.stRegistInfo.nPropensityLike01		= (WORD)pTargetUserInfo->DateMatchingInfo.pdwGoodFeeling[0];	
					stPacket.stSerchResult.stRegistInfo.nPropensityLike02		= (WORD)pTargetUserInfo->DateMatchingInfo.pdwGoodFeeling[1];	
					stPacket.stSerchResult.stRegistInfo.nPropensityLike03		= (WORD)pTargetUserInfo->DateMatchingInfo.pdwGoodFeeling[2];	
					stPacket.stSerchResult.stRegistInfo.nPropensityDisLike01	= (WORD)pTargetUserInfo->DateMatchingInfo.pdwBadFeeling[0];	
					stPacket.stSerchResult.stRegistInfo.nPropensityDisLike02	= (WORD)pTargetUserInfo->DateMatchingInfo.pdwBadFeeling[1];	
					stPacket.stSerchResult.nID									= pTargetUserInfo->dwCharacterID;
					stPacket.stSerchResult.nMatchingPoint						= (WORD)CalcMatchingPoint(&pTargetUserInfo->DateMatchingInfo, &pUserInfo->DateMatchingInfo);
					SafeStrCpy(stPacket.stSerchResult.szIntroduction, pTargetUserInfo->DateMatchingInfo.szIntroduce, MAX_INTRODUCE_LEN+1);
					g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
				}
				break;
			// 채팅끝
			case DATE_MATCHING_CHAT_RESULT_END_CHAT:
				{
					ASRV_InitDateMatchingChatState(pUserInfo);
					// 클라이언트에 알리기
					// ..신청자?에게 채팅끝 알리기
					pPacket->Category			= MP_RESIDENTREGIST;
					pPacket->Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
					pPacket->dwObjectID			= pUserInfo->dwCharacterID;
					pPacket->dwData1			= DATE_MATCHING_CHAT_RESULT_END_CHAT;
					pPacket->dwData2			= pTargetUserInfo->dwCharacterID;			// 상대ID
					g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)pPacket, sizeof(*pPacket) );
					// ..신청자의 맵서버에 신청상태 알리기
					pPacket->Category			= MP_RESIDENTREGIST;
					pPacket->Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
					pPacket->dwObjectID			= pUserInfo->dwCharacterID;
 					pPacket->dwData1			= pUserInfo->DateMatchingInfo.nRequestChatState;
					pPacket->dwData2			= gCurTime;
					g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, (char*)pPacket, sizeof(*pPacket) );
					if (pTargetUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_NONE)
					{
						ASRV_InitDateMatchingChatState(pTargetUserInfo);
						// ..대상자?에게 채팅끝 알리기
						pPacket->Category			= MP_RESIDENTREGIST;
						pPacket->Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
						pPacket->dwObjectID			= pTargetUserInfo->dwCharacterID;
						pPacket->dwData1			= DATE_MATCHING_CHAT_RESULT_END_CHAT;
						pPacket->dwData2			= pUserInfo->dwCharacterID;					// 상대ID
						g_Network.Send2User( pTargetUserInfo->dwConnectionIndex, (char*)pPacket, sizeof(*pPacket) );
						// ..대상자의 맵서버에 신청상태 알리기
						pPacket->Category			= MP_RESIDENTREGIST;
						pPacket->Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
						pPacket->dwObjectID			= pTargetUserInfo->dwCharacterID;
 						pPacket->dwData1			= pTargetUserInfo->DateMatchingInfo.nRequestChatState;
						pPacket->dwData2			= gCurTime;
						g_Network.Send2Server( pTargetUserInfo->dwMapServerConnectionIndex, (char*)pPacket, sizeof(*pPacket) );
					}
				}
				break;
			default:
				g_Network.Send2Server( dwConnectionID, pMsg, dwLength );
			}	
		}
		break;
	// 다른 에이전트의 플레이어와의 채팅 결과
	case MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT_DIFF_AGENT:
		{
			MSG_DWORD2* pPacket = (MSG_DWORD2*)pMsg;

			// 플레이어가 주민등록을 했나?
			if (pUserInfo->DateMatchingInfo.bIsValid == FALSE)
			{
				return;
			}
			
			int nResult = pPacket->dwData1;
			switch(nResult)
			{
			// 신청자가 취소
			case DATE_MATCHING_CHAT_RESULT_CANCEL:
				{
					// 신청자인가?
					if (pUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_REQUEST)
					{
						return;
					}

					// 대상자에게 취소 알리기
					// ..대상자 에이전트에 알리기
					MSG_DWORD2 stPacket;
					stPacket.Category			= MP_RESIDENTREGIST;
					stPacket.Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT_DIFF_AGENT;
					stPacket.dwObjectID			= pPacket->dwData2;
					stPacket.dwData1			= DATE_MATCHING_CHAT_RESULT_CANCEL;
					stPacket.dwData2			= pUserInfo->DateMatchingInfo.nChatPlayerID;
					g_Network.Send2SpecificAgentServer((char*)&stPacket, sizeof(stPacket) );
					// ..신청자의 맵서버에 신청상태 알리기
					stPacket.Category			= MP_RESIDENTREGIST;
					stPacket.Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
					stPacket.dwObjectID			= pUserInfo->dwCharacterID;
 					stPacket.dwData1			= DATE_MATCHING_CHAT_REQUEST_STATE_NONE;
					g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, (char*)&stPacket, sizeof(stPacket) );

					ASRV_InitDateMatchingChatState(pUserInfo);
				}
				break;
			// 거절함
			case DATE_MATCHING_CHAT_RESULT_REFUSE:
				{
					// 대상자인가?
					if (pUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_RECV_REQUEST)
					{
						return;
					}

					// 신청자에게 거절 알리기
					// ..신청자 에이전트에 알리기
					MSG_DWORD2 stPacket;
					stPacket.Category			= MP_RESIDENTREGIST;
					stPacket.Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT_DIFF_AGENT;
					stPacket.dwObjectID			= pPacket->dwData2;
					stPacket.dwData1			= DATE_MATCHING_CHAT_RESULT_REFUSE;
					stPacket.dwData2			= pUserInfo->DateMatchingInfo.nChatPlayerID;
					g_Network.Send2SpecificAgentServer((char*)&stPacket, sizeof(stPacket) );

					ASRV_InitDateMatchingChatState(pUserInfo);
				}
				break;
			// 채팅시작
			case DATE_MATCHING_CHAT_RESULT_START_CHAT:
				{
					// 대상자인가?
					if (pUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_RECV_REQUEST)
					{
						return;
					}

					// 신청자에게 채팅시작 알리기
					// ..신청자 에이전트에 알리기
					PACKET_SERCH_DATE_MATCHING stPacket;
					stPacket.Category											= MP_RESIDENTREGIST;
					stPacket.Protocol											= MP_RESIDENTREGIST_DATEMATCHING_RESULT_START_CHAT_DIFF_AGENT;
					stPacket.bMaster											= TRUE;
					stPacket.dwObjectID											= pPacket->dwData2;		
					stPacket.nRequestPlayerID									= pUserInfo->DateMatchingInfo.nChatPlayerID;
					SafeStrCpy(stPacket.stSerchResult.stRegistInfo.szNickName, pUserInfo->DateMatchingInfo.szName, MAX_NAME_LEN+1);
					stPacket.stSerchResult.stRegistInfo.nRace					= (WORD)pUserInfo->DateMatchingInfo.nRace;
					stPacket.stSerchResult.stRegistInfo.nSex					= (WORD)pUserInfo->DateMatchingInfo.byGender;			
					stPacket.stSerchResult.stRegistInfo.nAge					= (WORD)pUserInfo->DateMatchingInfo.dwAge;				
					stPacket.stSerchResult.stRegistInfo.nLocation				= (WORD)pUserInfo->DateMatchingInfo.dwRegion;			
					stPacket.stSerchResult.stRegistInfo.nFavor					= (WORD)pUserInfo->DateMatchingInfo.dwGoodFeelingDegree;	
					stPacket.stSerchResult.stRegistInfo.nPropensityLike01		= (WORD)pUserInfo->DateMatchingInfo.pdwGoodFeeling[0];	
					stPacket.stSerchResult.stRegistInfo.nPropensityLike02		= (WORD)pUserInfo->DateMatchingInfo.pdwGoodFeeling[1];	
					stPacket.stSerchResult.stRegistInfo.nPropensityLike03		= (WORD)pUserInfo->DateMatchingInfo.pdwGoodFeeling[2];	
					stPacket.stSerchResult.stRegistInfo.nPropensityDisLike01	= (WORD)pUserInfo->DateMatchingInfo.pdwBadFeeling[0];	
					stPacket.stSerchResult.stRegistInfo.nPropensityDisLike02	= (WORD)pUserInfo->DateMatchingInfo.pdwBadFeeling[1];	
					stPacket.stSerchResult.nID									= pUserInfo->dwCharacterID;
					stPacket.stSerchResult.nAgentID								= g_pServerSystem->GetServerNum();
					SafeStrCpy(stPacket.stSerchResult.szIntroduction, pUserInfo->DateMatchingInfo.szIntroduce, MAX_INTRODUCE_LEN+1);
					g_Network.Send2SpecificAgentServer( (char*)&stPacket, sizeof(stPacket) );

					pUserInfo->DateMatchingInfo.nRequestChatState		= DATE_MATCHING_CHAT_REQUEST_STATE_CHATTING;
				}
				break;
			// 채팅끝
			case DATE_MATCHING_CHAT_RESULT_END_CHAT:
				{
					MSG_DWORD2 stPacket;
					// 신창자에게 알리기
					// ..신청자?에게 채팅끝 알리기
					stPacket.Category			= MP_RESIDENTREGIST;
					stPacket.Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
					stPacket.dwObjectID			= pUserInfo->dwCharacterID;
					stPacket.dwData1			= DATE_MATCHING_CHAT_RESULT_END_CHAT;
					stPacket.dwData2			= pUserInfo->DateMatchingInfo.nChatPlayerID;			// 상대ID
					g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
					// ..신청자의 맵서버에 신청상태 알리기
					stPacket.Category			= MP_RESIDENTREGIST;
					stPacket.Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
					stPacket.dwObjectID			= pUserInfo->dwCharacterID;
 					stPacket.dwData1			= DATE_MATCHING_CHAT_REQUEST_STATE_NONE;
					g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, (char*)&stPacket, sizeof(stPacket) );

					// 대상자에게 알리기
					// ..대상자의 에이전트에 알리기
					stPacket.Category			= MP_RESIDENTREGIST;
					stPacket.Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT_DIFF_AGENT;
					stPacket.dwObjectID			= pPacket->dwData2;
					stPacket.dwData1			= DATE_MATCHING_CHAT_RESULT_END_CHAT;
					stPacket.dwData2			= pUserInfo->DateMatchingInfo.nChatPlayerID;
					g_Network.Send2SpecificAgentServer((char*)&stPacket, sizeof(stPacket) );

					ASRV_InitDateMatchingChatState(pUserInfo);
				}
				break;
			default:
				g_Network.Send2Server( dwConnectionID, pMsg, dwLength );
			}	
		}
		break;

	// 채팅
	case MP_RESIDENTREGIST_DATEMATCHING_CHAT:
		{
			MSG_DWORD2 stPacket;
			// 요청자가 채팅중인 플레이어인가?
			if (pUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_CHATTING)
			{
				return;
			}

			// 대상 플레이어 찾기
			USERINFO* pTargetUserInfo = g_pUserTableForObjectID->FindUser(pUserInfo->DateMatchingInfo.nChatPlayerID);
			if (pTargetUserInfo == NULL)
			{
				// 맵이동 또는 접속끊김의 경우(또는 시스템 오류)
				// ..채팅을 강제로 끝낸다.
				ASRV_InitDateMatchingChatState(pUserInfo);
				// 클라이언트에 알리기
				// ..신청자?에게 채팅끝 알리기
				stPacket.Category			= MP_RESIDENTREGIST;
				stPacket.Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
				stPacket.dwObjectID			= pUserInfo->dwCharacterID;
				stPacket.dwData1			= DATE_MATCHING_CHAT_RESULT_END_CHAT;
				g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
				// ..신청자의 맵서버에 신청상태 알리기
				stPacket.Category			= MP_RESIDENTREGIST;
				stPacket.Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
				stPacket.dwObjectID			= pUserInfo->dwCharacterID;
 				stPacket.dwData1			= pUserInfo->DateMatchingInfo.nRequestChatState;
				g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
				return;
			}

			// 대상 플레이어의 채팅 상대가 나인가?
			if (pTargetUserInfo->DateMatchingInfo.nChatPlayerID != pUserInfo->dwCharacterID)
			{
				// ..채팅을 강제로 끝낸다.
				ASRV_InitDateMatchingChatState(pUserInfo);
				// 클라이언트에 알리기
				// ..신청자?에게 채팅끝 알리기
				stPacket.Category			= MP_RESIDENTREGIST;
				stPacket.Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
				stPacket.dwObjectID			= pUserInfo->dwCharacterID;
				stPacket.dwData1			= DATE_MATCHING_CHAT_RESULT_END_CHAT;
				g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
				// ..신청자의 맵서버에 신청상태 알리기
				stPacket.Category			= MP_RESIDENTREGIST;
				stPacket.Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
				stPacket.dwObjectID			= pUserInfo->dwCharacterID;
 				stPacket.dwData1			= pUserInfo->DateMatchingInfo.nRequestChatState;
				g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
				return;
			}

			g_Network.Send2User( pTargetUserInfo->dwConnectionIndex, pMsg, dwLength );
		}
		break;
	// 다른 에이전트 플레이어간의 채팅
	case MP_RESIDENTREGIST_DATEMATCHING_CHAT_DIFF_AGENT:
		{
			MSG_CHAT_WITH_SENDERID* pPacket = (MSG_CHAT_WITH_SENDERID*)pMsg;
			// 요청자가 채팅중인 플레이어인가?
			if (pUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_CHATTING)
			{
				return;
			}

			// 대상자에게 메세지 보내기
			// ..대상자 에이전트에 알리기
			pPacket->Category			= MP_RESIDENTREGIST;
			pPacket->Protocol			= MP_RESIDENTREGIST_DATEMATCHING_CHAT_DIFF_AGENT;
			pPacket->dwObjectID			= pPacket->dwSenderID;
			pPacket->dwSenderID			= pUserInfo->DateMatchingInfo.nChatPlayerID;
			g_Network.Send2SpecificAgentServer((char*)pPacket, sizeof(*pPacket) );
		}
		break;
	default:
		{
			g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, pMsg, dwLength );
		}
		break;
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_ParseRequestFromServer Method																	   서버 요청 분석/처리(에이전트)
//
VOID CSHResidentRegistManager::ASRV_ParseRequestFromServer(DWORD dwConnectionID, char* pMsg, DWORD dwLength)
{
#if defined(_AGENTSERVER)
	static const int	MAX_EACH_FIND_NUM = 5000;
	static USERINFO*	pUser[11][MAX_EACH_FIND_NUM];
	static int			nUserCnt[11];

	MSGBASE* pTempMsg = (MSGBASE*)pMsg;

	switch(pTempMsg->Protocol)
	{
	case MP_RESIDENTREGIST_REGIST_NOTIFY:
		{
			char buf[256] = {0,};

			USERINFO* pInfo = NULL;
			g_pUserTable->SetPositionUserHead();
			while( (pInfo = (USERINFO *)g_pUserTable->GetUserData()) != NULL )
			{	
				memcpy( buf, pMsg, dwLength );

				g_Network.Send2User( pInfo->dwConnectionIndex, buf, dwLength );	//CHATMSG 040324
			}
		}
		break;
	// 다른 에이전트로부터의 검색 결과
	case MP_RESIDENTREGIST_DATEMATCHING_SERCH_TO_OTHER_AGENT_RESULT:
		{
			PACKET_SERCH_DATE_MATCHING* pPacket = (PACKET_SERCH_DATE_MATCHING*)pMsg;

			USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser(pPacket->nRequestPlayerID);
			if (pUserInfo == NULL) return;

			pPacket->Category						= MP_RESIDENTREGIST;
			pPacket->Protocol						= MP_RESIDENTREGIST_DATEMATCHING_SERCH;
			pPacket->dwObjectID						= pPacket->nRequestPlayerID;
			// 원래 ID가 1이상일 수 있기에, +1000을 해 줘서 클라이언트에서 1000 이상일 때 다른 에이전트간의 채팅으로 간주한다.
			pPacket->stSerchResult.nAgentID			+= 1000;
			g_Network.Send2User( pUserInfo->dwConnectionIndex, pMsg, dwLength );
		}
		break;
	// 다른 에이전트로부터의 검색
	case MP_RESIDENTREGIST_DATEMATCHING_SERCH_TO_OTHER_AGENT:
		{
			PACKET_SERCH_DATE_MATCHING_TO_OTHER_AGENT* pPacket = (PACKET_SERCH_DATE_MATCHING_TO_OTHER_AGENT*)pMsg;
			PACKET_SERCH_DATE_MATCHING stPacket;

			DWORD nAgeMin	= pPacket->nAgeMin;
			DWORD nAgeMax	= pPacket->nAgeMax;
			DWORD nLocation = pPacket->nLocation;
			DWORD nSex		= pPacket->nSex;
			DWORD nGroupIndex = pPacket->nGroupIndex;

			ZeroMemory(pUser,		sizeof(pUser));
			ZeroMemory(nUserCnt,	sizeof(nUserCnt));

			// 검색!
			USERINFO* pSerchInfo;
			g_pUserTable->SetPositionUserHead();

			int nCnt = 0;
			int nFindUserIndex = 0;
			UINT nSerchCnt = 0;
			while( (pSerchInfo = g_pUserTable->GetUserData()) != NULL )
			{
				// 주민등록이 되지 않은 캐릭터는 건너뛴다.
				if (pSerchInfo->DateMatchingInfo.bIsValid == FALSE) continue;
				// 성별 체크
				if (pSerchInfo->DateMatchingInfo.byGender != nSex) continue;

				// 나이 체크
				if (pSerchInfo->DateMatchingInfo.dwAge < nAgeMin || pSerchInfo->DateMatchingInfo.dwAge > nAgeMax) continue;
				// 지역 체크. 0은 모든 지역
				if (nLocation != 0 && pSerchInfo->DateMatchingInfo.dwRegion != nLocation) continue;

				// 100114 ONS 캐릭터선택창으로 이동한 상대는 검색하지 않는다.
				if (pSerchInfo->wUserMapNum == 0) continue;

				pSerchInfo->DateMatchingInfo.nMatchingPoint = (BYTE)CalcMatchingPoint(&pSerchInfo->DateMatchingInfo, &pPacket->stRegistInfo);
				nFindUserIndex = pSerchInfo->DateMatchingInfo.nMatchingPoint/10;
				if (nFindUserIndex > 10) continue;
				if (nUserCnt[nFindUserIndex] >= MAX_EACH_FIND_NUM) continue;

				nSerchCnt++;

				pUser[nFindUserIndex][nUserCnt[nFindUserIndex]] = pSerchInfo; 
				nUserCnt[nFindUserIndex]++;
			}

			// 검색 그룹 인덱스가 최대 그룹 수를 넘어가면 리턴
			if (nGroupIndex >= (nSerchCnt+MAX_SERCH_NUM_PER_AGENT-1) / MAX_SERCH_NUM_PER_AGENT)
			{
				return;
			}

			UINT nGroupCnt = 0;
			for(int i=10; i>=0; i--)
			{
				for(int j=0; j<nUserCnt[i]; j++)
				{
					nGroupCnt++;
					if (nGroupCnt < nGroupIndex*MAX_SERCH_NUM_PER_AGENT+1)
					{
						continue;
					}

					pSerchInfo = pUser[i][j];
					// 클라이언트에 알리기
					stPacket.Category						= MP_RESIDENTREGIST;
					stPacket.Protocol						= MP_RESIDENTREGIST_DATEMATCHING_SERCH_TO_OTHER_AGENT_RESULT;
					// ..요청한 에이전트에서 dwObjectID에 자신의 ID를 넣고 BroadCast 했다.
					// ..요청한 에이전트에 결과를 보내주기 위해 그 ID 로 Send2SpecificAgentServer 한다.
					stPacket.dwObjectID						= pPacket->dwObjectID;

					SafeStrCpy(stPacket.stSerchResult.stRegistInfo.szNickName, pSerchInfo->DateMatchingInfo.szName, MAX_NAME_LENGTH+1);
					stPacket.stSerchResult.stRegistInfo.nRace					= (WORD)pSerchInfo->DateMatchingInfo.nRace;
					stPacket.stSerchResult.stRegistInfo.nSex					= (WORD)pSerchInfo->DateMatchingInfo.byGender;			
					stPacket.stSerchResult.stRegistInfo.nAge					= (WORD)pSerchInfo->DateMatchingInfo.dwAge;
					stPacket.stSerchResult.stRegistInfo.nLocation				= (WORD)pSerchInfo->DateMatchingInfo.dwRegion;			
					stPacket.stSerchResult.stRegistInfo.nFavor					= (WORD)pSerchInfo->DateMatchingInfo.dwGoodFeelingDegree;	
					stPacket.stSerchResult.stRegistInfo.nPropensityLike01		= (WORD)pSerchInfo->DateMatchingInfo.pdwGoodFeeling[0];	
					stPacket.stSerchResult.stRegistInfo.nPropensityLike02		= (WORD)pSerchInfo->DateMatchingInfo.pdwGoodFeeling[1];	
					stPacket.stSerchResult.stRegistInfo.nPropensityLike03		= (WORD)pSerchInfo->DateMatchingInfo.pdwGoodFeeling[2];	
					stPacket.stSerchResult.stRegistInfo.nPropensityDisLike01	= (WORD)pSerchInfo->DateMatchingInfo.pdwBadFeeling[0];	
					stPacket.stSerchResult.stRegistInfo.nPropensityDisLike02	= (WORD)pSerchInfo->DateMatchingInfo.pdwBadFeeling[1];	
					stPacket.stSerchResult.nMatchingPoint						= pSerchInfo->DateMatchingInfo.nMatchingPoint;
					stPacket.stSerchResult.nID									= pSerchInfo->dwCharacterID;
					// ..플레이어가 속한 에이전트 ID 설정
					stPacket.stSerchResult.nAgentID								= g_pServerSystem->GetServerNum();
					// 100113 ONS 직업정보 추가
					stPacket.stSerchResult.nClass								= pSerchInfo->DateMatchingInfo.dwClass;
					SafeStrCpy(stPacket.stSerchResult.szIntroduction, pSerchInfo->DateMatchingInfo.szIntroduce, MAX_INTRODUCE_LEN+1);
					stPacket.nGroupIndex										= nGroupIndex;
					stPacket.nGroupMax											= (nSerchCnt+MAX_SERCH_NUM_PER_AGENT-1) / MAX_SERCH_NUM_PER_AGENT;
					stPacket.nRequestPlayerID									= pPacket->nRequestPlayerID;
					g_Network.Send2SpecificAgentServer((char*)&stPacket, sizeof(stPacket) );
					// 최대 MAX_SERCH_NUM_PER_AGENT명 까지만 처리하도록
					nCnt++;
					if (nCnt >= MAX_SERCH_NUM_PER_AGENT)
					{
						return;
					}
				}
			}
		}
		return;
	// 다른 에이전트로부터의 채팅 신청
	case MP_RESIDENTREGIST_DATEMATCHING_REQUEST_CHAT_DIFF_AGENT_TARGET_CHECK:
		{
			MSG_DWORD4* pPacket = (MSG_DWORD4*)pMsg;

			// 대상 플레이어 찾기
			USERINFO* pTargetUserInfo = g_pUserTableForObjectID->FindUser(pPacket->dwData1);
			if (pTargetUserInfo == NULL)
			{
				// 현재 접속되어 있지 않다.
				// ..신청자 에이전트에 알리기
				pPacket->Category			= MP_RESIDENTREGIST;
				pPacket->Protocol			= MP_RESIDENTREGIST_DATEMATCHING_REQUEST_CHAT_DIFF_AGENT_TARGET_CHECK_RESULT;
				pPacket->dwObjectID			= pPacket->dwData2;		// 요청 에이전트의 ID
				pPacket->dwData1			= pPacket->dwData3;		// 요청 플레이어의 ID
				pPacket->dwData2			= DATE_MATCHING_CHAT_RESULT_LOGOFF;
				g_Network.Send2SpecificAgentServer( (char*)pPacket, sizeof(*pPacket) );
				return;
			}

			// 대상 플레이어가 주민등록을 했나?
			if (pTargetUserInfo->DateMatchingInfo.bIsValid == FALSE)
			{
				return;
			}

			// 100629 ONS 데이트매칭 거부옵션 설정 상태 체크
			if( pTargetUserInfo->GameOption.bNoDateMatching == TRUE )
			{
				pPacket->Category			= MP_RESIDENTREGIST;
				pPacket->Protocol			= MP_RESIDENTREGIST_DATEMATCHING_REQUEST_CHAT_DIFF_AGENT_TARGET_CHECK_RESULT;
				pPacket->dwObjectID			= pPacket->dwData2;		// 요청 에이전트의 ID
				pPacket->dwData1			= pPacket->dwData3;		// 요청 플레이어의 ID
				pPacket->dwData2			= DATE_MATCHING_CHAT_RESULT_REJECTION;
				g_Network.Send2SpecificAgentServer( (char*)pPacket, sizeof(*pPacket) );
				return;
			}

			// 대상 플레이어가 대화 가능한가?
			if (pTargetUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_NONE)
			{
				// 채팅 중이거나 채팅 신청(받음) 중이다.
				// ..신청자 에이전트에 알리기
				pPacket->Category			= MP_RESIDENTREGIST;
				pPacket->Protocol			= MP_RESIDENTREGIST_DATEMATCHING_REQUEST_CHAT_DIFF_AGENT_TARGET_CHECK_RESULT;
				pPacket->dwObjectID			= pPacket->dwData2;		// 요청 에이전트의 ID
				pPacket->dwData1			= pPacket->dwData3;		// 요청 플레이어의 ID
				pPacket->dwData2			= DATE_MATCHING_CHAT_RESULT_BUSY;
				g_Network.Send2SpecificAgentServer( (char*)pPacket, sizeof(*pPacket) );
				return;
			}

			// 100107 ONS 대상자가 챌린지존에 있는지 여부를 검사한다.
			if(g_csDateManager.IsChallengeZone( pTargetUserInfo->wUserMapNum ))
			{
				// 신청 대상자가 챌린지존에 있다면 대화신청을 막는다.
				pPacket->Category			= MP_RESIDENTREGIST;
				pPacket->Protocol			= MP_RESIDENTREGIST_DATEMATCHING_REQUEST_CHAT_DIFF_AGENT_TARGET_CHECK_RESULT;
				pPacket->dwObjectID			= pPacket->dwData2;
				pPacket->dwData1			= pPacket->dwData3;		// 요청 플레이어의 ID
				pPacket->dwData2			= DATE_MATCHING_CHAT_RESULT_START_CHALLENGEZONE;
				g_Network.Send2SpecificAgentServer( (char*)pPacket, sizeof(*pPacket) );			
				return;
			}

			// 신청자에게 신청 했음을 알리기
			// ..신청자 에이전트에 알리기
			MSG_DWORD4 stPacket;
			stPacket.Category			= MP_RESIDENTREGIST;
			stPacket.Protocol			= MP_RESIDENTREGIST_DATEMATCHING_REQUEST_CHAT_DIFF_AGENT_TARGET_CHECK_RESULT;
			stPacket.dwObjectID			= pPacket->dwData2;			// 요청 에이전트의 ID
			stPacket.dwData1			= pPacket->dwData3;			// 요청 플레이어의 ID
			stPacket.dwData2			= DATE_MATCHING_CHAT_RESULT_REQUEST_AND_WAIT;
			stPacket.dwData3			= g_pServerSystem->GetServerNum();
			stPacket.dwData4			= pTargetUserInfo->dwCharacterID;
			g_Network.Send2SpecificAgentServer( (char*)&stPacket, sizeof(stPacket) );

			// 대상자 정보 설정
			pTargetUserInfo->DateMatchingInfo.nRequestChatState		= DATE_MATCHING_CHAT_REQUEST_STATE_RECV_REQUEST;
			pTargetUserInfo->DateMatchingInfo.nChatPlayerID			= pPacket->dwData3;
			pTargetUserInfo->DateMatchingInfo.nChatPlayerAgentID	= pPacket->dwData2+1000;
		}
		return;
	// 다른 에이전트에 있는 플레이어에게 대화 신청 후 결과 받기
	case MP_RESIDENTREGIST_DATEMATCHING_REQUEST_CHAT_DIFF_AGENT_TARGET_CHECK_RESULT:
		{
			MSG_DWORD4* pPacket = (MSG_DWORD4*)pMsg;

			// 신청자 찾기
			USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser(pPacket->dwData1);
			if (pUserInfo == NULL) return;

			// 플레이어가 주민등록을 했나?
			if (pUserInfo->DateMatchingInfo.bIsValid == FALSE)
			{
				return;
			}
			// 플레이어가 대화 가능한가?
			// ..해킹 또는 오류의 경우
			if (pUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_NONE)
			{
				return;
			}

			// 클라이언트에 결과 알리기
			MSG_DWORD2 stPacket;
			stPacket.Category			= MP_RESIDENTREGIST;
			stPacket.Protocol			= MP_RESIDENTREGIST_DATEMATCHING_REQUEST_CHAT;
			stPacket.dwObjectID			= pUserInfo->dwCharacterID;
			stPacket.dwData1			= pPacket->dwData2;
			g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );

			// 신청되었다면 정보 설정 및 대상자에게 신청자의 정보 보내기
			// ..대상자 에이전트에 알리기
			if (pPacket->dwData2 == DATE_MATCHING_CHAT_RESULT_REQUEST_AND_WAIT)
			{
				pUserInfo->DateMatchingInfo.nRequestChatState		= DATE_MATCHING_CHAT_REQUEST_STATE_REQUEST;
				pUserInfo->DateMatchingInfo.nChatPlayerID			= pPacket->dwData4;
				pUserInfo->DateMatchingInfo.nChatPlayerAgentID		= pPacket->dwData3+1000;

				PACKET_SERCH_DATE_MATCHING stPacket;

				stPacket.Category											= MP_RESIDENTREGIST;
				stPacket.Protocol											= MP_RESIDENTREGIST_DATEMATCHING_REQUEST_CHAT_DIFF_AGENT_SEND_REQUESTER_INFO;
				stPacket.dwObjectID											= pPacket->dwData3;				// 대상자 에이전트의 ID
				stPacket.nRequestPlayerID									= pPacket->dwData4;				// 대상자 플레이어 ID
				SafeStrCpy(stPacket.stSerchResult.stRegistInfo.szNickName, pUserInfo->DateMatchingInfo.szName, MAX_NAME_LEN+1);
				stPacket.stSerchResult.stRegistInfo.nRace					= (WORD)pUserInfo->DateMatchingInfo.nRace;
				stPacket.stSerchResult.stRegistInfo.nSex					= (WORD)pUserInfo->DateMatchingInfo.byGender;			
				stPacket.stSerchResult.stRegistInfo.nAge					= (WORD)pUserInfo->DateMatchingInfo.dwAge;				
				stPacket.stSerchResult.stRegistInfo.nLocation				= (WORD)pUserInfo->DateMatchingInfo.dwRegion;			
				stPacket.stSerchResult.stRegistInfo.nFavor					= (WORD)pUserInfo->DateMatchingInfo.dwGoodFeelingDegree;	
				stPacket.stSerchResult.stRegistInfo.nPropensityLike01		= (WORD)pUserInfo->DateMatchingInfo.pdwGoodFeeling[0];	
				stPacket.stSerchResult.stRegistInfo.nPropensityLike02		= (WORD)pUserInfo->DateMatchingInfo.pdwGoodFeeling[1];	
				stPacket.stSerchResult.stRegistInfo.nPropensityLike03		= (WORD)pUserInfo->DateMatchingInfo.pdwGoodFeeling[2];	
				stPacket.stSerchResult.stRegistInfo.nPropensityDisLike01	= (WORD)pUserInfo->DateMatchingInfo.pdwBadFeeling[0];	
				stPacket.stSerchResult.stRegistInfo.nPropensityDisLike02	= (WORD)pUserInfo->DateMatchingInfo.pdwBadFeeling[1];	
				stPacket.stSerchResult.nID									= pUserInfo->dwCharacterID;
				stPacket.stSerchResult.nAgentID								= g_pServerSystem->GetServerNum();

				SafeStrCpy(stPacket.stSerchResult.szIntroduction, pUserInfo->DateMatchingInfo.szIntroduce, MAX_INTRODUCE_LEN+1);
				g_Network.Send2SpecificAgentServer( (char*)&stPacket, sizeof(stPacket) );
			}

			// 맵서버에 알리기
			// ..신청자의 맵서버에 신청상태 알리기
			stPacket.Category			= MP_RESIDENTREGIST;
			stPacket.Protocol			= MP_RESIDENTREGIST_DATEMATCHING_REQUEST_CHAT;
			stPacket.dwObjectID			= pUserInfo->dwCharacterID;
 			stPacket.dwData1			= pUserInfo->DateMatchingInfo.nRequestChatState;
			g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
		}
		return;
	// 다른 에이전트에 있는 플레이어에게 대화 신청 받은 후 신청자 정보 받기
	case MP_RESIDENTREGIST_DATEMATCHING_REQUEST_CHAT_DIFF_AGENT_SEND_REQUESTER_INFO:
		{
			PACKET_SERCH_DATE_MATCHING* pPacket = (PACKET_SERCH_DATE_MATCHING*)pMsg;

			// 대상자 찾기
			USERINFO* pTargetUserInfo = g_pUserTableForObjectID->FindUser(pPacket->nRequestPlayerID);
			if (pTargetUserInfo == NULL) return;

			// 플레이어가 주민등록을 했나?
			if (pTargetUserInfo->DateMatchingInfo.bIsValid == FALSE)
			{
				return;
			}

			// 신청을 받은 상태인가?
			if (pTargetUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_RECV_REQUEST)
			{
				return;
			}

			// 대상자에게 누군가가 채팅 신청을 했음을 알리기
			MSG_DWORD_NAME stPacket;
 			stPacket.Category			= MP_RESIDENTREGIST;
			stPacket.Protocol			= MP_RESIDENTREGIST_DATEMATCHING_REQUEST_CHAT;
			stPacket.dwObjectID			= pTargetUserInfo->dwCharacterID;
			stPacket.dwData				= DATE_MATCHING_CHAT_RESULT_REQUESTED;
			SafeStrCpy(stPacket.Name, pPacket->stSerchResult.stRegistInfo.szNickName, MAX_NAME_LENGTH+1);
			g_Network.Send2User( pTargetUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );

			// 신청자 에이전트에서 받은 정보에 플레이어ID와 매칭포인트를 수정하고 클라이언트에 전송
			pPacket->Protocol						= MP_RESIDENTREGIST_DATEMATCHING_SOMEBODY_INFO;
			pPacket->dwObjectID						= pTargetUserInfo->dwCharacterID;
			pPacket->stSerchResult.nMatchingPoint	= (WORD)CalcMatchingPoint(&pTargetUserInfo->DateMatchingInfo, &pPacket->stSerchResult.stRegistInfo);
			pPacket->stSerchResult.nAgentID			+= 1000;
			g_Network.Send2User( pTargetUserInfo->dwConnectionIndex, (char*)pPacket, sizeof(*pPacket) );
		}
		return;
	// 다른 에이전트에 있는 플레이어에게 대화 신청 후 최종 결과
	case MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT_DIFF_AGENT:
		{
			MSG_DWORD2* pPacket = (MSG_DWORD2*)pMsg;

			// 대상자 찾기
			USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser(pPacket->dwData2);
			if (pUserInfo == NULL) return;

			// 플레이어가 주민등록을 했나?
			if (pUserInfo->DateMatchingInfo.bIsValid == FALSE)
			{
				return;
			}
			
			int nResult = pPacket->dwData1;
			switch(nResult)
			{
			// 신청자가 취소
			case DATE_MATCHING_CHAT_RESULT_CANCEL:
				{
					// 대상자인가?
					if (pUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_RECV_REQUEST)
					{
						return;
					}

					// 대상자에게 취소 알리기
					MSG_DWORD2 stPacket;
					stPacket.Category			= MP_RESIDENTREGIST;
					stPacket.Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
					stPacket.dwObjectID			= pPacket->dwData2;
					stPacket.dwData1			= DATE_MATCHING_CHAT_RESULT_CANCEL;
					stPacket.dwData2			= pUserInfo->DateMatchingInfo.nChatPlayerID;
					g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );

					ASRV_InitDateMatchingChatState(pUserInfo);
				}
				break;
			// 거절함
			case DATE_MATCHING_CHAT_RESULT_REFUSE:
				{
					// 신청자인가?
					if (pUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_REQUEST)
					{
						return;
					}

					// 신청자에게 거절 알리기
					MSG_DWORD2 stPacket;
					stPacket.Category			= MP_RESIDENTREGIST;
					stPacket.Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
					stPacket.dwObjectID			= pPacket->dwData2;
					stPacket.dwData1			= DATE_MATCHING_CHAT_RESULT_REFUSE;
					stPacket.dwData2			= pUserInfo->DateMatchingInfo.nChatPlayerID;
					g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );

					ASRV_InitDateMatchingChatState(pUserInfo);
				}
				break;
			// 채팅 끝
			case DATE_MATCHING_CHAT_RESULT_END_CHAT:
				{
					MSG_DWORD2 stPacket;
					// 대상자에게 알리기
					// ..대상자에게 채팅끝 알리기
					stPacket.Category			= MP_RESIDENTREGIST;
					stPacket.Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
					stPacket.dwObjectID			= pUserInfo->dwCharacterID;
					stPacket.dwData1			= DATE_MATCHING_CHAT_RESULT_END_CHAT;
					stPacket.dwData2			= pUserInfo->DateMatchingInfo.nChatPlayerID;			// 상대ID
					g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
					// ..대상자의 맵서버에 신청상태 알리기
					stPacket.Category			= MP_RESIDENTREGIST;
					stPacket.Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
					stPacket.dwObjectID			= pUserInfo->dwCharacterID;
 					stPacket.dwData1			= DATE_MATCHING_CHAT_REQUEST_STATE_NONE;
					g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, (char*)&stPacket, sizeof(stPacket) );

					ASRV_InitDateMatchingChatState(pUserInfo);
				}
				break;
			// 시간초과
			case DATE_MATCHING_CHAT_RESULT_TIMEOUT:
				{
					ASRV_InitDateMatchingChatState(pUserInfo);
					// 대상자에게 알리기
					// ..대상자에게 시간초과 되었음을 알리기
					pPacket->Category			= MP_RESIDENTREGIST;
					pPacket->Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
					pPacket->dwObjectID			= pUserInfo->dwCharacterID;
					pPacket->dwData1			= DATE_MATCHING_CHAT_RESULT_TIMEOUT;
					g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)pPacket, sizeof(*pPacket) );
				}
				break;
			}
		}
		return;
	// 채팅시작
	case MP_RESIDENTREGIST_DATEMATCHING_RESULT_START_CHAT_DIFF_AGENT:
		{
			PACKET_SERCH_DATE_MATCHING* pPacket = (PACKET_SERCH_DATE_MATCHING*)pMsg;

			// 신청자 찾기
			USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser(pPacket->nRequestPlayerID);
			if (pUserInfo == NULL) return;

			// 플레이어가 주민등록을 했나?
			if (pUserInfo->DateMatchingInfo.bIsValid == FALSE)
			{
				return;
			}

			if (pUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_CHATTING)
			{
				// 대상자에게 채팅시작 알리기
				// ..대상자 에이전트에 알리기
				PACKET_SERCH_DATE_MATCHING stPacket;
				stPacket = *pPacket;
				SafeStrCpy(stPacket.stSerchResult.stRegistInfo.szNickName, pUserInfo->DateMatchingInfo.szName, MAX_NAME_LEN+1);
				stPacket.stSerchResult.stRegistInfo.nRace					= (WORD)pUserInfo->DateMatchingInfo.nRace;
				stPacket.stSerchResult.stRegistInfo.nSex					= (WORD)pUserInfo->DateMatchingInfo.byGender;			
				stPacket.stSerchResult.stRegistInfo.nAge					= (WORD)pUserInfo->DateMatchingInfo.dwAge;				
				stPacket.stSerchResult.stRegistInfo.nLocation				= (WORD)pUserInfo->DateMatchingInfo.dwRegion;			
				stPacket.stSerchResult.stRegistInfo.nFavor					= (WORD)pUserInfo->DateMatchingInfo.dwGoodFeelingDegree;	
				stPacket.stSerchResult.stRegistInfo.nPropensityLike01		= (WORD)pUserInfo->DateMatchingInfo.pdwGoodFeeling[0];	
				stPacket.stSerchResult.stRegistInfo.nPropensityLike02		= (WORD)pUserInfo->DateMatchingInfo.pdwGoodFeeling[1];	
				stPacket.stSerchResult.stRegistInfo.nPropensityLike03		= (WORD)pUserInfo->DateMatchingInfo.pdwGoodFeeling[2];	
				stPacket.stSerchResult.stRegistInfo.nPropensityDisLike01	= (WORD)pUserInfo->DateMatchingInfo.pdwBadFeeling[0];	
				stPacket.stSerchResult.stRegistInfo.nPropensityDisLike02	= (WORD)pUserInfo->DateMatchingInfo.pdwBadFeeling[1];	
				SafeStrCpy(stPacket.stSerchResult.szIntroduction, pUserInfo->DateMatchingInfo.szIntroduce, MAX_INTRODUCE_LEN+1);

				stPacket.bMaster											= FALSE;
				stPacket.dwObjectID											= pPacket->stSerchResult.nAgentID;		
				stPacket.nRequestPlayerID									= pPacket->stSerchResult.nID;
				stPacket.stSerchResult.nID									= pUserInfo->dwCharacterID;
				stPacket.stSerchResult.nAgentID								= g_pServerSystem->GetServerNum();
				g_Network.Send2SpecificAgentServer( (char*)&stPacket, sizeof(stPacket) );
			}

			// 신청자에게 채팅시작 알리기
			pPacket->Category						= MP_RESIDENTREGIST;
			pPacket->Protocol						= MP_RESIDENTREGIST_DATEMATCHING_RESULT_START_CHAT;
			pPacket->dwObjectID						= pUserInfo->dwCharacterID;
			pPacket->stSerchResult.nMatchingPoint	= (WORD)CalcMatchingPoint(&pUserInfo->DateMatchingInfo, &pPacket->stSerchResult.stRegistInfo);
			pPacket->stSerchResult.nAgentID			+= 1000;
			g_Network.Send2User( pUserInfo->dwConnectionIndex, pMsg, dwLength );
			// 신청자의 맵서버에 신청상태 알리기
			MSG_DWORD2 stPacket;
			stPacket.Category						= MP_RESIDENTREGIST;
			stPacket.Protocol						= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
			stPacket.dwObjectID						= pUserInfo->dwCharacterID;
 			stPacket.dwData1						= DATE_MATCHING_CHAT_REQUEST_STATE_CHATTING;
			g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, (char*)&stPacket, sizeof(stPacket) );

			pUserInfo->DateMatchingInfo.nRequestChatState		= DATE_MATCHING_CHAT_REQUEST_STATE_CHATTING;
		}
		return;
	// 다른 에이전트 플레이어간의 채팅
	case MP_RESIDENTREGIST_DATEMATCHING_CHAT_DIFF_AGENT:
		{
			MSG_CHAT_WITH_SENDERID* pPacket = (MSG_CHAT_WITH_SENDERID*)pMsg;

			// 대상자 찾기
			USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser(pPacket->dwSenderID);
			if (pUserInfo == NULL) return;

			// 대상자가 채팅중인 플레이어인가?
			if (pUserInfo->DateMatchingInfo.nRequestChatState != DATE_MATCHING_CHAT_REQUEST_STATE_CHATTING)
			{
				return;
			}

			// 대상자에게 메세지 보내기
			pPacket->Protocol = MP_RESIDENTREGIST_DATEMATCHING_CHAT;
			g_Network.Send2User( pUserInfo->dwConnectionIndex, pMsg, dwLength );
		}
		return;
	// 100113 ONS 주민등록 정보가 변경되었을 경우 적용( 예) 전직으로 인한 클래스정보 변경시 )
	case MP_RESIDENTREGIST_REGIST_CHANGE:
		{
			MSG_DWORD* pPacket = (MSG_DWORD*)pMsg;
			DWORD dwClass = pPacket->dwData;

			if( dwClass > 0 )
			{
				USERINFO* pSerchInfo = NULL;
				g_pUserTable->SetPositionUserHead();
				while( (pSerchInfo = g_pUserTable->GetUserData()) != NULL )
				{
					if( pSerchInfo->dwCharacterID != pPacket->dwObjectID )
						continue;

					pSerchInfo->DateMatchingInfo.dwClass = dwClass;
				}
			}
		}
		return;
	}

	USERINFO* pUserInfo = g_pUserTableForObjectID->FindUser(pTempMsg->dwObjectID);
	if (pUserInfo == NULL) return;

	switch( pTempMsg->Protocol )
	{
	case MP_RESIDENTREGIST_REGIST:
		{
			PACKET_RESIDENTREGIST_REGIST* pPacket = (PACKET_RESIDENTREGIST_REGIST*)pMsg;

			SafeStrCpy(pUserInfo->DateMatchingInfo.szName, pPacket->stInfo.szNickName, MAX_NAME_LENGTH+1);
			pUserInfo->DateMatchingInfo.bIsValid			= TRUE;
			pUserInfo->DateMatchingInfo.nRace				= (DWORD)pPacket->stInfo.nRace;						// 090504 ShinJS --- 종족 설정
			pUserInfo->DateMatchingInfo.byGender			= (BYTE)pPacket->stInfo.nSex;
			pUserInfo->DateMatchingInfo.dwAge				= pPacket->stInfo.nAge;
			pUserInfo->DateMatchingInfo.dwRegion			= pPacket->stInfo.nLocation;
			pUserInfo->DateMatchingInfo.dwGoodFeelingDegree	= pPacket->stInfo.nFavor;
			pUserInfo->DateMatchingInfo.pdwGoodFeeling[0]	= pPacket->stInfo.nPropensityLike01;
			pUserInfo->DateMatchingInfo.pdwGoodFeeling[1]	= pPacket->stInfo.nPropensityLike02;
			pUserInfo->DateMatchingInfo.pdwGoodFeeling[2]	= pPacket->stInfo.nPropensityLike03;
			pUserInfo->DateMatchingInfo.pdwBadFeeling[0]	= pPacket->stInfo.nPropensityDisLike01;
			pUserInfo->DateMatchingInfo.pdwBadFeeling[1]	= pPacket->stInfo.nPropensityDisLike02;
			// 100113 ONS 직업정보 추가
			pUserInfo->DateMatchingInfo.dwClass				= pPacket->stInfo.nClass;

			g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)pPacket, sizeof(*pPacket) );
		}
		break;
	case MP_RESIDENTREGIST_UPDATE_INTRODUCTION:
		{
			PACKET_RESIDENTREGIST_UPDATE_INTRODUCTION* pPacket = (PACKET_RESIDENTREGIST_UPDATE_INTRODUCTION*)pMsg;

			SafeStrCpy(pUserInfo->DateMatchingInfo.szIntroduce, pPacket->szIntroduction, MAX_INTRODUCE_LEN+1);
			
			if(FILTERTABLE->IsCharInString(pUserInfo->DateMatchingInfo.szIntroduce, "'"))
			{
				MSG_DWORD msg;
				msg.Category = MP_RESIDENTREGIST;
				msg.Protocol = MP_RESIDENTREGIST_UPDATE_INTRODUCTION_NACK;
				msg.dwObjectID = pPacket->dwObjectID;
				g_Network.Send2User(pUserInfo->dwConnectionIndex, (char*)&msg, sizeof(msg));
				return;
			}

			g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)pPacket, sizeof(*pPacket) );
		}
		break;
	case MP_RESIDENTREGIST_DATEMATCHING_SERCH:
		{
			MSG_DWORD5* pPacket = (MSG_DWORD5*)pMsg;   
			PACKET_SERCH_DATE_MATCHING stPacket;

			DWORD nAgeMin	= pPacket->dwData1;
			DWORD nAgeMax	= pPacket->dwData2;
			DWORD nLocation = pPacket->dwData3;
			DWORD nSex		= pPacket->dwData4;
			DWORD nGroupIndex = pPacket->dwData5;

			ZeroMemory(pUser,		sizeof(pUser));
			ZeroMemory(nUserCnt,	sizeof(nUserCnt));

			// 검색!
			USERINFO* pSerchInfo;
			g_pUserTable->SetPositionUserHead();

			int nCnt = 0;
			int nFindUserIndex = 0;
			UINT nSerchCnt = 0;
			while( (pSerchInfo = g_pUserTable->GetUserData()) != NULL )
			{
				// 주민등록이 되지 않은 캐릭터는 건너뛴다.
				if (pSerchInfo->DateMatchingInfo.bIsValid == FALSE) continue;
				// 자신도 건너뛴다.
				if (pSerchInfo->dwCharacterID == pUserInfo->dwCharacterID) continue;
				// 성별 체크
				if (pSerchInfo->DateMatchingInfo.byGender != nSex) continue;

				// 나이 체크
				if (pSerchInfo->DateMatchingInfo.dwAge < nAgeMin || pSerchInfo->DateMatchingInfo.dwAge > nAgeMax) continue;
				// 지역 체크. 0은 모든 지역
				if (nLocation != 0 && pSerchInfo->DateMatchingInfo.dwRegion != nLocation) continue;

				// 100114 ONS 캐릭터선택창으로 이동한 상대는 검색하지 않는다.
				if (pSerchInfo->wUserMapNum == 0) continue;

				pSerchInfo->DateMatchingInfo.nMatchingPoint = (BYTE)CalcMatchingPoint(&pSerchInfo->DateMatchingInfo, &pUserInfo->DateMatchingInfo);
				nFindUserIndex = pSerchInfo->DateMatchingInfo.nMatchingPoint/10;
				if (nFindUserIndex > 10) continue;
				if (nUserCnt[nFindUserIndex] >= MAX_EACH_FIND_NUM) continue;

				nSerchCnt++;

				pUser[nFindUserIndex][nUserCnt[nFindUserIndex]] = pSerchInfo; 
				nUserCnt[nFindUserIndex]++;
			}

			// 검색 그룹 인덱스가 최대 그룹 수를 넘어가면 리턴
			if (nGroupIndex >= (nSerchCnt+MAX_SERCH_NUM_PER_AGENT-1) / MAX_SERCH_NUM_PER_AGENT)
			{
				goto STOP_SERCH;
			}

			UINT nGroupCnt = 0;
			for(int i=10; i>=0; i--)
			{
				for(int j=0; j<nUserCnt[i]; j++)
				{
					nGroupCnt++;
					if (nGroupCnt < nGroupIndex*MAX_SERCH_NUM_PER_AGENT+1)
					{
						continue;
					}

					pSerchInfo = pUser[i][j];
					// 클라이언트에 알리기
					stPacket.Category						= MP_RESIDENTREGIST;
					stPacket.Protocol						= MP_RESIDENTREGIST_DATEMATCHING_SERCH;
					stPacket.dwObjectID						= pTempMsg->dwObjectID;

					SafeStrCpy(stPacket.stSerchResult.stRegistInfo.szNickName, pSerchInfo->DateMatchingInfo.szName, MAX_NAME_LENGTH+1);
					stPacket.stSerchResult.stRegistInfo.nRace					= (WORD)pSerchInfo->DateMatchingInfo.nRace;
					stPacket.stSerchResult.stRegistInfo.nSex					= (WORD)pSerchInfo->DateMatchingInfo.byGender;			
					stPacket.stSerchResult.stRegistInfo.nAge					= (WORD)pSerchInfo->DateMatchingInfo.dwAge;
					stPacket.stSerchResult.stRegistInfo.nLocation				= (WORD)pSerchInfo->DateMatchingInfo.dwRegion;			
					stPacket.stSerchResult.stRegistInfo.nFavor					= (WORD)pSerchInfo->DateMatchingInfo.dwGoodFeelingDegree;	
					stPacket.stSerchResult.stRegistInfo.nPropensityLike01		= (WORD)pSerchInfo->DateMatchingInfo.pdwGoodFeeling[0];	
					stPacket.stSerchResult.stRegistInfo.nPropensityLike02		= (WORD)pSerchInfo->DateMatchingInfo.pdwGoodFeeling[1];	
					stPacket.stSerchResult.stRegistInfo.nPropensityLike03		= (WORD)pSerchInfo->DateMatchingInfo.pdwGoodFeeling[2];	
					stPacket.stSerchResult.stRegistInfo.nPropensityDisLike01	= (WORD)pSerchInfo->DateMatchingInfo.pdwBadFeeling[0];	
					stPacket.stSerchResult.stRegistInfo.nPropensityDisLike02	= (WORD)pSerchInfo->DateMatchingInfo.pdwBadFeeling[1];	
					stPacket.stSerchResult.nMatchingPoint						= pSerchInfo->DateMatchingInfo.nMatchingPoint;
					stPacket.stSerchResult.nID									= pSerchInfo->dwCharacterID;
					stPacket.stSerchResult.nAgentID								= 0;
					SafeStrCpy(stPacket.stSerchResult.szIntroduction, pSerchInfo->DateMatchingInfo.szIntroduce, MAX_INTRODUCE_LEN+1);
					stPacket.nGroupIndex										= nGroupIndex;
					stPacket.nGroupMax											= (nSerchCnt+MAX_SERCH_NUM_PER_AGENT-1) / MAX_SERCH_NUM_PER_AGENT;
					stPacket.nRequestPlayerID									= 0;
					// 100113 ONS 직업정보 추가
					stPacket.stSerchResult.nClass								= pSerchInfo->DateMatchingInfo.dwClass;
					g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
					// 최대 MAX_SERCH_NUM_PER_AGENT명 까지만 처리하도록
					nCnt++;
					if (nCnt >= MAX_SERCH_NUM_PER_AGENT) goto STOP_SERCH;
				}
			}
STOP_SERCH:
			// 다른 에이전트에 알리기
			{
				PACKET_SERCH_DATE_MATCHING_TO_OTHER_AGENT stPacket;

				stPacket.Category						= MP_RESIDENTREGIST;
				stPacket.Protocol						= MP_RESIDENTREGIST_DATEMATCHING_SERCH_TO_OTHER_AGENT;
				stPacket.dwObjectID						= g_pServerSystem->GetServerNum();
				stPacket.nAgeMin						= nAgeMin;
				stPacket.nAgeMax						= nAgeMax;
				stPacket.nLocation						= nLocation;
				stPacket.nSex							= nSex;
				stPacket.nGroupIndex					= nGroupIndex;
				stPacket.nRequestPlayerID				= pUserInfo->dwCharacterID;
				SafeStrCpy(stPacket.stRegistInfo.szNickName, pUserInfo->DateMatchingInfo.szName, MAX_NAME_LENGTH+1);
				stPacket.stRegistInfo.nRace					= (WORD)pUserInfo->DateMatchingInfo.nRace;
				stPacket.stRegistInfo.nSex					= (WORD)pUserInfo->DateMatchingInfo.byGender;			
				stPacket.stRegistInfo.nAge					= (WORD)pUserInfo->DateMatchingInfo.dwAge;				
				stPacket.stRegistInfo.nLocation				= (WORD)pUserInfo->DateMatchingInfo.dwRegion;			
				stPacket.stRegistInfo.nFavor				= (WORD)pUserInfo->DateMatchingInfo.dwGoodFeelingDegree;	
				stPacket.stRegistInfo.nPropensityLike01		= (WORD)pUserInfo->DateMatchingInfo.pdwGoodFeeling[0];	
				stPacket.stRegistInfo.nPropensityLike02		= (WORD)pUserInfo->DateMatchingInfo.pdwGoodFeeling[1];	
				stPacket.stRegistInfo.nPropensityLike03		= (WORD)pUserInfo->DateMatchingInfo.pdwGoodFeeling[2];	
				stPacket.stRegistInfo.nPropensityDisLike01	= (WORD)pUserInfo->DateMatchingInfo.pdwBadFeeling[0];	
				stPacket.stRegistInfo.nPropensityDisLike02	= (WORD)pUserInfo->DateMatchingInfo.pdwBadFeeling[1];	
				g_Network.Broadcast2AgentServerExceptSelf((char*)&stPacket, sizeof(stPacket));
			}
		}
		break;
	case MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT:
		{
 			MSG_DWORD2* pPacket = (MSG_DWORD2*)pMsg;

			// 플레이어가 주민등록을 했나?
			if (pUserInfo->DateMatchingInfo.bIsValid == FALSE)
			{
				return;
			}

			// 대상 플레이어가 다른 에이전트에 있나?
			if (pUserInfo->DateMatchingInfo.nChatPlayerAgentID)
			{
				int nResult = pPacket->dwData1;
				switch(nResult)
				{
				// 시간초과
				case DATE_MATCHING_CHAT_RESULT_TIMEOUT:
					{
						// 클라이언트에 알리기
						// ..신청자에게 시간초과 되었음을 알리기
						pPacket->Category			= MP_RESIDENTREGIST;
						pPacket->Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
						pPacket->dwObjectID			= pUserInfo->dwCharacterID;
						pPacket->dwData1			= DATE_MATCHING_CHAT_RESULT_TIMEOUT;
						g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)pPacket, sizeof(*pPacket) );
						// 대상자에게 시간초과 되었음을 알리기
						// ..대상자 에이전트에 알리기
						MSG_DWORD2 stPacket;
						stPacket.Category			= MP_RESIDENTREGIST;
						stPacket.Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT_DIFF_AGENT;
						stPacket.dwObjectID			= pUserInfo->DateMatchingInfo.nChatPlayerAgentID-1000;
						stPacket.dwData1			= DATE_MATCHING_CHAT_RESULT_TIMEOUT;
						stPacket.dwData2			= pUserInfo->DateMatchingInfo.nChatPlayerID;			// 상대ID
						g_Network.Send2SpecificAgentServer((char*)&stPacket, sizeof(stPacket) );

						ASRV_InitDateMatchingChatState(pUserInfo);
					}
					break;
				}

				return;
			}

			// 대상 플레이어 찾기
			USERINFO* pTargetUserInfo = g_pUserTableForObjectID->FindUser(pUserInfo->DateMatchingInfo.nChatPlayerID);
			if (pTargetUserInfo == NULL)
			{
				ASRV_InitDateMatchingChatState(pUserInfo);
				// ..신청자의 맵서버에 신청상태 알리기
				pPacket->Category			= MP_RESIDENTREGIST;
				pPacket->Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
				pPacket->dwObjectID			= pUserInfo->dwCharacterID;
 				pPacket->dwData1			= pUserInfo->DateMatchingInfo.nRequestChatState;
				pPacket->dwData2			= gCurTime;
				g_Network.Send2Server( pUserInfo->dwMapServerConnectionIndex, (char*)pPacket, sizeof(*pPacket) );
				return;
			}

			// 대상 플레이어가 주민등록을 했나?
			if (pTargetUserInfo->DateMatchingInfo.bIsValid == FALSE)
			{
				return;
			}
			
			int nResult = pPacket->dwData1;
			switch(nResult)
			{
			// 시간초과
			case DATE_MATCHING_CHAT_RESULT_TIMEOUT:
				{
					ASRV_InitDateMatchingChatState(pUserInfo);
					ASRV_InitDateMatchingChatState(pTargetUserInfo);
					// 클라이언트에 알리기
					// ..신청자에게 시간초과 되었음을 알리기
					pPacket->Category			= MP_RESIDENTREGIST;
					pPacket->Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
					pPacket->dwObjectID			= pUserInfo->dwCharacterID;
					pPacket->dwData1			= DATE_MATCHING_CHAT_RESULT_TIMEOUT;
					g_Network.Send2User( pUserInfo->dwConnectionIndex, (char*)pPacket, sizeof(*pPacket) );
					// ..대상자에게 시간초과 되었음을 알리기
					pPacket->Category			= MP_RESIDENTREGIST;
					pPacket->Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
					pPacket->dwObjectID			= pTargetUserInfo->dwCharacterID;
					pPacket->dwData1			= DATE_MATCHING_CHAT_RESULT_TIMEOUT;
					g_Network.Send2User( pTargetUserInfo->dwConnectionIndex, (char*)pPacket, sizeof(*pPacket) );
				}
				break;
			}
		}
		break;
	default:
		g_Network.Send2User( dwConnectionID, (char*)pMsg, dwLength );
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_EndDateMatchingChat Method																   로그아웃 등등의 상황에 따른 채팅 종료
//
VOID CSHResidentRegistManager::ASRV_EndDateMatchingChat(USERINFO* pUserInfo)
{
#if defined(_AGENTSERVER)
	if (pUserInfo->DateMatchingInfo.nChatPlayerID)
	{
		// 다른 에이전트에 있나?
		if (pUserInfo->DateMatchingInfo.nChatPlayerAgentID)
		{
			MSG_DWORD2 stPacket;
			stPacket.Category			= MP_RESIDENTREGIST;
			stPacket.Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT_DIFF_AGENT;
			stPacket.dwObjectID			= pUserInfo->DateMatchingInfo.nChatPlayerAgentID-1000;
			stPacket.dwData1			= DATE_MATCHING_CHAT_RESULT_END_CHAT;
			stPacket.dwData2			= pUserInfo->DateMatchingInfo.nChatPlayerID;			// 상대ID
			g_Network.Send2SpecificAgentServer((char*)&stPacket, sizeof(stPacket) );
		}
		else
		{
			USERINFO* pTargetUserInfo = g_pUserTableForObjectID->FindUser(pUserInfo->DateMatchingInfo.nChatPlayerID);
			if (pTargetUserInfo)
			{
				ASRV_InitDateMatchingChatState(pTargetUserInfo);

				MSG_DWORD2 stPacket;
				stPacket.Category			= MP_RESIDENTREGIST;
				stPacket.Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
				stPacket.dwObjectID			= pTargetUserInfo->dwCharacterID;
				stPacket.dwData1			= DATE_MATCHING_CHAT_RESULT_END_CHAT;
				stPacket.dwData2			= NULL;										// 상대ID
				g_Network.Send2User( pTargetUserInfo->dwConnectionIndex, (char*)&stPacket, sizeof(stPacket) );
			}
		}
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  ASRV_InitDateMatchingChatState Method																	데이트 매칭 채팅 상태 초기화
//
VOID CSHResidentRegistManager::ASRV_InitDateMatchingChatState(USERINFO* pUserInfo)
{
#if defined(_AGENTSERVER)
	pUserInfo->DateMatchingInfo.nRequestChatState			= DATE_MATCHING_CHAT_REQUEST_STATE_NONE;
	pUserInfo->DateMatchingInfo.nChatPlayerID				= NULL;
	pUserInfo->DateMatchingInfo.nChatPlayerAgentID			= NULL;
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_ParseRequestFromClient Method																			클라이언트 요청 분석/처리
//
VOID CSHResidentRegistManager::SRV_ParseRequestFromClient(DWORD dwConnectionID, char* pMsg, DWORD dwLength)
{
#if defined(_MAPSERVER_)
	MSGBASE* pPacket = (MSGBASE*)pMsg;

	// 잘못된 플레이어 처리
	CPlayer* pPlayer = (CPlayer*)g_pUserTable->FindUser( pPacket->dwObjectID );
	if( !pPlayer )	
	{
		return;
	}

	switch(pPacket->Protocol)
	{
	// 등록
	case MP_RESIDENTREGIST_REGIST:
		{
			PACKET_RESIDENTREGIST_REGIST* pPacket = (PACKET_RESIDENTREGIST_REGIST*)pMsg;
			SRV_RegistResident(pPlayer, &pPacket->stInfo);

			// TODO : 주민등록시 전체공지
		}
		break;
	// 자기소개 업데이트
	case MP_RESIDENTREGIST_UPDATE_INTRODUCTION:
		{
			PACKET_RESIDENTREGIST_UPDATE_INTRODUCTION* pPacket = (PACKET_RESIDENTREGIST_UPDATE_INTRODUCTION*)pMsg;
			SRV_UpdateIntroduction(pPlayer, pPacket->szIntroduction);
		}
		break;
	// 데이트매칭 검색
	case MP_RESIDENTREGIST_DATEMATCHING_SERCH:
		{
			MSG_WORD4* pPacket = (MSG_WORD4*)pMsg;
			SRV_SerchDateMatching(pPlayer, pPacket->wData1, pPacket->wData2, pPacket->wData3, pPacket->wData4);
		}
		break;
	// 데이트매칭 채팅 신청
	case MP_RESIDENTREGIST_DATEMATCHING_REQUEST_CHAT:
		{
			MSG_DWORD2* pPacket = (MSG_DWORD2*)pMsg;
			SRV_RequestDateMatchingChat(pPlayer, pPacket);
		}
		break;
	// 데이트매칭 채팅 신청 결과
	case MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT:
		{
			MSG_DWORD2* pPacket = (MSG_DWORD2*)pMsg;
			SRV_ResultRequestDateMatchingChat(pPlayer, pPacket);
		}
		break;
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_RegistResident Method																							   주민등록 처리
//
VOID CSHResidentRegistManager::SRV_RegistResident(CPlayer* pPlayer, stRegistInfo* pInfo)
{
#if defined(_MAPSERVER_)
	CSHMain::RESULT eResult = CSHMain::RESULT_OK;
	PACKET_RESIDENTREGIST_REGIST stPacket;
	MSG_DWORD_NAME msg;

	// 이미 발급 받았나?
	if (pPlayer->GetResidentRegistInfo()->bIsValid)
	{
		eResult = CSHMain::RESULT_FAIL_INVALID;
		goto FAILED;
	}

	// 나이 범위 체크
	if (pInfo->nAge == 0 || pInfo->nAge > 100)
	{
		eResult = CSHMain::RESULT_FAIL_INVALID;
		goto FAILED;
	}

	// 성향 범위 체크
	if (pInfo->nPropensityLike01 == 0 ||
		pInfo->nPropensityLike02 == 0 ||
		pInfo->nPropensityLike03 == 0 ||
		pInfo->nPropensityDisLike01 == 0 ||
		pInfo->nPropensityDisLike02 == 0 ||
		pInfo->nPropensityLike01 > m_stAddInfo.m_nMaxPropensity ||
		pInfo->nPropensityLike02 > m_stAddInfo.m_nMaxPropensity ||
		pInfo->nPropensityLike03 > m_stAddInfo.m_nMaxPropensity ||
		pInfo->nPropensityDisLike01 > m_stAddInfo.m_nMaxPropensity ||
		pInfo->nPropensityDisLike02 > m_stAddInfo.m_nMaxPropensity)
	{
		eResult = CSHMain::RESULT_FAIL_INVALID;
		goto FAILED;
	}

	// 접속지역 범위 체크
	if (pInfo->nLocation == 0 || pInfo->nLocation >= m_stAddInfo.m_nMaxLocation)
	{
		eResult = CSHMain::RESULT_FAIL_INVALID;
		goto FAILED;
	}

	// 초기화가 필요한 것들..
	pInfo->nFavor = 0;
	pInfo->nSex = (WORD)pPlayer->GetGender();
	SafeStrCpy(pInfo->szNickName, pPlayer->GetObjectName(), MAX_NAME_LENGTH+1);
	// 090504 ShinJS --- 종족 설정
	pInfo->nRace = pPlayer->GetRacial();

	// 100113 ONS 주민등록정보중 클래스정보가 변경되었을 경우 에이전트로 전달한다.
	CHARACTER_TOTALINFO TotalInfo;
	ZeroMemory(&TotalInfo, sizeof(TotalInfo));
	pPlayer->GetCharacterTotalInfo(&TotalInfo);
	WORD idx = 1;
	if( TotalInfo.JobGrade > 1 )
	{
		idx = TotalInfo.Job[TotalInfo.JobGrade - 1];
	}
	pInfo->nClass = ( TotalInfo.Job[0] * 1000 ) + ( ( TotalInfo.Race + 1 ) * 100 ) + ( TotalInfo.JobGrade * 10 ) + idx;
	

	// 맵서버의 플레이어 데이터 설정
	DATE_MATCHING_INFO stInfo;
	ZeroMemory(&stInfo, sizeof(stInfo));
	stInfo.bIsValid				= TRUE;
	SafeStrCpy(stInfo.szName, pPlayer->GetObjectName(), MAX_NAME_LENGTH+1);
	stInfo.nRace				= pInfo->nRace;
	stInfo.byGender				= (BYTE)pInfo->nSex;
	stInfo.dwAge				= pInfo->nAge;
	stInfo.dwRegion				= pInfo->nLocation;
	stInfo.dwGoodFeelingDegree	= pInfo->nFavor;
	stInfo.pdwGoodFeeling[0]	= pInfo->nPropensityLike01;
	stInfo.pdwGoodFeeling[1]	= pInfo->nPropensityLike02;
	stInfo.pdwGoodFeeling[2]	= pInfo->nPropensityLike03;
	stInfo.pdwBadFeeling[0]		= pInfo->nPropensityDisLike01;
	stInfo.pdwBadFeeling[1]		= pInfo->nPropensityDisLike02;
	// 100113 ONS 직업정보 추가
	stInfo.dwClass				= pInfo->nClass;
	pPlayer->SetResidentRegistInfo(&stInfo);

	ResidentRegist_SaveInfo(pPlayer->GetID(), pInfo);

	// 클라이언트에 알리기
	// ..클라이언트에 바로 가지 않고 Agent에서 처리할 것을 처리하고 간다 (자신의 경우)
	// ..하트알리미 때문에 그리드내의 다른 캐릭터에게도 알려주기 위해 PACKEDDATA_OBJ->QuickSendExceptObjectSelf 한다.
	stPacket.Category			= MP_RESIDENTREGIST;
	stPacket.Protocol			= MP_RESIDENTREGIST_REGIST;
	stPacket.dwObjectID			= pPlayer->GetID();
	stPacket.stInfo				= *pInfo;

	pPlayer->SendMsg(&stPacket, sizeof(stPacket));
	PACKEDDATA_OBJ->QuickSendExceptObjectSelf(pPlayer, &stPacket, sizeof(stPacket));


	msg.Category = MP_RESIDENTREGIST;
	msg.Protocol = MP_RESIDENTREGIST_REGIST_NOTIFY;
	strcpy( msg.Name, pPlayer->GetObjectName() );

	//GiftItemInsert( pPlayer->GetID(), pPlayer->GetUserID(), 0, 21000282, 5, 1 );
	//GiftItemInsert( pPlayer->GetID(), pPlayer->GetUserID(), 0, 21000285, 10, 1 );
	
	ITEMMGR->ObtainMonstermeterItem( pPlayer, 21000282, 5 );
	ITEMMGR->ObtainMonstermeterItem( pPlayer, 21000285, 10 );

	g_Network.Broadcast2AgentServer( (char*)&msg, sizeof(msg) );
FAILED:
	;
#endif
}

VOID CSHResidentRegistManager::SRV_UpdateIntroduction(CPlayer* pPlayer, char* szIntroduction)
{
#if defined(_MAPSERVER_)
	ResidentRegist_SaveInfo(pPlayer->GetID(), szIntroduction, 1);

	// 클라이언트에 알리기
	// ..클라이언트에 바로 가지 않고 Agent에서 처리할 것을 처리하고 간다
	PACKET_RESIDENTREGIST_UPDATE_INTRODUCTION stPacket;
	stPacket.Category			= MP_RESIDENTREGIST;
	stPacket.Protocol			= MP_RESIDENTREGIST_UPDATE_INTRODUCTION;
	stPacket.dwObjectID			= pPlayer->GetID();
	
	SafeStrCpy(stPacket.szIntroduction, szIntroduction, sizeof(stPacket.szIntroduction));

	pPlayer->SendMsg(&stPacket, sizeof(stPacket));
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_SerchDateMatching Method																						데이트 매칭 검색
//
VOID CSHResidentRegistManager::SRV_SerchDateMatching(CPlayer* pPlayer, WORD nAgeRange, WORD nLocation, WORD nSex, WORD nGroupIndex)
{
#if defined(_MAPSERVER_)
	CSHMain::RESULT eResult = CSHMain::RESULT_OK;
	MSG_DWORD5 stPacket;

	// 나이 범위 체크
	// ..0은 모든 나이
	if (nAgeRange >= m_stAddInfo.m_nMaxSerchAgeRange)
	{
		eResult = CSHMain::RESULT_FAIL_INVALID;
		goto FAILED;
	}

	// 지역 범위 체크
	if (nLocation >= m_stAddInfo.m_nMaxLocation)
	{
		eResult = CSHMain::RESULT_FAIL_INVALID;
		goto FAILED;
	}

	// 검색 허용 시간 체크
	if (gCurTime - pPlayer->GetResidentRegistInfo()->nSerchTimeTick < DATE_MATCHING_RESERCH_TIME)
	{
		eResult = CSHMain::RESULT_FAIL_01;
		goto FAILED;
	}
	pPlayer->SetDateMatchingSerchTimeTick(gCurTime);

	// 클라이언트에 알리기
	// ..클라이언트에 바로 가지 않고 Agent에서 검색 기능을 처리하고 간다.
	stPacket.Category			= MP_RESIDENTREGIST;
	stPacket.Protocol			= MP_RESIDENTREGIST_DATEMATCHING_SERCH;
	stPacket.dwObjectID			= pPlayer->GetID();
	stPacket.dwData1			= m_stAddInfo.m_pTblSerchAgeRange[nAgeRange].nMin;
	stPacket.dwData2			= m_stAddInfo.m_pTblSerchAgeRange[nAgeRange].nMax;
	stPacket.dwData3			= m_stAddInfo.m_pTblLocation[nLocation].nID;
	stPacket.dwData4			= nSex;
	stPacket.dwData5			= nGroupIndex;

	pPlayer->SendMsg(&stPacket, sizeof(stPacket));
FAILED:
	;
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_RequestDateMatchingChat Method																		  데이트 매칭 채팅 신청 처리
//
VOID CSHResidentRegistManager::SRV_RequestDateMatchingChat(CPlayer* pPlayer, MSG_DWORD2* pPacket)
{
#if defined(_MAPSERVER_)
	pPlayer->SetDateMatchingChatState(pPacket->dwData1);
	pPlayer->SetDateMatchingChatTimeTick(gCurTime);
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_ResultRequestDateMatchingChat Method																  데이트 매칭 채팅 결과 처리
//
VOID CSHResidentRegistManager::SRV_ResultRequestDateMatchingChat(CPlayer* pPlayer, MSG_DWORD2* pPacket)
{
#if defined(_MAPSERVER_)
	pPlayer->SetDateMatchingChatState(pPacket->dwData1);
	pPlayer->SetDateMatchingChatTimeTick(gCurTime);
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  SRV_ProcDateMatchingChatTimeOut Method																  데이트 매칭 채팅 시간 초과 처리
//
VOID CSHResidentRegistManager::SRV_ProcDateMatchingChatTimeOut(CPlayer* pPlayer)
{
#if defined(_MAPSERVER_)
	// 채팅 신청 중이면 시간 초과 처리를 한다.
	if (pPlayer->GetResidentRegistInfo()->nRequestChatState == DATE_MATCHING_CHAT_REQUEST_STATE_REQUEST)
	{
		if (gCurTime - pPlayer->GetResidentRegistInfo()->nRequestChatTimeTick > SHMath_SECOND(10))
		{
			pPlayer->SetDateMatchingChatState(DATE_MATCHING_CHAT_REQUEST_STATE_NONE);
			pPlayer->SetDateMatchingChatTimeTick(gCurTime);

			MSG_DWORD2 stPacket;

			stPacket.Category			= MP_RESIDENTREGIST;
			stPacket.Protocol			= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
			stPacket.dwObjectID			= pPlayer->GetID();
			stPacket.dwData1			= DATE_MATCHING_CHAT_RESULT_TIMEOUT;
			pPlayer->SendMsg(&stPacket, sizeof(stPacket));
		}
	}
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CalcMatchingPoint Method																							매칭 포인트 계산 
//
UINT CSHResidentRegistManager::CalcMatchingPoint(DATE_MATCHING_INFO* pstInfo, stRegistInfo* pstTargetInfo)
{
	int nMatchingPoint = 100;
	int nMinusPoint = 0;

	// 차이나는 나이만큼 -, 최대 -값은 30
	nMinusPoint += abs((int)pstInfo->dwAge - (int)pstTargetInfo->nAge);
	if (nMinusPoint > 30) nMinusPoint = 30;

	// 접속 지역이 다르면 -10
	if (pstInfo->dwRegion != pstTargetInfo->nLocation)
	{
		nMinusPoint += 10;
	}

	// 차이나는 좋아하는 성향 개수만큼 -10
	int nMaxPro = sizeof(pstInfo->pdwGoodFeeling)/sizeof(pstInfo->pdwGoodFeeling[0]);
	int nTmp[10];
	nTmp[0] = pstTargetInfo->nPropensityLike01;
	nTmp[1] = pstTargetInfo->nPropensityLike02;
	nTmp[2] = pstTargetInfo->nPropensityLike03;
	for(int i=0; i<nMaxPro; i++)
	{
		for(int j=0; j<nMaxPro; j++)
		{
			if (pstInfo->pdwGoodFeeling[i] == (DWORD)nTmp[j])
			{
				goto NEXT1;
			}
		}
		nMinusPoint += 10;
	NEXT1:;
	}

	// 차이나는 싫어하는 성향 개수만큼 -10
	nMaxPro = sizeof(pstInfo->pdwBadFeeling)/sizeof(pstInfo->pdwBadFeeling[0]);
	nTmp[0] = pstTargetInfo->nPropensityDisLike01;
	nTmp[1] = pstTargetInfo->nPropensityDisLike02;
	for(int i=0; i<nMaxPro; i++)
	{
		for(int j=0; j<nMaxPro; j++)
		{
			if (pstInfo->pdwBadFeeling[i] == (DWORD)nTmp[j])
			{
				goto NEXT2;
			}
		}
		nMinusPoint += 10;
	NEXT2:;
	}

	nMatchingPoint -= nMinusPoint;
	if (nMatchingPoint < 0)	nMatchingPoint = 0;

	return nMatchingPoint;
}
UINT CSHResidentRegistManager::CalcMatchingPoint(DATE_MATCHING_INFO* pstInfo, DATE_MATCHING_INFO* pstTargetInfo)
{
	int nMatchingPoint = 100;
	int nMinusPoint = 0;

	// 차이나는 나이만큼 -, 최대 -값은 30
	nMinusPoint += abs((int)pstInfo->dwAge - (int)pstTargetInfo->dwAge);
	if (nMinusPoint > 30) nMinusPoint = 30;

	// 접속 지역이 다르면 -10
	if (pstInfo->dwRegion != pstTargetInfo->dwRegion)
	{
		nMinusPoint += 10;
	}

	// 차이나는 좋아하는 성향 개수만큼 -10
	int nMaxPro = sizeof(pstInfo->pdwGoodFeeling)/sizeof(pstInfo->pdwGoodFeeling[0]);
	for(int i=0; i<nMaxPro; i++)
	{
		for(int j=0; j<nMaxPro; j++)
		{
			if (pstInfo->pdwGoodFeeling[i] == pstTargetInfo->pdwGoodFeeling[j])
			{
				goto NEXT1;
			}
		}
		nMinusPoint += 10;
	NEXT1:;
	}

	// 차이나는 싫어하는 성향 개수만큼 -10
	nMaxPro = sizeof(pstInfo->pdwBadFeeling)/sizeof(pstInfo->pdwBadFeeling[0]);
	for(int i=0; i<nMaxPro; i++)
	{
		for(int j=0; j<nMaxPro; j++)
		{
			if (pstInfo->pdwBadFeeling[i] == pstTargetInfo->pdwBadFeeling[j])
			{
				goto NEXT2;
			}
		}
		nMinusPoint += 10;
	NEXT2:;
	}

	nMatchingPoint -= nMinusPoint;
	if (nMatchingPoint < 0)	nMatchingPoint = 0;

	return nMatchingPoint;
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_RequestRegist Method																						서버에 주민등록 요청
//
VOID CSHResidentRegistManager::CLI_RequestRegist(stRegistInfo* pInfo)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	PACKET_RESIDENTREGIST_REGIST stPacket;
	stPacket.Category 		= MP_RESIDENTREGIST;
	stPacket.Protocol 		= MP_RESIDENTREGIST_REGIST;
	stPacket.dwObjectID		= HEROID;
	stPacket.stInfo			= *pInfo;

	NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_RequestRegist Method																			   서버에 자기소개 업데이트 요청
//
VOID CSHResidentRegistManager::CLI_RequestUpdateIntroduction(char* szIntroduction)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	PACKET_RESIDENTREGIST_UPDATE_INTRODUCTION stPacket;
	stPacket.Category 		= MP_RESIDENTREGIST;
	stPacket.Protocol 		= MP_RESIDENTREGIST_UPDATE_INTRODUCTION;
	stPacket.dwObjectID		= HEROID;

	SafeStrCpy(stPacket.szIntroduction, szIntroduction, MAX_INTRODUCE_LEN+1);
	// ' 는 문자열을 나타내는 표시이므로 문자열로 사용할 수 없다. " 로 대체하도록 함.
	for(int i=0; i<MAX_INTRODUCE_LEN+1; i++)
	{
		if (stPacket.szIntroduction[i] == '\'')
		{
			stPacket.szIntroduction[i] = '"';
		}
	}

	NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_RequestSerchDateMatching Method																		서버에 데이트 매칭 검색 요청
//
VOID CSHResidentRegistManager::CLI_RequestSerchDateMatching(WORD nAgeRange, WORD nLocation, WORD nSex, int nGroupIndexDelta)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
  	if (GAMEIN->GetDateMatchingDlg()->GetPartnerListDlg()->GetSerchGroupIndex() + nGroupIndexDelta < 0) return;
	// 0번 인덱스의 그룹을 검색한다면 초기화..
	if (nGroupIndexDelta == 0)
	{
		GAMEIN->GetDateMatchingDlg()->GetPartnerListDlg()->SetSerchGroupIndex(0);
		GAMEIN->GetDateMatchingDlg()->GetPartnerListDlg()->SetSerchGroupNum(0);
		GAMEIN->GetDateMatchingDlg()->GetPartnerListDlg()->UpdateGroupInfo();
	}

	m_nSerchResultNum		= 0;

	MSG_WORD4 stPacket;
	stPacket.Category 	= MP_RESIDENTREGIST;
	stPacket.Protocol 	= MP_RESIDENTREGIST_DATEMATCHING_SERCH;
	stPacket.dwObjectID	= gHeroID;
	stPacket.wData1		= nAgeRange;
	stPacket.wData2		= nLocation;
	stPacket.wData3		= nSex;
	stPacket.wData4		= WORD(GAMEIN->GetDateMatchingDlg()->GetPartnerListDlg()->GetSerchGroupIndex() + nGroupIndexDelta);

	NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_RequestRecommendOtherChar Method																서버에 다른 캐릭터 추천하기 요청
//
VOID CSHResidentRegistManager::CLI_RequestRecommendOtherChar(DWORD nTargetPlayerID)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	MSG_DWORD stPacket;
	stPacket.Category 		= MP_RESIDENTREGIST;
	stPacket.Protocol 		= MP_RESIDENTREGIST_DATEMATCHING_RECOMMEND_OTHER_CHAR;
	stPacket.dwObjectID		= HEROID;
	stPacket.dwData			= nTargetPlayerID;

	NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
#endif
}


// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_RequestDateMatchingChat Method																   서버에 데이트 매칭 채팅 신청 요청
//
VOID CSHResidentRegistManager::CLI_RequestDateMatchingChat(stSerchResult* pSerchTarget)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	m_stChatPartner.nAgentID = pSerchTarget->nAgentID;

	MSG_DWORD2 stPacket;
	stPacket.Category 		= MP_RESIDENTREGIST;
	stPacket.Protocol 		= MP_RESIDENTREGIST_DATEMATCHING_REQUEST_CHAT;
	stPacket.dwObjectID		= HEROID;
	stPacket.dwData1		= pSerchTarget->nID;
	stPacket.dwData2		= m_stChatPartner.nAgentID;

	// 다른 에이전트의 플레이어라면 프로토콜 변경
	// ..다른 에이전트 플레이어간의 채팅일 경우 검색 결과에서 nAgentID에 +1000을 한다. 
	// ..유저에게 전송할 때만 +1000을 하고 내부적으로는 nAgentID값을 그대로 사용하기 때문에 -1000 해서 보내야 한다.
	if (m_stChatPartner.nAgentID >= 1000)
	{
		stPacket.Protocol 	= MP_RESIDENTREGIST_DATEMATCHING_REQUEST_CHAT_DIFF_AGENT;
		stPacket.dwData2	= m_stChatPartner.nAgentID - 1000;
	}

	NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_CancelDateMatchingChat Method																   서버에 데이트 매칭 채팅 신청 취소
//
VOID CSHResidentRegistManager::CLI_CancelDateMatchingChat()
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	MSG_DWORD2 stPacket;
	stPacket.Category 		= MP_RESIDENTREGIST;
	stPacket.Protocol 		= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
	stPacket.dwObjectID		= HEROID;
	stPacket.dwData1		= DATE_MATCHING_CHAT_RESULT_CANCEL;
	stPacket.dwData2		= m_stChatPartner.nAgentID;
	// 다른 에이전트의 플레이어라면 프로토콜 변경
	// ..다른 에이전트 플레이어간의 채팅일 경우 검색 결과에서 nAgentID에 +1000을 한다. 
	// ..유저에게 전송할 때만 +1000을 하고 내부적으로는 nAgentID값을 그대로 사용하기 때문에 -1000 해서 보내야 한다.
	if (m_stChatPartner.nAgentID >= 1000)
	{
		stPacket.Protocol 	= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT_DIFF_AGENT;
		stPacket.dwData2	= m_stChatPartner.nAgentID - 1000;
	}

	NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_RefuseDateMatchingChat Method																   서버에 데이트 매칭 채팅 신청 거절
//
VOID CSHResidentRegistManager::CLI_RefuseDateMatchingChat()
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	MSG_DWORD2 stPacket;
	stPacket.Category 		= MP_RESIDENTREGIST;
	stPacket.Protocol 		= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
	stPacket.dwObjectID		= HEROID;
	stPacket.dwData1		= DATE_MATCHING_CHAT_RESULT_REFUSE;
	stPacket.dwData2		= m_stChatPartner.nAgentID;
	// 다른 에이전트의 플레이어라면 프로토콜 변경
	// ..다른 에이전트 플레이어간의 채팅일 경우 검색 결과에서 nAgentID에 +1000을 한다. 
	// ..유저에게 전송할 때만 +1000을 하고 내부적으로는 nAgentID값을 그대로 사용하기 때문에 -1000 해서 보내야 한다.
	if (m_stChatPartner.nAgentID >= 1000)
	{
		stPacket.Protocol 	= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT_DIFF_AGENT;
		stPacket.dwData2	= m_stChatPartner.nAgentID - 1000;
	}

	NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_StartDateMatchingChat Method																   서버에 데이트 매칭 채팅 신청 승락
//
VOID CSHResidentRegistManager::CLI_StartDateMatchingChat()
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	MSG_DWORD2 stPacket;
	stPacket.Category 		= MP_RESIDENTREGIST;
	stPacket.Protocol 		= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
	stPacket.dwObjectID		= HEROID;
	stPacket.dwData1		= DATE_MATCHING_CHAT_RESULT_START_CHAT;
	stPacket.dwData2		= m_stChatPartner.nAgentID;
	// 다른 에이전트의 플레이어라면 프로토콜 변경
	// ..다른 에이전트 플레이어간의 채팅일 경우 검색 결과에서 nAgentID에 +1000을 한다. 
	// ..유저에게 전송할 때만 +1000을 하고 내부적으로는 nAgentID값을 그대로 사용하기 때문에 -1000 해서 보내야 한다.
	if (m_stChatPartner.nAgentID >= 1000)
	{
		stPacket.Protocol 	= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT_DIFF_AGENT;
		stPacket.dwData2	= m_stChatPartner.nAgentID - 1000;
	}

	NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_StartDateMatchingChat Method																     서버에 데이트 매칭 채팅 끝 요청
//
VOID CSHResidentRegistManager::CLI_EndDateMatchingChat()
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	MSG_DWORD2 stPacket;
	stPacket.Category 		= MP_RESIDENTREGIST;
	stPacket.Protocol 		= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT;
	stPacket.dwObjectID		= HEROID;
	stPacket.dwData1		= DATE_MATCHING_CHAT_RESULT_END_CHAT;
	stPacket.dwData2		= m_stChatPartner.nAgentID;
	// 다른 에이전트의 플레이어라면 프로토콜 변경
	// ..다른 에이전트 플레이어간의 채팅일 경우 검색 결과에서 nAgentID에 +1000을 한다. 
	// ..유저에게 전송할 때만 +1000을 하고 내부적으로는 nAgentID값을 그대로 사용하기 때문에 -1000 해서 보내야 한다.
	if (m_stChatPartner.nAgentID >= 1000)
	{
		stPacket.Protocol 	= MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT_DIFF_AGENT;
		stPacket.dwData2	= m_stChatPartner.nAgentID - 1000;
	}

	NETWORK->Send( (MSGROOT*)&stPacket, sizeof(stPacket) );
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_SendDateMatchingChatText Method																 서버에 데이트 매칭 채팅 내용 보내기
//
VOID CSHResidentRegistManager::CLI_SendDateMatchingChatText(char* szText)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	MSG_CHAT_WITH_SENDERID stPacket;
	stPacket.Category 		= MP_RESIDENTREGIST;
	stPacket.Protocol 		= MP_RESIDENTREGIST_DATEMATCHING_CHAT;
	stPacket.dwObjectID		= HEROID;
	SafeStrCpy(stPacket.Name, HERO->GetObjectName(), MAX_NAME_LENGTH+1);
	SafeStrCpy(stPacket.Msg, szText, MAX_CHAT_LENGTH+1);
	stPacket.dwSenderID		= m_stChatPartner.nAgentID;
	// 다른 에이전트의 플레이어라면 프로토콜 변경
	// ..다른 에이전트 플레이어간의 채팅일 경우 검색 결과에서 nAgentID에 +1000을 한다. 
	// ..유저에게 전송할 때만 +1000을 하고 내부적으로는 nAgentID값을 그대로 사용하기 때문에 -1000 해서 보내야 한다.
	if (m_stChatPartner.nAgentID >= 1000)
	{
		stPacket.Protocol 		= MP_RESIDENTREGIST_DATEMATCHING_CHAT_DIFF_AGENT;
		stPacket.dwSenderID		= m_stChatPartner.nAgentID - 1000;
	}

	NETWORK->Send( (MSGROOT*)&stPacket, stPacket.GetMsgLength() );
#endif
}

// -------------------------------------------------------------------------------------------------------------------------------------
//  CLI_ParseAnswerFromSrv Method																			  서버로부터의 응답 분석/처리
//
VOID CSHResidentRegistManager::CLI_ParseAnswerFromSrv(void* pMsg)
{
#if !defined(_MAPSERVER_) && !defined(_AGENTSERVER)
	MSGBASE* pTmp = (MSGBASE*)pMsg;

	switch(pTmp->Protocol)
	{
	// 등록 처리
	case MP_RESIDENTREGIST_REGIST:
		{
			PACKET_RESIDENTREGIST_REGIST* pPacket = (PACKET_RESIDENTREGIST_REGIST*)pMsg;
			DATE_MATCHING_INFO stInfo;

			ZeroMemory(&stInfo, sizeof(DATE_MATCHING_INFO));
			SafeStrCpy(stInfo.szName, pPacket->stInfo.szNickName, MAX_NAME_LENGTH+1);
			stInfo.bIsValid				= TRUE;
			stInfo.nRace				= pPacket->stInfo.nRace;
			stInfo.byGender				= (BYTE)pPacket->stInfo.nSex;
			stInfo.dwAge				= pPacket->stInfo.nAge;
			stInfo.dwRegion				= pPacket->stInfo.nLocation;
			stInfo.dwGoodFeelingDegree	= pPacket->stInfo.nFavor;
			stInfo.pdwGoodFeeling[0]	= pPacket->stInfo.nPropensityLike01;
			stInfo.pdwGoodFeeling[1]	= pPacket->stInfo.nPropensityLike02;
			stInfo.pdwGoodFeeling[2]	= pPacket->stInfo.nPropensityLike03;
			stInfo.pdwBadFeeling[0]		= pPacket->stInfo.nPropensityDisLike01;
			stInfo.pdwBadFeeling[1]		= pPacket->stInfo.nPropensityDisLike02;

			if (gHeroID == pPacket->dwObjectID)
			{
				HERO->SetResidentRegistInfo(&stInfo);
				HERO->CheckAllHeart();

				// update matching dialog.
				GAMEIN->GetDateMatchingDlg()->UpdateMyInfo(&stInfo) ;

				// active matching dialog.
				CDateMatchingDlg* pDlg = GAMEIN->GetDateMatchingDlg() ;
				pDlg->GetPartnerListDlg()->SettingControls() ;
				//pDlg->GetPartnerListDlg()->UpdateInfo() ;
				pDlg->SetActive(TRUE) ;

				// Deactive identification dialog.
				GAMEIN->GetIdentificationDlg()->SetActive(FALSE) ;

				// close favor icon dialog.
				GAMEIN->GetFavorIconDlg()->SetActive(FALSE) ;

				// Deactive npc script dialog.
				if( HERO->GetState() != eObjectState_Die )
					OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);

				// notice to chatting dialog.
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1298 ) );

				// 080122 LYW --- SHResidentRegistManager : npc 다이얼로그 닫기.
				cNpcScriptDialog* pNpcDlg = NULL ;
				pNpcDlg = GAMEIN->GetNpcScriptDialog() ;
				
				if( pNpcDlg )
				{
					pNpcDlg->SetActive(FALSE) ;
				}
			}
			else
 			{
				CPlayer* pPlayer = (CPlayer*)OBJECTMGR->GetObject(pPacket->dwObjectID);
				if( !pPlayer )	return;

				pPlayer->SetResidentRegistInfo(&stInfo);

				// 히어로와 매칭시킨다.
				if( pPlayer->CheckHeart( HERO ) )
				{
					HERO->AddMatch();

					pPlayer->SetHeart( TRUE );
				}
			}
		}
		break;
	// 자기소개 처리
	case MP_RESIDENTREGIST_UPDATE_INTRODUCTION:
		{
			PACKET_RESIDENTREGIST_UPDATE_INTRODUCTION* pPacket = (PACKET_RESIDENTREGIST_UPDATE_INTRODUCTION*)pMsg;

			if (gHeroID == pPacket->dwObjectID)
			{
				DATE_MATCHING_INFO stInfo;

				stInfo = HERO->GetDateMatchingInfo();
				SafeStrCpy(stInfo.szIntroduce, pPacket->szIntroduction, MAX_INTRODUCE_LEN+1);
				HERO->SetResidentRegistInfo(&stInfo);
			}

			// setting introduce to read only.
			cTextArea* pTextArea = GAMEIN->GetMatchMyInfoDlg()->GetIntroduce() ;

			if( pTextArea )
			{
				pTextArea->SetReadOnly(TRUE) ;
				pTextArea->SetFocusEdit(FALSE) ;
			}

			// notice to chatting dialog.
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1299 ) );
		}
		break;
	//  검색 결과 처리
	case MP_RESIDENTREGIST_UPDATE_INTRODUCTION_NACK:
		{
			MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;

			if(gHeroID == pmsg->dwObjectID)
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(1813));
			}
		}
		break;
	case MP_RESIDENTREGIST_DATEMATCHING_SERCH:
		{
   			PACKET_SERCH_DATE_MATCHING* pPacket = (PACKET_SERCH_DATE_MATCHING*)pMsg;

			// 최대 개수를 초과하면 처리불가
			if (m_nSerchResultNum >= MAX_SERCH_NUM_CLIENT-1) break;
			m_pstSerchResult[m_nSerchResultNum] = pPacket->stSerchResult;
			m_nSerchResultNum++;

			// 검색한 그룹 인덱스
			GAMEIN->GetDateMatchingDlg()->GetPartnerListDlg()->SetSerchGroupIndex(pPacket->nGroupIndex);
			// 에이전트마다 접속자 수가 차이가 날 수 있는데 이럴 경우
			// 가장 많은 그룹 수를 가진 에이전트에서 보낸 그룹 수로 설정하기 위해 받은 그릅 수가 현재 그룹 수보다
			// 클 때 그룹 수를 설정한다. 또한 검색 시작시에는 그와 상관없이 무조건 그룹 수를 설정해야 한다.
			if (pPacket->nGroupMax > GAMEIN->GetDateMatchingDlg()->GetPartnerListDlg()->GetSerchGroupNum())
			{
				GAMEIN->GetDateMatchingDlg()->GetPartnerListDlg()->SetSerchGroupNum(pPacket->nGroupMax);
			}
			if (m_nSerchResultNum == 1)
			{
				// 인터페이스 갱신
				GAMEIN->GetDateMatchingDlg()->GetPartnerListDlg()->ClearPartnerList();
				GAMEIN->GetDateMatchingDlg()->GetPartnerListDlg()->SetSerchGroupNum(pPacket->nGroupMax);
				GAMEIN->GetDateMatchingDlg()->GetPartnerListDlg()->UpdateGroupInfo();
			}

// 			for(int i=0; i<74; i++)
			{
//				pPacket->stSerchResult.nID = i+10;
//				pPacket->stSerchResult.stRegistInfo.nAge = rand()/100;
				// setting to partner list.
				GAMEIN->GetDateMatchingDlg()->GetPartnerListDlg()->UpdateInfo(&pPacket->stSerchResult) ;
			}
		}
		break;
	// 데이트매칭 채팅 신청
	case MP_RESIDENTREGIST_DATEMATCHING_REQUEST_CHAT:
		{
			MSG_DWORD2* stPacket = (MSG_DWORD2*)pMsg;

			switch(stPacket->dwData1)
			{
			// 접속 중이 아님
			case DATE_MATCHING_CHAT_RESULT_LOGOFF:
				{
					WINDOWMGR->MsgBox( MBI_MATCHAT_CANCEL, MBT_CANCEL, CHATMGR->GetChatMsg( 211 ) );
				}
				break;
			// 대상자가 이미 채팅 중 or 채팅 신청중 or 채팅 신청받는 중 
			case DATE_MATCHING_CHAT_RESULT_BUSY:
				{
					//MessageBox(NULL, "상대방이 채팅 중이거나 그 과정 중..", NULL, NULL);
/*
					cMsgBox* pMsgBox = (cMsgBox*)WINDOWMGR->GetWindowForID(MBI_MATCHAT_CANCEL) ;
					if( pMsgBox )
					{
						pMsgBox->ForceClose() ;

						WINDOWMGR->MsgBox( MBI_MATCHAT_CANTCHAT, MBT_OK, "상대방이 현재 대화 준비 중입니다. 잠시 후 다시 신청해 주세요." );
					}
*/
					WINDOWMGR->MsgBox( MBI_MATCHAT_CANCEL, MBT_CANCEL, CHATMGR->GetChatMsg( 1320 ) );
				}
				break;
			// 신청자에게 신청처리와 대기를 알림
			case DATE_MATCHING_CHAT_RESULT_REQUEST_AND_WAIT:
				{
					//MessageBox(NULL, "채팅을 신청했음. 응답 대기", NULL, NULL);
					WINDOWMGR->MsgBox( MBI_MATCHAT_CANCEL, MBT_CANCEL, CHATMGR->GetChatMsg( 1321 ) );
				}
				break;
			// 대상자에게 누군가의 채팅신청 알림
			case DATE_MATCHING_CHAT_RESULT_REQUESTED:
				{
					MSG_DWORD_NAME* pPacket = (MSG_DWORD_NAME*)pMsg;
					char szTxt[256];
					//sprintf(szTxt, "%s(으)로부터의 채팅요청~", pPacket->Name);
					//MessageBox(NULL, szTxt, NULL, NULL);
					sprintf(szTxt, CHATMGR->GetChatMsg( 1323 ), pPacket->Name);
					WINDOWMGR->MsgBox( MBI_MATCHAT_INVITE, MBT_YESNO, szTxt );
				}
				break;
			// 100107 ONS 대화신청 대상자가 챌린지존에 존재
			case DATE_MATCHING_CHAT_RESULT_START_CHALLENGEZONE:
				{
					WINDOWMGR->MsgBox( MBI_MATCHAT_CANCEL, MBT_CANCEL, CHATMGR->GetChatMsg( 1391 ) );
				}
				break;
			// 100629 ONS 데이트매칭거부 메세지 출력
			case DATE_MATCHING_CHAT_RESULT_REJECTION:
				{
					WINDOWMGR->MsgBox( MBI_MATCHAT_CANCEL, MBT_OK, CHATMGR->GetChatMsg( 2316 ) );
				}
				break;
			}
		}
		break;
	// 데이트매칭 채팅 신청 결과
	case MP_RESIDENTREGIST_DATEMATCHING_RESULT_REQUEST_CHAT:
		{
			MSG_DWORD2* stPacket = (MSG_DWORD2*)pMsg;

			switch(stPacket->dwData1)
			{
			// 취소
			case DATE_MATCHING_CHAT_RESULT_CANCEL:
				{
					//MessageBox(NULL, "취소", NULL, NULL);
					WINDOWMGR->MsgBox( MBI_MATCHAT_CANCEL, MBT_OK, CHATMGR->GetChatMsg( 1322 ) );
				}
				break;
			// 시간 초과
			case DATE_MATCHING_CHAT_RESULT_TIMEOUT:
				{
					//MessageBox(NULL, "시간 초과..", NULL, NULL);
					//cDialog* pMsgBox = WINDOWMGR->GetWindowForID(MBI_MATCHAT_NOTICE) ;
					cMsgBox* pMsgBox = NULL ;
					
					pMsgBox = (cMsgBox*)WINDOWMGR->GetWindowForID(MBI_MATCHAT_CANCEL) ;
					if( pMsgBox )
					{
						pMsgBox->ForceClose() ;

						WINDOWMGR->MsgBox( MBI_MATCHAT_TIMEOVER, MBT_OK, CHATMGR->GetChatMsg( 1324 ) );
					}

					pMsgBox = (cMsgBox*)WINDOWMGR->GetWindowForID(MBI_MATCHAT_INVITE) ;
					if( pMsgBox )
					{
						pMsgBox->ForceClose() ;

						WINDOWMGR->MsgBox( MBI_MATCHAT_TIMEOVER, MBT_OK, CHATMGR->GetChatMsg( 1325 ) );
					}
				}
				break;
			// 거절당함
			case DATE_MATCHING_CHAT_RESULT_REFUSE:
				{
					//MessageBox(NULL, "거절당했..!!!", NULL, NULL);
					// deactive wait msg.
					//ForcePressButton(m_pButton[i]->GetID());
					cMsgBox* pMsgBox = (cMsgBox*)WINDOWMGR->GetWindowForID(MBI_MATCHAT_CANCEL) ;
					if( pMsgBox )
					{
						pMsgBox->ForceClose() ;
					}
					WINDOWMGR->MsgBox( MBI_MATCHAT_REJECT, MBT_OK, CHATMGR->GetChatMsg( 1326 ) );
				}
				break;
				break;
			// 채팅 끝
			case DATE_MATCHING_CHAT_RESULT_END_CHAT:
				{
					CDateMatchingDlg* pDlg = GAMEIN->GetDateMatchingDlg() ;

					if( pDlg )
					{
						if (pDlg->GetChatingDlg()->IsOnChatMode())
						{
							WINDOWMGR->MsgBox( MBI_MATCHAT_STOP, MBT_OK, CHATMGR->GetChatMsg( 1327 ) );
						}
						// return to partner list.
						pDlg->SetUpperDlg(e_PartnerListDlg) ;

						CDateMatchingChatDlg* pChatDlg = pDlg->GetChatingDlg() ;

						// setting chat mode to off.
						if(pChatDlg)
						{
							pChatDlg->SetOnOffChatMode(FALSE) ;

							// clear chat list.
							pChatDlg->GetChatList()->RemoveAll() ;
						}

						GAMEIN->GetChallengeZoneListDlg()->SetActive(FALSE);

						cMsgBox* pMsgBox = (cMsgBox*)WINDOWMGR->GetWindowForID(MBI_MATCHAT_CONFIRM) ;
						if(pMsgBox)
							pMsgBox->SetActive(FALSE);
					}
				}
				break;
			}
		}
		break;

	// 채팅시작
	case MP_RESIDENTREGIST_DATEMATCHING_RESULT_START_CHAT:
		{
			//MessageBox(NULL, "채팅시작~", NULL, NULL);
			PACKET_SERCH_DATE_MATCHING* pPacket = (PACKET_SERCH_DATE_MATCHING*)pMsg;

			m_stChatPartner = pPacket->stSerchResult;

			// deactive wait msg.
			cMsgBox* pMsgBox = (cMsgBox*)WINDOWMGR->GetWindowForID(MBI_MATCHAT_CANCEL) ;
			if( pMsgBox )
			{
				pMsgBox->ForceClose() ;
			}

			// open chatting tab.
			CDateMatchingDlg* pDlg = GAMEIN->GetDateMatchingDlg() ;

			if( pDlg )
			{
				// open matching dialog.
				if( !pDlg->IsActive() )
				{
					pDlg->SetActive(TRUE) ;
				}

				// active chatting tab.
				pDlg->SetUpperDlg(e_ChatDlg) ;
				pDlg->GetChatingDlg()->SetOnOffChatMode(TRUE) ;

				// active partner info tab.
				pDlg->GetPartnerInfoDlg()->UpdateInfo(&pPacket->stSerchResult) ;
				pDlg->SetLowerDlg(e_PartnerInfoDlg) ;

				if(pPacket->bMaster)
					GAMEIN->GetChallengeZoneListDlg()->SetActive(TRUE);
			}
		}
		break;

	// 채팅
	case MP_RESIDENTREGIST_DATEMATCHING_CHAT:
		{
			MSG_CHAT_WITH_SENDERID* pPacket = (MSG_CHAT_WITH_SENDERID*)pMsg;
			
			static char szText[256];

			sprintf(szText, "[%s]:%s", pPacket->Name, pPacket->Msg);
			//MessageBox(NULL, szText, "데이트 매칭 채팅", NULL);

			CDateMatchingChatDlg* pDlg = GAMEIN->GetDateMatchingDlg()->GetChatingDlg() ;

			if( pDlg )
			{
				cListDialog* pListDlg = pDlg->GetChatList() ;

				if( pListDlg )
				{
					DWORD dwColor = RGB(255, 255, 255) ;

					pDlg->AddMsg(szText, dwColor) ;
					//pListDlg->AddItem(szText, dwColor, nLineNum) ;
				}
			}
		}
		break;
	// 다른 누군가의 정보
	case MP_RESIDENTREGIST_DATEMATCHING_SOMEBODY_INFO:
		{
			PACKET_SERCH_DATE_MATCHING* pPacket = (PACKET_SERCH_DATE_MATCHING*)pMsg;

			if (HERO == NULL) return;

			m_stChatPartner = pPacket->stSerchResult;

			// deactive wait msg.
			cMsgBox* pMsgBox = (cMsgBox*)WINDOWMGR->GetWindowForID(MBI_MATCHAT_CANCEL) ;
			if( pMsgBox )
			{
				pMsgBox->ForceClose() ;
			}

			// open chatting tab.
			CDateMatchingDlg* pDlg = GAMEIN->GetDateMatchingDlg() ;

			if( pDlg )
			{
				// open matching dialog.
				if( !pDlg->IsActive() )
				{
					pDlg->GetPartnerListDlg()->SettingControls() ;
					pDlg->SetActive(TRUE) ;
				}

				// active partner info tab.
				pDlg->GetPartnerInfoDlg()->UpdateInfo(&pPacket->stSerchResult) ;
				pDlg->SetLowerDlg(e_PartnerInfoDlg) ;
			}
		}
		break;
	case MP_RESIDENTREGIST_REGIST_NOTIFY:
		{
			MSG_DWORD_NAME* pmsg = ( MSG_DWORD_NAME* )pMsg;

			CHATMGR->AddMsg( CTC_OPERATOR2, CHATMGR->GetChatMsg( 1535 ), pmsg->Name );
		}
		break;
	}
#endif
}
