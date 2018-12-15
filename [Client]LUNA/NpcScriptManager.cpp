#include "StdAfx.h"
#include "NpcScriptManager.h"
#include "ObjectStateManager.h"
#include "WindowIDEnum.h"
#include "interface/cWindowManager.h"
#include "MHMap.h"
#include "GameIn.h"
#include "cDialogueList.h"
#include "cHyperTextList.h"
#include "cMsgBox.h"
#include "cPage.h"
#include "Npc.h"
#include "DateMatchingDlg.h"
#include "GuildMarkDialog.h"
#include "InventoryExDialog.h"
#include "NpcScriptDialog.h"
#include "StorageDialog.h"
#include "GuildManager.h"
#include "MoveManager.h"
#include "ChatManager.h"
#include "ObjectManager.h"
#include "../hseos/Family/SHFamilyManager.h"
#include "NpcNoticeDlg.h"
#include "../[CC]Header/GameResourceStruct.h"
#include "MHFile.h"

CNpcScriptManager::CNpcScriptManager() :
m_pDialogue(new cDialogueList),
m_pHyperText(new cHyperTextList),
m_pCurSelectedNpc(0)
{
	LoadScript();
}

CNpcScriptManager::~CNpcScriptManager()													// 소멸자 함수.
{
	for(NpcMap::iterator npcIterator = mNpcMap.begin();
		mNpcMap.end() != npcIterator;
		++npcIterator)
	{
		PageMap& pageMap = npcIterator->second;

		for(PageMap::iterator pageIterator = pageMap.begin();
			pageMap.end() != pageIterator;
			++pageIterator)
		{
			SAFE_DELETE(pageIterator->second);
		}
	}

	SAFE_DELETE(m_pDialogue);
	SAFE_DELETE(m_pHyperText);
}

CNpcScriptManager* CNpcScriptManager::GetInstance()
{
	static CNpcScriptManager instance;

	return &instance;
}

void CNpcScriptManager::AddScript(DWORD npcIndex, DWORD pageIndex, BYTE emotion)
{
	PageMap& pageMap = mNpcMap[npcIndex];
	
	if(pageMap.end() == pageMap.find(pageIndex))
	{
		if(pageMap.empty())
		{
			mMainPageMap.insert(
				std::make_pair(npcIndex, pageIndex));
		}

        pageMap.insert(
			std::make_pair(pageIndex, new cPage));
	}

	cPage* const page = pageMap[pageIndex];
	page->Init(emotion);
}

cPage* CNpcScriptManager::GetMainPage(DWORD npcIndex)
{
	MainPageMap::iterator iterator = mMainPageMap.find(npcIndex);

	if(mMainPageMap.end() == iterator)
	{
		return 0;
	}

	const DWORD	pageIndex = iterator->second;

	return GetPage(npcIndex, pageIndex);
}

cPage* CNpcScriptManager::GetPage( DWORD dwNpcId, DWORD dwPageId )
{
	NpcMap::iterator npcIterator = mNpcMap.find(dwNpcId);

	if(mNpcMap.end() == npcIterator)
	{
		return 0;
	}

	PageMap& pageMap = npcIterator->second;
	PageMap::iterator pageIterator = pageMap.find(dwPageId);

	return pageMap.end() == pageIterator ? 0 : pageIterator->second;
}

void CNpcScriptManager::LoadScript()
{
	CMHFile file;
	file.Init(
		"Data/Script/Npc/Npc_Script.bin",
		"rb");

	DWORD npcIndex = 0;
	DWORD pageIndex = 0;

	while(FALSE == file.IsEOF())
	{
		TCHAR buffer[MAX_PATH] = {0};
		file.GetLine(
			buffer,
			sizeof(buffer) / sizeof(*buffer));

		LPCTSTR seperator = " \t#$";
		LPCTSTR token = _tcstok(
			buffer,
			seperator);

		if(0 == token)
		{
			continue;
		}
		else if(0 == _tcsicmp("npcId", token))
		{
			LPCTSTR textIndex = _tcstok(0, seperator);
			npcIndex = _ttoi(textIndex ? textIndex : "");
		}
		else if(0 == _tcsicmp("pageInfo", token))
		{
			LPCTSTR textPageIndex = _tcstok(0, seperator);
			_tcstok(0, seperator);
			_tcstok(0, seperator);
			LPCTSTR textEmotion = _tcstok(0, seperator);

			pageIndex = _ttoi(textPageIndex ? textPageIndex : "");
			const BYTE emotion = BYTE(_ttoi(textEmotion ? textEmotion : ""));
			AddScript(
				npcIndex,
				pageIndex,
				emotion);
		}
		else if(0 == _tcsicmp("dialogue", token))
		{
			cPage* const page = GetPage(
				npcIndex,
				pageIndex);

			if(0 == page)
			{
				continue;
			}

			for(LPCTSTR textIndex = _tcstok(0, seperator);
				0 != textIndex;
				textIndex = _tcstok(0, seperator))
			{
				page->AddDialogue(_ttoi(textIndex ? textIndex : ""));
			}
		}
		else if(0 == _tcsicmp("hyperlink", token))
		{
			LPCTSTR textLinkIndex = _tcstok(0, seperator);
			LPCTSTR textLinkType = _tcstok(0, seperator);
			LPCTSTR textData = _tcstok(0, seperator);
			LPCTSTR textX = _tcstok(0, seperator);
			LPCTSTR textZ = _tcstok(0, seperator);

			HYPERLINK hyperLink = {0};
			hyperLink.wLinkId = WORD(_ttoi(textLinkIndex ? textLinkIndex : ""));
			hyperLink.wLinkType = LINKTYPE(_ttoi(textLinkType ? textLinkType : "") + 1);
			hyperLink.dwData = _ttoi(textData ? textData : "");
			hyperLink.fXpos = float(_tstof(textX ? textX : ""));
			hyperLink.fZpos = float(_tstof(textZ ? textZ : ""));

			cPage* const page = GetPage(
				npcIndex,
				pageIndex);

			if(page)
			{
				page->AddHyperLink(hyperLink);
			}
		}
	}
}

void CNpcScriptManager::StartNpcScript( CNpc* pNpc )									// NPC 스크립트를 시작하는 함수.
{
	if(GetSelectedNpc())
	{
		GAMEIN->GetNpcScriptDialog()->EndDialog();
	}

	VECTOR3 pos = {0};
	HERO->GetPosition(&pos);
	MOVEMGR->SetLookatPos(
		pNpc,
		&pos,
		0,
		gCurTime);
	SetSelectedNpc(pNpc);

	// 090925 ShinJS --- Npc Notice Dlg 를 사용하는 경우
	if( pNpc->IsUseNoticeDlg() )
	{
		cNpcNoticeDlg* pNpcNoticeDlg = (cNpcNoticeDlg*)WINDOWMGR->GetWindowForID( NND_NOTICEDLG );
		if( pNpcNoticeDlg &&
			pNpcNoticeDlg->OpenDialog( pNpc->GetNpcUniqueIdx(), pNpc->GetNpcListInfo()->ModelNum ) == FALSE )
		{
			if( HERO->GetState() == eObjectState_Deal )
				OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);
		}

		return;
	}

	// 080402 NYJ --- 다이얼로그를 여는데 실패했을경우 처리
	//GAMEIN->GetNpcScriptDialog()->OpenDialog( pNpc->GetNpcUniqueIdx(), pNpc->GetNpcListInfo()->ModelNum );	// 다이얼로그를 연다.
	if(!GAMEIN->GetNpcScriptDialog()->OpenDialog( pNpc->GetNpcUniqueIdx(), pNpc->GetNpcListInfo()->ModelNum ))
	{
		if(GAMEIN->GetNpcScriptDialog()->IsActive())
		{
			NPCSCRIPTMGR->SetSelectedNpc(NULL);
			GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
		}

		if( HERO->GetState() == eObjectState_Deal )
			OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);
	}
}

void CNpcScriptManager::StartNpcBusiness(DWORD dwData, DWORD dwParam)					// NPC 비지니스를 시작하는 함수.
{
	if( m_pCurSelectedNpc == NULL && dwParam == 0 )										// 현재 선택 된 NPC 정보가 없거나, 파라메터 값이 0과 같으면,
	{
		ASSERT(0);																		// ASSERT 체크를 한다.

		if( HERO->GetState() == eObjectState_Deal )										// HERO의 상태가 거래 중이면,
		{
			OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);					// 거래 상태를 종료한다.
		}
		
		return;																			// 리턴 처리를 한다.
	}

	if( m_pCurSelectedNpc )																// 현재 선택 된 NPC 정보가 있다면,
	{
		WORD wJobKind = m_pCurSelectedNpc->GetNpcJob() ;								// NPC 직업을 받는다.

		switch( wJobKind )																// NPC 직업을 확인한다.
		{
		case DEALER_ROLE :			Business_Dealer() ;					break ;			// 딜러라면, 딜을 시작한다.
		case CHANGGO_ROLE :			Business_Changgo(dwData, dwParam) ;	break ;			// 창고지기라면, 창고 비지니스를 시작한다.
		case MUNPA_ROLE :			Business_Guild(dwData) ;			break ;			// 길드 관리인 이라면, 길드 비지니스를 시작한다.
		case MAPCHANGE_ROLE :		Business_MapChange(dwData) ;		break ;			// 맵 체인지 NPC 라면, 맵 체인지 비지니스를 시작한다.
		case FAMILY_ROLE :			Business_Family(dwData) ;			break ;			// 패밀리 관리인 이라면, 패밀리 관련 비지니스를 시작한다.
		case IDENTIFICATION_ROLE :	Business_Identifycation() ;			break ;			// 주민등록 관리 인이라면, 주민등록 비지니스를 시작한다.
		case HOUSING_ROLE:			Business_Housing(dwData) ;			break ;
		default :																		// 그 외의 경우,
			{
				ASSERT(0);																// ASSERT 체크를 한다.

				if( HERO->GetState() == eObjectState_Deal )								// HERO의 상태가 거래중이라면,
				{
					OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);			// 거래 상태를 끝낸다.
				}

				return;																	// 리턴 처리를 한다.
			}
			break ;
		}
	}

	if( dwParam == eNpcParam_ShowpStorage )												// 파라메터 값이 eNpcParam_ShowpStorage와 같으면,
	{
		Business_Changgo(dwData, dwParam) ;												// 창고 비지니스를 시작한다.
	}

	GAMEIN->GetNpcScriptDialog()->DisActiveEx();										// NPC 스크립트 다이얼로그의 DisActiveEx()를 호출한다.
}

void CNpcScriptManager::Business_Dealer()  												// 딜러 비지니스.
{
	MSG_WORD msg;																		// 메시지 구조체를 선언한다.
	msg.Category = MP_ITEM;																// 카테고리를 아이템으로 세팅한다.
	msg.Protocol = MP_ITEM_DEALER_SYN;													// 프로토콜을 아이템 딜러 요청으로 세팅한다.
	msg.dwObjectID = HEROID;															// HERO의 아이디를 세팅한다.
	msg.wData = m_pCurSelectedNpc->GetNpcUniqueIdx();									// NPC 유니크 아이디를 세팅한다.

	NETWORK->Send(&msg, sizeof(msg));													// 메시지를 전송한다.

	GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);										// 스크립트 다이얼로그를 비활성화 한다.
}

void CNpcScriptManager::Business_Changgo(DWORD dwData, DWORD dwParam)					// 창고 비지니스.
{
	// 080513 KTH -- NPC 스크립트가 열려 있어야만 열수 있도록 변경 (defense hack)
	if( !GAMEIN->GetNpcScriptDialog()->IsActive() )
		return;

	if(dwData == 0)																		// 데이터를 확인한다.
	{	
		if(GAMEIN->GetStorageDialog()->IsItemInit())									// 창고 아이템의 아이템 초기화가 되었으면,
		{
			if( !GAMEIN->GetInventoryDialog()->IsActive() )								// 인벤토리 다이얼로그가 활성화 중이면,
			{
				GAMEIN->GetStorageDialog()->ShowStorageDlg(TRUE) ;							// 창고 다이얼로그를 활성화 한다.
			}
			else
			{
				GAMEIN->GetStorageDialog()->SetActive(TRUE) ;							// 창고 다이얼로그를 활성화 한다.
			}

			if(HERO->GetStorageNum() == 0)												// 개설한 창고가 없다면,
			{

				GAMEIN->GetStorageDialog()->ShowStorageMode(eStorageMode_NoWare);		// 창고가 없다는 모드로 세팅한다.

				GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);							// 창고 다이얼로그를 비활성화 한다.
			}
			else																		// 개설한 창고가 있다면,
			{
				GAMEIN->GetStorageDialog()->ShowStorageMode(eStorageMode_StorageWare1);	// 창고 1을 활성화 한 모드로 한다.
				GAMEIN->GetStorageDialog()->AddStorageMode(eStorageMode_StorageWare1);	// 창고 모드를 추가한다.

				GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);							// 스크립트 다이얼로그를 비활성화 한다.
			}
		}
		else																			// 창고의 아이템의 아이템 초기화가 되지 않았다면,
		{
			if(HERO->GetStorageNum() == 0)												// 개설한 창고가 없으면,
			{
				Business_Changgo(
				1,
				0);
				return;
			}
			else																		// 개설한 창고가 있다면,
			{
				MAP->SetVillage(TRUE) ;													// 마을로 세팅한다.

				if( dwParam == eNpcParam_ShowpStorage || MAP->IsVillage() == TRUE )		// 창고 npc 이고, 마을이 맞으면,
				{						
					MSG_WORD2 msg;														// 메시지 구조체를 선언한다.
					msg.Category = MP_ITEM;												// 카테고리를 아이템으로 세팅한다.
					msg.Protocol = MP_ITEM_STORAGE_ITEM_INFO_SYN;						// 프로토콜을 아이템 창고, 아이템 정보 싱크로 세팅한다.
					msg.dwObjectID = HEROID;											// HERO 아이디를 세팅한다.
					
					if( dwParam==eNpcParam_ShowpStorage )								// NPC가 창고 NPC 이면,
					{
						msg.wData1=79 ;													// 데이터를 79로 세팅한다.
					}
					else																// NPC가 창고 NPC가 아니면,
					{
						msg.wData1=m_pCurSelectedNpc->GetNpcUniqueIdx();				// 데이터를 NPC 유니크 인덱스로 세팅한다.
					}
					//dwParam==eNpcParam_ShowpStorage ? msg.wData1=79 : msg.wData1=m_pCurSelectedNpc->GetNpcUniqueIdx();

					msg.wData2 = eNpcParam_ShowpStorage;								// 데이터 2를 NPC 데이터로 세팅한다.

					NETWORK->Send(&msg, sizeof(msg));									// 메시지를 전송한다.
				}
				else																	// NPC가 창고 NPC가 아니거나, 마을이 아니면,
					return;																// 리턴 처리를 한다.
			}
		}

		OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Deal);						// HERO의 상태를 거래 상태로 세팅한다.
	}	
	else if(dwData == 1)																// 가격표/구입
	{
		GAMEIN->GetStorageDialog()->SetStorageListInfo();								// 창고 다이얼로그의 리스트 정보를 세팅한다.
		GAMEIN->GetStorageDialog()->ShowStorageDlg(TRUE) ;								// 창고 다이얼로그의 창고 다이얼로그를 보여준다.

		GAMEIN->GetStorageDialog()->ShowStorageMode(eStorageMode_StorageListInfo);		// 창고 리스트 정보로 창고 모드를 세팅한다.
		GAMEIN->GetStorageDialog()->AddStorageMode(eStorageMode_StorageListInfo);		// 창고 다이얼로그에 창고 모드를 추가한다.

		GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);									// 스크립트 다이얼로그를 비활성화 한다.
	}
	else if(dwData == 2)																// 길드 창고
	{
		GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);

		if( ! HERO->GetGuildIdx() )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 325 ) );
			return;
		}

		const CGuildManager::LevelSetting* setting = GUILDMGR->GetLevelSetting( GUILDMGR->GetLevel() );
		ASSERT( setting );

		if( !	setting ||
			!	setting->mWarehouseSize )
		{
			CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 613 ) );
			return;
		}
		
		MSGBASE message;

		message.Category	= MP_GUILD;
		message.Protocol	= MP_GUILD_OPEN_WAREHOUSE_SYN;
		message.dwObjectID	= HEROID;

		NETWORK->Send( &message, sizeof( message ) );
	}
	else if(dwData == 3)																// 아이템몰창고
	{
		// 091105 pdy 조합/인챈트/강화/분해 중인경우 아이템몰 열기 불가능 
		BOOL isOpen = FALSE;
		{
			cDialog* mixDialog			= WINDOWMGR->GetWindowForID( ITMD_MIXDLG );
			cDialog* enchantDialog		= WINDOWMGR->GetWindowForID( ITD_UPGRADEDLG );
			cDialog* reinforceDialog	= WINDOWMGR->GetWindowForID( ITR_REINFORCEDLG );
			cDialog* dissoloveDialog	= WINDOWMGR->GetWindowForID( DIS_DISSOLUTIONDLG );

			ASSERT( mixDialog && enchantDialog && reinforceDialog && dissoloveDialog );

			isOpen =	mixDialog->IsActive()		||
						enchantDialog->IsActive()	||
						reinforceDialog->IsActive()	||
						dissoloveDialog->IsActive();
		}

		if( isOpen == TRUE )
		{
			CHATMGR->AddMsg( CTC_OPERATEITEM, CHATMGR->GetChatMsg( 1082 ) ) ;
			return;
		}

		MSGBASE msg;																	// 메시지 구조체를 선언한다.
		msg.Category = MP_ITEM;															// 카테고리를 아이템으로 세팅한다.
		msg.Protocol = MP_ITEM_SHOPITEM_INFO_SYN;										// 프로토콜을 아이템 샵 정보 싱크로 세팅한다.
		msg.dwObjectID = gHeroID;
		NETWORK->Send(&msg, sizeof(msg));												// 메시지를 전송한다.

		GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);									// 스크립트 다이얼로그를 비활성화 한다.
		OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Deal);						// HERO의 상태를 거래 상태로 세팅한다.
	}
}

void CNpcScriptManager::Business_Guild(DWORD dwData)  									// 길드 비지니스.
{
	switch( dwData )																	// 데이터를 확인한다.
	{
	case 0:																				// 길드를 생성하려고 한다면,
		{
			if(HERO->GetGuildIdx())														// HERO의 길드 인덱스가 유효하면,
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(327));					// 이미 길드가 있다는 메시지를 출력한다.
				GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);							// NPC 스크립트 다이얼로그를 비활성화 한다.

				return;																	// 리턴 처리를 한다.
			}

			cDialog* pDlg = (cDialog*)GAMEIN->GetGuildCreateDlg();						// 길드 생성 다이얼로그 정보를 받아온다.

			if( pDlg )																	// 길드 생성 다이얼로그 정보가 유효한지 체크한다.
			{
				pDlg->SetActive(TRUE);													// 길드 생성 다이얼로그를 활성화 한다.
			}

			GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);								// NPC 스크립트 다이얼로그를 닫는다.
		}
		break;

	case 1:																				// 길드를 해제하려고 한다면,
		{
			if(HERO->GetGuildIdx() == 0)												// HERO의 길드 인덱스가 유효하지 않으면,
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(325));					// 길드에 가입되어 있지 않다는 메시지를 출력한다.
				GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);							// NPC 스크립트 다이얼로그를 비활성화 한다.

				return;																	// 리턴 처리를 한다.
			}
			else if(HERO->GetGuildMemberRank() == GUILD_MASTER)							// HERO의 직위가 길드 마스터와 같다면,
			{
				WINDOWMGR->MsgBox( MBI_GUILD_BREAKUP, MBT_YESNO, CHATMGR->GetChatMsg( 329 ) );	// 패널티를 받게 된다는 선택창을 띄운다.
				GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);							// NPC 스크립트 다이얼로그를 닫는다.

				return ;																// 리턴 처리를 한다.
			}
			else																		// HERO의 직위기 길드 마스터가 아니라면,
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(323));					// 권한이 없다는 메시지를 출력한다.
				GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);							// NPC 스크립트 다이얼로그를 닫는다.

				return;																	// 리턴 처리를 한다.
			}
		}
		break;

	case 2:																				// 길드 레벨업 관련.
		{
			if(HERO->GetGuildIdx() == 0)												// HERO의 길드 인덱스를 체크한다.
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(325));					// 길드에 가입되어 있지 않다는 메시지를 출력한다.
			}
			else if(HERO->GetGuildMemberRank() != GUILD_MASTER)							// HERO의 직위가 길드 마스터와 같지 않다면,
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(323));					// 권한이 없다는 메시지를 출력한다.
			}
			else
			{
				cDialog* const guildLevelUpDialog = WINDOWMGR->GetWindowForID(GD_LEVELUPDLG);

				if(guildLevelUpDialog)
				{
					guildLevelUpDialog->SetActive(TRUE);
				}
			}	

			GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
		}
		break;

	case 3:																				// 길드 마크 등록.
		{
			if( HERO->GetGuildIdx() )
			{
				GAMEIN->GetGuildMarkDlg()->ShowGuildMark();
				//GAMEIN->GetGuildMarkDlg()->SetActive( TRUE );
			}
			else
			{
				CHATMGR->AddMsg(CTC_SYSMSG, CHATMGR->GetChatMsg(325));
			}

			GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
		}
		break;
	// 동맹 마크 등록
	case 4:
		{
			GAMEIN->GetGuildMarkDlg()->ShowGuildUnionMark();

			GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);
		}
		break;
	}
}

void CNpcScriptManager::Business_MapChange(DWORD dwData)  								// 맵 체인지 비지니스.
{
	if(dwData == 0)																		// 데이터가 0과 같으면,
	{
		MSG_DWORD msg;																	// 메시지 구조체를 선언한다.
		msg.Category = MP_NPC;															// 카테고리를 NPC 로 세팅한다.
		msg.Protocol = MP_NPC_DOJOB_SYN;												// 프로토콜을 NPC 직업 시작 요청으로 세팅한다.
		msg.dwObjectID = HEROID;														// HERO 아이디를 세팅한다.
		msg.dwData = m_pCurSelectedNpc->GetID();										// 현재 선택 된 NPC의 아이디를 세팅한다.

		NETWORK->Send(&msg, sizeof(msg));												// 메시지를 전송한다.
	}
	else																				// 데이터가 0이 아니면,
	{
		if(GAMEIN->GetNpcScriptDialog()->IsActive())									// NPC 스크립트 다이얼로그가 열려있으면,
		{
			NPCSCRIPTMGR->SetSelectedNpc(NULL);											// 현재 선택 된 NPC를 해제한다.
			GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);								// NPC 스크립트 다이얼로그를 닫는다.
		}
		
		if( HERO->GetState() == eObjectState_Deal )										// HERO 상태가 거래중이면,
		{
			OBJECTSTATEMGR->EndObjectState(HERO, eObjectState_Deal);					// 거래 상태를 끝낸다.
		}
	}
}

void CNpcScriptManager::Business_Family(DWORD dwData)  									// 패밀리 비지니스.
{
	if( dwData == 0 )																	// 패밀리를 생성하려고 하면,
	{
		if(GAMEIN->GetNpcScriptDialog()->IsActive())									// NPC스크립트 다이얼로가 열려있으면,
		{
			NPCSCRIPTMGR->SetSelectedNpc(NULL);											// 선택 된 NPC를 해제한다.
			GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);								// NPC 스크립트 다이얼로그를 닫는다.
		}

		cDialog* const familyCreateDialog = WINDOWMGR->GetWindowForID(FD_CREATEDLG);

		if(familyCreateDialog)
		{
			familyCreateDialog->SetActive(TRUE);
		}
	}
	else if( dwData == 1 )																// 패밀리를 해체하려고 하면,
	{
		if(GAMEIN->GetNpcScriptDialog()->IsActive())									// NPC 스크립트가 열려 있다면,
		{
			NPCSCRIPTMGR->SetSelectedNpc(NULL);											// 선택 된 NPC를 해제 한다.
			GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);								// NPC 스크립트를 닫는다.
		}				

   		WINDOWMGR->MsgBox( MBI_FAMILY_BREAKUP, MBT_YESNO, CHATMGR->GetChatMsg( 1144 ), int((CSHFamilyManager::LEAVE_PENALTY_EXP_DOWN_RATE[CSHFamilyManager::FLK_BREAKUP]+0.001)*100), CSHFamilyManager::LEAVE_PENALTY_REJOIN_TIME[CSHFamilyManager::FLK_BREAKUP] );
	}
	else if(dwData == 2 )																// 주민등록을 발급 받으려고 하면,
	{
		const DATE_MATCHING_INFO& dateInfo = HERO->GetDateMatchingInfo() ;				// 데이트 매칭 정보를 받는다.

		if(dateInfo.bIsValid)
		{
			WINDOWMGR->MsgBox( MBI_IDENTIFICATION_ALREADY_HAVE, MBT_OK, CHATMGR->GetChatMsg( 830 ) );

			if(cNpcScriptDialog* pNpcDlg = GAMEIN->GetNpcScriptDialog())
			{
				pNpcDlg->SetActive(FALSE) ;
			}
		}
		else
		{
			if(CIdentification* pIdentificationDlg = GAMEIN->GetIdentificationDlg())
			{
				pIdentificationDlg->SetActive(TRUE);
			}

			if(CFavorIcon* pFavorIconDlg = GAMEIN->GetFavorIconDlg())
			{
				pFavorIconDlg->SetActiveRecursive(
					TRUE);
			}
		}
	}
}

void CNpcScriptManager::Business_Identifycation()  										// 주민등록 비지니스.
{
	const DATE_MATCHING_INFO& dateInfo = HERO->GetDateMatchingInfo() ;					// 데이트 매칭 정보를 받는다.

	if( dateInfo.bIsValid )															// 이미 발급 받았다면,
	{
		WINDOWMGR->MsgBox( MBI_IDENTIFICATION_ALREADY_HAVE, MBT_OK, CHATMGR->GetChatMsg( 830 ) );

		if(cNpcScriptDialog* pNpcDlg = GAMEIN->GetNpcScriptDialog())
		{
			pNpcDlg->SetActive(FALSE);
		}
	}
	else
	{
		if(CIdentification* pIdentificationDlg = GAMEIN->GetIdentificationDlg())
		{
			pIdentificationDlg->SetActive(TRUE);
		}


		if(CFavorIcon* pFavorIconDlg = GAMEIN->GetFavorIconDlg())
		{
			pFavorIconDlg->SetActiveRecursive(TRUE);
		}
	}	
}

// 하우징 비지니스.
void CNpcScriptManager::Business_Housing(DWORD dwData) 														
{
	// 데이터타입에 맞게 처리하자 
	switch( dwData )
	{
	case 0:														// 거래하기 	
		{
			// 091007 pdy 하우스 관리인도 거래하기가 가능하게끔 추가
			MSG_WORD msg;																		// 메시지 구조체를 선언한다.
			msg.Category = MP_ITEM;																// 카테고리를 아이템으로 세팅한다.
			msg.Protocol = MP_ITEM_DEALER_SYN;													// 프로토콜을 아이템 딜러 요청으로 세팅한다.
			msg.dwObjectID = HEROID;															// HERO의 아이디를 세팅한다.
			msg.wData = m_pCurSelectedNpc->GetNpcUniqueIdx();									// NPC 유니크 아이디를 세팅한다.

			NETWORK->Send(&msg, sizeof(msg));													// 메시지를 전송한다.

			GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);										// 스크립트 다이얼로그를 비활성화 한다.
		}
		break;
	case 1:													 // 집을 생성하려고 한다면,												
		{
			const DATE_MATCHING_INFO& dateInfo = HERO->GetDateMatchingInfo() ;

			//090527 pdy 하우징 시스템메세지 주민등록증 미보유시 [집 생성]
			if(! dateInfo.bIsValid )
			{
				CHATMGR->AddMsg( CTC_SYSMSG, CHATMGR->GetChatMsg( 1859 ) );	//1859	"주민등록증을 발급 받지 않으셨습니다."
				GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);	
				return;
			}

			//090527 pdy 하우징 시스템메세지 탑승상태시 제한행동 [모션]
			{
				cDialog* const houseNameDialog = WINDOWMGR->GetWindowForID(HOUSE_NAMEDLG);

				if(houseNameDialog)
				{
					houseNameDialog->SetActive(TRUE);
				}
			}

			GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);	

		}
		break;
	case 2:													// 내집을 방문하고 싶다면																			
		{
			//090604 pdy 하우징 Static Npc를 통해 내집링크입장 추가 
			CNpc* pNpc = NPCSCRIPTMGR->GetSelectedNpc();

			if(pNpc) 
			{
				//090604 pdy 하우징 팝업창 [StaticNpc 내집링크입장]
				cMsgBox* pBox = WINDOWMGR->MsgBox( MBI_HOUSE_VISIT_LINK_MYHOME_AREYOUSURE, MBT_YESNO, CHATMGR->GetChatMsg(1897) );	//1897	"집으로 이동하시겠습니까?"
				pBox->SetParam( pNpc->GetID() );
				OBJECTSTATEMGR->StartObjectState(HERO, eObjectState_Deal) ;
			}
	
			GAMEIN->GetNpcScriptDialog()->SetActive(FALSE);	
		}
		break;
	}
}