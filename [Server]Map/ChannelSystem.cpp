#include "StdAfx.h"
#include "ChannelSystem.h"
#include "MHFile.h"
#include "BattleSystem_Server.h"
#include "Object.h"
#include "NetWork.h"
#include "QuestMapMgr.h"
#include "../hseos/Date/SHDateManager.h"
#include "HousingMgr.h"
#include "Dungeon/DungeonMgr.h"

extern int g_nServerSetNum;

CChannelSystem::CChannelSystem()
{
	m_dwCount = 0;
	m_bQuestMap = FALSE;
	m_nChallengeZoneLastChannelID = 0;
	ZeroMemory(m_nChallengeZoneAllocatedCH,		sizeof(m_nChallengeZoneAllocatedCH));
	ZeroMemory(m_nChallengeZoneAgentChannelID, sizeof(m_nChallengeZoneAgentChannelID));
	m_nChallengeZoneAllocatedCHNum = 0;
	m_nChallengeZonePlayerAgentChannelID= 0;
}

CChannelSystem::~CChannelSystem()
{
	for(DWORD i=0; i<m_dwCount; ++i)
	{
		if(m_Channel[i])
		{		
			delete m_Channel[i];
			m_Channel[i] = NULL;
		}
	}
}

void CChannelSystem::LoadChannelInfo()
{
	CMHFile file;
	BYTE Kind, num = 0;
	char value[64] = {0,};
	
	char filename[256];
	sprintf(filename,"serverset/%d/ChannelInfo.bin",g_nServerSetNum);

	file.Init(filename, "rb");

	if(file.IsInited() == FALSE)
	{
		MessageBox(NULL,"Can't Open ChannelInfo File", NULL, NULL);
		return;
	}

	while(1)
	{
		if(file.IsEOF())
			break;
		
		strcpy( value, strupr(file.GetString()) );

		if( strcmp( value,"*CHANNEL" ) == 0 )
		{
			Kind = file.GetByte();
			num = file.GetByte();
			file.GetStringInQuotation( m_ChannelName );
		}
	}
	file.Release();
}

void CChannelSystem::CreateChannel(BYTE Kind, BYTE num)
{
	for(BYTE i=0; i<num; ++i)
	{			
		 DWORD ChannelIDX = BATTLESYSTEM->CreateChannel(); // 그리드 아이디 == ChannelIDX
		 m_Channel[i] = new CChannel;
		 m_Channel[i]->SetChannelID(ChannelIDX);
	} 
	
	m_dwCount = num;
	m_bQuestMap = FALSE;
}

void CChannelSystem::IncreasePlayerNum(DWORD ChannelID)
{
	if(HOUSINGMAPNUM == g_pServerSystem->GetMapNum())
	{
		return;
	}
	else if(DungeonMGR->IsDungeon(g_pServerSystem->GetMapNum()))
	{
		return;
	}
	else if(QUESTMAPMGR->IsQuestMap())
	{
		m_QMChannel[ChannelID-1].IncreaseNum();
		return;
	}

	if(ChannelID > m_dwCount)
	{
		return;
	}

	m_Channel[ChannelID-1]->IncreaseNum();
}

void CChannelSystem::DecreasePlayerNum(DWORD ChannelID)
{
	if(HOUSINGMAPNUM == g_pServerSystem->GetMapNum())
	{
		return;
	}
	else if(DungeonMGR->IsDungeon(g_pServerSystem->GetMapNum()))
	{
		return;
	}
	else if(QUESTMAPMGR->IsQuestMap())
	{
		m_QMChannel[ChannelID-1].DecreaseNum();
		return;
	}

	if(ChannelID > m_dwCount)
	{
		return;
	}

	m_Channel[ChannelID-1]->DecreaseNum();
}


void CChannelSystem::SendChannelInfo(MSG_DWORD* pInfo, DWORD dwConnectionIndex)
{
	if(g_pServerSystem->GetStart() == FALSE || m_bQuestMap )
	{
		MSG_BYTE nmsg;
		nmsg.Category = MP_USERCONN;
		nmsg.Protocol = MP_USERCONN_CHANNELINFO_NACK;
		nmsg.bData = 0;
		nmsg.dwObjectID = pInfo->dwObjectID;
		g_Network.Send2Server(dwConnectionIndex,(char*)&nmsg,sizeof(nmsg));
		return;
	}

	MSG_CHANNEL_INFO msg;
	memset(&msg, 0, sizeof(MSG_CHANNEL_INFO));
	msg.Category = MP_USERCONN;
	msg.Protocol = MP_USERCONN_CHANNELINFO_ACK;
	SafeStrCpy(msg.ChannelName, m_ChannelName, MAX_CHANNEL_NAME+1);

	for(DWORD i=0; i<m_dwCount; ++i)
	{
		msg.PlayerNum[i] = m_Channel[i]->GetPlayerNum();
	}
	msg.dwObjectID = pInfo->dwObjectID;
	msg.dwUniqueIDinAgent = pInfo->dwData;
	msg.Count = (BYTE)m_dwCount;
	g_Network.Send2Server(dwConnectionIndex,(char*)&msg,sizeof(msg));
}

DWORD CChannelSystem::GetChannelID(int num)
{
	if( m_bQuestMap )
	{
		return 0;
	}
	else if(HOUSINGMAPNUM == g_pServerSystem->GetMapNum())
	{
		// 하우징은 자동생성 채널이므로 맵이동주문서를 사용할 수 있도록 1을 반환
		return 1;
	}
	else if(DungeonMGR->IsDungeon(g_pServerSystem->GetMapNum()))
	{
		// 인던은 자동생성 채널이므로 맵이동주문서를 사용할 수 있도록 1을 반환
		return 1;
	}

	if(((DWORD)num >= m_dwCount) || (num < 0))
	{
		ASSERT(0);
		return 0;
	}
		
	return m_Channel[num]->GetChannelID();
}

void CChannelSystem::SendChannelInfoToMS( DWORD dwConnectionIndex )
{
	if( m_bQuestMap )	return;
	
	MSG_CHANNEL_INFO msg;
	memset(&msg, 0, sizeof(MSG_CHANNEL_INFO));
	msg.Category = MP_MORNITORMAPSERVER;
	msg.Protocol = MP_MORNITORMAPSERVER_QUERY_CHANNELINFO_ACK;
	SafeStrCpy(msg.ChannelName, m_ChannelName, MAX_CHANNEL_NAME+1);
	for(DWORD i=0; i<m_dwCount; ++i)
	{
		msg.PlayerNum[i] = m_Channel[i]->GetPlayerNum();
	}
	msg.Count = (BYTE)m_dwCount;
	g_Network.Send2Server(dwConnectionIndex,(char*)&msg,sizeof(msg));
}

void CChannelSystem::InitQuestMapChannel()
{
	m_bQuestMap = TRUE;
}

DWORD CChannelSystem::CreateQuestMapChannel(DWORD nFirstEnterPlayerID) 
{
	if( !m_bQuestMap )	return 0;

	if (g_csDateManager.IsChallengeZoneHere())
	{
		// 에이전트에서 1000 이상으로 보낸다. 1000 이라는 값은 기본으로 설정한 채널과 구분하기 위한 수치임.
		if (m_nChallengeZonePlayerAgentChannelID >= 1000)
		{
			// 여기에서의 카운트 1000에 대해서 설명.
			// 동시에 2000명이 챌린지 존에 입장할 경우, 1000명이 먼저 챌린지 존에 도착할 수 있다.
			// 그러면 나중에 들어오는 플레이어는 먼저 들어온 1000명의 식별코드 중 자신의 식별코드와 일치하는 경우에
			// 먼저 들어온 플레이어의 채널ID를 공유한다.
			// 일반(정상)적이라면 2000명이 챌린지 존에 입장할 경우, 2000명의 일부가 먼저 들어오고, 그 일부의 파트너가 들어오고,
			// 또 나머지의 일부가 들어오고, 그 일부의 파트너가 들어오고 할 텐데, 최악의 경우는
			// 혼자들어온 경우만 1000건이 될 수가 있다. 이와 같이 혼자들어온 최악의 경우의 수가 1000 임.
			DWORD nCnt = 1000;
			if (nCnt > m_nChallengeZoneAllocatedCHNum) nCnt = m_nChallengeZoneAllocatedCHNum;

			for(DWORD i=0; i<nCnt; i++)
			{
				if (m_nChallengeZoneAgentChannelID[i] == m_nChallengeZonePlayerAgentChannelID)
				{
					DWORD nChannel = m_nChallengeZoneAllocatedCH[i][0];

					// 챌린지 시작 설정
					g_csDateManager.SetChallengeZoneStart(TRUE);
					g_csDateManager.SetChallengeZoneFirstEnterPlayerID(m_nChallengeZoneAllocatedCH[i][1]);
					if (g_pServerSystem->IsTestServer())
					{
						g_Console.LOG(4, "Share ChallengeZone Channel:%d FirstEnterPlayerID:%d Agent CHID:%d)", nChannel, m_nChallengeZoneAllocatedCH[i][1], m_nChallengeZonePlayerAgentChannelID);
					}
					// 잊지 말고 초기화
					m_nChallengeZoneAllocatedCH[i][0]	= 0;
					m_nChallengeZoneAllocatedCH[i][1]	= 0;
					m_nChallengeZoneAgentChannelID[i]	= 0;
					return nChannel;
				}
			}
		}
	}

	DWORD ChannelIDX = BATTLESYSTEM->CreateChannel();

	for( int i = 0; i < MAX_QUEST_OR_CHALLENGE_CHANNEL_NUM; ++i )
	{
		if( m_QMChannel[i].GetChannelID() == 0 )
			break;
	}

	if( i > MAX_QUEST_OR_CHALLENGE_CHANNEL_NUM-1 )	return 0;

	m_QMChannel[i].SetChannelID( ChannelIDX );
	
	++m_dwCount;

	if (g_csDateManager.IsChallengeZoneHere())
	{
		for(DWORD i=0; i<1000; i++)
		{
			if (m_nChallengeZoneAllocatedCH[i][0] == 0)
			{
				m_nChallengeZoneAllocatedCH[i][0]	= ChannelIDX;
				m_nChallengeZoneAllocatedCH[i][1]	= nFirstEnterPlayerID;
				m_nChallengeZoneAgentChannelID[i]	= m_nChallengeZonePlayerAgentChannelID;
				m_nChallengeZoneLastChannelID		= ChannelIDX;
				m_nChallengeZoneAllocatedCHNum++;
				// ..파트너없이 혼자 접속되었을 경우에 대비해서 일정시간 파트너의 접속이 없어서 초기화가 되지 않은 정보는 초기화한다.
				if (i >= 500)
				{
					m_nChallengeZoneAllocatedCH[i-500][0]	= 0;
					m_nChallengeZoneAllocatedCH[i-500][1]	= 0;
					m_nChallengeZoneAgentChannelID[i-500]	= 0;
				}
				if (g_pServerSystem->IsTestServer())
				{
					g_Console.LOG(4, "Create ChallengeZone Channel:%d ChannelQty(or EnterFreq):%d Agent CHID:%d", ChannelIDX, m_nChallengeZoneAllocatedCHNum, m_nChallengeZonePlayerAgentChannelID);
				}
				break;
			}
		}
	}

	return ChannelIDX;
}

BOOL CChannelSystem::GetChallengeZonePartnerCH(DWORD nChallengeZoneAgentChannelID)
{
	DWORD nCnt = 1000;
	if (nCnt > m_nChallengeZoneAllocatedCHNum) nCnt = m_nChallengeZoneAllocatedCHNum;

	for(DWORD i=0; i<nCnt; i++)
	{
		if (m_nChallengeZoneAgentChannelID[i] == nChallengeZoneAgentChannelID)
		{
			return m_nChallengeZoneAllocatedCH[i][0];
		}
	}

	return 0;
}

void CChannelSystem::DestroyQuestMapChannel( DWORD dwChannel )
{
	if( !m_bQuestMap )	return;

	for( int i = 0; i < MAX_QUEST_OR_CHALLENGE_CHANNEL_NUM; ++i )
	{
		if( m_QMChannel[i].GetChannelID() == dwChannel )
		{
			m_QMChannel[i].SetChannelID( 0 );
			break;
		}
	}
	--m_dwCount;

	BATTLESYSTEM->DestroyChannel( dwChannel );

	// desc_hseos_데이트 존_01
	// S 데이트 존 추가 added by hseos 2007.11.28
	// ..2명의 플레이어 입장에 사용하는 채널과 파트너ID 값을 초기화한다.
	// ..초기화하지 않을 경우, 먼저 들어온 플레이어가 접속을 끊고, 채널을 지웠는데
	// ..나중에 들어온 플레이어가 지운 채널을 할당받아서 거기로 들어간다.
	// ..이럴 경우 아마 다른 팀과 같은 채널을 사용하게 된다거나 뭔가 꼬이는 일이 발생할 듯..
	if (g_csDateManager.IsChallengeZoneHere())
	{
		if (g_pServerSystem->IsTestServer())
		{
			g_Console.LOG(4, "Delete ChallengeZone Channel:%d", dwChannel);
		}

		DWORD nCnt = 1000;
		if (nCnt > m_nChallengeZoneAllocatedCHNum) nCnt = m_nChallengeZoneAllocatedCHNum;

		for(DWORD i=0; i<nCnt; i++)
		{
			if (m_nChallengeZoneAllocatedCH[i][0] == dwChannel)
			{
				m_nChallengeZoneAllocatedCH[i][0]	= 0;
				m_nChallengeZoneAllocatedCH[i][1]	= 0;
				m_nChallengeZoneAgentChannelID[i]	= 0;
				if (g_pServerSystem->IsTestServer())
				{
					g_Console.LOG(4, "Delete ChallengeZone ChannelInfo(Alone):%d", dwChannel);
				}
				break;
			}
		}
	}
}

void CChannelSystem::Init( WORD wMapNum, WORD wChannelNum )
{
	if( QUESTMAPMGR->Init( wMapNum ) )	return;

	LoadChannelInfo();
	CreateChannel(0, BYTE( wChannelNum ) );
}

DWORD CChannelSystem::GetPlayerNumInChannel(int index)
{
	if(HOUSINGMAPNUM == g_pServerSystem->GetMapNum())
	{
		return HOUSINGMGR->GetPlayerNumInHouse(index);
	}
	else if(DungeonMGR->IsDungeon(g_pServerSystem->GetMapNum()))
	{
		return DungeonMGR->GetPlayerNumInDungeon(index);
	}
	else if(QUESTMAPMGR->IsQuestMap())
	{
		DWORD dwPlayerNum = m_QMChannel[index-1].GetPlayerNum();
		return dwPlayerNum;
	}

	--index;

	if( int(m_dwCount) <= index )
	{
		return 0;
	}

	return m_Channel[index]->GetPlayerNum();
}