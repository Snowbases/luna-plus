// GameResourceManager.cpp: implementation of the CGameResourceManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
// 090102 LUJ, SkillScript.bin을 파싱하기 위해 참조
#include <bitset>
#include <limits>
#include "GameResourceManager.h"
#include "MHFile.h"
#ifdef _CLIENT_
#include "./Audio/MHAudioManager.h"
#include "../[cc]skill/client/manager/skillmanager.h"
#include "CharSelect.h"
#include "../[cc]skill/client/info/ActiveSkillInfo.h"
#include "./FadeDlg.h"
#include "./GameIn.h"
#include "DungeonMgr.h"
#else
#include "../[cc]skill/server/manager/skillmanager.h"
#include "../[CC]Skill/Server/Object/ActiveSkillObject.h"
#include "ItemDrop.h"
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
GLOBALTON(CGameResourceManager);

BOOL g_bTESTSERVER = FALSE;
extern int g_nServerSetNum;


CGameResourceManager::CGameResourceManager()
{
	m_bMonsterListLoad = FALSE;
	m_bMonsterDropItem = FALSE;
	m_SMonsterListTable.Initialize(100);
	m_MonsterRewardItem.Initialize(20);

	// 081203 LUJ, 스킬 스크립트를 읽는다
	LoadSkillScript( mSkillScriptMap );
	// 081231 LUJ, 세트 스크립트를 읽는다
	LoadSetScript( mSetScriptMap );
	// 090129 LUJ, 합성 스크립트를 읽는다
	LoadComposeScript( mItemComposeScriptMap );
	// 090114 LUJ, 맵 이동 스크롤 스크립트를 읽는다
	// 090205 LUJ, 누락 코드 추가
	LoadMovableMapWithScroll( mMovableMapSet );
	// 090114 LUJ, 구매 아이템 목록을 읽는다
	// 090205 LUJ, 누락 코드 추가
	LoadBuyItem( mBuyItemSet );
	// 090316 LUJ, 탈것 스크립트를 읽는다
	mVehicleScriptHashTable.Initialize( 10 );
	LoadVehicleScript();
	LoadEnchantScript();
	LoadDropOptionScript();
	LoadApplyOptionScript();
	LoadReinforceScript();

	m_FunitureList.Initialize(2000);
	m_HouseBonusList.Initialize( 200 );
	m_HouseActionGroupList.Initialize( 200 );
	m_HouseActionList.Initialize( 400 );
	m_DungeonKeyList.Initialize(300);
	// 090227 ShinJS --- 이동NPC 정보 테이블 초기화
	m_htMoveNpcStaticInfo.Initialize( MAX_MOVENPC_NUM );

	m_StaticNpcTable.Initialize( 300 );
	m_WareNpcTable.Initialize( 20 );

#ifdef _CLIENT_RESOURCE_FIELD_
	m_PreLoadDataTable.Initialize(4);	
	m_PreLoadEffectTable.Initialize(25);

	m_PreLoadItemlist = NULL;

	// 090227 ShinJS --- 클라이언트인 경우 StaticNpc.bin의 이동NPC 정보를 불러온다
	LoadMoveNPCInfo();
#endif	

#ifdef _SERVER_RESOURCE_FIELD_
	m_HideNpcTable.Initialize(3);

//---KES 스킬초기화돈	071207
	m_htSkillMoney.Initialize(100);
#endif

	// 100225 ShinJS --- 캐릭터들의 초기 스탯 정보를 가져온다.
	LoadCharBaseStatusInfo();
}

CGameResourceManager::~CGameResourceManager()
{
	SAFE_DELETE_ARRAY(m_NpcChxList);	

	ResetMonsterList();

	stMonsterEventRewardItemInfo* pRewardInfo;
	m_MonsterRewardItem.SetPositionHead();
	while((pRewardInfo = m_MonsterRewardItem.GetData())!= NULL)
	{
		if(pRewardInfo)
		{
			delete pRewardInfo;
			pRewardInfo = NULL;
		}
	}

#ifdef _CLIENT_RESOURCE_FIELD_
	DeletePreLoadData();
#endif

	ReleaseStaticNpc();

#ifdef _SERVER_RESOURCE_FIELD_
	HIDE_NPC_INFO* info = NULL;
	m_HideNpcTable.SetPositionHead();
	while((info = m_HideNpcTable.GetData())!= NULL)
	{
		if(info)
		{
			delete info;
			info = NULL;
		}
	}
	m_HideNpcTable.RemoveAll();

//---KES 스킬초기화돈	071207
	m_htSkillMoney.SetPositionHead();
	SKILL_MONEY* pSkillMoney = NULL ;
	while( (pSkillMoney = m_htSkillMoney.GetData())!= NULL )
	{
		delete pSkillMoney;
	}
	m_htSkillMoney.RemoveAll();
//----------------------------

#endif
	//090318 pdy Housing Script작업 추가
	ReleaseHousing_AllList();

	// 090227 ShinJS --- 이동NPC 정보 제거(클라이언트에서만 delete한다. 서버에서는 m_StaticNpcTable과 같은 정보를 공유하기때문에 delete할 필요가 없다)
#ifdef _CLIENT_RESOURCE_FIELD_
	m_htMoveNpcStaticInfo.SetPositionHead();
	while(STATIC_NPCINFO* pData = m_htMoveNpcStaticInfo.GetData())
	{
		delete pData;
		pData = NULL;
	}

	for(size_t race = 0; race < RaceType_Max; ++race)
	{
		for(size_t gender = 0; gender < GENDER_MAX; ++gender)
		{
			SAFE_DELETE_ARRAY(
				m_ModFileList[race][gender].ModFile);
			SAFE_DELETE_ARRAY(
				m_HairModList[race][gender].ModFile);
			SAFE_DELETE_ARRAY(
				m_FaceModList[race][gender].ModFile);
		}
	}

	SAFE_DELETE_ARRAY(
		mPetModList.ModFile);
#endif
	m_htMoveNpcStaticInfo.RemoveAll();
	ReleaseDungeonKeyList();
}

BOOL CGameResourceManager::LoadDeveloperInfo()
{	
	CMHFile file;
	
	if(!file.Init(
			"../DeveloperWorking/Developer.ini", 
			"rt", 
			MHFILE_FLAG_DONOTDISPLAY_NOTFOUNDERROR ) )
	{
		return FALSE;
	}
		
	char value[256];

	while( ! file.IsEOF() )
	{
		file.GetString( value );
		
		if( ! strcmpi(value,"*WINDOWMODE") )
		{
#ifdef _CLIENT_
			m_GameDesc.dispInfo.dispType = file	.GetBool() ? WINDOW_WITH_BLT : FULLSCREEN_WITH_BLT;
#endif
		}
		if( ! strcmpi(value,"*ASSERTMSGBOX") )
		{
			g_bAssertMsgBox = file.GetBool();
		}
		else if( ! strcmpi(value,"*TESTSERVER") )
		{
			g_bTESTSERVER = file.GetBool();
		}
	}

	return TRUE;
}

void CGameResourceManager::ResetMonsterList()
{
	BASE_MONSTER_LIST * pInfo = NULL;
	m_SMonsterListTable.SetPositionHead();
	while((pInfo = m_SMonsterListTable.GetData())!= NULL)
	{
		SAFE_DELETE(pInfo);
	}
	m_SMonsterListTable.RemoveAll();
}

void CGameResourceManager::LoadMonsterRewardList()
{
	CMHFile file;	

	if(!file.Init("System/Resource/MonsterReward.bin", "rb"))
	{
		ASSERTMSG(0, "Not found MonsterReward.bin file!");
		return ;
	}

	//WORD MonsterKind;
	while(1)
	{
		if(file.IsEOF() != FALSE)
			break;
		
		stMonsterEventRewardItemInfo* pInfo = new stMonsterEventRewardItemInfo;
		pInfo->wMonsterType = file.GetWord();
		pInfo->dwItemIndex = file.GetDword();

		m_MonsterRewardItem.Add(pInfo, pInfo->wMonsterType);
	}

	file.Release();
}

DWORD CGameResourceManager::GetMonsterEventRewardItem(WORD MonsterKind)
{
	stMonsterEventRewardItemInfo* pInfo = m_MonsterRewardItem.GetData(MonsterKind);
	if(pInfo)
		return pInfo->dwItemIndex;

	return 0;
}

BASE_MONSTER_LIST* CGameResourceManager::GetMonsterListInfo(int idx)
{
	return m_SMonsterListTable.GetData(idx);
}

int CGameResourceManager::GetMonsterIndexForName( char* pStrName )
{
	m_SMonsterListTable.SetPositionHead();
	BASE_MONSTER_LIST* pList;
	while( (pList = m_SMonsterListTable.GetData() )!= NULL)
	{
		if( strcmp( pList->Name, pStrName ) == 0 )
			return pList->MonsterKind;
	}

	return 0;
}
BOOL CGameResourceManager::LoadMonsterList()
{
	char filename[64];
	//	char value[64] = {0,};
	CMHFile file;

	sprintf(filename,"./System/Resource/MonsterList.bin");	
	if(!file.Init(filename, "rb"))
		return FALSE;

	while(FALSE == file.IsEOF())
	{
		BASE_MONSTER_LIST* pList = new BASE_MONSTER_LIST;
		ZeroMemory(
			pList,
			sizeof(*pList));

		pList->MonsterKind				= file.GetWord();		// 번호
		SafeStrCpy( pList->Name, file.GetString(), MAX_MONSTER_NAME_LENGTH+1 );	// 이름
		file.GetString();// 영문명
		SafeStrCpy( pList->ChxName, file.GetString(), 32 );	// CHX 파일

		pList->Damage1DramaNum			= FindEffectNum(file.GetString());// 맞음 1 연출 번호	
		pList->Die1DramaNum				= FindEffectNum(file.GetString());// 죽음 1 연출 번호	
		pList->Die2DramaNum				= FindEffectNum(file.GetString());// 죽음 2 연출 번호	

		pList->SpecialType				= file.GetWord();
		pList->ObjectKind				= EObjectKind(file.GetWord());
		pList->dwMonsterTargetType		= file.GetDword();
		pList->MonsterType				= file.GetWord();
		pList->MonsterAttribute			= file.GetWord();
		pList->MonsterRace				= file.GetWord();
		pList->Level					= (LEVELTYPE)file.GetLevel();
		pList->ExpPoint					= file.GetDword();
		pList->Life						= file.GetDword();
		pList->BaseReinforce			= file.GetByte();
		pList->Scale					= file.GetFloat();		// 크기	
		pList->Weight					= file.GetFloat();		// 무게
		pList->Gravity					= file.GetDword();		// 비중
		pList->MonsterRadius			= file.GetWord();		// 반지름 	
		pList->Tall						= file.GetWord();		// 키(이름출력 위치)

		pList->Accuracy					= file.GetWord();
		pList->Avoid					= file.GetWord();

		pList->AttackPhysicalMax		= file.GetWord();// 공격 최대물리 공격력	
		pList->AttackPhysicalMin		= ( WORD )( pList->AttackPhysicalMax * 0.95f );// 공격 최소물리 공격력	
		pList->PhysicAttackReinforce	= file.GetByte();
		pList->AttackMagicalMax			= file.GetWord();// 공격 최대물리 공격력	
		pList->AttackMagicalMin			= ( WORD )( pList->AttackMagicalMax * 0.95f );// 공격 최소물리 공격력	
		pList->MagicAttackReinforce		= file.GetByte();
		pList->CriticalPercent			= file.GetWord();// 크리티컬 퍼센트
		file.GetFloat();
		pList->PhysicalDefense			= file.GetWord();// 물리 방어력	
		pList->PhysicDefenseReinforce	= file.GetByte();
		pList->MagicalDefense			= file.GetWord();// 물리 방어력	
		pList->MagicDefenseReinforce	= file.GetByte();
		pList->WalkMove					= file.GetWord();// 몬스터걷기이동력	
		pList->RunMove					= file.GetWord();// 몬스터뛰기이동력	
		pList->RunawayMove				= pList->RunMove;// 몬스터도망이동력


		pList->StandTime				= file.GetDword();
		pList->StandRate				= file.GetByte();
		pList->WalkRate					= file.GetByte();
		pList->RunRate					= 0;
		pList->DomainRange				= file.GetDword();
		// 100104 LUJ, 몬스터 이동 중에 이 값이 최소 1이 되도록 항상 검사했음. 스크립트로 이동시킴
		pList->DomainRange = max(1, pList->DomainRange);
		pList->PursuitForgiveTime		= file.GetDword();
		pList->PursuitForgiveDistance	= file.GetDword();

		pList->bForeAttack				= file.GetBool();					// 선공격
		pList->bDetectSameRace			= file.GetBool();
		pList->mHelpType				= BASE_MONSTER_LIST::HelpType(file.GetInt());
		pList->bHearing					= file.GetBool();
		pList->HearingDistance			= file.GetDword();

		pList->SearchPeriodicTime		= file.GetDword();				// 탐색 주기
		pList->TargetSelect				= file.GetWord();					// 타겟 선택 : FIND_CLOSE, FIND_FIRST
		pList->SearchRange				= file.GetWord();
		pList->SearchAngle				= file.GetByte();
		pList->TargetChange				= file.GetWord();
		pList->AttackNum				= file.GetWord();
		
		for( WORD  cnt = 0; cnt < 5; cnt++ )
		{
			pList->AttackIndex[cnt]	= file.GetDword();
			pList->SkillInfo[cnt] = (cActiveSkillInfo*)SKILLMGR->GetSkillInfo(pList->AttackIndex[cnt]);
		}
		for( cnt = 0; cnt < 5; cnt++ )
		{
			pList->AttackRate[cnt]		= file.GetWord();
		}

		// 070423 LYW --- GameResourceManager : Applay drop item.
		pList->dropMoney		= file.GetDword();
		pList->dwMoneyDropRate	= file.GetDword();

		for( BYTE count = 0 ; count < eDROPITEMKIND_MAX-1 ; ++count )
		{
			file.GetString();
			pList->dropItem[count].wItemIdx = file.GetDword();
			pList->dropItem[count].byCount  = file.GetByte() ;
			pList->dropItem[count].dwDropPercent = file.GetDword() ;
		}

		m_SMonsterListTable.Add(pList, pList->MonsterKind);
	}

	return TRUE;
}

BOOL CGameResourceManager::LoadNpcList()
{
	CMHFile fp;
	fp.Init("System/Resource/NpcList.bin", "rb");
	size_t idx = 0;
	
	while(FALSE == fp.IsEOF())
	{
		if(_countof(m_NpcList) <= idx)
		{
			break;
		}

		NPC_LIST& npcList = m_NpcList[idx++];
		npcList.NpcKind = fp.GetWord();
		SafeStrCpy(
			npcList.Name,
			fp.GetString(),
			_countof(npcList.Name));
		npcList.ModelNum = fp.GetWord();
		npcList.JobKind = fp.GetWord();
		npcList.Scale = fp.GetFloat();
		npcList.Tall = fp.GetWord();
		npcList.ShowJob = fp.GetBool();
	}

	return TRUE;
}

BOOL CGameResourceManager::LoadStaticNpc()
{
	CMHFile file;
	char filename[64];
	sprintf( filename, "./System/Resource/StaticNpc.bin" );	
	if( !file.Init( filename, "rb" ) )
		return FALSE;
	STATIC_NPCINFO Info;
	while( 1 )
	{
		if( file.IsEOF() )
			break;

		Info.MapNum = file.GetWord();
		Info.wNpcJob = file.GetWord();
		//file.GetString( Info.NpcName );
		SafeStrCpy( Info.NpcName, file.GetString(), MAX_NAME_LENGTH+1 );
		Info.wNpcUniqueIdx = file.GetWord();
		Info.vPos.x = file.GetFloat();
		Info.vPos.y = 0.0f;
		Info.vPos.z = file.GetFloat();
		file.GetFloat();
		file.GetByte();

#ifdef _SERVER_RESOURCE_FIELD_
		if( m_LoadingMapNum != Info.MapNum )
			continue;
#endif

		STATIC_NPCINFO* pInfo = new STATIC_NPCINFO;
		*pInfo = Info;


		m_StaticNpcTable.Add( pInfo, pInfo->wNpcUniqueIdx );

		if( pInfo->wNpcJob == 8 )
			m_WareNpcTable.Add( pInfo, pInfo->wNpcUniqueIdx );
	}

	file.Release();

	sprintf( filename, "./system/Resource/QuestNpcList.bin" );	
	if( !file.Init( filename, "rb" ) )
		return FALSE;

	while( 1 )
	{
		if( file.IsEOF() )
			break;

		Info.MapNum = file.GetWord();
		Info.wNpcJob = file.GetWord();
		//		file.GetString( Info.NpcName );
		SafeStrCpy( Info.NpcName, file.GetString(), MAX_NAME_LENGTH+1 );
		Info.wNpcUniqueIdx = file.GetWord();
		Info.vPos.x = file.GetFloat();
		Info.vPos.y = 0.0f;
		Info.vPos.z = file.GetFloat();
		file.GetFloat();

#ifdef _SERVER_RESOURCE_FIELD_
		if( m_LoadingMapNum != Info.MapNum )
			continue;
#endif

		STATIC_NPCINFO* pInfo = new STATIC_NPCINFO;
		*pInfo = Info;
		m_StaticNpcTable.Add( pInfo, pInfo->wNpcUniqueIdx );
	}

	file.Release();

	return TRUE;
}

void CGameResourceManager::ReleaseStaticNpc()
{
	STATIC_NPCINFO* pData = NULL;
	m_StaticNpcTable.SetPositionHead();
	while( (pData = m_StaticNpcTable.GetData() )!= NULL)
		delete pData;
	m_StaticNpcTable.RemoveAll();
}

STATIC_NPCINFO* CGameResourceManager::GetStaticNpcInfo( WORD wNpcUniqueIdx )
{
	return m_StaticNpcTable.GetData( wNpcUniqueIdx );	
}

#ifdef _SERVER_RESOURCE_FIELD_
NPC_LIST* CGameResourceManager::GetNpcList(WORD JobKind)
{
	for(int i=0; i<MAX_NPC_NUM; ++i)
	{
		if(m_NpcList[i].NpcKind == JobKind)
			return &m_NpcList[i];
	}
	return NULL;
}
#endif


//060608 - Add by wonju
// 071214 LUJ, 코드 정리
BOOL CGameResourceManager::LoadMapChangeArea()
{
	CMHFile file;

	if( ! file.Init( "System/Resource/MapMoveList.bin", "rb" ) )
	{
		return FALSE;
	}

	for( int i = 0; ! file.IsEOF(); ++i  )
	{
		MAPCHANGE_INFO& info = m_MapChangeArea[ i ];

		info.Kind			= file.GetWord();
		SafeStrCpy( info.CurMapName, file.GetString(), sizeof( info.CurMapName ) );
		SafeStrCpy( info.ObjectName, file.GetString(), sizeof( info.ObjectName ) );

		info.CurMapNum		= file.GetWord();
		info.MoveMapNum		= file.GetWord();
		info.CurPoint.x		= file.GetFloat();
		info.CurPoint.y		= 0;
		info.CurPoint.z		= file.GetFloat();
		info.MovePoint.x	= file.GetFloat();
		info.MovePoint.y	= 0;
		info.MovePoint.z	= file.GetFloat();
		info.chx_num		= file.GetWord();
	}

	return TRUE;

}


// 071214 LUJ, 코드 정리
BOOL CGameResourceManager::LoadMapChangeArea(MAPCHANGE_INFO* pMapChangeArea)
{
	CMHFile file;

	if( ! file.Init( "System/Resource/MapMoveList.bin", "rb" ) )
	{
		return FALSE;
	}

	for( ; ! file.IsEOF(); ++pMapChangeArea  )
	{
		MAPCHANGE_INFO& info = *pMapChangeArea;

		info.Kind			= file.GetWord();
		SafeStrCpy( info.CurMapName, file.GetString(), sizeof( info.CurMapName ) );
		SafeStrCpy( info.ObjectName, file.GetString(), sizeof( info.ObjectName ) );

		info.CurMapNum		= file.GetWord();
		info.MoveMapNum		= file.GetWord();
		info.CurPoint.x		= file.GetFloat();
		info.CurPoint.y		= 0;
		info.CurPoint.z		= file.GetFloat();
		info.MovePoint.x	= file.GetFloat();
		info.MovePoint.y	= 0;
		info.MovePoint.z	= file.GetFloat();
		info.chx_num		= file.GetWord();
	}

	return TRUE;
}


// 070121 LYW --- Add function to check potal position.
WORD CGameResourceManager::CheckMapChange( VECTOR3* pVector, WORD curMapNum )
{
	int count = 0 ;

	float fDistance  ;
	
	for( count = 0 ; count < MAX_POINT_NUM ; ++count )
	{

		fDistance = CalcDistanceXZ( pVector, &( m_MapChangeArea[count].CurPoint ) ) ;

		// 070206 LYW --- Modified this line.
		//if( fDistance <= POTAL_RAD )
		if( (fDistance <= POTAL_RAD) && (m_MapChangeArea[ count ].CurMapNum == curMapNum ) )
		{
			// 080930 LYW --- GameResourceManager : 같은 맵 내에서 페이드 아웃 연출을 하며 맵이동을 하는 처리 추가.
#ifdef _CLIENT_
			if( m_MapChangeArea[ count ].chx_num == 91 )
			{
				CFadeDlg* pDlg = NULL ;
				pDlg = GAMEIN->GetFadeDlg() ;

				if( pDlg )
				{
					pDlg->FadeOut() ;

					MOVE_POS msg ;
  					msg.Category = MP_MOVE ; 
  					msg.Protocol = MP_FADEMOVE_SYN ;
  					msg.dwObjectID = HEROID ;
  					msg.dwMoverID = HEROID ;
					msg.cpos.Compress(&m_MapChangeArea[ count ].MovePoint) ;

  					NETWORK->Send( &msg, sizeof( MOVE_POS ) ) ;
				}

				return 65535 ;
			}
#endif //_CLIENT_
			return m_MapChangeArea[ count ].Kind ;
		}
	}

	return 65535 ;
}

BOOL CGameResourceManager::LoadLoginPoint(LOGINPOINT_INFO* pLogin)
{
	char filename[64];
	CMHFile file;
	sprintf(filename,"System/Resource/LoginPoint.bin");	
	if(!file.Init(filename, "rb"))
		return FALSE;

	int n =0;
	while(1)
	{
		if(file.IsEOF())
			break;
		pLogin[n].Kind = file.GetWord();
		//file.GetString(pLogin[n].MapName);
		SafeStrCpy( pLogin[n].MapName, file.GetString(), MAX_NAME_LENGTH+1 );
		pLogin[n].MapNum = file.GetWord();
		pLogin[n].MapLoginPointNum = file.GetByte();
		if(pLogin[n].MapLoginPointNum == 0)
		{
			MessageBox(NULL,"LoginPointNum is Zero!!!",0,0);
		}
		if(pLogin[n].MapLoginPointNum > 10)
		{
			MessageBox(NULL,"LoginPointNum is too big!!!",0,0);
		}
		for(int i=0;i<pLogin[n].MapLoginPointNum;++i)
		{
			pLogin[n].CurPoint[i].x = file.GetFloat();
			pLogin[n].CurPoint[i].y = 0;
			pLogin[n].CurPoint[i].z = file.GetFloat();
		}
		pLogin[n].chx_num = file.GetWord();
		++n;
	}

	return TRUE;
}

// 081203 LUJ, 스킬 스크립트를 읽는다
void CGameResourceManager::LoadSkillScript( SkillScriptMap& scriptMap )
{
	// 081203 LUJ, 키워드에 따라 속성값을 반환한다
	class 
	{
	public:
		SkillScript::Type operator()( const TCHAR* keyword ) const
		{
			if( ! keyword )
			{
				return SkillScript::TypeNone;
			}
			else if( ! _tcsicmp( keyword, _T( "strength" ) ) )
			{
				return SkillScript::TypeStrength;
			}
			else if( ! _tcsicmp( keyword, _T( "dexterity" ) ) )
			{
				return SkillScript::TypeDexterity;
			}
			else if( ! _tcsicmp( keyword, _T( "vitality" ) ) )
			{
				return SkillScript::TypeVitality;
			}
			else if( ! _tcsicmp( keyword, _T( "intelligence" ) ) )
			{
				return SkillScript::TypeIntelligence;
			}
			else if( ! _tcsicmp( keyword, _T( "wisdom" ) ) )
			{
				return SkillScript::TypeWisdom;
			}
			else if( ! _tcsicmp( keyword, _T( "physic_attack" ) ) )
			{
				return SkillScript::TypePhysicAttack;
			}
			else if( ! _tcsicmp( keyword, _T( "magic_attack" ) ) )
			{
				return SkillScript::TypeMagicAttack;
			}
			else if( ! _tcsicmp( keyword, _T( "physic_defense" ) ) )
			{
				return SkillScript::TypePhysicDefence;
			}
			else if( ! _tcsicmp( keyword, _T( "magic_defense" ) ) )
			{
				return SkillScript::TypeMagicDefence;
			}
			else if( ! _tcsicmp( keyword, _T( "accuracy" ) ) )
			{
				return SkillScript::TypeAccuracy;
			}
			else if( ! _tcsicmp( keyword, _T( "evade" ) ) )
			{
				return SkillScript::TypeEvade;
			}
			else if( ! _tcsicmp( keyword, _T( "critical_rate" ) ) )
			{
				return SkillScript::TypeCriticalRate;
			}
			else if( ! _tcsicmp( keyword, _T( "critical_damage" ) ) )
			{
				return SkillScript::TypeCriticalDamage;
			}
			else if( ! _tcsicmp( keyword, _T( "move_speed" ) ) )
			{
				return SkillScript::TypeMoveSpeed;
			}

			return SkillScript::TypeNone;
		}
	}
	GetValueType;

	// 081203 LUJ, 키워드에 따라 속성값을 반환한다
	class 
	{
	public:
		eStatusKind operator()( const TCHAR* keyword ) const
		{
			if( ! keyword )
			{
				return eStatusKind_None;
			}
			else if( ! _tcsicmp( keyword, _T( "poison" ) ) )
			{
				return eStatusKind_Poison;
			}
			else if( ! _tcsicmp( keyword, _T( "bleed" ) ) )
			{
				return eStatusKind_Bleeding;
			}
			else if( ! _tcsicmp( keyword, _T( "burn" ) ) )
			{
				return eStatusKind_Burning;
			}
			else if( ! _tcsicmp( keyword, _T( "holy_damage" ) ) )
			{
				return eStatusKind_HolyDamage;
			}
			else if( ! _tcsicmp( keyword, _T( "shield" ) ) )
			{
				return eStatusKind_Shield;
			}
			else if( ! _tcsicmp( keyword, _T( "hide" ) ) )
			{
				return eStatusKind_Hide;
			}
			else if( ! _tcsicmp( keyword, _T( "detect" ) ) )
			{
				return eStatusKind_Detect;
			}
			else if( ! _tcsicmp( keyword, _T( "paralysis" ) ) )
			{
				return eStatusKind_Paralysis;
			}
			else if( ! _tcsicmp( keyword, _T( "stun" ) ) )
			{
				return eStatusKind_Stun;
			}
			else if( ! _tcsicmp( keyword, _T( "sleep" ) ) )
			{
				return eStatusKind_Slip;
			}
			else if( ! _tcsicmp( keyword, _T( "freeze" ) ) )
			{
				return eStatusKind_Freezing;
			}
			else if( ! _tcsicmp( keyword, _T( "stone" ) ) )
			{
				return eStatusKind_Stone;
			}
			else if( ! _tcsicmp( keyword, _T( "attract" ) ) )
			{
				return eStatusKind_Attract;
			}
			else if( ! _tcsicmp( keyword, _T( "slience" ) ) )
			{
				return eStatusKind_Silence;
			}
			else if( ! _tcsicmp( keyword, _T( "block_attack" ) ) )
			{
				return eStatusKind_BlockAttack;
			}
			else if( ! _tcsicmp( keyword, _T( "god" ) ) )
			{
				return eStatusKind_God;
			}
			else if( ! _tcsicmp( keyword, _T( "move_stop" ) ) )
			{
				return eStatusKind_MoveStop;
			}
			else if( ! _tcsicmp( keyword, _T( "unable_use_item" ) ) )
			{
				return eStatusKind_UnableUseItem;
			}
			else if( ! _tcsicmp( keyword, _T( "unable_buff" ) ) )
			{
				return eStatusKind_UnableBuff;
			}

			return eStatusKind_None;
		}
	}
	GetStatusKind;

	CMHFile file;
	if( ! file.Init( "System/Resource/skillScript.bin", "rb" ) )
	{
#ifdef _GMTOOL_
		_tcprintf( "skillScript.bin is not found\n" );
#endif
		return;
	}

	enum Block
	{
		BlockNone,
		BlockScript,
		BlockScriptBonus,
		BlockScriptBuff,
		BlockScriptDuration,
		// 090226 LUJ, 검사 여부
		BlockScriptCheck,
		// 090630 ONS 스킬 그룹속성 추가
		BlockScriptSkillGroup,
	}
	blockType = BlockNone;

	int				openCount	= 0;
	int				line		= 0;
	SkillScript*	script		= 0;

	// 081203 LUJ, 메모리 해제 누락의 위험을 피하기 위해 정적 리스트에서 스크립트 저장 영역을 참조한다
	typedef std::list< SkillScript >	ScriptList;
	static ScriptList					scriptList;

	for(	TCHAR text[ MAX_PATH ];
			! file.IsEOF();
			ZeroMemory( text, sizeof( text ) ), ++line )
	{
		file.GetLine( text, sizeof( text ) / sizeof( *text ) );
		const int length = _tcslen( text );

		// 081203 LUJ, 중간의 주석 제거
		for( int i = 0; i < length - 1; ++i )
		{
			const char a = text[ i ];
			const char b = text[ i + 1 ];

			if( '/' == a && '/' == b )
			{
				text[ i ] = 0;
				break;
			}
		}

		TCHAR buffer[ MAX_PATH ]	= { 0 };
		SafeStrCpy( buffer, text, sizeof( text ) / sizeof( *text ) );
		const TCHAR*	separator	= _T( " ~\n\t=,+\"()" );
		const TCHAR*	token		= _tcstok( text, separator );
		const TCHAR*	markBegin	= _T( "{" );
		const TCHAR*	markEnd		= _T( "}" );

		if( ! token )
		{
			continue;
		}
		else if( ! _tcsicmp( token, _T( "skill" ) ) )
		{
			blockType = BlockScript;

			// 090102 LUJ, 여러줄에 걸쳐 스킬 번호를 지정하도록 한번만 스크립트 포인터를 받아온다
			if( ! script )
			{
				scriptList.push_back( SkillScript() );
				script = &( scriptList.back() );
			}

			// 090102 LUJ, 와일드카드(*)가 있는 스킬 번호를 파싱해서 스크립트 맵에 담는다
			for(	const TCHAR* keyword = 0;
					(keyword = _tcstok( 0, separator ))!= NULL; )
			{
				const size_t	length		= _tcslen( keyword );
				int				position	= int( pow( 10.0f, int( length ) - 1 ) );
				int				maxIndex	= 0;
				int				minIndex	= 0;
				// 090102 LUJ, 컴파일러에서 지원하는 int 형의 비트만큼 비트셋을 만든다
				typedef std::bitset< std::numeric_limits< int >::digits > BitSet;
				// 090102 LUJ, 모든 비트를 1로 만든다
				BitSet bitMask( DWORD(~0) );
				BitSet testBitSet( DWORD(~0) );

				// 090102 LUJ, 다음 역할을 루프에서 수행한다
				//	- 최소, 최대값을 얻는다
				//	- 마스크 값을 얻는다. 마스킹을 쉽게 하기 위해 BCD 코드로 변환한다
				//		예) 5*2* -> 0000 1111 0000 1111
				//	- 마스킹할 기본값을 얻는다.  역시 BCD 코드로 변환한다
				//		예) 5*2* -> 0101 1111 0010 1111
				for(	const TCHAR* ch = keyword;
						*ch;
						ch = _tcsinc( ch ), position /= 10 )
				{
					// 090102 LUJ, 왼쪽으로 1 바이트씩 시프트하면서 값을 넣는다
					bitMask	<<= sizeof( int );
					testBitSet <<= sizeof( int );

					// 090102 LUJ, 와일드 카드가 나올 경우 마스크 값을 갱신한다
					if( '*' == *ch )
					{
						maxIndex += 9 * position;

						// 090102 LUJ, 0xf = 1111
						bitMask		|= BitSet( 0xf );
						testBitSet	|= BitSet( 0xf );
					}
					// 090102 LUJ, 숫자가 나올 경우 기본 값을 갱신한다
					else if( isalnum( *ch ) )
					{
						const int digit	= ( *ch - '0' );
						const int value	= digit * position;
						maxIndex += value;
						minIndex += value;

						testBitSet |= BitSet( digit );
					}
				}

				// 090102 LUJ, 최소값~최대값을 순환하면서 마스크 검사를 통과하는 값만 스크립트에 넣는다
				for(	int index = minIndex;
						maxIndex >= index;
						++index )
				{
					BitSet indexBitSet( DWORD(~0) );

					// 090102 LUJ, 숫자를 BCD 코드로 변환한다
					{
						int currentIndex = index;

						for(	int position = int( pow( 10.0f, int( length ) - 1 ) );
								0 < position;
								position /= 10 )
						{
							indexBitSet <<= sizeof( int );
							indexBitSet |= BitSet( currentIndex / position );

							currentIndex %= position;
						}
					}

					// 090102 LUJ, 마스킹 값을 OR 연산한 후 기본 값에 XOR 연산하여
					//			모든 비트가 0이 나와야 마스킹 검사에 적합한 값이다.
					indexBitSet |= bitMask;
					indexBitSet ^= testBitSet;

					if( indexBitSet.none() )
					{
						scriptMap[ index ] = script;
					}
				}
			}
		}
		else if( ! _tcsicmp( token, _T( "bonus" ) ) )
		{
			blockType = BlockScriptBonus;
		}
		else if( ! _tcsicmp( token, _T( "duration" ) ) )
		{
			blockType = BlockScriptDuration;
		}
		else if( ! _tcsicmp( token, _T( "buff" ) ) )
		{
			blockType = BlockScriptBuff;
		}
		else if( ! _tcsicmp( token, _T( "noCheck" ) ) )
		{
			blockType = BlockScriptCheck;
		}
		// 090630 ONS 스킬 그룹속성 추가
		else if( ! _tcsicmp( token, _T( "skill_group" ) ) )
		{
			blockType = BlockScriptSkillGroup;
		}

		else if( ! _tcsnicmp( markBegin, token, strlen( markBegin ) ) )
		{
			++openCount;
		}
		else if( ! _tcsnicmp( markEnd, token, strlen( markEnd ) ) )
		{
			--openCount;

			switch( blockType )
			{
			case BlockScript:
				{
					// 090102 LUJ, 여러 줄에 걸쳐 스킬 번호를 지정하도록 초기화를 스크립트가 끝난 후 한다
					script = 0;

					blockType = BlockNone;
					break;
				}
			case BlockScriptBonus:
			case BlockScriptDuration:
			case BlockScriptBuff:
				// 090226 LUJ, 스크립트 체크
			case BlockScriptCheck:
				// 090630 ONS 스킬그룹속성 추가
			case BlockScriptSkillGroup:
				{
					blockType = BlockScript;
					break;
				}
			default:
				{
#ifdef _GMTOOL_
					_tcprintf( _T( "script is invalid in line %d at skillScript.bin\n" ), line );
#endif
					blockType = BlockNone;
					break;
				}
			}
		}
		// 081203 LUJ, 스크립트 파싱
		else
		{
			switch( blockType )
			{
			case BlockScriptBonus:
				{
                    const TCHAR*	keyword		= _tcstok( buffer,	separator );
					const TCHAR*	textValue	= _tcstok( 0, separator );
					const size_t	size		= strlen( textValue ? textValue : _T( "" ) );
					
					if( 1 > size )
					{
						break;
					}
					
					SkillScript::Value value;
					// 081203 LUJ, %표시가 있으면 퍼센트 변수에, 아니면 플러스 변수에 값을 넣는다
					( '%' == textValue[ size - 1 ] ? value.mPercent : value.mPlus ) = float( _tstof( textValue ? textValue : _T( "" ) ) );

					static const SkillScript::Value	emptyValue;

					if( ! memcmp( &emptyValue, &value, sizeof( emptyValue ) ) )
					{
						break;
					}

					value.mType	= GetValueType( keyword );
					script->mStatusBonusList.push_back( value );
					break;
				}
			case BlockScriptBuff:
				{
					const TCHAR*	keyword	= _tcstok( buffer, separator );
					const TCHAR*	value	= _tcstok( 0, separator );

					SkillScript::Buff buff;
					buff.mKind	= GetStatusKind( keyword );
					buff.mValue	= float( _tstof( value ? value : _T( "" ) ) );
					script->mBuffList.push_back( buff );
					break;
				}
			case BlockScriptDuration:
				{
					const TCHAR*	textValue	= _tcstok( buffer, separator );
					_tcstok( 0, separator );
					const TCHAR*	keyword		= _tcstok( 0, separator );
					const size_t	size		= strlen( textValue );

					if( 1 > size )
					{
						break;
					}

					SkillScript::Value value;
					// 081203 LUJ, %표시가 있으면 퍼센트 변수에, 아니면 플러스 변수에 값을 넣는다
					( '%' == textValue[ size - 1 ] ? value.mPercent : value.mPlus ) = float( _tstof( textValue ? textValue : _T( "" ) ) );

					static const SkillScript::Value	emptyValue;

					if( ! memcmp( &emptyValue, &value, sizeof( emptyValue ) ) )
					{
						break;
					}

					value.mType	= GetValueType( keyword );
					script->mDurationBonusList.push_back( value );
					break;
				}
				// 090226 LUJ, 스크립트 체크 여부
			case BlockScriptCheck:
				{
					const TCHAR* const keyword = _tcstok( buffer, separator );

					if( ! keyword )
					{
						break;
					}
					else if( ! _tcsicmp( keyword, _T( "true" ) ) )
					{
						script->mIsNeedCheck = FALSE;
					}

					break;
				}
				// 090630 ONS 스킬그룹번호 저장
			case BlockScriptSkillGroup:
				{
					static DWORD nGroudCount = 1;
					const TCHAR* const keyword = _tcstok( buffer, separator );

					if( ! keyword )
					{
						break;
					}
					else if( ! _tcsicmp( keyword, _T( "true" ) ) )
					{
						script->mSkillGroupNum = nGroudCount++;
					}

					break;
				}
			}
		}
	}

#ifdef _GMTOOL_
	if( openCount )
	{
		// 090106 LUJ, 괄호가 잘못되어 경고가 나타나는 것을 수정함
		_tcprintf( _T( "OpenCount %d is invalid at skillScript.bin" ), openCount );
	}
#endif
}

// 081203 LUJ, 스킬 번호로 스크립트를 반환한다
const SkillScript& CGameResourceManager::GetSkillScript( DWORD index ) const
{
	const SkillScriptMap::const_iterator it = mSkillScriptMap.find( index );

	static const SkillScript emptyScript;

	if( mSkillScriptMap.end() == it )
	{
		return emptyScript;
	}

	const SkillScript* const script = it->second;

	return script ? *script : emptyScript;
}

// 090114 LUJ, MapMoveScroll.bin에서 이동 가능한 맵 번호를 읽어온다
void CGameResourceManager::LoadMovableMapWithScroll( MovableMapSet& movableMapSet )
{
	CMHFile file;
	file.Init(
		"System/Resource/MapMoveScroll.bin",
		"rb" );

	while( ! file.IsEOF() )
	{
		const MAPTYPE mapType = file.GetWord();

		movableMapSet.insert( mapType );
	}
}

// 090114 LUJ, 맵 이동 스크롤로 이동 가능한 맵인지 반환한다
BOOL CGameResourceManager::IsMovableMapWithScroll( MAPTYPE mapType ) const
{
	const MovableMapSet::const_iterator iterator = mMovableMapSet.find( mapType );

	return mMovableMapSet.end() != iterator;
}

// 090114 LUJ, 구매 아이템 정보를 읽는다
void CGameResourceManager::LoadBuyItem( BuyItemSet& buyItemSet )
{
	CMHFile file;
	file.Init( 
		"System/Resource/BRList.bin",
		"rb" );

	while( ! file.IsEOF() )
	{
		file.GetWord();
		file.GetBool();
		const DWORD	itemIndex = file.GetDword();

		buyItemSet.insert( itemIndex );
	}
}

// 090114 LUJ, 구매 아이템 여부를 반환한다
BOOL CGameResourceManager::IsBuyItem( DWORD itemIndex ) const
{
	const BuyItemSet::const_iterator it = mBuyItemSet.find( itemIndex );

	return mBuyItemSet.end() != it;
}

// 080916 LUJ, 합성 스크립트를 읽는다
void CGameResourceManager::LoadComposeScript( ItemComposeScriptMap& itemComposeScriptMap )
{
	enum State
	{
		StateNone,
		StateCompose,
		StateResult,
		StateOption,
	}
	state = StateNone;

	CMHFile file;
	file.Init( "system/resource/ItemCompose.bin", "rb" );

	char line[ MAX_PATH * 5 ] = { 0 };

	int				openCount	= 0;
	ComposeScript*	script		= 0;

	while( ! file.IsEOF() )
	{
		file.GetLine( line, sizeof( line ) );

		const int length = strlen( line );

		// 080916 LUJ, 중간의 주석 제거
		for( int i = 0; i < length - 1; ++i )
		{
			const char a = line[ i ];
			const char b = line[ i + 1 ];

			if( '/' == a &&
				'/' == b )
			{
				line[ i ] = 0;
				break;
			}
		}

		const char* separator	= " \n\t=,+\"~()";
		const char* token		= strtok( line, separator );
		const char* markBegin	= "{";
		const char* markEnd		= "}";

		if( ! token )
		{
			continue;
		}
		else if( ! strnicmp( markBegin, token, strlen( markBegin ) ) )
		{
			++openCount;
		}
		else if( ! strnicmp( markEnd, token, strlen( markEnd ) ) )
		{
			--openCount;

			switch( state )
			{
			case StateCompose:
				{
					script	= 0;
					state	= StateNone;
					break;
				}
			case StateResult:
				{
					state	= StateCompose;
					break;
				}
			case StateOption:
				{
					state	= StateCompose;
					break;
				}
			}
		}
		// 080916 LUJ, 스크립트가 저장될 메모리 공간을 확보하고, 합성될 아이템 대상을 읽어들인다.
		else if( ! stricmp( "compose", token ) )
		{
			if( openCount )
			{
				// 080916 LUJ, 스크립트가 저장될 메모리 주소를 알지 못한채 파싱을 시작하려함. 치명적 오류
				continue;
			}
			else if( ! script )
			{
				mComposeScriptList.push_back( ComposeScript() );
				script = &( mComposeScriptList.back() );
			}

			// 080916 LUJ, 합성될 아이템 별로 스크립트를 연결시킨다
			while(( token = strtok( 0, separator ) )!= NULL)
			{
				itemComposeScriptMap[ atoi( token ? token : "" ) ] = script;
			}
		}
		// 080916 LUJ, 이 예약어로 지정된 아이템을 사용해야 합성 창이 표시된다
		else if( ! stricmp( "keyItem", token ) )
		{
			token					= strtok( 0, separator );
			script->mKeyItemIndex	= atoi( token ? token : "" );

			// 080916 LUJ, 키 아이템으로 스크립트를 꺼낼 수 있도록 컨테이너에 넣는다
			mKeyItemComposeScriptMap[ script->mKeyItemIndex ].insert( script );
		}
		// 080916 LUJ, 소요되는 아이템 개수
		else if( ! stricmp( "needSize", token ) )
		{
			token				= strtok( 0, separator );
			script->mSourceSize	= atoi( token ? token : "" );
		}
		// 080916 LUJ, 결과 키워드
		else if( ! stricmp( "result", token ) )
		{
			state = StateResult;
		}
		else if( ! stricmp( "item", token ) )
		{
			const char* textItemIndex	= strtok( 0, separator );
			const char* textRate		= strtok( 0, separator );
			const char* textHidden		= strtok( 0, separator );

			if( ! textItemIndex ||
				! textRate )
			{
				continue;
			}

			textHidden = ( textHidden ? textHidden : "" );

			ComposeScript::ResultMap&	resultMap	= script->mResultMap;
			float						rate		= float( atof( textRate ) ) / 100.0f;

			// 080916 LUJ, 이미 확률이 들어있는 경우, 마지막 확률 값을 누적한다. std::map의 lower_bound()기능을 사용하기 위함이다.
			//				예를 들어 마지막에 입력된 확률이 0.1, 이번에 읽어들인 확률이 0.5라고 하자. 그럼 0.6의 값을 unitMap에 넣는다.
			//				이렇게 하면 0~1 사이의 구간을 가진 확률 분포를 얻을 수 있다.
			if( ! resultMap.empty() )
			{
				ComposeScript::ResultMap::const_iterator it = resultMap.end();
				rate = rate + ( --it )->first;
			}

			// 080916 LUJ, 구간에 따라 배치하고 값을 넣는다
			ComposeScript::Result& result = resultMap[ rate ];
			result.mItemIndex	= atoi( textItemIndex );
			result.mIsHidden	= ( ! stricmp( "hidden", textHidden ) );

			// 080916 LUJ, 숨겨진 아이템은 검사용 컨테이너에 넣지 않는다. 이로써 숨겨진 아이템을 결과로 직접 지정할 수 없다
			if( ! result.mIsHidden )
			{
				script->mResultSet.insert( result.mItemIndex );
			}
		}
		// 080916 LUJ, 합성 결과에 보너스로 붙는 옵션
		else if( ! stricmp( "option", token ) )
		{
			state = StateOption;
		}

		// 080916 LUJ, 구문 파싱
		switch( state )
		{
		case StateOption:
			{
				// 080916 LUJ, 문자열과 퍼센트 플래그를 받아 드롭 옵션 키를 반환한다
				class
				{
				public:
					ITEM_OPTION::Drop::Key operator()( const char* text, BOOL isPercent ) const
					{
						typedef ITEM_OPTION::Drop Drop;

						if( ! text )
						{
							return Drop::KeyNone;
						}
						else if( ! stricmp( "strength", text ) )
						{
							return isPercent ? Drop::KeyPercentStrength : Drop::KeyPlusStrength;
						}
						else if( ! stricmp( "intelligence", text ) )
						{
							return isPercent ? Drop::KeyPercentIntelligence : Drop::KeyPlusIntelligence;
						}
						else if( ! stricmp( "dexterity", text ) )
						{
							return isPercent ? Drop::KeyPercentDexterity : Drop::KeyPlusDexterity;
						}
						else if( ! stricmp( "wisdom", text ) )
						{
							return isPercent ? Drop::KeyPercentWisdom : Drop::KeyPlusWisdom;
						}
						else if( ! stricmp( "vitality", text ) )
						{
							return isPercent ? Drop::KeyPercentVitality : Drop::KeyPlusVitality;
						}
						else if( ! stricmp( "physic_attack", text ) )
						{
							return isPercent ? Drop::KeyPercentPhysicalAttack : Drop::KeyPlusPhysicalAttack;
						}
						else if( ! stricmp( "physic_defense", text ) )
						{
							return isPercent ? Drop::KeyPercentPhysicalDefence : Drop::KeyPlusPhysicalDefence;
						}
						else if( ! stricmp( "magic_attack", text ) )
						{
							return isPercent ? Drop::KeyPercentMagicalAttack : Drop::KeyPlusMagicalAttack;
						}
						else if( ! stricmp( "magic_defense", text ) )
						{
							return isPercent ? Drop::KeyPercentMagicalDefence : Drop::KeyPlusMagicalDefence;
						}
						else if( ! stricmp( "critical_rate", text ) )
						{
							return isPercent ? Drop::KeyPercentCriticalRate : Drop::KeyPlusCriticalRate;
						}
						else if( ! stricmp( "critical_damage", text ) )
						{
							return isPercent ? Drop::KeyPercentCriticalDamage : Drop::KeyPlusCriticalDamage;
						}
						else if( ! stricmp( "accuracy", text ) )
						{
							return isPercent ? Drop::KeyPercentAccuracy : Drop::KeyPlusAccuracy;
						}
						else if( ! stricmp( "evade", text ) )
						{
							return isPercent ? Drop::KeyPercentEvade : Drop::KeyPlusEvade;
						}
						else if( ! stricmp( "move_speed", text ) )
						{
							return isPercent ? Drop::KeyPercentMoveSpeed : Drop::KeyPlusMoveSpeed;
						}
						else if( ! stricmp( "life", text ) )
						{
							return isPercent ? Drop::KeyPercentLife : Drop::KeyPlusLife;
						}
						else if( ! stricmp( "mana", text ) )
						{
							return isPercent ? Drop::KeyPercentMana : Drop::KeyPlusMana;
						}
						else if( ! stricmp( "recovery_life", text ) )
						{
							return isPercent ? Drop::KeyPercentLifeRecovery : Drop::KeyPlusLifeRecovery;
						}
						else if( ! stricmp( "recovery_mana", text ) )
						{
							return isPercent ? Drop::KeyPercentManaRecovery : Drop::KeyPlusManaRecovery;
						}

						return Drop::KeyNone;
					}
				}
				GetOptionKey;

				const char* textOption		= token;
				const char* textOptionValue	= strtok( 0, separator );
				const char* textHidden		= strtok( 0, separator );
				const char* textHiddenBonus	= strtok( 0, separator );

				if( ! textOption	||
					! textOptionValue )
				{
					continue;
				}

				const BOOL isPercent = ( _T( '%' ) == textOptionValue[ _tcslen( textOptionValue ) - 1 ] );

				textHidden		= ( textHidden		? textHidden		: "" );
				textHiddenBonus	= ( textHiddenBonus ? textHiddenBonus	: "" );

				const ITEM_OPTION::Drop::Key optionKey = GetOptionKey( textOption, isPercent );

				// 090122 LUJ, 값이 있을 경우에만 삽입하도록 한다
				if( ITEM_OPTION::Drop::KeyNone != optionKey )
				{
					// 080916 LUJ, 부여할 옵션 정보를 설정해준다
					ComposeScript::Option& option = script->mOptionMap[ optionKey ];
					ZeroMemory( &option, sizeof( option ) );
					option.mValue		= float( atof( textOptionValue ) / ( isPercent ? 100.0f : 1.0f ) );

					// 090122 LUJ, 히든 보너스 정보를 읽는다
					if( ! strcmpi( "hidden", textHidden ) )
					{
						const BOOL isHiddenValuePercent	= ( _T( '%' ) == textHiddenBonus[ _tcslen( textHiddenBonus ) - 1 ] );

						option.mHiddenBonus.mKey	= GetOptionKey( textOption, isHiddenValuePercent );
						option.mHiddenBonus.mValue	= float( atof( textHiddenBonus ) / ( isHiddenValuePercent ? 100.0f : 1.0f ) );
					}
				}

				break;
			}
		}
	}

#ifdef _GMTOOL_
	if( openCount )
	{
		cprintf( "It detected in ItemCompose.bin: open count check failed" );
	}
#endif
}

// 080916 LUJ, 아이템에 해당하는 합성 스크립트를 반환한다
const ComposeScript* CGameResourceManager::GetComposeScript( DWORD itemIndex ) const
{
	const ItemComposeScriptMap::const_iterator it = mItemComposeScriptMap.find( itemIndex );

	return mItemComposeScriptMap.end() == it ? 0 : it->second;
}

// 080916 LUJ, 키 아이템이 합성에 가능한 것인지 조회한다
BOOL CGameResourceManager::IsComposeKeyItem( DWORD keyItemIndex ) const
{
	return mKeyItemComposeScriptMap.end() != mKeyItemComposeScriptMap.find( keyItemIndex );
}

// 081231 LUJ, 아이템 인덱스에 해당하는 세트 스크립트를 반환한다
const SetScript* const CGameResourceManager::GetSetScript( DWORD itemIndex ) const
{
	const SetScriptMap::const_iterator iterator = mSetScriptMap.find( itemIndex );

	return mSetScriptMap.end() == iterator ? 0 : iterator->second;
}

// 081231 LUJ, 아이템 번호로 세트 스크립트에서 정의하는 추가 속성 정의를 반환한다
const SetScript::Element& CGameResourceManager::GetItemElement( DWORD itemIndex ) const
{
	static SetScript::Element emptyElement;

	const SetScript* const setScript = GetSetScript( itemIndex );

	if( ! setScript )
	{
		return emptyElement;
	}

	const SetScript::ItemElementMap&				itemElementMap	= setScript->mItemElementMap;
	const SetScript::ItemElementMap::const_iterator elementIterator = itemElementMap.find( itemIndex );

	return itemElementMap.end() == elementIterator ? emptyElement : elementIterator->second;
}

// 081231 LUJ, 세트 스크립트를 로딩한다
void CGameResourceManager::LoadSetScript( SetScriptMap& setScriptMap )
{
	SetItemName setName;

	// 081231 LUJ, 세트 이름을 로드하자
	{
		CMHFile file;
		file.Init( "data/interface/windows/setItemName.bin", "rb" );

		while( ! file.IsEOF() )
		{
			TCHAR line[ MAX_PATH ] = { 0 };
			file.GetLine( line, sizeof( line ) / sizeof( *line ) );

			const TCHAR*	separator	= _T( " \"\t\n" );
			const TCHAR*	token		= _tcstok( line, separator );
			const DWORD		index		= _tcstoul( token, 0, 10 );

			if( setName.end() != setName.find( index ) )
			{
#ifdef _GMTOOL
				TCHAR buffer[ 128 ];
				_stprintf( buffer, "%d번이 중복되었습니다. 나중에 나온 동일 번호의 이름은 무시됩니다", index );
				OutputDebugString( buffer );
#endif
				continue;
			}

			std::string &	name	= setName[ index ];
			const TCHAR*	position = line;

			while( (token = _tcstok( 0, separator ) )!= NULL)
			{
				name		+= token;
				position	+= _tcslen( token );

				if( 0 == *( position + 1 ) )
				{
					break;
				}

				name += ' ';
			}
		}
	}

	// 081231 LUJ, 세트 아이템 스크립트를 로딩한다
	{
		CMHFile file;
		file.Init( "system/resource/setItem.bin", "rb" );

		std::list< std::string >	text;
		SetScript*					script		= 0;
		int							openCount	= 0;

		while( ! file.IsEOF() )
		{
			TCHAR line[ MAX_PATH ] = { 0 };
			file.GetLine( line, sizeof( line ) / sizeof( *line ) );

			const int length = _tcslen( line );

			// 중간의 주석 제거
			for( int i = 0; i < length - 1; ++i )
			{
				const char a = line[ i ];
				const char b = line[ i + 1 ];

				if( '/' == a &&
					'/' == b )
				{
					line[ i ] = 0;
					break;
				}
			}

			text.push_back( line );

			const TCHAR* separator	= _T( " \n\t=,+\"()" );
			const TCHAR* token		= _tcstok( line, separator );
			const TCHAR* markBegin	= _T( "{" );
			const TCHAR* markEnd	= _T( "}" );

			if( ! token )
			{
				text.pop_back();
			}
			// 080912 LUJ, set로 지정된 아이템 번호는 "{"가 열리기 전에는 한 세트 그룹로 인식하도록 한다
			else if( ! _tcsicmp( token, "set" ) )
			{
				if( ! openCount &&
					! script )
				{
					mSetScriptList.push_back( SetScript() );

					script = &( mSetScriptList.back() );
				}

				while( (token = _tcstok( 0, separator ) )!= NULL)
				{
					const int index = _ttoi( token );

					script->mItem.insert( index );
#ifdef _GMTOOL_
					if( setScriptMap.end() != setScriptMap.find( index ) )
					{
						_stprintf( line, "아이템 인덱스가 중복되었습니다. %d", index );
						ASSERT( 0 && line );
					}
#endif
					setScriptMap[ index ] = script;
				}

				text.pop_back();
			}
			else if( 0 == _tcsnicmp( markBegin, token, strlen( markBegin ) ) )
			{
				++openCount;
			}
			else if( 0 == _tcsnicmp( markEnd, token, strlen( markEnd ) ) )
			{
				if( 0 == --openCount )
				{
					if( ! script )
					{
						return;
					}

					ParseSetScript( text, *script, setName );

					script = 0;
					text.clear();
				}
			}
		}
#ifdef _GMTOOL_
		if( openCount )
		{
			ASSERT( 0 && "세트 스크립트의 {} 블록이 올바로 닫히지 않았습니다" );
		}
#endif
	}
}

void CGameResourceManager::ParseSetScript(std::list< std::string >& text, SetScript& data, const SetItemName& name )
{
	// 080912 LUJ, 인덱스로 세트 아이템 이름을 가져오는 내부 클래스
	class SetItemNameContainer
	{
	public:
		SetItemNameContainer( const SetItemName& name ):
		  mSetName( name )
		  {}

		  const TCHAR* GetName( const TCHAR* index ) const
		  {
			  const SetItemName::const_iterator it = mSetName.find( _ttoi( index ) );

			  return mSetName.end() == it ? "" : it->second.c_str();
		  }

	private:
		const SetItemName& mSetName;
	}
	setItemNameContainer( name );

	int					openCount			= 0;
	SetScript::Element*	setElement			= 0;
	SetScript::Element* itemElement			= 0;

	for(	std::list< std::string >::const_iterator it = text.begin();
			text.end() != it;
			++it )
	{
		TCHAR line[ MAX_PATH ] = { 0 };
		_tcscpy( line, it->c_str() );

		const TCHAR* separator	= _T( " \n\t=,+\"()" );
		const TCHAR* markBegin	= _T( "{" );
		const TCHAR* markEnd	= _T( "}" );
		const TCHAR* token		= _tcstok( line, separator );
		
		if( ! token )
		{
			continue;
		}
		else if( ! _tcsicmp( "name", token ) )
		{
			token					= _tcstok( 0, separator );
			const DWORD nameIndex	= _tcstoul( token, 0, 10 );

			SetItemName::const_iterator name_it = name.find( nameIndex );

			if( name.end() != name_it )
			{
				data.mName = name_it->second;
			}
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "hand", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Weapon ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "shield", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Shield ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "armor", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Dress ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "helmet", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Hat ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "gloves", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Glove ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "boots", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Shoes ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "ring", token ) )
		{
			token = _tcstok( 0, separator );

			data.mSlotNameMap[ eWearedItem_Ring1 ] = setItemNameContainer.GetName( token );
			data.mSlotNameMap[ eWearedItem_Ring2 ] = setItemNameContainer.GetName( token );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "necklace", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Necklace ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		else if( ! _tcsicmp( "earring", token ) )
		{
			token = _tcstok( 0, separator );

			data.mSlotNameMap[ eWearedItem_Earring1 ] = setItemNameContainer.GetName( token );
			data.mSlotNameMap[ eWearedItem_Earring2 ] = setItemNameContainer.GetName( token );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "belt", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Belt ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "band", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Band ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "glasses", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Glasses ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "mask", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Mask ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "wing", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Wing ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "costume_head", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Costume_Head ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "costume_dress", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Costume_Dress ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "costume_glove", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Costume_Glove ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 080916 LUJ, 슬롯 이름 재지정
		else if( ! _tcsicmp( "costume_shoes", token ) )
		{
			data.mSlotNameMap[ eWearedItem_Costume_Shoes ] = setItemNameContainer.GetName( _tcstok( 0, separator ) );
		}
		// 개수별 능력 파싱
		else if( ! _tcsicmp( "size", token ) )
		{
			// 081231 LUJ, 세트/아이템 능력 두 가지를 세트 스크립트에서 처리하므로, 값이 저장될 곳을 구분할 수 있도록 포인터를 초기화시켜야한다.
			setElement	= 0;
			itemElement	= 0;

			const size_t size = _tcstoul( _tcstok( 0, separator ), 0, 10 );

			if( setElement )
			{
#ifdef _GMTOOL_
				ASSERT( 0 && "세트 스크립트의 블록이 올바로 닫히지 않았습니다" );
#endif
				return;
			}

			setElement = &( data.mAbility[ size ] );
		}
		// 081231 LUJ, 아이템의 기능을 확장한다
		else if( ! _tcsicmp( "item", token ) )
		{
			// 081231 LUJ, 세트/아이템 능력 두 가지를 세트 스크립트에서 처리하므로, 값이 저장될 곳을 구분할 수 있도록 포인터를 초기화시켜야한다.
			setElement	= 0;
			itemElement	= 0;

			const TCHAR* const	text		= _tcstok( 0, separator );
			const DWORD			itemIndex	= _ttoi( text ? text : _T( "" ) );

			itemElement = &( data.mItemElementMap[ itemIndex ] );
		}
		else if( 0 == _tcsnicmp( markBegin, token, strlen( markBegin ) ) )
		{
			++openCount;
		}
		else if( 0 == _tcsnicmp( markEnd, token, strlen( markEnd ) ) )
		{
			--openCount;

			setElement = 0;
		}
		// 081231 LUJ, 세트 스크립트를 통해 아이템에 추가 능력을 부여할 수 있도록 한다
		else if( setElement || itemElement )
		{
			// 081231 LUJ, 인자로 받아들여진 포인터 중 널이 아닌 저장 공간을 참조로 반환하는 내부 함수
			class
			{
			public:
				PlayerStat& operator()( SetScript::Element* const setElement, SetScript::Element* const itemElement ) const
				{
					if( setElement )
					{
						return setElement->mStat;
					}
					else if( itemElement )
					{
						return itemElement->mStat;
					}

					static PlayerStat emptyStat;
					return emptyStat;
				}
			}
			GetValidStat;

			PlayerStat& stat = GetValidStat( setElement, itemElement );

			if( 0 == _tcsicmp( token, "physic_attack" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mPhysicAttack.mPercent = value / 100.0f;
				}
				else
				{
					stat.mPhysicAttack.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "magic_attack" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mMagicAttack.mPercent	= value / 100.0f;
				}
				else
				{
					stat.mMagicAttack.mPlus	= value;
				}
			}
			else if( ! _tcsicmp( token, "physic_defense" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mPhysicDefense.mPercent = value / 100.0f;
				}
				else
				{
					stat.mPhysicDefense.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "magic_defense" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mMagicDefense.mPercent	= value / 100.0f;
				}
				else
				{
					stat.mMagicDefense.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "strength" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mStrength.mPercent	= value / 100.0f;
				}
				else
				{
					stat.mStrength.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "dexterity" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mDexterity.mPercent = value / 100.0f;
				}
				else
				{
					stat.mDexterity.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "vitality" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mVitality.mPercent	= value / 100.0f;
				}
				else
				{
					stat.mVitality.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "intelligence" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mIntelligence.mPercent	= value / 100.0f;
				}
				else
				{
					stat.mIntelligence.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "wisdom" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mWisdom.mPercent = value / 100.0f;
				}
				else
				{
					stat.mWisdom.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "life" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mLife.mPercent	= value / 100.0f;
				}
				else
				{
					stat.mLife.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "mana" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mMana.mPercent	= value / 100.0f;
				}
				else
				{
					stat.mMana.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "recovery_life" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mRecoveryLife.mPercent	= value / 100.0f;
				}
				else
				{
					stat.mRecoveryLife.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "recovery_mana" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mRecoveryMana.mPercent	= value / 100.0f;
				}
				else
				{
					stat.mRecoveryMana.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "accuracy" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mAccuracy.mPercent	= value / 100.0f;
				}
				else
				{
					stat.mAccuracy.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "evade" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mEvade.mPercent = value / 100.0f;
				}
				else
				{
					stat.mEvade.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "critical_rate" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mCriticalRate.mPercent	= value / 100.0f;
				}
				else
				{
					stat.mCriticalRate.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "critical_damage" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mCriticalDamage.mPercent = value / 100.0f;
				}
				else
				{
					stat.mCriticalDamage.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "move_speed" ) )
			{
				const float value = float( _tstof( token = _tcstok( 0, separator ) ) );

				if( _T( '%' ) == token[ _tcslen( token ) - 1 ] )
				{
					stat.mMoveSpeed.mPercent = value / 100.0f;
				}
				else
				{
					stat.mMoveSpeed.mPlus = value;
				}
			}
			else if( ! _tcsicmp( token, "skill" ) )
			{
				SetScript::Element::Skill* skillMap = 0;

				if( setElement )
				{
					skillMap = &( setElement->mSkill );
				}
				else if( itemElement )
				{
					skillMap = &( itemElement->mSkill );
				}
				else
				{
					static SetScript::Element::Skill emptySkillMap;
					skillMap = &emptySkillMap;
				}
				
				while( (token = _tcstok( 0, separator ) )!= NULL)
				{
					const DWORD index	= _tcstoul( token, 0, 10 );
					const BYTE	level	= BYTE( _ttoi( _tcstok( 0, separator ) ) );

					( *skillMap )[ index ] = level;
				}
			}
#ifdef _GMTOOL_
			else
			{
				_stprintf( line, "세트 스크립트에서 쓰지 않는 키워드가 발견되었습니다. %s", token );
				ASSERT( 0 && line );
			}
#endif
		}
	}

#ifdef _GMTOOL_
	if( openCount )
	{
		ASSERT( 0 && "세트 스크립트의 {} 블록이 올바로 닫히지 않았습니다" );
	}
#endif
}

// 090316 LUJ, 탈것 스크립트를 읽는다
void CGameResourceManager::LoadVehicleScript()
{
	enum Block
	{
		BlockNone,
		BlockScript,
		BlockScriptSeat,
		BlockScriptCondition,
		BlockScriptMotion,
	}
	blockType = BlockNone;

	DWORD openCount = 0;
	VehicleScript* vehicleScript = 0;

	CMHFile file;
	file.Init( "system/resource/vehicleScript.bin", "rb" );

	while( ! file.IsEOF() )
	{
		char text[ MAX_PATH ] = { 0 };
		file.GetLine( text, sizeof( text ) / sizeof( *text ) );

		const int length = _tcslen( text );

		// 090316 LUJ, 중간의 주석 제거
		for( int i = 0; i < length - 1; ++i )
		{
			const char a = text[ i ];
			const char b = text[ i + 1 ];

			if( '/' == a && '/' == b )
			{
				text[ i ] = 0;
				break;
			}
		}

		char buffer[ MAX_PATH ]	= { 0 };
		SafeStrCpy( buffer, text, sizeof( text ) / sizeof( *text ) );
		const char* const separator = " ~\n\t\"=,+()";
		const char* const markBegin = "{";
		const char* const markEnd = "}";
		const char* token = strtok( text, separator );

		if( ! token )
		{
			continue;
		}
		else if( ! stricmp( "vehicle", token ) )
		{
			blockType = BlockScript;

			static std::list< VehicleScript > vehicleScriptList;
			vehicleScriptList.push_back( VehicleScript() );
			vehicleScript = &( vehicleScriptList.back() );

			token = strtok( 0, separator );
			vehicleScript->mMonsterKind = (WORD)atoi( token ? token : "" );
			mVehicleScriptHashTable.Add( vehicleScript, vehicleScript->mMonsterKind );
		}
		else if( ! stricmp( "seat", token ) )
		{
			blockType = BlockScriptSeat;
		}
		else if( ! stricmp( "condition", token ) )
		{
			blockType = BlockScriptCondition;
		}
		else if( ! stricmp( "motion", token ) )
		{
			blockType = BlockScriptMotion;
		}
		else if( ! strnicmp( markBegin, token, strlen( markBegin ) ) )
		{
			++openCount;
		}
		else if( ! strnicmp( markEnd, token, strlen( markEnd ) ) )
		{
			--openCount;
			
			switch( blockType )
			{
			case BlockScript:
				{
					blockType = BlockNone;
					vehicleScript = 0;
					break;
				}
			case BlockScriptSeat:
			case BlockScriptCondition:
			case BlockScriptMotion:
				{
					blockType = BlockScript;
					break;
				}
			}
		}
		// 090316 LUJ, 스크립트 파싱
		else if( vehicleScript )
		{
			switch( blockType )
			{
			case BlockScriptSeat:
				{
					const char* const keyword = ( token ? token : "" );
					const BOOL isSeatForDriver = ( ! stricmp( "driver", keyword ) ? TRUE : stricmp( "passenger", keyword ) );
					const int seatMaxSize = sizeof( vehicleScript->mSeat ) / sizeof( *( vehicleScript->mSeat ) );

					for( int i = 0; i < seatMaxSize; ++i )
					{
						VehicleScript::Seat& seat = vehicleScript->mSeat[ i ];
						const VehicleScript::Seat emptySeat = { 0 };
						const BOOL isAllocatedSeat = memcmp( &emptySeat, &seat, sizeof( emptySeat ) );
						
						if( isAllocatedSeat )
						{
							continue;
						}
						
						char attachObjectName[ MAX_PATH ] = { 0 };
						
						for( ; (token = strtok( 0, separator ))!= NULL; strcat( attachObjectName, " " ) )
						{
							strcat( attachObjectName, token );
						}

						// 090318 LUJ, 끝 공백을 자른다
						if( attachObjectName[ 0 ] )
						{
							attachObjectName[ strlen( attachObjectName ) - 1 ] = 0;
						}

						// 090316 LUJ, 어태치할 오브젝트 이름을 복사한다.
						SafeStrCpy(
							seat.mAttachedName,
							attachObjectName,
							sizeof( seat.mAttachedName ) / sizeof( *( seat.mAttachedName ) ) );
						seat.mIsEnableControl = isSeatForDriver;
						vehicleScript->mSeatSize = min( vehicleScript->mSeatSize + 1, VehicleScript::Seat::MaxSize );
						break;
					}

					break;
				}
			case BlockScriptCondition:
				{
					const char* const keyword = token;
					const char* value = strtok( 0, separator );

					if( ! keyword ||
						! value )
					{
						break;
					}

					VehicleScript::Condition& condition = vehicleScript->mCondition;

					if( ! stricmp( "summonRange", keyword ) )
					{
						condition.mSummonRange = float( atof( value ) );
					}
					else if( ! stricmp( "money", keyword ) )
					{
						condition.mTollMoney = atoi( value );
					}
					else if( ! stricmp( "playerLevel", keyword ) )
					{
						condition.mPlayerMinimumLevel = LEVELTYPE(
							_ttoi(value));
						value = strtok(0, separator);
						condition.mPlayerMaximumLevel = LEVELTYPE(
							_ttoi(value ? value : "9999"));
					}
					else if( ! stricmp( "buff", keyword ) )
					{
						DWORD i = 0;

						do
						{
							condition.mBuffSkillIndex[ i ] = atoi( value );
							value = strtok( 0, separator );

							if( 0 == value )
							{
								break;
							}
						}
						while ( VehicleScript::Condition::BuffMaxSize > ++i );
					}

					break;
				}
			case BlockScriptMotion:
				{
					const char* const keyword = token;
					const char* const value = strtok( 0, separator );

					if( ! keyword ||
						! value )
					{
						break;
					}

					VehicleScript::Motion& motion = vehicleScript->mMotion;

					// 091109 ShinJS --- Beff 파일이름을 저장
					if( ! stricmp( "mount", keyword ) )
					{
						motion.mMount = atoi( value );
					}
					else if( ! stricmp( "fixObjectPlay", keyword ) )
					{
						motion.mBeffFileList.insert( VehicleScript::Motion::MotionInfo( VehicleScript::Motion::FixObjectPlay, value ) );
					}
					else if( ! stricmp( "fixObjectStop", keyword ) )
					{
						motion.mBeffFileList.insert( VehicleScript::Motion::MotionInfo( VehicleScript::Motion::FixObjectStop, value ) );
					}
					else if( ! stricmp( "moveObjectIdle", keyword ) )
					{
						motion.mBeffFileList.insert( VehicleScript::Motion::MotionInfo( VehicleScript::Motion::MoveObjectIdle, value ) );
					}
					else if( ! stricmp( "moveObjectWalk", keyword ) )
					{
						motion.mBeffFileList.insert( VehicleScript::Motion::MotionInfo( VehicleScript::Motion::MoveObjectWalk, value ) );
					}
					else if( ! stricmp( "moveObjectRun", keyword ) )
					{
						motion.mBeffFileList.insert( VehicleScript::Motion::MotionInfo( VehicleScript::Motion::MoveObjectRun, value ) );
					}

					break;
				}
			}
		}
	}
}

// 090316 LUJ, 탈것 스크립트를 반환한다
const VehicleScript& CGameResourceManager::GetVehicleScript( DWORD monsterKind )
{
	const VehicleScript* const vehicleScript = mVehicleScriptHashTable.GetData( monsterKind );

	static const VehicleScript emptyVehicleScript;
	return vehicleScript ? *vehicleScript : emptyVehicleScript;
}

LPCTSTR CGameResourceManager::GetRiderAttachObjName( DWORD monsterKind, WORD pos )
{
	const VehicleScript& script = GetVehicleScript( monsterKind );

	if( VehicleScript::Seat::MaxSize < pos )
	{
		return "";
	}

	return script.mSeat[ pos ].mAttachedName;
}

//090318 pdy Housing Script작업 추가
void CGameResourceManager::LoadHousing_AllList()
{
	LoadHousing_FurnitureList();
	LoadHousing_BonuseList();
	LoadHousing_ActionGroupList();
	LoadHousing_ActionList();
}

//090318 pdy Housing Script작업 추가
void CGameResourceManager::ReleaseHousing_AllList()
{
	ReleaseHousing_FurnitureList();
	ReleaseHousing_BonusList();
	ReleaseHousing_ActionGroupList();
	ReleaseHousing_ActionList();
}

//090318 pdy Housing Script작업 추가
stFunitureInfo* CGameResourceManager::GetFunitureInfo(DWORD FunitureIndex)
{
	return m_FunitureList.GetData(FunitureIndex);
}

stHouseBonusInfo*	CGameResourceManager::GetHouseBonusInfo(DWORD dwBonusIndex)
{
	return m_HouseBonusList.GetData(dwBonusIndex);
}


stHouseActionGroupInfo*	CGameResourceManager::GetHouseActionGroupInfo(DWORD dwGroupIndex)
{
	return m_HouseActionGroupList.GetData(dwGroupIndex);
}

stHouseActionInfo*		CGameResourceManager::GetHouseActionInfo(DWORD dwActionIndex)
{
	return m_HouseActionList.GetData(dwActionIndex);
}

void CGameResourceManager::LoadDungeonKeyList()
{
	// 인던 키 스크립트 로딩
	m_DungeonKeyList.RemoveAll();
	char buf[MAX_PATH] = {0,};

	CMHFile file;
	if(! file.Init( "system/resource/DungeonKeyList.bin", "rb" ))
		return;

	while(FALSE == file.IsEOF())
	{
		file.GetString(buf);
		if(0 != strcmp(buf, "#KEYINFO"))
		{
			file.GetLineX(buf, MAX_PATH);
			continue;
		}

		stDungeonKey* pKey = new stDungeonKey;
		if(pKey)
		{
			pKey->dwIndex = file.GetDword();
			pKey->wMapNum = file.GetWord();
			pKey->difficulty = (eDIFFICULTY)file.GetInt();
			pKey->dwEntranceNpcJobIndex = file.GetDword();

#ifdef _CLIENT_
			// 100329 ShinJS --- 인던포탈 입장기능 추가
			if( pKey->dwEntranceNpcJobIndex )
			{
				DungeonMGR->AddEntranceNpc( pKey->dwIndex, pKey->dwEntranceNpcJobIndex );
			}
#endif

			pKey->dwPosX = file.GetDword();
			pKey->dwPosZ = file.GetDword();

			pKey->wMinLevel = file.GetWord();
			pKey->wMaxLevel = file.GetWord();
			pKey->wMaxCount = file.GetWord();
			pKey->wDelayMin = file.GetWord();

			pKey->dwTooltipIndex = file.GetDword();

			m_DungeonKeyList.Add(pKey, pKey->dwIndex);
		}
	}
}

void CGameResourceManager::ReleaseDungeonKeyList()
{
	stDungeonKey* pKey = NULL;
	m_DungeonKeyList.SetPositionHead();
	while((pKey = m_DungeonKeyList.GetData()) != NULL)
	{
		SAFE_DELETE(pKey);
	}
	m_DungeonKeyList.RemoveAll();
}

stDungeonKey* CGameResourceManager::GetDungeonKey(DWORD dwKeyIndex)
{
	return m_DungeonKeyList.GetData(dwKeyIndex);
}

//090318 pdy Housing Script작업 추가
void CGameResourceManager::LoadHousing_FurnitureList()
{
	ReleaseHousing_FurnitureList();

	CMHFile file;

	char StrBuf[256] = {0,};

	//모델파일종류 0: MOD, 1: CHX, 2:CHR
	char FileTypeName[3][10]= 
	{
		{".MOD"},
		{".CHX"},
		{".CHR"},
	};

	bool bParsing = FALSE;
	bool bFunitureListParsing = FALSE;

	if( ! file.Init("System/Resource/Housing_FunitureList.bin", "rb" ) )
		return;

	//@@가구인덱스 @@가구이름  @@액션인덱스 @@꾸미기포인트 @@쌓기 @@회전 @@마킹 @@충돌검사방식 @@모델파일 종류 @@재질수

	while( ! file.IsEOF() )
	{
		file.GetString(StrBuf);

		if(! bParsing)
		{
			if( StrBuf[0] != '#' )
			{
				file.GetLine(StrBuf,256);
				continue;
			}
			else if( strcmp( StrBuf , "#FURNITURELIST_START" ) == 0 )
			{
				bParsing = bFunitureListParsing = TRUE;
				continue;
			}
		}
		else if( strcmp( StrBuf , "#FURNITURELIST_END" ) == 0 )
		{
			bParsing = bFunitureListParsing = FALSE;
			continue;
		}
		else if( bFunitureListParsing )
		{
			//Todo Load ItemList Sctipt
			stFunitureInfo* pInfo		= new stFunitureInfo;
			pInfo->dwFurnitureIndex		= (DWORD)atoi(StrBuf);									//@@가구인덱스
			SafeStrCpy( pInfo->szName, file.GetString(), MAX_HOUSING_NAME_LENGTH );				//@@아이템이름
			pInfo->dwActionIndex		= file.GetDword();										//@@액션인덱스	
			pInfo->dwDecoPoint			= file.GetDword();										//@@꾸미기포인트
			pInfo->bStackable			= file.GetBool();										//@@쌓기가능
			pInfo->bRotatable			= file.GetBool();										//@@회전가능
			pInfo->bTileDisable			= file.GetBool();										//@@마킹여부
			pInfo->bMeshTest			= file.GetBool();										//@@삼각형테스트
			pInfo->byModelFileFlag		= file.GetByte();										//@@모델파일 종류
			pInfo->byMaxMaterialNum		= file.GetByte();										//@@재질수
			pInfo->byAttachBoneNum		= file.GetByte();										//@@어태치본 갯수
			pInfo->bHasAlpha			= file.GetBool();										//@@자체알파 보유여부

			//090618 pdy 리소스가 꼬이거나 잘못되었을 경우 메세지 박스를 띄우고 종료시킨다.  
			char *pName = ConvertFunitureIndex2FileName(pInfo->dwFurnitureIndex);
			if( pName == NULL )
			{
				#if defined(_DEBUG)
				char MsgBuf[128] = {0,};
				sprintf(MsgBuf,"Invalid Funiture Index : %d" ,pInfo->dwFurnitureIndex );
				MessageBox(NULL, MsgBuf , "Error",  MB_OK);
				#endif
				__asm int 3;
			}
			SafeStrCpy( pInfo->szModelFileName , pName , 256 );	//@@모델파일이름 
			//090325 pdy
			strcat( pInfo->szModelFileName , FileTypeName[pInfo->byModelFileFlag]);
			m_FunitureList.Add(pInfo, pInfo->dwFurnitureIndex);
			continue;
		}
	}
}

//090318 pdy Housing Script작업 추가
void CGameResourceManager::ReleaseHousing_FurnitureList()
{
	//가구정보 Release
	stFunitureInfo * pInfo = NULL;
	m_FunitureList.SetPositionHead();
	while((pInfo = m_FunitureList.GetData())!= NULL)
		SAFE_DELETE( pInfo);
	m_FunitureList.RemoveAll();
}

void CGameResourceManager::LoadHousing_BonuseList()
{
	ReleaseHousing_BonusList();

	CMHFile file;

	char StrBuf[256] = {0,};

	bool bPasing				= FALSE;
	bool bNomalBonusPasing		= FALSE;
	bool bSpacialBonusPasing	= FALSE;

	if( ! file.Init("System/Resource/Housing_BonusList.bin", "rb" ) )
		return;

	//@@꾸미기포인트 @@효과이름 @@아이템인덱스 @@스킬인덱스 @@하루사용제한수
	while( ! file.IsEOF() )
	{
		file.GetString(StrBuf);

		if(! bPasing)		
		{
			//Todo OneLine Passing 
			if( StrBuf[0] != '#' )
			{
				file.GetLine(StrBuf,256);
				continue;
			}
			else if( strcmp( StrBuf , "#BONUS_NOMAL_START" ) == 0 )
			{
				bPasing = bNomalBonusPasing = TRUE;
				continue;
			}
			else if( strcmp( StrBuf , "#BONUS_SPACIAL_START" ) == 0 )
			{
				bPasing = bSpacialBonusPasing = TRUE;
			}
		}
		else if( strcmp( StrBuf , "#BONUS_NOMAL_END" ) == 0 )
		{
			bPasing = bNomalBonusPasing = FALSE;
			continue;
		}
		else if( strcmp( StrBuf , "#BONUS_SPACIAL_END" ) == 0 )
		{
			bPasing = bSpacialBonusPasing = FALSE;
			continue;
		}
		else if( bNomalBonusPasing )
		{
			//Todo Load NomalBonus Sctipt
			stHouseBonusInfo* pInfo = new stHouseBonusInfo;
			pInfo->byKind	= 0 ;								//0 범용보너스 , 1랭커용 보너스  

			pInfo->dwBonusIndex			= (DWORD)atoi(StrBuf);	//효과인덱스
			pInfo->dwDecoPoint				= file.GetDword();	//꾸미기포인트
			SafeStrCpy( pInfo->szName, file.GetString(), 32 );	//보너스이름
			pInfo->dwTooltipIndex		= file.GetDword();		//보너스툴팁인덱스
			pInfo->dwBonusType			= file.GetByte();		//보너스타입      0 버프스킬   ,1 맵이동   ,2 아이템지급
			pInfo->dwBonusTypeValue		= file.GetDword();		//타입에따른 정보  (스킬Idx)  , (맵번호)  , (아이템인덱스)
			pInfo->byTargetType			= file.GetByte();		//적용될 타겟정보  0 집주인 , 1 방에있는사람모두 
			pInfo->dwUsePoint		= file.GetDword();			//차감포인트 

			m_HouseBonusList.Add(pInfo,pInfo->dwBonusIndex);
			continue;
		}
		else if( bSpacialBonusPasing )
		{
			//Todo Load SpacialBonus Sctipt
			stHouseBonusInfo* pInfo = new stHouseBonusInfo;
			pInfo->byKind	= 1 ;								//0 범용보너스 , 1랭커용 보너스 

			pInfo->dwBonusIndex			= (DWORD)atoi(StrBuf);	//효과인덱스
			pInfo->dwDecoPoint				= file.GetDword();		//꾸미기포인트
			SafeStrCpy( pInfo->szName, file.GetString(), 32 );	//보너스이름
			pInfo->dwTooltipIndex		= file.GetDword();		//보너스툴팁인덱스
			pInfo->dwBonusType			= file.GetByte();		//보너스타입      0 버프스킬   ,1 맵이동   ,2 아이템지급
			pInfo->dwBonusTypeValue		= file.GetDword();		//타입에따른 정보  (스킬Idx)  , (맵번호)  , (아이템인덱스)
			pInfo->byTargetType			= file.GetByte();		//적용될 타겟정보  0 집주인 , 1 방에있는사람모두 
			pInfo->dwUsePoint		= file.GetDword();		//차감포인트 
			m_HouseBonusList.Add(pInfo,pInfo->dwBonusIndex);
			continue;
		}
	}
}

void CGameResourceManager::ReleaseHousing_BonusList()
{
	//모델파일정보 Release
	stHouseBonusInfo* pBonusInfo = NULL;
	m_HouseBonusList.SetPositionHead();
	while((pBonusInfo = m_HouseBonusList.GetData())!= NULL)
		SAFE_DELETE( pBonusInfo);
	m_HouseBonusList.RemoveAll();
}

void CGameResourceManager::LoadHousing_ActionGroupList()
{
	ReleaseHousing_ActionGroupList();

	CMHFile file;

	char StrBuf[256] = {0,};

	bool bParsing = FALSE;
	bool bActionGroupListParsing = FALSE;

	if( ! file.Init("System/Resource/Housing_actionGroupList.bin", "rb" ) )
		return;

	//Index	Name 캐릭터애니 캐릭터이펙트 모델애니 모델이펙트 ActionType ActionValue	
	while( ! file.IsEOF() )
	{
		file.GetString(StrBuf);

		if(! bParsing)
		{
			if( StrBuf[0] != '#' )
			{
				file.GetLine(StrBuf,256);
				continue;
			}
			else if( strcmp( StrBuf , "#ACTIONGROUPLIST_START" ) == 0 )
			{
				bParsing = bActionGroupListParsing = TRUE;
				continue;
			}
		}
		else if( strcmp( StrBuf , "#ACTIONGROUPLIST_END" ) == 0 )
		{
			bParsing = bActionGroupListParsing = FALSE;
			continue;
		}
		else if( bActionGroupListParsing )
		{
			stHouseActionGroupInfo* pGroupInfo = new stHouseActionGroupInfo();
			pGroupInfo->dwGroupIndex = (DWORD)atoi(StrBuf);

			DWORD dwCount = 0;
			for( int i=0; i < MAX_HOUSE_ACTION_SLOT ; i++)
			{
				pGroupInfo->dwActionIndexList[dwCount] = file.GetDword();
				
				if( pGroupInfo->dwActionIndexList[dwCount] )
				{
					dwCount++;
				}
			}

			pGroupInfo->byActionNum = (BYTE)dwCount;

			m_HouseActionGroupList.Add(pGroupInfo,pGroupInfo->dwGroupIndex);
			continue;
		}
	}
}

void CGameResourceManager::ReleaseHousing_ActionGroupList()
{
		//모델파일정보 Release
	stHouseActionGroupInfo* pGroupInfo = NULL;
	m_HouseActionGroupList.SetPositionHead();
	while((pGroupInfo = m_HouseActionGroupList.GetData())!= NULL)
		SAFE_DELETE( pGroupInfo);
	m_HouseActionGroupList.RemoveAll();

}

void CGameResourceManager::LoadHousing_ActionList()
{
	ReleaseHousing_ActionList();

	CMHFile file;

	char StrBuf[256] = {0,};

	bool bParsing = FALSE;
	bool bActionListParsing = FALSE;

	if( ! file.Init("System/Resource/Housing_actionList.bin", "rb" ) )
		return;

	//Index	Name 캐릭터애니 캐릭터이펙트 모델애니 모델이펙트 ActionType ActionValue	
	while( ! file.IsEOF() )
	{
		file.GetString(StrBuf);

		if(! bParsing)
		{
			if( StrBuf[0] != '#' )
			{
				file.GetLine(StrBuf,256);
				continue;
			}
			else if( strcmp( StrBuf , "#ACTIONLIST_START" ) == 0 )
			{
				bParsing = bActionListParsing = TRUE;
				continue;
			}
		}
		else if( strcmp( StrBuf , "#ACTIONLIST_END" ) == 0 )
		{
			bParsing = bActionListParsing = FALSE;
			continue;
		}
		else if( bActionListParsing )
		{
			stHouseActionInfo* pActionInfo = new stHouseActionInfo();

			pActionInfo->dwActionIndex  = (DWORD)atoi(StrBuf);
			SafeStrCpy( pActionInfo->szName, file.GetString(), MAX_HOUSING_NAME_LENGTH );
			pActionInfo->dwPlayer_MotionIndex		= file.GetDword();
			pActionInfo->dwFurniture_EffectIndex	= file.GetDword();
			pActionInfo->dwFurniture_MotionIndex	= file.GetDword();
			pActionInfo->dwFurniture_EffectIndex	= file.GetDword();
			pActionInfo->dwActionType				= file.GetDword();
			pActionInfo->dwActionValue				= file.GetDword();
			m_HouseActionList.Add(pActionInfo,pActionInfo->dwActionIndex);
			continue;
		}
	}
}
void CGameResourceManager::ReleaseHousing_ActionList()
{
	//모델파일정보 Release
	stHouseActionInfo* pActionInfo = NULL;
	m_HouseActionList.SetPositionHead();
	while((pActionInfo = m_HouseActionList.GetData())!= NULL)
		SAFE_DELETE( pActionInfo);
	m_HouseActionList.RemoveAll();
}

void CGameResourceManager::LoadLimitDungeonScript()
{
	mLimitDungeonScriptMap.clear();

	enum State
	{
		StateNone,
		StateName,
		StateDescription,
		StateMap,
		StateLevel,
		StateSummon,
		StateHome,
		StateItem,
		StateOpen,
		StateAlarm,
	}
	state = StateNone;

	CMHFile file;
	file.Init(
		"system/resource/LimitDungeon.bin",
		"rb");
	LimitDungeonKey key;
	BOOL isComment = FALSE;

	while(FALSE == file.IsEOF())
	{
		TCHAR text[MAX_PATH] = {0};
		file.GetLine(
			text,
			sizeof(text) / sizeof(*text));
		const int length = _tcslen(text);

		// 090316 LUJ, 중간의 주석 제거
		for(int i = 0; i < length - 1; ++i)
		{
			char& first = text[i];
			char& second = text[i+1];

			if('/' == first && '/' == second)
			{
				first = 0;
				break;
			}
			else if('/' == first && '*' == second)
			{
				first = 0;
				isComment = TRUE;
				break;
			}
			else if('*' == first && '/' == second)
			{
				first = ' ';
				second = ' ';
				isComment = FALSE;
				break;
			}
		}

		LPCTSTR seperator = " \t ~ =\n():,";
		LPCTSTR token = _tcstok(text, seperator);

		if(0 == token)
		{
			continue;
		}
		else if(isComment)
		{
			continue;
		}
		else if(0 == _tcsicmp("map", token))
		{
			state = StateMap;
			LPCTSTR mapText = _tcstok(0, seperator);
			LPCTSTR channelText = _tcstok(0, seperator);
			key.first = MAPTYPE(_ttoi(mapText ? mapText : ""));
			key.second = _ttoi(channelText ? channelText : "");
			LimitDungeonScript& script = mLimitDungeonScriptMap[key];
			script.mMapType = key.first;
			script.mChannel = key.second;
		}
		else if(0 == _tcsicmp("name", token))
		{
			state = StateName;
			LPCTSTR nameText = _tcstok(0, seperator);
			LimitDungeonScript& script = mLimitDungeonScriptMap[key];
			script.mNameIndex = _ttoi(nameText ? nameText : "");
		}
		else if(0 == _tcsicmp("level", token))
		{
			state = StateLevel;
			LPCTSTR minText = _tcstok(0, seperator);
			LPCTSTR maxText = _tcstok(0, seperator);
			LimitDungeonScript& script = mLimitDungeonScriptMap[key];
			script.mLevel.mMin = LEVELTYPE(_ttoi(minText ? minText : ""));
			script.mLevel.mMax = LEVELTYPE(_ttoi(maxText ? maxText : ""));

			if(0 == script.mLevel.mMax)
			{
				script.mLevel.mMax = USHRT_MAX;
			}
		}
		else if(0 == _tcsicmp("summon", token))
		{
			state = StateSummon;
			LPCTSTR monsterScriptText = _tcstok(0, seperator);
			LimitDungeonScript& script = mLimitDungeonScriptMap[key];
			_stprintf(
				script.mMonsterScript,
				"system\\resource\\%s",
				monsterScriptText);
		}
		else if(0 == _tcsicmp("home", token))
		{
			state = StateHome;
			LPCTSTR mapText = _tcstok(0, seperator);
			LPCTSTR xPositionText = _tcstok(0, seperator);
			LPCTSTR zPoisiotnText = _tcstok(0, seperator);
			LimitDungeonScript& script = mLimitDungeonScriptMap[key];
			LimitDungeonScript::Home& home = script.mHome;
			home.mMapType = MAPTYPE(_ttoi(mapText ? mapText : ""));
			home.mPosition.x = float(_tstof(xPositionText ? xPositionText : "")) * 100.0f;
			home.mPosition.z = float(_tstof(zPoisiotnText ? zPoisiotnText : "")) * 100.0f;
		}
		else if(0 == _tcsicmp("item", token))
		{
			state = StateItem;
			LPCTSTR itemIndexText = _tcstok(0, seperator);
			LPCTSTR itemQuantityText = _tcstok(0, seperator);
			LimitDungeonScript& script = mLimitDungeonScriptMap[key];
			LimitDungeonScript::Item& item = script.mItem;
			item.mItemIndex = _ttoi(itemIndexText ? itemIndexText : "");
			item.mQuantity = _ttoi(itemQuantityText ? itemQuantityText : "");
		}
		else if(0 == _tcsicmp("open", token))
		{
			state = StateOpen;
			LPCTSTR dayOfWeekText = _tcstok(0, seperator);
			LPCTSTR hourText = _tcstok(0, seperator);
			LPCTSTR minuteText = _tcstok(0, seperator);
			LPCTSTR durationText = _tcstok(0, seperator);
			LimitDungeonScript& script = mLimitDungeonScriptMap[key];
			LimitDungeonScript::Date date = {0};
			
			if(0 == _tcsicmp("sunday", dayOfWeekText))
			{
				date.mDayOfWeek = 0;
			}
			else if(0 == _tcsicmp("monday", dayOfWeekText))
			{
				date.mDayOfWeek = 1;
			}
			else if(0 == _tcsicmp("tuesday", dayOfWeekText))
			{
				date.mDayOfWeek = 2;
			}
			else if(0 == _tcsicmp("wednesday", dayOfWeekText))
			{
				date.mDayOfWeek = 3;
			}
			else if(0 == _tcsicmp("thursday", dayOfWeekText))
			{
				date.mDayOfWeek = 4;
			}
			else if(0 == _tcsicmp("friday", dayOfWeekText))
			{
				date.mDayOfWeek = 5;
			}
			else if(0 == _tcsicmp("saturday", dayOfWeekText))
			{
				date.mDayOfWeek = 6;
			}

			date.mStartHour = _ttoi(hourText ? hourText : "");
			date.mStartmMinute = _ttoi(minuteText ? minuteText : "");
			date.mPlayTime = _ttoi(durationText ? durationText : "") * 60;
			script.mDateList.push_back(date);
		}
		else if(0 == _tcsicmp("alarm", token))
		{
			state = StateAlarm;
			LimitDungeonScript& script = mLimitDungeonScriptMap[key];
			const LimitDungeonScript::Alarm emptyAlarm = {0};
			script.mAlarmList.push_back(emptyAlarm);
		}
		else if(0 == _tcsicmp("description", token))
		{
			state = StateDescription;
			LimitDungeonScript& script = mLimitDungeonScriptMap[key];
			LimitDungeonScript::DescriptionList& descriptionList = script.mDescriptionList;

            for(LPCTSTR indexText = _tcstok(0, seperator);
				0 < indexText;
				indexText = _tcstok(0, seperator))
			{
				descriptionList.push_back(_ttoi(indexText));
			}
		}

		switch(state)
		{
		case StateAlarm:
			{
				LimitDungeonScript& script = mLimitDungeonScriptMap[key];
				LimitDungeonScript::Alarm& alarm = script.mAlarmList.back();

				if(0 == _tcsicmp("minute", token))
				{
					LPCTSTR minuteText = _tcstok(0, seperator);
					alarm.mTime = _ttoi(minuteText ? minuteText : "") * 60;
				}
				else if(0 == _tcsicmp("systemMessage", token))
				{
					LPCTSTR messageText = _tcstok(0, seperator);
					alarm.mMessage = _ttoi(messageText ? messageText : "");
				}

				break;
			}
		}
	}
}

const LimitDungeonScript& CGameResourceManager::GetLimitDungeonScript(MAPTYPE mapType, DWORD channelIndex) const
{
	const LimitDungeonKey key(mapType, channelIndex);
	const LimitDungeonScriptMap::const_iterator iter = mLimitDungeonScriptMap.find(key);
	static const LimitDungeonScript emptyScript;

	return mLimitDungeonScriptMap.end() == iter ? emptyScript : iter->second;
}

void CGameResourceManager::Parse(LPCTSTR parsingLine, BUFF_SKILL_INFO& info) const
{
	TCHAR text[MAX_PATH] = {0};
	SafeStrCpy(
		text,
		parsingLine,
		sizeof(text) / sizeof(*text));
	LPCTSTR separator = "\t";

	LPCTSTR textIndex = _tcstok(text, separator);
	info.Index = _ttoi(textIndex ? textIndex : "");

	LPCTSTR textName = _tcstok(0, separator);
	SafeStrCpy(
		info.Name,
		textName,
		sizeof(info.Name) / sizeof(*info.Name));

	LPCTSTR textLevel = _tcstok(0, separator);
	info.Level = WORD(_ttoi(textLevel ? textLevel : ""));

	LPCTSTR textImage = _tcstok(0, separator);
	info.Image = _ttoi(textImage ? textImage : "");

	LPCTSTR textTip = _tcstok(0, separator);
	info.Tooltip = _ttoi(textTip ? textTip : "");

	LPCTSTR textSkillEffect = _tcstok(0, separator);
	info.SkillEffect = _ttoi(textSkillEffect ? textSkillEffect : "");

	LPCTSTR textKind = _tcstok(0, separator);
	info.Kind = SKILLKIND(_ttoi(textKind ? textKind : ""));

	LPCTSTR textDelayTime = _tcstok(0, separator);
	info.DelayTime = _ttoi(textDelayTime ? textDelayTime : "");

	// 091211 LUJ, 계열 값. 사용하지 않음
	_tcstok(0, separator);

	LPCTSTR textWeapon = _tcstok(0, separator);

	// 091211 LUJ, 방패 값이 갑옷 변수에 들어갈 수 있다. 방패 값이 설정된 후 0값으로 덮지 않도록 먼저 대입한다
	LPCTSTR textArmor = _tcstok(0, separator);
	info.mArmorType = eArmorType(_ttoi(textArmor ? textArmor : ""));

	// 091211 LUJ, 장비 제한 처리를 간단히 하기 위해 게임 내 쓰이는 enum 상수로 대체한다
	{
		enum eBuffSkillWeapon
		{
			eBuffSkillWeapon_None,
			eBuffSkillWeapon_Sword,
			eBuffSkillWeapon_Shield,
			eBuffSkillWeapon_Mace,
			eBuffSkillWeapon_Axe,
			eBuffSkillWeapon_Staff,
			eBuffSkillWeapon_Bow,
			eBuffSkillWeapon_Gun,
			eBuffSkillWeapon_Dagger,
			eBuffSkillWeapon_Spear,
			eBuffSkillWeapon_TwoHanded,
			eBuffSkillWeapon_OneHanded,
			eBuffSkillWeapon_TwoBlade,
			eBuffSkillWeapon_BothDagger,
			eBuffSkillWeapon_Cooking,
			eBuffSkillWeapon_FishingPole,
		};

		switch(eBuffSkillWeapon(_ttoi(textWeapon ? textWeapon : "")))
		{
		case eBuffSkillWeapon_Sword:
			{
				info.mWeaponType = eWeaponType_Sword;
				break;
			}
		case eBuffSkillWeapon_Shield:
			{
				info.mArmorType = eArmorType_Shield;
				break;
			}
		case eBuffSkillWeapon_Mace:
			{
				info.mWeaponType = eWeaponType_Mace;
				break;
			}
		case eBuffSkillWeapon_Axe:
			{
				info.mWeaponType = eWeaponType_Axe;
				break;
			}
		case eBuffSkillWeapon_Staff:
			{
				info.mWeaponType = eWeaponType_Staff;
				break;
			}
		case eBuffSkillWeapon_Bow:
			{
				info.mWeaponType = eWeaponType_Bow;
				break;
			}
		case eBuffSkillWeapon_Gun:
			{
				info.mWeaponType = eWeaponType_Gun;
				break;
			}
		case eBuffSkillWeapon_Dagger:
			{
				info.mWeaponType = eWeaponType_Dagger;
				break;
			}
		case eBuffSkillWeapon_Spear:
			{
				info.mWeaponAnimationType = eWeaponAnimationType_Spear;
				break;
			}
		case eBuffSkillWeapon_TwoHanded:
			{
				info.mWeaponAnimationType = eWeaponAnimationType_TwoHand;
				break;
			}
		case eBuffSkillWeapon_OneHanded:
			{
				info.mWeaponAnimationType = eWeaponAnimationType_OneHand;
				break;
			}
		case eBuffSkillWeapon_TwoBlade:
			{
				info.mWeaponAnimationType = eWeaponAnimationType_TwoBlade;
				break;
			}
		case eBuffSkillWeapon_BothDagger:
			{
				info.mWeaponType = eWeaponType_BothDagger;
				break;
			}
		case eBuffSkillWeapon_Cooking:
			{
				info.mWeaponType = eWeaponType_CookUtil;
				break;
			}
		case eBuffSkillWeapon_FishingPole:
			{
				info.mWeaponType = eWeaponType_FishingPole;
				break;
			}
		}
	}

	LPCTSTR textStatus = _tcstok(0, separator);
	info.Status = eStatusKind(_ttoi(textStatus ? textStatus : ""));

	LPCTSTR textStatusDataType = _tcstok(0, separator);
	LPCTSTR textStatuaDataValue = _tcstok(0, separator);

	switch( info.Status )
	{
	case eStatusKind_EventSkillToVictim:
	case eStatusKind_EventSkillToKiller:
		{
			info.mEventSkillIndex = _ttoi(textStatuaDataValue ? textStatuaDataValue : "");
			break;
		}
	default:
		{
			info.StatusDataType = BUFF_SKILL_INFO::DataType(_ttoi(textStatusDataType ? textStatusDataType : ""));
			info.StatusData = float(_tstof(textStatuaDataValue ? textStatuaDataValue : ""));
			break;
		}
	}

	LPCTSTR textConditionType = _tcstok(0, separator);
	info.mCondition.mType = BUFF_SKILL_INFO::Condition::Type(_ttoi(textConditionType ? textConditionType : ""));

	LPCTSTR textConditionOperation = _tcstok(0, separator);
	info.mCondition.mOperation = BUFF_SKILL_INFO::Condition::Operation(_ttoi(textConditionOperation ? textConditionOperation : ""));

	LPCTSTR textConditionValue = _tcstok(0, separator);
	info.mCondition.mValue = float(_tstof(textConditionValue ? textConditionValue : ""));

	LPCTSTR textBattle = _tcstok(0, separator);
	info.mRuleBattle = eBuffSkillActiveRule(_ttoi(textBattle ? textBattle : ""));

	LPCTSTR textPeace = _tcstok(0, separator);
	info.mRulePeace = eBuffSkillActiveRule(_ttoi(textPeace ? textPeace : ""));

	LPCTSTR textMove = _tcstok(0, separator);
	info.mRuleMove = eBuffSkillActiveRule(_ttoi(textMove ? textMove : ""));

	LPCTSTR textStop = _tcstok(0, separator);
	info.mRuleStop = eBuffSkillActiveRule(_ttoi(textStop ? textStop : ""));

	LPCTSTR textRest = _tcstok(0, separator);
	info.mRuleRest = eBuffSkillActiveRule(_ttoi(textRest ? textRest : ""));

	LPCTSTR textDie = _tcstok(0, separator);
	info.Die = _ttoi(textDie ? textDie : "");

	LPCTSTR textNoUpdate = _tcstok(0, separator);
	info.NoUpdate = _ttoi(textNoUpdate ? textNoUpdate : "");

	LPCTSTR textUseItem1Count = _tcstok(0, separator);
	info.ItemCount[0] = WORD(_ttoi(textUseItem1Count ? textUseItem1Count : ""));

	LPCTSTR textUseItem2Count = _tcstok(0, separator);
	info.ItemCount[1] = WORD(_ttoi(textUseItem2Count ? textUseItem2Count : ""));

	LPCTSTR textUseItem1Index = _tcstok(0, separator);
	info.Item[0] = DWORD(_ttoi(textUseItem1Index ? textUseItem1Index : ""));

	LPCTSTR textUseItem2Index = _tcstok(0, separator);
	info.Item[1] = DWORD(_ttoi(textUseItem2Index ? textUseItem2Index : ""));

	LPCTSTR textMoney = _tcstok(0, separator);
	info.Money = _ttoi(textMoney ? textMoney : "");

	LPCTSTR textMana = _tcstok(0, separator);
	info.mMana.mPlus = float(_tstof(textMana ? textMana : ""));

	LPCTSTR textLife = _tcstok(0, separator);
	info.mLife.mPlus = float(_tstof(textLife ? textLife : ""));

	LPCTSTR textCount = _tcstok(0, separator);
	info.Count = _ttoi(textCount ? textCount : "");

	LPCTSTR textCountType = _tcstok(0, separator);
	info.CountType = eBuffSkillCountType(_ttoi(textCountType ? textCountType : ""));

	LPCTSTR textIsEndTime = _tcstok(0, separator);
	info.IsEndTime = _ttoi(textIsEndTime ? textIsEndTime : "");
}

void CGameResourceManager::Parse(LPCTSTR parsingLine, ACTIVE_SKILL_INFO& info) const
{
	TCHAR text[MAX_PATH] = {0};
	SafeStrCpy(
		text,
		parsingLine,
		sizeof(text) / sizeof(*text));
	LPCTSTR	separator = "\t";

	LPCTSTR textIndex = _tcstok(text, separator);
	info.Index = _ttoi(textIndex ? textIndex : "");

	LPCTSTR textName = _tcstok(0, separator);
	SafeStrCpy(
		info.Name,
		textName,
		sizeof(info.Name) / sizeof(*info.Name));

	LPCTSTR textLevel = _tcstok(0, separator);
	info.Level = WORD(_ttoi(textLevel ? textLevel : ""));

	LPCTSTR textImage = _tcstok(0, separator);
	info.Image = _ttoi(textImage ? textImage : "");

	LPCTSTR textToolTip = _tcstok(0, separator);
	info.Tooltip = _ttoi(textToolTip ? textToolTip : "");

	LPCTSTR textOperatorEffect = _tcstok(0, separator);
	info.OperatorEffect = _ttoi(textOperatorEffect ? textOperatorEffect : "");

	LPCTSTR textTargetEffect = _tcstok(0, separator);
	info.TargetEffect = _ttoi(textTargetEffect ? textTargetEffect : "");

	LPCTSTR textSkillEffect = _tcstok(0, separator);
	info.SkillEffect = _ttoi(textSkillEffect ? textSkillEffect : "");

	LPCTSTR textAnimationTime = _tcstok(0, separator);
	info.AnimationTime = _ttoi(textAnimationTime ? textAnimationTime : "");

	LPCTSTR textCastingTime = _tcstok(0, separator);
	info.CastingTime = _ttoi(textCastingTime ? textCastingTime : "");

	LPCTSTR textCoolTime = _tcstok(0, separator);
	info.CoolTime = _ttoi(textCoolTime ? textCoolTime : "");

	LPCTSTR textDelayTime = _tcstok(0, separator);
	info.DelayTime = _ttoi(textDelayTime ? textDelayTime : "");

	LPCTSTR textDelayType = _tcstok(0, separator);
	info.DelayType = WORD(_ttoi(textDelayType ? textDelayType : ""));

	LPCTSTR textBattle = _tcstok(0, separator);
	info.mIsBattle = _ttoi(textBattle ? textBattle : "");

	LPCTSTR textPeace = _tcstok(0, separator);
	info.mIsPeace = _ttoi(textPeace ? textPeace : "");

	LPCTSTR textMove = _tcstok(0, separator);
	info.mIsMove = _ttoi(textMove ? textMove : "");

	LPCTSTR textStop = _tcstok(0, separator);
	info.mIsStop = _ttoi(textStop ? textStop : "");

	// 091211 LUJ, 휴식 시 사용 여부. 사용하지 않음
	_tcstok(0, separator);

	LPCTSTR textRequiredBuff = _tcstok(0, separator);
	info.RequiredBuff = _ttoi(textRequiredBuff ? textRequiredBuff : "");

	LPCTSTR textTarget = _tcstok(0, separator);
	info.Target = WORD(_ttoi(textTarget ? textTarget : ""));

	LPCTSTR textKind = _tcstok(0, separator);
	info.Kind = SKILLKIND(_ttoi(textKind ? textKind : ""));

	// 091211 LUJ, 계열 값. 사용하지 않음
	_tcstok(0, separator);

	LPCTSTR textEquip = _tcstok(0, separator);

	// 091211 LUJ, 장비 제한 처리를 간단히 하기 위해 게임 내 쓰이는 enum 상수로 대체한다
	{
		// 090204 LUJ, 장비 속성 추가. EquipItem 배열 대신 사용함
		enum Equip
		{
			EquipNone,
			EquipWeaponOneHand,
			EquipWeaponTwoHand,
			EquipWeaponTwoBlade,
			EquipWeaponBow,
			EquipWeaponGun,
			EquipWeaponDagger,
			EquipWeaponSpear,
			EquipWeaponStaff,
			EquipWeaponShield,
			EquipArmorRobe,
			EquipArmorLeather,
			EquipArmorMetal,
			EquipWeaponSwordOneHand,
			EquipWeaponSwordTwoHand,
			EquipWeaponSwordAll,
			EquipWeaponMaceOneHand,
			EquipWeaponMaceTwoHand,
			EquipWeaponMaceAll,
			EquipWeaponAxeOneHand,
			EquipWeaponAxeTwoHand,
			EquipWeaponAxeAll,
			EquipWeaponBothDagger,
		};

		switch(Equip(_ttoi(textEquip ? textEquip : "")))
		{
		case EquipWeaponOneHand:
			{
				info.mWeaponAnimationType = eWeaponAnimationType_OneHand;
				break;
			}
		case EquipWeaponTwoHand:
			{
				info.mWeaponAnimationType = eWeaponAnimationType_TwoHand;
				break;
			}
		case EquipWeaponTwoBlade:
			{
				info.mWeaponAnimationType = eWeaponAnimationType_TwoBlade;
				break;
			}
		case EquipWeaponBow:
			{
				info.mWeaponType = eWeaponType_Bow;
				break;
			}
		case EquipWeaponGun:
			{
				info.mWeaponType = eWeaponType_Gun;
				break;
			}
		case EquipWeaponDagger:
			{
				info.mWeaponType = eWeaponType_Dagger;
				break;
			}
		case EquipWeaponSpear:
			{
				info.mWeaponAnimationType = eWeaponAnimationType_Spear;
				break;
			}
		case EquipWeaponStaff:
			{
				info.mWeaponType = eWeaponType_Staff;
				break;
			}
		case EquipWeaponShield:
			{
				info.mArmorType = eArmorType_Shield;
				break;
			}
		case EquipArmorRobe:
			{
				info.mArmorType = eArmorType_Robe;
				break;
			}
		case EquipArmorLeather:
			{
				info.mArmorType = eArmorType_Leather;
				break;
			}
		case EquipArmorMetal:
			{
				info.mArmorType = eArmorType_Metal;
				break;
			}
		case EquipWeaponSwordOneHand:
			{
				info.mWeaponType = eWeaponType_Sword;
				info.mWeaponAnimationType = eWeaponAnimationType_OneHand;
				break;
			}
		case EquipWeaponSwordTwoHand:
			{
				info.mWeaponType = eWeaponType_Sword;
				info.mWeaponAnimationType = eWeaponAnimationType_TwoHand;
				break;
			}
		case EquipWeaponSwordAll:
			{
				info.mWeaponType = eWeaponType_Sword;
				break;
			}
		case EquipWeaponMaceOneHand:
			{
				info.mWeaponType = eWeaponType_Mace;
				break;
			}
		case EquipWeaponMaceTwoHand:
			{
				info.mWeaponType = eWeaponType_Mace;
				info.mWeaponAnimationType = eWeaponAnimationType_TwoHand;
				break;
			}
		case EquipWeaponMaceAll:
			{
				info.mWeaponType = eWeaponType_Mace;
				break;
			}
		case EquipWeaponAxeOneHand:
			{
				info.mWeaponType = eWeaponType_Axe;
				info.mWeaponAnimationType = eWeaponAnimationType_OneHand;
				break;
			}
		case EquipWeaponAxeTwoHand:
			{
				info.mWeaponType = eWeaponType_Axe;
				info.mWeaponAnimationType = eWeaponAnimationType_TwoHand;
				break;
			}
		case EquipWeaponAxeAll:
			{
				info.mWeaponType = eWeaponType_Axe;
				break;
			}
		case EquipWeaponBothDagger:
			{
				info.mWeaponType = eWeaponType_BothDagger;
				break;
			}
		}
	}

	// 090204 LUJ, 무기2 항목. 사용하지 않음
	_tcstok(0, separator);

	LPCTSTR textArea = _tcstok(0, separator);
	info.Area = TARGET_KIND(_ttoi(textArea ? textArea : ""));

	LPCTSTR textAreaTarget = _tcstok(0, separator);
	info.AreaTarget = eSkillAreaTarget(_ttoi(textAreaTarget ? textAreaTarget : ""));

	LPCTSTR textAreaData = _tcstok(0, separator);
	info.AreaData = WORD(_ttoi(textAreaData ? textAreaData : ""));

	LPCTSTR textUnit = _tcstok(0, separator);
	info.Unit = UNITKIND(_ttoi(textUnit ? textUnit : ""));

	LPCTSTR textUnitDataType = _tcstok(0, separator);
	info.UnitDataType = WORD(_ttoi(textUnitDataType ? textUnitDataType : ""));

	LPCTSTR textUnitDataValue = _tcstok(0, separator);
	info.UnitData = _ttoi(textUnitDataValue ? textUnitDataValue : "");

	LPCTSTR textSuccess = _tcstok(0, separator);
	info.Success = WORD(_ttoi(textSuccess ? textSuccess : ""));

	LPCTSTR textCancel = _tcstok(0, separator);
	info.Cancel = WORD(_ttoi(textCancel ? textCancel : ""));

	LPCTSTR textRange = _tcstok(0, separator);
	info.Range = WORD(_ttoi(textRange ? textRange : ""));

	LPCTSTR textConsumeType = _tcstok(0, separator);
	info.mConsumePoint.mType = ACTIVE_SKILL_INFO::ConsumePoint::Type(_ttoi(textConsumeType ? textConsumeType : ""));

	LPCTSTR textConsumeValue = _tcstok(0, separator);
	info.mConsumePoint.mValue = _ttoi(textConsumeValue ? textConsumeValue : "");

	LPCTSTR textConsumeItemIndex = _tcstok(0, separator);
	info.mConsumeItem.wIconIdx = _ttoi(textConsumeItemIndex ? textConsumeItemIndex : "");

	LPCTSTR textConsumeItemQuantity = _tcstok(0, separator);
	info.mConsumeItem.Durability = _ttoi(textConsumeItemQuantity ? textConsumeItemQuantity : "");

	LPCTSTR textMoney = _tcstok(0, separator);
	info.Money = _ttoi(textMoney ? textMoney : "");

	LPCTSTR textMana = _tcstok(0, separator);

	if(textMana)
	{
		if('%' == textMana[_tcslen(textMana) - 1])
		{
			info.mMana.mPercent= float(_ttoi(textMana) / 100.0f);
		}
		else
		{
			info.mMana.mPlus = float(_ttoi(textMana));
		}
	}

	LPCTSTR textLife = _tcstok(0, separator);

	if(textLife)
	{
		if('%' == textLife[_tcslen(textLife) - 1])
		{
			info.mLife.mPercent= float(_ttoi(textLife) / 100.0f);
		}
		else
		{
			info.mLife.mPlus= float(_ttoi(textLife));
		}
	}

	LPCTSTR textTrainPoint = _tcstok(0, separator);
	info.TrainPoint = WORD(_ttoi(textTrainPoint ? textTrainPoint : ""));

	LPCTSTR textTrainMoney = _tcstok(0, separator);
	info.TrainMoney = _ttoi(textTrainMoney ? textTrainMoney : "");

	for(DWORD i = 0; i < MAX_BUFF_COUNT; ++i)
	{
		LPCTSTR textBuffIndex = _tcstok(0, separator);
		LPCTSTR textBuffRate = _tcstok(0, separator);

		if(0 == textBuffIndex)
		{
			continue;
		}
		else if(0 == textBuffRate)
		{
			continue;
		}

		info.Buff[i] = _ttoi(textBuffIndex ? textBuffIndex : "");
		info.BuffRate[i] = WORD(_ttoi(textBuffRate ? textBuffRate : ""));
	}

	LPCTSTR textIsMove = _tcstok(0, separator);
	info.IsMove = _ttoi(textIsMove ? textIsMove : "");

	LPCTSTR textRequiredPlayerLevel = _tcstok(0, separator);
	info.RequiredPlayerLevel = LEVELTYPE(_ttoi(textRequiredPlayerLevel ? textRequiredPlayerLevel : ""));
}

const EnchantScript* CGameResourceManager::GetEnchantScript(DWORD itemIndex) const
{
	EnchantScriptMap::const_iterator it = mEnchantScriptMap.find(
		itemIndex);

	return mEnchantScriptMap.end() == it ? 0 : &(it->second);
}


const EnchantScript* CGameResourceManager::GetEnchantScript(EWEARED_ITEM wearedItem) const
{
	SlotToEnchantScript::const_iterator it = mSlotToEnchantScript.find(
		wearedItem);

	return mSlotToEnchantScript.end() == it ? 0 : &(it->second);
}

void CGameResourceManager::LoadEnchantScript()
{
	CMHFile file;
	file.Init(
		"system/resource/itemEnchant.bin",
		"rb");

	enum Type
	{
		TypeNone,
		TypeProtectionAll,
		TypeResult,
		TypeSlot,
		TypeGoldRate,
		TypeLvBonusRate,
		TypeFailSetting,
		TypeSuccessRate,
	}
	type = TypeNone;

	while(FALSE == file.IsEOF())
	{
		char buffer[MAX_PATH * 5] = {0};
		file.GetLine(
			buffer,
			_countof(buffer));

		const int length = _tcslen(buffer);

		if(0 == length)
		{
			continue;
		}

		// 중간의 주석 제거
		for(int i = 0; i < length - 1; ++i)
		{
			const char a = buffer[ i ];
			const char b = buffer[ i + 1 ];

			if('/' == a &&
				'/' == b)
			{
				buffer[ i ] = 0;
				break;
			}
		}

		LPCTSTR separator = "~\n\t(), :%";
		LPCTSTR token = strtok(buffer, separator);

		if(0 == token)
		{
			continue;
		}
		else if(! stricmp("#protection", token))
		{
			type = TypeProtectionAll;
			continue;
		}
		else if(! stricmp("#result", token))
		{
			type = TypeResult;
			continue;
		}
		else if(! stricmp("#Slot", token))
		{
			type = TypeSlot;
			continue;
		}
		else if(! stricmp("#GoldRate", token))
		{
			type = TypeGoldRate;
			continue;
		}
		else if(! stricmp("#LevelBonusRate", token))
		{
			type = TypeLvBonusRate;
			continue;
		}
		else if(! stricmp("#FailSetting", token))
		{
			type = TypeFailSetting;
			continue;
		}
		else if(! stricmp("#SuccessRate", token))
		{
			type = TypeSuccessRate;
			continue;
		}

		switch(type)
		{
		case TypeProtectionAll:
			{
				const DWORD itemIndex = atoi(token);

				EnchantProtection::Data& data = mEnchantProtection.mItemMap[ itemIndex ];
				ZeroMemory(&data.mEnchantLevelRange,	sizeof(data.mEnchantLevelRange));
				ZeroMemory(&data.mItemLimitLevelRange, sizeof(data.mItemLimitLevelRange));

				data.mEnchantLevelRange.mMin = atoi(strtok(0, separator));
				data.mEnchantLevelRange.mMax = atoi(strtok(0, separator));
				data.mItemLimitLevelRange.mMin = atoi(strtok(0, separator));
				data.mItemLimitLevelRange.mMax = atoi(strtok(0, separator));
				data.mMinimumEnchantLevel = atoi(strtok(0, separator));
				data.mAddedPercent = int(atof(strtok(0, separator)) / 100.0f * 1000.0f);

				// 080328 LUJ, 인챈트 플러스/마이너스 보너스 확률 맵을 만든다
				for(float plusRate	= 0, minusRate = 0;;)
				{
					const char* token1	= strtok(0, separator);
					const char* token2	= strtok(0, separator);

					if(! token1 ||
						! token2)
					{
						break;
					}

					const int level = atoi(token1);
					const float	rate = float(atof(token2) / 100.0f);

					if(0 < level)
					{
						data.mPlusBonusMap[ plusRate += rate ] = level;
					}
					else if(0 > level)
					{
						data.mMinusBonusMap[ minusRate += rate ] = level;
					}
				}

				break;
			}
		case TypeResult:
			{
				const DWORD itemIndex = atoi(token);

				EnchantScript& script = mEnchantScriptMap[itemIndex];
				script.mItemIndex = itemIndex;
				script.mItemLevel.mMin = atoi(strtok(0, separator));
				script.mItemLevel.mMax = atoi(strtok(0, separator));
				script.mEnchantMaxLevel	= atoi(strtok(0, separator));

				for(DWORD abilitySize = 2;
					abilitySize--;
				)
					// 강화 가능한 능력 설정
				{
					token = strtok(0, separator);

					if(! strcmpi("strength", token))
					{
						script.mAbility.insert(EnchantScript::eTypeStrength);
					}
					else if(! strcmpi("dexterity", token))
					{
						script.mAbility.insert(EnchantScript::eTypeDexterity);
					}
					else if(! strcmpi("vitality", token))
					{
						script.mAbility.insert(EnchantScript::eTypeVitality);
					}
					else if(! strcmpi("intelligence", token))
					{
						script.mAbility.insert(EnchantScript::eTypeIntelligence);
					}
					else if(! strcmpi("wisdom", token))
					{
						script.mAbility.insert(EnchantScript::eTypeWisdom);
					}
					else if(! strcmpi("mana", token))
					{
						script.mAbility.insert(EnchantScript::eTypeMana);
					}
					else if(! strcmpi("recovery_mana", token))
					{
						script.mAbility.insert(EnchantScript::eTypeManaRecovery);
					}
					else if(! strcmpi("life", token))
					{
						script.mAbility.insert(EnchantScript::eTypeLife);
					}
					else if(! strcmpi("recovery_life", token))
					{
						script.mAbility.insert(EnchantScript::eTypeLifeRecovery);
					}
					else if(! strcmpi("physic_defense", token))
					{
						script.mAbility.insert(EnchantScript::eTypePhysicDefence);
					}
					else if(! strcmpi("physic_attack", token))
					{
						script.mAbility.insert(EnchantScript::eTypePhysicAttack);
					}
					else if(! strcmpi("magic_defense", token))
					{
						script.mAbility.insert(EnchantScript::eTypeMagicDefence);
					}
					else if(! strcmpi("magic_attack", token))
					{
						script.mAbility.insert(EnchantScript::eTypeMagicAttack);
					}
					else if(! strcmpi("move_speed", token))
					{
						script.mAbility.insert(EnchantScript::eTypeMoveSpeed);
					}
					else if(! strcmpi("evade", token))
					{
						script.mAbility.insert(EnchantScript::eTypeEvade);
					}
					else if(! strcmpi("accuracy", token))
					{
						script.mAbility.insert(EnchantScript::eTypeAccuracy);
					}
					else if(! strcmpi("critical_rate", token))
					{
						script.mAbility.insert(EnchantScript::eTypeCriticalRate);
					}
					else if(! strcmpi("critical_damage", token))
					{
						script.mAbility.insert(EnchantScript::eTypeCriticalDamage);
					}
					else if(! strcmpi("none", token))
					{
						// to do nothing
					}
					else
					{
						mEnchantScriptMap.erase(itemIndex);
					}
				}

				// 업글 가능한 아이템 종류 세팅
				{
					if(const std::string("1") == strtok(0, separator))
					{
						script.mSlot.insert(eWearedItem_Weapon);
					}

					if(const std::string("1") == strtok(0, separator))
					{
						script.mSlot.insert(eWearedItem_Shield);
					}

					if(const std::string("1") == strtok(0, separator))
					{
						script.mSlot.insert(eWearedItem_Dress);
					}

					if(const std::string("1") == strtok(0, separator))
					{
						script.mSlot.insert(eWearedItem_Hat);
					}

					if(const std::string("1") == strtok(0, separator))
					{
						script.mSlot.insert(eWearedItem_Glove);
					}

					if(const std::string("1") == strtok(0, separator))
					{
						script.mSlot.insert(eWearedItem_Shoes);
					}

					if(const std::string("1") == strtok(0, separator))
					{
						script.mSlot.insert(eWearedItem_Ring1);
						script.mSlot.insert(eWearedItem_Ring2);
					}

					if(const std::string("1") == strtok(0, separator))
					{
						script.mSlot.insert(eWearedItem_Necklace);
					}

					if(const std::string("1") == strtok(0, separator))
					{
						script.mSlot.insert(eWearedItem_Earring1);
						script.mSlot.insert(eWearedItem_Earring2);
					}

					if(const std::string("1") == strtok(0, separator))
					{
						script.mSlot.insert(eWearedItem_Belt);
					}

					if(const std::string("1") == strtok(0, separator))
					{
						script.mSlot.insert(eWearedItem_Band);
					}
				}

				break;
			}

		case TypeSlot:
			{
				EnchantScript::eType abilityType = EnchantScript::eType::eTypeNone;

				if(! strcmpi("physic_defense", token))
				{
					abilityType = EnchantScript::eTypePhysicDefence;
				}
				else if(! strcmpi("physic_attack", token))
				{
					abilityType = EnchantScript::eTypePhysicAttack;
				}
				else if(! strcmpi("magic_defense", token))
				{
					abilityType = EnchantScript::eTypeMagicDefence;
				}
				else if(! strcmpi("magic_attack", token))
				{
					abilityType = EnchantScript::eTypeMagicAttack;
				}
				else
				{
					break;
				}

				while((token = strtok(0, separator)) != NULL)
				{
					EWEARED_ITEM slot = EWEARED_ITEM( atoi(token) - 1 );
					EnchantScript& slotScript = mSlotToEnchantScript[ slot ];
					slotScript.mAbility.insert(abilityType);
				}

				break;
			}
		case TypeGoldRate:
			{
				m_EnchantGoldRateInfo.fLevelRate = float(atof(token));
				m_EnchantGoldRateInfo.fGradeRate = float(atof(strtok(0, separator)));
				m_EnchantGoldRateInfo.fBasicRate = float(atof(strtok(0, separator)));
				break;
			}
		case TypeLvBonusRate:
			{
				const LEVELTYPE minimumLevel = LEVELTYPE(atoi(token));
				const LEVELTYPE maxmiumLevel = LEVELTYPE(strtok(0, separator));

				stEnchantLvBonusRate& bonus = m_EnchantLvBonusInfoList[BYTE(maxmiumLevel)];
				bonus.minLevel = minimumLevel;
				bonus.maxLevel = maxmiumLevel;
				bonus.fEnchantLvRate1 = float(atof(strtok(0, separator)));
				bonus.fEnchantLvRate2 = float(atof(strtok(0, separator)));
				break;
			}

			// 090917 ShinJS --- 인챈트 실패시의 작업 설정 정보
		case TypeFailSetting:
			{
				const ITEM_INFO::eKind itemKind = ITEM_INFO::eKind(atoi(token));

				stEnchantFailSetting& setting = m_htEnchantFailSetting[itemKind];
				setting.dwItemKind = itemKind;
				setting.dwManusMinLv = LEVELTYPE(atoi(strtok(0, separator)));
				setting.dwManusMaxLv = LEVELTYPE(atoi(strtok(0, separator)));
				setting.dwZeroMinLv = LEVELTYPE(atoi(strtok(0, separator)));
				setting.dwZeroMaxLv = LEVELTYPE(atoi(strtok(0, separator)));
				setting.dwDeleteItemMinLv = LEVELTYPE(atoi(strtok(0, separator)));
				setting.dwDeleteItemMaxLv = LEVELTYPE(atoi(strtok(0, separator)));
				break;
			}
		case TypeSuccessRate:
			{
				const BYTE enchantLevel = BYTE(atoi(token));
				const float succuessRate = float(_tstof(strtok(0, separator)));

				m_mapEnchantSuccessRate[enchantLevel] = succuessRate * 10.0f;
				break;
			}
		}
	}
}

float CGameResourceManager::GetEnchantSuccessRate(BYTE enchantLevel) const
{
	const EnchantSuccessRateContainer::const_iterator iterator = m_mapEnchantSuccessRate.find(
		enchantLevel);

	return m_mapEnchantSuccessRate.end() == iterator ? 0 : iterator->second;
}

const stEnchantFailSetting* CGameResourceManager::GetEnchantFailSetting(ITEM_INFO::eKind kind) const
{
	const EnchantFailContainer::const_iterator iterator = m_htEnchantFailSetting.find(
		kind);

	return m_htEnchantFailSetting.end() == iterator ? 0 : &(iterator->second);
}

void CGameResourceManager::AddEnchantValue(PlayerStat& stat, const EnchantScript::Ability& ability, int value) const
{
	for(EnchantScript::Ability::const_iterator it = ability.begin();
		ability.end() != it;
		++it )
	{
		switch(EnchantScript::eType(*it))
		{
		case ReinforceScript::eTypeStrength:
			{
				stat.mStrength.mPlus += float(value);
				break;
			}
		case ReinforceScript::eTypeDexterity:
			{
				stat.mDexterity.mPlus += float(value);
				break;
			}
		case ReinforceScript::eTypeVitality:
			{
				stat.mVitality.mPlus += float(value);
				break;
			}
		case ReinforceScript::eTypeWisdom:
			{
				stat.mWisdom.mPlus += float(value);
				break;
			}
		case ReinforceScript::eTypeIntelligence:
			{
				stat.mIntelligence.mPlus += float(value);
				break;
			}
		case ReinforceScript::eTypeLife:
			{
				stat.mLife.mPlus += float(value);
				break;
			}
		case ReinforceScript::eTypeLifeRecovery:
			{
				stat.mRecoveryLife.mPlus += float(value);
				break;
			}
		case ReinforceScript::eTypeMana:
			{
				stat.mMana.mPlus += float(value);
				break;
			}
		case ReinforceScript::eTypeManaRecovery:
			{
				stat.mRecoveryLife.mPlus += float(value);
				break;
			}
		case ReinforceScript::eTypePhysicAttack:
			{
				stat.mPhysicAttack.mPlus += float(value);
				break;
			}
		case ReinforceScript::eTypePhysicDefence:
			{
				stat.mPhysicDefense.mPlus += float(value);
				break;
			}
		case ReinforceScript::eTypeMagicAttack:
			{
				stat.mMagicAttack.mPlus += float(value);
				break;
			}
		case ReinforceScript::eTypeMagicDefence:
			{
				stat.mMagicDefense.mPlus += float(value);
				break;
			}
		case ReinforceScript::eTypeMoveSpeed:
			{
				stat.mMoveSpeed.mPlus += float(value);
				break;
			}
		case ReinforceScript::eTypeEvade:
			{
				stat.mEvade.mPlus += float(value);
				break;
			}
		case ReinforceScript::eTypeAccuracy:
			{
				stat.mAccuracy.mPlus += float(value);
				break;
			}
		case ReinforceScript::eTypeCriticalRate:
			{
				stat.mCriticalRate.mPlus += float(value);
				break;
			}
		case ReinforceScript::eTypeCriticalDamage:
			{
				stat.mCriticalDamage.mPlus += float(value);
				break;
			}
		default:
			{
				OutputDebug(
					"invalid process: %s(%d)"
					__FUNCTION__,
					*it);
				break;
			}
		}
	}
}

void CGameResourceManager::AddEnchantValue(ITEM_OPTION& option, const EnchantScript::Ability& ability, int value) const
{
	for(EnchantScript::Ability::const_iterator it = ability.begin();
		ability.end() != it;
		++it)
	{
		ITEM_OPTION::Reinforce& reinforce = option.mReinforce;

		switch(EnchantScript::eType(*it))
		{
		case ReinforceScript::eTypeStrength:
			{
				reinforce.mStrength	+= value;
				break;
			}
		case ReinforceScript::eTypeDexterity:
			{
				reinforce.mDexterity += value;
				break;
			}
		case ReinforceScript::eTypeVitality:
			{
				reinforce.mVitality += value;
				break;
			}
		case ReinforceScript::eTypeWisdom:
			{
				reinforce.mWisdom += value;
				break;
			}
		case ReinforceScript::eTypeIntelligence:
			{
				reinforce.mIntelligence	+= value;
				break;
			}
		case ReinforceScript::eTypeLife:
			{
				reinforce.mLife	+= value;
				break;
			}
		case ReinforceScript::eTypeLifeRecovery:
			{
				reinforce.mLifeRecovery += value;
				break;
			}
		case ReinforceScript::eTypeMana:
			{
				reinforce.mMana	+= value;
				break;
			}
		case ReinforceScript::eTypeManaRecovery:
			{
				reinforce.mManaRecovery	+= value;
				break;
			}
		case ReinforceScript::eTypePhysicAttack:
			{
				reinforce.mPhysicAttack	+= value;
				break;
			}
		case ReinforceScript::eTypePhysicDefence:
			{
				reinforce.mPhysicDefence += value;
				break;
			}
		case ReinforceScript::eTypeMagicAttack:
			{
				reinforce.mMagicAttack += value;
				break;
			}
		case ReinforceScript::eTypeMagicDefence:
			{
				reinforce.mMagicDefence += value;
				break;
			}
		case ReinforceScript::eTypeMoveSpeed:
			{
				reinforce.mMoveSpeed += value;
				break;
			}
		case ReinforceScript::eTypeEvade:
			{
				reinforce.mEvade += value;
				break;
			}
		case ReinforceScript::eTypeAccuracy:
			{
				reinforce.mAccuracy += value;
				break;
			}
		case ReinforceScript::eTypeCriticalRate:
			{
				reinforce.mCriticalRate	+= value;
				break;
			}
		case ReinforceScript::eTypeCriticalDamage:
			{
				reinforce.mCriticalDamage += value;
				break;
			}
		default:
			{
				OutputDebug(
					"invalid process: %s(%d)"
					__FUNCTION__,
					*it);
				break;
			}
		}
	}
}

const stEnchantLvBonusRate& CGameResourceManager::GetEnchantLvBonusInfo(BYTE enchantLevel) const
{
	const EnchantBonusContainer::const_iterator iterator = m_EnchantLvBonusInfoList.lower_bound(
		enchantLevel);

	if(m_EnchantLvBonusInfoList.end() == iterator)
	{
		static stEnchantLvBonusRate emptyBonus = {0};

		return emptyBonus;
	}

	return iterator->second;
}

MONEYTYPE CGameResourceManager::GetEnchantCost(const ITEM_INFO& itemInfo) const
{
	const double base = pow(
		double(itemInfo.LimitLevel),
		2);

	return MONEYTYPE(base * m_EnchantGoldRateInfo.fLevelRate + m_EnchantGoldRateInfo.fBasicRate + base * m_EnchantGoldRateInfo.fLevelRate * (itemInfo.kind + m_EnchantGoldRateInfo.fGradeRate));
}

void CGameResourceManager::LoadDropOptionScript()
{
	CMHFile file;
	file.Init( "system/resource/ItemDropOption.bin", "rb" );

	typedef std::list< std::string > TextList;
	TextList textList;

	DropOptionScript::OptionMap* optionMap = 0;

	enum ScriptState
	{
		ScriptNone,
		ScriptBody,
		ScriptEnchant,
		ScriptEnchantLevel,
		ScriptOption,
		ScriptOptionSize,
		ScriptOptionLevel,
	}
	scriptState = ScriptNone;

	int openCount = 0;

	DropOptionScript* script = 0;

	while(FALSE == file.IsEOF())
	{
		char line[ 1024 ] = {0};	
		file.GetLine( line, sizeof( line ) );

		const int length = strlen( line );

		// 중간의 주석 제거
		for( int i = 0; i < length - 1; ++i )
		{
			const char a = line[ i ];
			const char b = line[ i + 1 ];

			if( '/' == a &&
				'/' == b )
			{
				line[ i ] = 0;
				break;
			}
		}

		textList.push_back( line );

		const char* separator	= " \n\t=,+\"~()";
		const char* token		= strtok( line, separator );
		const char* markBegin	= "{";
		const char* markEnd		= "}";

		if( ! token )
		{
			textList.pop_back();
		}
		// 새로운 클로저가 열리면 헤드를 파싱한다
		else if( 0 == strnicmp( markBegin, token, strlen( markBegin ) ) )
		{
			textList.pop_back();

			++openCount;

			switch( scriptState )
			{
			case ScriptNone:
				{
					mDropOptionScriptList.push_back( DropOptionScript() );

					script = &( mDropOptionScriptList.back() );
					scriptState = ScriptBody;

					for(TextList::const_iterator it = textList.begin();
						textList.end() != it;
						++it)
					{
						eWeaponType	weaponType = eWeaponType_None;
						EWEARED_ITEM equipSlot = eWearedItem_Max;

						SafeStrCpy( line, it->c_str(), sizeof( line ) );

						for(
							token = strtok( line, separator );
							token;
						token = strtok( 0, separator ) )
						{
							if( ! stricmp( token, "sword" ) )
							{
								equipSlot = eWearedItem_Weapon;
								weaponType = eWeaponType_Sword;
							}
							else if( ! stricmp( token, "axe" ) )
							{
								equipSlot = eWearedItem_Weapon;
								weaponType = eWeaponType_Axe;
							}
							else if( ! stricmp( token, "mace" ) )
							{
								equipSlot = eWearedItem_Weapon;
								weaponType = eWeaponType_Mace;								
							}
							else if( ! stricmp( token, "dagger" ) )
							{
								equipSlot = eWearedItem_Weapon;
								weaponType = eWeaponType_Dagger;

								mDropOptionScriptMap.insert( std::make_pair( GetItemKey( equipSlot, weaponType, eArmorType_None, eWeaponAnimationType_Dagger ), script ) );
								mDropOptionScriptMap.insert( std::make_pair( GetItemKey( equipSlot, weaponType, eArmorType_None, eWeaponAnimationType_BothDagger ), script ) );
							}
							else if( ! stricmp( token, "bow" ) )
							{
								equipSlot	= eWearedItem_Weapon;
								weaponType	= eWeaponType_Bow;

								mDropOptionScriptMap.insert( std::make_pair( GetItemKey( equipSlot, weaponType, eArmorType_None, eWeaponAnimationType_Bow ), script ) );
							}
							else if( ! stricmp( token, "gun" ) )
							{
								equipSlot	= eWearedItem_Weapon;
								weaponType	= eWeaponType_Gun;

								mDropOptionScriptMap.insert( std::make_pair( GetItemKey( equipSlot, weaponType, eArmorType_None, eWeaponAnimationType_Gun ), script ) );
							}
							else if( ! stricmp( token, "staff" ) )
							{
								equipSlot	= eWearedItem_Weapon;
								weaponType	= eWeaponType_Staff;

								mDropOptionScriptMap.insert( std::make_pair( GetItemKey( equipSlot, weaponType, eArmorType_None, eWeaponAnimationType_Staff ), script ) );
							}
							else if( ! stricmp( token, "two_blade" ) )
							{
								equipSlot	= eWearedItem_Weapon;
								weaponType	= eWeaponType_BothDagger;
							
								mDropOptionScriptMap.insert( std::make_pair( GetItemKey( equipSlot, weaponType, eArmorType_None, eWeaponAnimationType_TwoBlade ), script ) );
							}
							else if( ! stricmp( token, "armor" ) )
							{
								equipSlot = eWearedItem_Dress;
								weaponType = eWeaponType_None;
							}
							else if( ! stricmp( token, "helmet" ) )
							{
								equipSlot	= eWearedItem_Hat;
								weaponType	= eWeaponType_None;
							}
							else if( ! stricmp( token, "gloves" ) )
							{
								equipSlot	= eWearedItem_Glove;
								weaponType	= eWeaponType_None;
							}
							else if( ! stricmp( token, "shoes" ) )
							{
								equipSlot	= eWearedItem_Shoes;
								weaponType	= eWeaponType_None;
							}
							else if( ! stricmp( token, "shield" ) )
							{
								mDropOptionScriptMap.insert( std::make_pair( GetItemKey( eWearedItem_Shield, eWeaponType_None, eArmorType_Shield, eWeaponAnimationType_None ), script ) );
							}
							else if( ! stricmp( token, "ring" ) )
							{
								mDropOptionScriptMap.insert( std::make_pair( GetItemKey( eWearedItem_Ring1, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ), script ) );
								mDropOptionScriptMap.insert( std::make_pair( GetItemKey( eWearedItem_Ring2, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ), script ) );
							}
							else if( ! stricmp( token, "necklace" ) )
							{
								mDropOptionScriptMap.insert( std::make_pair( GetItemKey( eWearedItem_Necklace, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ), script ) );
							}
							else if( ! stricmp( token, "earring" ) )
							{
								mDropOptionScriptMap.insert( std::make_pair( GetItemKey( eWearedItem_Earring1, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ), script ) );
								mDropOptionScriptMap.insert( std::make_pair( GetItemKey( eWearedItem_Earring2, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ), script ) );
							}
							else if( ! stricmp( token, "belt" ) )
							{
								mDropOptionScriptMap.insert( std::make_pair( GetItemKey( eWearedItem_Belt, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ), script ) );
							}
							else if( ! stricmp( token, "band" ) )
							{
								mDropOptionScriptMap.insert( std::make_pair( GetItemKey( eWearedItem_Band, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ), script ) );
							}
							else if( ! stricmp( token, "glasses" ) )
							{
								mDropOptionScriptMap.insert( std::make_pair( GetItemKey( eWearedItem_Glasses, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ), script ) );
							}
							else if( ! stricmp( token, "mask" ) )
							{
								mDropOptionScriptMap.insert( std::make_pair( GetItemKey( eWearedItem_Mask, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ), script ) );
							}
							else if( ! stricmp( token, "costume_body" ) )
							{
								mDropOptionScriptMap.insert( std::make_pair( GetItemKey( eWearedItem_Costume_Dress, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ), script ) );
							}
							else if( ! stricmp( token, "costume_foot" ) )
							{
								mDropOptionScriptMap.insert( std::make_pair( GetItemKey( eWearedItem_Costume_Shoes, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ), script ) );
							}
							else if( ! stricmp( token, "costume_hand" ) )
							{
								mDropOptionScriptMap.insert( std::make_pair( GetItemKey( eWearedItem_Costume_Glove, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ), script ) );
							}
							else if( ! stricmp( token, "costume_head" ) )
							{
								mDropOptionScriptMap.insert( std::make_pair( GetItemKey( eWearedItem_Costume_Head, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ), script ) );
							}
							else if( ! stricmp( token, "wing" ) )
							{
								mDropOptionScriptMap.insert( std::make_pair( GetItemKey( eWearedItem_Wing, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ), script ) );
							}
							else if( ! stricmp( token, "one_hand" ) )
							{
								mDropOptionScriptMap.insert( std::make_pair( GetItemKey( equipSlot, weaponType, eArmorType_None, eWeaponAnimationType_OneHand ), script ) );
							}
							else if( ! stricmp( token, "two_hand" ) )
							{
								mDropOptionScriptMap.insert( std::make_pair( GetItemKey( equipSlot, weaponType, eArmorType_None, eWeaponAnimationType_TwoHand ), script ) );
							}
							else if( ! stricmp( token, "robe" ) )
							{
								mDropOptionScriptMap.insert( std::make_pair( GetItemKey( equipSlot, eWeaponType_None, eArmorType_Robe, eWeaponAnimationType_None ), script ) );
							}
							else if( ! stricmp( token, "light_armor" ) )
							{
								mDropOptionScriptMap.insert( std::make_pair( GetItemKey( equipSlot, eWeaponType_None, eArmorType_Leather, eWeaponAnimationType_None ), script ) );
							}
							else if( ! stricmp( token, "heavy_armor" ) )
							{
								mDropOptionScriptMap.insert( std::make_pair( GetItemKey( equipSlot, eWeaponType_None, eArmorType_Metal, eWeaponAnimationType_None ), script ) );
							}
							else if( ! stricmp( token, "all" ) )
							{
								switch( equipSlot )
								{
									// 무기
								case eWearedItem_Weapon:
									{
										mDropOptionScriptMap.insert( std::make_pair( GetItemKey( eWearedItem_Weapon, weaponType, eArmorType_None, eWeaponAnimationType_OneHand ), script ) );
										mDropOptionScriptMap.insert( std::make_pair( GetItemKey( eWearedItem_Weapon, weaponType, eArmorType_None, eWeaponAnimationType_TwoHand ), script ) );
										mDropOptionScriptMap.insert( std::make_pair( GetItemKey( eWearedItem_Weapon, weaponType, eArmorType_None, eWeaponAnimationType_TwoBlade ), script ) );
										mDropOptionScriptMap.insert( std::make_pair( GetItemKey( eWearedItem_Weapon, weaponType, eArmorType_None, eWeaponAnimationType_BothDagger ), script ) );
										break;
									}
								case eWearedItem_Dress:
								case eWearedItem_Hat:
								case eWearedItem_Glove:
								case eWearedItem_Shoes:
									{
										mDropOptionScriptMap.insert( std::make_pair( GetItemKey( equipSlot, eWeaponType_None, eArmorType_Robe, eWeaponAnimationType_None ), script ) );
										mDropOptionScriptMap.insert( std::make_pair( GetItemKey( equipSlot, eWeaponType_None, eArmorType_Leather, eWeaponAnimationType_None ), script ) );
										mDropOptionScriptMap.insert( std::make_pair( GetItemKey( equipSlot, eWeaponType_None, eArmorType_Metal, eWeaponAnimationType_None ), script ) );
										break;
									}
								}
							}
						}
					}

					scriptState = ScriptBody;
					break;
				}
			case ScriptBody:
				{
					for(TextList::const_iterator it = textList.begin();
						textList.end() != it;
						++it)
					{
						SafeStrCpy( line, it->c_str(), sizeof( line ) );

						token = strtok( line, separator );

						if( ! token )
						{
							continue;
						}
						else if( ! stricmp( token, "enchant" ) )
						{
							scriptState = ScriptEnchant;
						}
						else if( ! stricmp( token, "option") )
						{
							scriptState	= ScriptOption;
						}
					}

					break;
				}
			case ScriptEnchant:
				{
					for(TextList::const_iterator it = textList.begin();
						textList.end() != it;
						++it)
					{
						SafeStrCpy( line, it->c_str(), sizeof( line ) );

						token = strtok( line, separator );

						if( ! token )
						{
							continue;
						}
						else if( ! stricmp( token, "size" ) )
						{
							scriptState = ScriptEnchantLevel;
						}
					}

					break;
				}
			case ScriptOption:
				{
					for(TextList::const_iterator it = textList.begin();
						textList.end() != it;
						++it)
					{
						SafeStrCpy( line, it->c_str(), sizeof( line ) );

						token = strtok( line, separator );

						if( ! token )
						{
							continue;
						}
						else if( ! stricmp( token, "size" ) )
						{
							scriptState = ScriptOptionSize;
						}
						else if( ! stricmp( token, "level" ) )
						{
							scriptState = ScriptOptionLevel;

							const char* minLevel	= strtok( 0, separator );
							const char* maxLevel	= strtok( 0, separator );

							if( ! minLevel ||
								! maxLevel )
							{
								break;
							}

							optionMap = &( script->mLevelMap[ atoi( maxLevel ) ] );
						}
					}

					break;
				}
			}

			textList.clear();
		}
		else if( 0 == strnicmp( markEnd, token, strlen( markEnd ) ) )
		{
			textList.pop_back();

			--openCount;

			switch( scriptState )
			{
			case ScriptBody:
				{
					script = 0;
					scriptState	= ScriptNone;

					break;
				}
			case ScriptEnchant:
			case ScriptOption:
				{
					scriptState = ScriptBody;
					break;
				}
			case ScriptEnchantLevel:
				{
					scriptState = ScriptEnchant;

					float step = 0.0f;

					for(TextList::const_iterator it = textList.begin();
						textList.end() != it;
						++it)
					{
						SafeStrCpy( line, it->c_str(),  sizeof( line ) );

						const char* level	= strtok( line, separator );
						const float rate	= float( atof( strtok( 0, separator ) ) );

						if( ! level ||
							! rate )
						{
							continue;
						}

						step = step + rate / 100.0f;

						script->mEnchantLevelMap.insert( std::make_pair( step, atoi( level ) ) );
					}

					break;
				}
			case ScriptOptionSize:
				{
					scriptState = ScriptOption;

					float step = 0.0f;

					for(TextList::const_iterator it = textList.begin();
						textList.end() != it;
						++it)
					{
						SafeStrCpy( line, it->c_str(),  sizeof( line ) );

						const char* size	= strtok( line, separator );
						const float rate	= float( atof( strtok( 0, separator ) ) );

						if( ! size ||
							! rate )
						{
							continue;
						}

						step = step + rate / 100.0f;

						script->mOptionSizeMap.insert( std::make_pair( step, atoi( size ) ) );
					}

					break;
				}
			case ScriptOptionLevel:
				{
					scriptState = ScriptOption;

					DropOptionScript::Option option;
					ZeroMemory( &option, sizeof( option ) );

					float step = 0;

					for(TextList::const_iterator it = textList.begin();
						textList.end() != it;
						++it)
					{
						SafeStrCpy( line, it->c_str(),  sizeof( line ) );

						const char*	type		= strtok( line, separator );
						const float rate		= float( atof( strtok( 0,	separator ) ) );
						const char* beginValue	= strtok( 0,	separator );
						const char* endValue	= strtok( 0,	separator );

						if( ! type		||
							! rate		||
							! beginValue	||
							! endValue	)
						{
							continue;
						}

						step = step + rate / 100.0f;

						if( '%' == beginValue[ strlen( beginValue ) - 1 ] )
						{
							option.mBeginValue	= float( atof( beginValue ) ) / 100.0f;
						}
						else
						{
							option.mBeginValue	= float( atof( beginValue ) );
						}

						if( '%' == endValue[ strlen( endValue ) - 1 ] )
						{
							option.mEndValue	= float( atof( endValue ) ) / 100.0F;
						}
						else
						{
							option.mEndValue	= float( atof( endValue ) );
						}

						if( ! stricmp( type, "plus_strength" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusStrength;
						}
						else if( ! stricmp( type, "plus_intelligence" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusIntelligence;
						}
						else if( ! stricmp( type, "plus_dexterity" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusDexterity;
						}
						else if( ! stricmp( type, "plus_wisdom" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusWisdom;
						}
						else if( ! stricmp( type, "plus_vitality" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusVitality;
						}
						else if( ! stricmp( type, "plus_physic_attack" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusPhysicalAttack;
						}
						else if( ! stricmp( type, "plus_physic_defence" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusPhysicalDefence;
						}
						else if( ! stricmp( type, "plus_magic_attack" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusMagicalAttack;
						}
						else if( ! stricmp( type, "plus_magic_defence" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusMagicalDefence;
						}
						else if( ! stricmp( type, "plus_critical_rate" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusCriticalRate;
						}
						else if( ! stricmp( type, "plus_critical_damage" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusCriticalDamage;
						}
						else if( ! stricmp( type, "plus_accuracy" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusAccuracy;
						}
						else if( ! stricmp( type, "plus_evade" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusEvade;
						}
						else if( ! stricmp( type, "plus_move_speed" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusMoveSpeed;
						}
						else if( ! stricmp( type, "plus_life" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusLife;
						}
						else if( ! stricmp( type, "plus_mana" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusMana;
						}
						else if( ! stricmp( type, "plus_recovery_life" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusLifeRecovery;
						}
						else if( ! stricmp( type, "plus_recovery_mana" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusManaRecovery;
						}
						else if( ! stricmp( type, "percent_strength" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentStrength;
						}
						else if( ! stricmp( type, "percent_intelligence" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentIntelligence;
						}
						else if( ! stricmp( type, "percent_dexterity" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentDexterity;
						}
						else if( ! stricmp( type, "percent_wisdom" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentWisdom;
						}
						else if( ! stricmp( type, "percent_vitality" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentVitality;
						}
						else if( ! stricmp( type, "percent_physic_attack" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentPhysicalAttack;
						}
						else if( ! stricmp( type, "percent_physic_defence" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentPhysicalDefence;
						}
						else if( ! stricmp( type, "percent_magic_attack" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentMagicalAttack;
						}
						else if( ! stricmp( type, "percent_magic_defence" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentMagicalDefence;
						}
						else if( ! stricmp( type, "percent_critical_rate" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentCriticalRate;
						}
						else if( ! stricmp( type, "percent_critical_damage" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentCriticalDamage;
						}
						else if( ! stricmp( type, "percent_accuracy" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentAccuracy;
						}
						else if( ! stricmp( type, "percent_evade" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentEvade;
						}
						else if( ! stricmp( type, "percent_move_speed" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentMoveSpeed;
						}
						else if( ! stricmp( type, "percent_life" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentLife;
						}
						else if( ! stricmp( type, "percent_mana" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentMana;
						}
						else if( ! stricmp( type, "percent_recovery_life" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentLifeRecovery;
						}
						else if( ! stricmp( type, "percent_recovery_mana" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentManaRecovery;
						}

						optionMap->insert( std::make_pair( step, option ) );
					}

					break;
				}
			}

			textList.clear();
		}
	}
}

void CGameResourceManager::LoadApplyOptionScript()
{
	CMHFile file;
	file.Init( "system/resource/ItemApplyOption.bin", "rb" );

	typedef std::list< std::string > TextList;
	TextList textList;

	enum ScriptState
	{
		ScriptNone,
		ScriptItem,
		ScriptTarget,
		ScriptTargetLevel,
		ScriptTargetType,
		ScriptOption,
		ScriptOptionSize,
		ScriptOptionType,
	}
	scriptState = ScriptNone;

	int openCount = 0;
	ApplyOptionScript* script = 0;

	while(FALSE == file.IsEOF())
	{
		char line[ 1024 ] = {0};
		file.GetLine( line, sizeof( line ) );

		const int length = strlen( line );

		// 중간의 주석 제거
		for( int i = 0; i < length - 1; ++i )
		{
			const char a = line[ i ];
			const char b = line[ i + 1 ];

			if( '/' == a &&
				'/' == b )
			{
				line[ i ] = 0;
				break;
			}
		}

		textList.push_back( line );

		const char* separator	= " \n\t=,+\"~()";
		const char* token		= strtok( line, separator );
		const char* markBegin	= "{";
		const char* markEnd		= "}";

		if( ! token )
		{
			textList.pop_back();
		}
		// 새로운 클로저가 열리면 헤드를 파싱한다
		else if( 0 == strnicmp( markBegin, token, strlen( markBegin ) ) )
		{
			textList.pop_back();

			switch( scriptState )
			{
			case ScriptNone:
				{
					++openCount;

					scriptState = ScriptItem;

					for(TextList::const_iterator it = textList.begin();
						textList.end() != it;
						++it)
					{
						SafeStrCpy( line, it->c_str(), sizeof( line ) );

						token = strtok( line, separator );

						if( ! stricmp( token, "item" ) )
						{
							const char* itemIndex = strtok( 0, separator );

							script = &( mApplyOptionScriptMap[ atoi( itemIndex ) ] );

							ZeroMemory( &( script->mLevel ), sizeof( script->mLevel ) );
						}
					}

					break;
				}
			case ScriptItem:
				{
					for(TextList::const_iterator it = textList.begin();
						textList.end() != it;
						++it)
					{
						SafeStrCpy( line, it->c_str(), sizeof( line ) );

						token = strtok( line, separator );

						if( ! stricmp( token, "target" ) )
						{
							scriptState = ScriptTarget;
						}
						else if( ! stricmp( token, "option" ) )
						{
							scriptState = ScriptOption;
						}
					}

					break;
				}
			case ScriptTarget:
				{
					for(TextList::const_iterator it = textList.begin();
						textList.end() != it;
						++it)
					{
						SafeStrCpy( line, it->c_str(), sizeof( line ) );

						token = strtok( line, separator );

						if( ! stricmp( token, "level" ) )
						{
							scriptState = ScriptTargetLevel;
						}
						else if( ! stricmp( token, "type" ) )
						{
							scriptState = ScriptTargetType;
						}
					}

					break;
				}
			case ScriptOption:
				{
					for(TextList::const_iterator it = textList.begin();
						textList.end() != it;
						++it)
					{
						SafeStrCpy( line, it->c_str(), sizeof( line ) );

						token = strtok( line, separator );

						if( ! stricmp( token, "size" ) )
						{
							scriptState = ScriptOptionSize;
						}
						else if( ! stricmp( token, "type" ) )
						{
							scriptState = ScriptOptionType;
						}
					}

					break;
				}
			}

			textList.clear();
		}
		else if( 0 == strnicmp( markEnd, token, strlen( markEnd ) ) )
		{
			textList.pop_back();

			--openCount;

			switch( scriptState )
			{
			case ScriptItem:
				{
					script = 0;
					scriptState	= ScriptNone;
					break;
				}
			case ScriptTarget:
				{
					scriptState	= ScriptItem;
					break;
				}
			case ScriptOption:
				{
					scriptState = ScriptItem;
					break;
				}
			case ScriptTargetLevel:
				{
					scriptState	= ScriptTarget;

					for(TextList::const_iterator it = textList.begin();
						textList.end() != it;
						++it)
					{
						SafeStrCpy( line, it->c_str(),  sizeof( line ) );

						script->mLevel.mMin	= atoi( strtok( line, separator ) );
						script->mLevel.mMax	= atoi( strtok( 0, separator ) );
					}

					break;
				}
			case ScriptTargetType:
				{
					scriptState	= ScriptTarget;

					for(TextList::const_iterator it = textList.begin();
						textList.end() != it;
						++it)
					{
						eWeaponType	weaponType = eWeaponType_None;
						EWEARED_ITEM	equipSlot = eWearedItem_Max;

						SafeStrCpy( line, it->c_str(), sizeof( line ) );

						for(token = strtok( line, separator );
							token;
							token = strtok( 0, separator ) )
						{
							if( ! stricmp( token, "sword" ) )
							{
								equipSlot	= eWearedItem_Weapon;
								weaponType	= eWeaponType_Sword;

								script->mTargetTypeSet.insert( GetItemKey( equipSlot, eWeaponType_Sword, eArmorType_None, eWeaponAnimationType_None ) );
							}
							else if( ! stricmp( token, "axe" ) )
							{
								equipSlot	= eWearedItem_Weapon;
								weaponType	= eWeaponType_Axe;

								script->mTargetTypeSet.insert( GetItemKey( equipSlot, eWeaponType_Axe, eArmorType_None, eWeaponAnimationType_None ) );
							}
							else if( ! stricmp( token, "mace" ) )
							{
								equipSlot	= eWearedItem_Weapon;
								weaponType	= eWeaponType_Mace;

								script->mTargetTypeSet.insert( GetItemKey( equipSlot, eWeaponType_Mace, eArmorType_None, eWeaponAnimationType_None ) );
							}
							else if( ! stricmp( token, "dagger" ) )
							{
								equipSlot	= eWearedItem_Weapon;
								weaponType	= eWeaponType_Dagger;

								script->mTargetTypeSet.insert( GetItemKey( equipSlot, eWeaponType_Dagger, eArmorType_None, eWeaponAnimationType_None ) );
							}
							else if( ! stricmp( token, "bow" ) )
							{
								equipSlot	= eWearedItem_Weapon;
								weaponType	= eWeaponType_Bow;

								script->mTargetTypeSet.insert( GetItemKey( equipSlot, eWeaponType_Bow, eArmorType_None, eWeaponAnimationType_None ) );
							}
							// 080910 LUJ, 총 추가
							else if( ! stricmp( token, "gun" ) )
							{
								equipSlot	= eWearedItem_Weapon;
								weaponType	= eWeaponType_Gun;

								script->mTargetTypeSet.insert( GetItemKey( equipSlot, eWeaponType_Gun, eArmorType_None, eWeaponAnimationType_None ) );
							}
							// 100107 ONS, 마족무기 추가
							else if( ! stricmp( token, "dual_weapon" ) )
							{
								equipSlot	= eWearedItem_Weapon;
								weaponType	= eWeaponType_BothDagger;

								script->mTargetTypeSet.insert( GetItemKey( equipSlot, eWeaponType_BothDagger, eArmorType_None, eWeaponAnimationType_BothDagger ) );
							}
							else if( ! stricmp( token, "staff" ) )
							{
								equipSlot	= eWearedItem_Weapon;
								weaponType	= eWeaponType_Staff;

								script->mTargetTypeSet.insert( GetItemKey( equipSlot, eWeaponType_Staff, eArmorType_None, eWeaponAnimationType_None ) );
							}
							else if( ! stricmp( token, "armor" ) )
							{
								equipSlot	= eWearedItem_Dress;

								script->mTargetTypeSet.insert( GetItemKey( equipSlot, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ) );
							}
							else if( ! stricmp( token, "helmet" ) )
							{
								equipSlot	= eWearedItem_Hat;

								script->mTargetTypeSet.insert( GetItemKey( equipSlot, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ) );
							}
							else if( ! stricmp( token, "gloves" ) )
							{
								equipSlot	= eWearedItem_Glove;

								script->mTargetTypeSet.insert( GetItemKey( equipSlot, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ) );
							}
							else if( ! stricmp( token, "shoes" ) )
							{
								equipSlot	= eWearedItem_Shoes;

								script->mTargetTypeSet.insert( GetItemKey( equipSlot, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ) );
							}
							else if( ! stricmp( token, "shield" ) )
							{
								equipSlot	= eWearedItem_Shield;

								script->mTargetTypeSet.insert( GetItemKey( equipSlot, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ) );
							}
							else if( ! stricmp( token, "ring" ) )
							{
								equipSlot	= eWearedItem_Ring1;

								script->mTargetTypeSet.insert( GetItemKey( eWearedItem_Ring1, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ) );
								script->mTargetTypeSet.insert( GetItemKey( eWearedItem_Ring2, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ) );
							}
							else if( ! stricmp( token, "necklace" ) )
							{
								equipSlot	= eWearedItem_Necklace;

								script->mTargetTypeSet.insert( GetItemKey( equipSlot, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ) );
							}
							else if( ! stricmp( token, "earring" ) )
							{
								equipSlot	= eWearedItem_Earring1;

								script->mTargetTypeSet.insert( GetItemKey( eWearedItem_Earring1, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ) );
								script->mTargetTypeSet.insert( GetItemKey( eWearedItem_Earring2, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ) );
							}
							else if( ! stricmp( token, "belt" ) )
							{
								equipSlot	= eWearedItem_Belt;

								script->mTargetTypeSet.insert( GetItemKey( equipSlot, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ) );
							}
							else if( ! stricmp( token, "band" ) )
							{
								equipSlot	= eWearedItem_Band;

								script->mTargetTypeSet.insert( GetItemKey( equipSlot, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ) );
							}						
							else if( ! stricmp( token, "glasses" ) )
							{
								equipSlot	= eWearedItem_Glasses;

								script->mTargetTypeSet.insert( GetItemKey( equipSlot, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ) );
							}
							else if( ! stricmp( token, "mask" ) )
							{
								equipSlot	= eWearedItem_Mask;

								script->mTargetTypeSet.insert( GetItemKey( equipSlot, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ) );
							}
							else if( ! stricmp( token, "costume_body" ) )
							{
								equipSlot	= eWearedItem_Costume_Dress;

								script->mTargetTypeSet.insert( GetItemKey( equipSlot, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ) );
							}
							else if( ! stricmp( token, "costume_foot" ) )
							{
								equipSlot	= eWearedItem_Costume_Shoes;

								script->mTargetTypeSet.insert( GetItemKey( equipSlot, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ) );
							}
							else if( ! stricmp( token, "costume_hand" ) )
							{
								equipSlot	= eWearedItem_Costume_Glove;

								script->mTargetTypeSet.insert( GetItemKey( equipSlot, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ) );
							}
							else if( ! stricmp( token, "costume_head" ) )
							{
								equipSlot	= eWearedItem_Costume_Head;

								script->mTargetTypeSet.insert( GetItemKey( equipSlot, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ) );
							}
							// 080602 LUJ, 누락된 '날개' 아이템 설정 추가
							else if( ! stricmp( token, "wing" ) )
							{
								script->mTargetTypeSet.insert( GetItemKey( eWearedItem_Wing, eWeaponType_None, eArmorType_None, eWeaponAnimationType_None ) );
							}
							else if( ! stricmp( token, "one_hand" ) )
							{
								script->mTargetTypeSet.insert( GetItemKey( equipSlot, weaponType, eArmorType_None, eWeaponAnimationType_OneHand ) );
							}
							else if( ! stricmp( token, "two_hand" ) )
							{
								script->mTargetTypeSet.insert( GetItemKey( equipSlot, weaponType, eArmorType_None, eWeaponAnimationType_TwoHand ) );
							}
							else if( ! stricmp( token, "two_blade" ) )
							{
								script->mTargetTypeSet.insert( GetItemKey( equipSlot, weaponType, eArmorType_None, eWeaponAnimationType_TwoBlade ) );
							}
							else if( ! stricmp( token, "robe" ) )
							{
								script->mTargetTypeSet.insert( GetItemKey( equipSlot, weaponType, eArmorType_Robe, eWeaponAnimationType_None ) );
							}
							else if( ! stricmp( token, "light_armor" ) )
							{
								script->mTargetTypeSet.insert( GetItemKey( equipSlot, weaponType, eArmorType_Leather, eWeaponAnimationType_None ) );
							}
							else if( ! stricmp( token, "heavy_armor" ) )
							{
								script->mTargetTypeSet.insert( GetItemKey( equipSlot, weaponType, eArmorType_Metal, eWeaponAnimationType_None ) );
							}
							else if( ! stricmp( token, "all" ) )
							{
								switch( equipSlot )
								{
								case eWearedItem_Weapon:
									{
										script->mTargetTypeSet.insert( GetItemKey( eWearedItem_Weapon, weaponType, eArmorType_None, eWeaponAnimationType_OneHand ) );
										script->mTargetTypeSet.insert( GetItemKey( eWearedItem_Weapon, weaponType, eArmorType_None, eWeaponAnimationType_TwoHand ) );
										script->mTargetTypeSet.insert( GetItemKey( eWearedItem_Weapon, weaponType, eArmorType_None, eWeaponAnimationType_TwoBlade ) );
										script->mTargetTypeSet.insert( GetItemKey( eWearedItem_Weapon, weaponType, eArmorType_None, eWeaponAnimationType_Bow ) );
										script->mTargetTypeSet.insert( GetItemKey( eWearedItem_Weapon, weaponType, eArmorType_None, eWeaponAnimationType_Staff ) );
										script->mTargetTypeSet.insert( GetItemKey( eWearedItem_Weapon, weaponType, eArmorType_None, eWeaponAnimationType_Gun ) );
										script->mTargetTypeSet.insert( GetItemKey( eWearedItem_Weapon, weaponType, eArmorType_None, eWeaponAnimationType_Dagger ) );
										break;
									}
								case eWearedItem_Dress:
								case eWearedItem_Hat:
								case eWearedItem_Glove:
								case eWearedItem_Shoes:
									{
										script->mTargetTypeSet.insert( GetItemKey( equipSlot, eWeaponType_None, eArmorType_Robe, eWeaponAnimationType_None ) );
										script->mTargetTypeSet.insert( GetItemKey( equipSlot, eWeaponType_None, eArmorType_Leather, eWeaponAnimationType_None ) );
										script->mTargetTypeSet.insert( GetItemKey( equipSlot, eWeaponType_None, eArmorType_Metal, eWeaponAnimationType_None ) );
										break;
									}
								}
							}
						}
					}

					break;
				}
			case ScriptOptionSize:
				{
					scriptState	= ScriptOption;
					float step = 0.0f;

					for(TextList::const_iterator it = textList.begin();
						textList.end() != it;
						++it)
					{
						SafeStrCpy( line, it->c_str(),  sizeof( line ) );

						const char* size = strtok( line, separator );
						const float rate = float( atof( strtok( 0, separator ) ) );

						if( ! size ||
							! rate )
						{
							continue;
						}

						step = step + rate / 100.0f;

						script->mOptionSizeMap.insert( std::make_pair( step, atoi( size ) ) );
					}

					break;
				}
			case ScriptOptionType:
				{
					scriptState = ScriptOption;

					ApplyOptionScript::Option option;
					ZeroMemory( &option, sizeof( option ) );

					float step = 0;

					for(TextList::const_iterator it = textList.begin();
						textList.end() != it;
						++it)
					{
						SafeStrCpy( line, it->c_str(),  sizeof( line ) );

						const char* type		= strtok( line, separator );
						const float rate		= float( atof( strtok( 0,	separator ) ) );
						const char* beginValue	= strtok( 0,	separator );
						const char* endValue	= strtok( 0,	separator );

						if( ! type		||
							! rate		||
							! beginValue	||
							! endValue	)
						{
							continue;
						}

						step = step + rate / 100.0f;

						if( '%' == beginValue[ strlen( beginValue ) - 1 ] )
						{
							option.mBeginValue	= float( atof( beginValue ) ) / 100.0f;
						}
						else
						{
							option.mBeginValue	= float( atof( beginValue ) );
						}

						if( '%' == endValue[ strlen( endValue ) - 1 ] )
						{
							option.mEndValue	= float( atof( endValue ) ) / 100.0F;
						}
						else
						{
							option.mEndValue	= float( atof( endValue ) );
						}

						if( ! stricmp( type, "plus_strength" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusStrength;
						}
						else if( ! stricmp( type, "plus_intelligence" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusIntelligence;
						}
						else if( ! stricmp( type, "plus_dexterity" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusDexterity;
						}
						else if( ! stricmp( type, "plus_wisdom" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusWisdom;
						}
						else if( ! stricmp( type, "plus_vitality" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusVitality;
						}
						else if( ! stricmp( type, "plus_physic_attack" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusPhysicalAttack;
						}
						else if( ! stricmp( type, "plus_physic_defence" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusPhysicalDefence;
						}
						else if( ! stricmp( type, "plus_magic_attack" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusMagicalAttack;
						}
						else if( ! stricmp( type, "plus_magic_defence" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusMagicalDefence;
						}
						else if( ! stricmp( type, "plus_critical_rate" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusCriticalRate;
						}
						else if( ! stricmp( type, "plus_critical_damage" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusCriticalDamage;
						}
						else if( ! stricmp( type, "plus_accuracy" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusAccuracy;
						}
						else if( ! stricmp( type, "plus_evade" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusEvade;
						}
						else if( ! stricmp( type, "plus_move_speed" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusMoveSpeed;
						}
						else if( ! stricmp( type, "plus_life" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusLife;
						}
						else if( ! stricmp( type, "plus_mana" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusMana;
						}
						else if( ! stricmp( type, "plus_recovery_life" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusLifeRecovery;
						}
						else if( ! stricmp( type, "plus_recovery_mana" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPlusManaRecovery;
						}
						else if( ! stricmp( type, "percent_strength" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentStrength;
						}
						else if( ! stricmp( type, "percent_intelligence" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentIntelligence;
						}
						else if( ! stricmp( type, "percent_dexterity" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentDexterity;
						}
						else if( ! stricmp( type, "percent_wisdom" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentWisdom;
						}
						else if( ! stricmp( type, "percent_vitality" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentVitality;
						}
						else if( ! stricmp( type, "percent_physic_attack" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentPhysicalAttack;
						}
						else if( ! stricmp( type, "percent_physic_defence" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentPhysicalDefence;
						}
						else if( ! stricmp( type, "percent_magic_attack" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentMagicalAttack;
						}
						else if( ! stricmp( type, "percent_magic_defence" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentMagicalDefence;
						}
						else if( ! stricmp( type, "percent_critical_rate" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentCriticalRate;
						}
						else if( ! stricmp( type, "percent_critical_damage" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentCriticalDamage;
						}
						else if( ! stricmp( type, "percent_accuracy" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentAccuracy;
						}
						else if( ! stricmp( type, "percent_evade" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentEvade;
						}
						else if( ! stricmp( type, "percent_move_speed" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentMoveSpeed;
						}
						else if( ! stricmp( type, "percent_life" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentLife;
						}
						else if( ! stricmp( type, "percent_mana" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentMana;
						}
						else if( ! stricmp( type, "percent_recovery_life" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentLifeRecovery;
						}
						else if( ! stricmp( type, "percent_recovery_mana" ) )
						{
							option.mKey = ITEM_OPTION::Drop::KeyPercentManaRecovery;
						}

						script->mOptionTypeMap.insert( std::make_pair( step, option ) );
					}

					break;
				}
			}

			textList.clear();
		}
	}
}

LONGLONG CGameResourceManager::GetItemKey(EWEARED_ITEM slot, eWeaponType weapon, eArmorType armor, eWeaponAnimationType animation ) const
{
	const short key1	= short( slot );
	const short key2	= short( weapon );
	const short key3	= short( armor );
	const short key4	= short( animation );

	LONGLONG key = 0;
	memcpy(	( short* )&key + 0, &key1, sizeof( key1 ) );
	memcpy(	( short* )&key + 1, &key2, sizeof( key2 ) );
	memcpy(	( short* )&key + 2, &key3, sizeof( key3 ) );
	memcpy(	( short* )&key + 3, &key4, sizeof( key4 ) );

	return key;
}

const ApplyOptionScript& CGameResourceManager::GetApplyOptionScript(DWORD itemIndex) const
{
	const ApplyOptionScriptMap::const_iterator iterator = mApplyOptionScriptMap.find(
		itemIndex);

	static ApplyOptionScript emptyScript;
	return mApplyOptionScriptMap.end() == iterator ? emptyScript : iterator->second;
}

BOOL CGameResourceManager::IsApplyOptionItem(DWORD itemIndex) const
{
	return mApplyOptionScriptMap.end() != mApplyOptionScriptMap.find( itemIndex );
}

const DropOptionScript& CGameResourceManager::GetDropOptionScript(const ITEM_INFO& itemInfo) const
{
	const LONGLONG key = GetItemKey(
		itemInfo.EquipSlot,
		itemInfo.WeaponType,
		itemInfo.ArmorType,
		itemInfo.WeaponAnimation);

	static DropOptionScript emptyScript;
	const DropOptionScriptMap::const_iterator iterator = mDropOptionScriptMap.find(
		key);

	return mDropOptionScriptMap.end() == iterator ? emptyScript : *(iterator->second);
}

void CGameResourceManager::LoadReinforceScript()
{
	CMHFile file;
	file.Init("system/resource/itemReinforce.bin", "rb");

	enum Type
	{
		TypeNone,
		TypeProtection,
		TypeResult,
		// 080929 LUJ, 강화 보조
		TypeSupport,
	}
	type = TypeNone;

	while(FALSE == file.IsEOF())
	{
		char buffer[MAX_PATH * 5] = {0};
		file.GetLine(buffer, sizeof(buffer));

		const int length = _tcslen(buffer);

		if(! length)
		{
			continue;
		}

		// 중간의 주석 제거
		for(int i = 0; i < length - 1; ++i)
		{
			const char a = buffer[i];
			const char b = buffer[i + 1];

			if('/' == a &&
				'/' == b)
			{
				buffer[i] = 0;
				break;
			}
		}

		const char*	separator = "~\n\t(), ";
		const char*	token = strtok(buffer, separator);

		if(! token)
		{
			continue;
		}
		else if(! stricmp("#protection", token))
		{
			type = TypeProtection;
			continue;
		}
		else if(! stricmp("#result", token))
		{
			type = TypeResult;
			continue;
		}
		// 080929 LUJ, 보조 스크립트
		else if(! stricmp("#support", token))
		{
			type = TypeSupport;
			continue;
		}

		switch(type)
		{
		case TypeProtection:
			{
				const DWORD itemIndex = atoi(token);

				mReinforceProtectionItemSet.insert(itemIndex);
				break;
			}
		case TypeResult:
			{
				const DWORD itemIndex = atoi(token);
				ASSERTMSG(mReinforceScriptMap.end() == mReinforceScriptMap.find(itemIndex), "it's duplicated item index");

				ReinforceScript& script = mReinforceScriptMap[itemIndex];

				const std::string reinforce = strtok(0, separator);

				script.mMax		=		 atoi(strtok(0, separator));
				script.mBias	= float(atof(strtok(0, separator)));
				script.mForRare	= ! strcmpi("true", strtok(0, separator));

				// 강화 가능한 아이템 종류 세팅
				{
					if(const std::string("1") == strtok(0, separator))
					{
						script.mSlot.insert(eWearedItem_Weapon);
					}

					if(const std::string("1") == strtok(0, separator))
					{
						script.mSlot.insert(eWearedItem_Shield);
					}

					if(const std::string("1") == strtok(0, separator))
					{
						script.mSlot.insert(eWearedItem_Dress);
					}

					if(const std::string("1") == strtok(0, separator))
					{
						script.mSlot.insert(eWearedItem_Hat);
					}

					if(const std::string("1") == strtok(0, separator))
					{
						script.mSlot.insert(eWearedItem_Glove);
					}

					if(const std::string("1") == strtok(0, separator))
					{
						script.mSlot.insert(eWearedItem_Shoes);
					}

					if(const std::string("1") == strtok(0, separator))
					{
						script.mSlot.insert(eWearedItem_Ring1);
						script.mSlot.insert(eWearedItem_Ring2);
					}

					if(const std::string("1") == strtok(0, separator))
					{
						script.mSlot.insert(eWearedItem_Necklace);
					}

					if(const std::string("1") == strtok(0, separator))
					{
						script.mSlot.insert(eWearedItem_Earring1);
						script.mSlot.insert(eWearedItem_Earring2);
					}

					if(const std::string("1") == strtok(0, separator))
					{
						script.mSlot.insert(eWearedItem_Belt);
					}

					if(const std::string("1") == strtok(0, separator))
					{
						script.mSlot.insert(eWearedItem_Band);
					}
				}

				// 강화 가능한 능력 설정
				{
					if(! strcmpi("strength", reinforce.c_str()))
					{
						script.mType	= ReinforceScript::eTypeStrength;
					}
					else if(! strcmpi("dexterity", reinforce.c_str()))
					{
						script.mType	= ReinforceScript::eTypeDexterity;
					}
					else if(! strcmpi("vitality", reinforce.c_str()))
					{
						script.mType	= ReinforceScript::eTypeVitality;
					}
					else if(! strcmpi("intelligence", reinforce.c_str()))
					{
						script.mType	= ReinforceScript::eTypeIntelligence;
					}
					else if(! strcmpi("wisdom", reinforce.c_str()))
					{
						script.mType	= ReinforceScript::eTypeWisdom;
					}
					else if(! strcmpi("mana", reinforce.c_str()))
					{
						script.mType	= ReinforceScript::eTypeMana;
					}
					else if(! strcmpi("recovery_mana", reinforce.c_str()))
					{
						script.mType	= ReinforceScript::eTypeManaRecovery;
					}
					else if(! strcmpi("life", reinforce.c_str()))
					{
						script.mType	= ReinforceScript::eTypeLife;
					}
					else if(! strcmpi("recovery_life", reinforce.c_str()))
					{
						script.mType	= ReinforceScript::eTypeLifeRecovery;
					}
					else if(! strcmpi("physic_defense", reinforce.c_str()))
					{
						script.mType	= ReinforceScript::eTypePhysicDefence;
					}
					else if(! strcmpi("physic_attack", reinforce.c_str()))
					{
						script.mType	= ReinforceScript::eTypePhysicAttack;
					}
					else if(! strcmpi("magic_defense", reinforce.c_str()))
					{
						script.mType	= ReinforceScript::eTypeMagicDefence;
					}
					else if(! strcmpi("magic_attack", reinforce.c_str()))
					{
						script.mType	= ReinforceScript::eTypeMagicAttack;
					}
					else if(! strcmpi("move_speed", reinforce.c_str()))
					{
						script.mType	= ReinforceScript::eTypeMoveSpeed;
					}
					else if(! strcmpi("evade", reinforce.c_str()))
					{
						script.mType	= ReinforceScript::eTypeEvade;
					}
					else if(! strcmpi("accuracy", reinforce.c_str()))
					{
						script.mType	= ReinforceScript::eTypeAccuracy;
					}
					else if(! strcmpi("critical_rate", reinforce.c_str()))
					{
						script.mType	= ReinforceScript::eTypeCriticalRate;
					}
					else if(! strcmpi("critical_damage", reinforce.c_str()))
					{
						script.mType	= ReinforceScript::eTypeCriticalDamage;
					}
					else
					{
						ASSERTMSG(0, "itemReinforce.bin에 정의되어 있지 않은 키워드가 있습니다");

						mReinforceScriptMap.erase(itemIndex);
					}
				}

				break;
			}
			// 080929 LUJ, 강화 보조 스크립트 로딩
		case TypeSupport:
			{
				const char* itemIndex		= token;
				const char* minValue		= strtok(0, separator);
				const char* maxValue		= strtok(0, separator);
				const char* bonusMinValue	= strtok(0, separator);
				const char* bonusMaxValue	= strtok(0, separator);
				const char* bias			= strtok(0, separator);

				if(	! itemIndex		||
					! minValue		||
					! maxValue		||
					! bonusMinValue	||
					! bonusMaxValue	||
					! bias)
				{
					break;
				}

				ReinforceSupportScript& script = mReinforceSupportScriptMap[atoi(itemIndex)];
				script.mValueRange.mMin	= float(atof(minValue));
				script.mValueRange.mMax	= float(atof(maxValue));
				script.mBonusRange.mMin	= float(atof(bonusMinValue));
				script.mBonusRange.mMax	= float(atof(bonusMaxValue));
				script.mBias			= float(atof(bias));

				while((token = strtok(0, separator)) != NULL)
				{
					script.mMaterialSet.insert(atoi(token));
				}

				break;
			}
		}
	}
}

const ReinforceScript* CGameResourceManager::GetReinforceScript(DWORD itemIndex) const
{
	ReinforceScriptMap::const_iterator it = mReinforceScriptMap.find(itemIndex);

	return mReinforceScriptMap.end() == it ? 0 : &(it->second);
}

const ReinforceSupportScript* CGameResourceManager::GetReinforceSupportScript(DWORD itemIndex) const
{
	const ReinforceSupportScriptMap::const_iterator it = mReinforceSupportScriptMap.find(itemIndex);

	return mReinforceSupportScriptMap.end() == it ? 0 : &(it->second);
}

BOOL CGameResourceManager::IsReinforceProtectionItem(const ITEMBASE& item) const
{
	return mReinforceProtectionItemSet.end() != mReinforceProtectionItemSet.find(item.wIconIdx);
}

#ifdef _MAPSERVER_

void CGameResourceManager::GetRegenPoint(VECTOR3& pos,VECTOR3& RandPos,WORD LoginIndex,WORD MapchangeIndex)
{
	if(MapchangeIndex)
	{
		MAPCHANGE_INFO* changeinfo = GetMapChangeInfo(MapchangeIndex);
		if(!changeinfo)
		{
			char temp[128];
			sprintf(temp,"MapChangePoint is abnormal mappoint:%d,loginpoin:%d",MapchangeIndex,LoginIndex);
			ASSERTMSG(0,temp);
			pos.x = 25000;
			pos.y = 0;
			pos.z = 25000;
		}
		else
		{
			if(changeinfo->MoveMapNum != GAMERESRCMNGR->GetLoadMapNum())
				MapchangeIndex = 0;
			else
			{
				pos.x = changeinfo->MovePoint.x;
				pos.y = 0;
				pos.z = changeinfo->MovePoint.z;
			}
		}
	}
	if(MapchangeIndex == 0)
	{
		LOGINPOINT_INFO* LoginPoint = GetLoginPointInfoFromMapNum(GAMERESRCMNGR->GetLoadMapNum());//LoginIndex);
		if(!LoginPoint)
		{
			char temp[128];
			sprintf(temp,"LoginPoint is abnormal mappoint:%d,loginpoin:%d",MapchangeIndex,LoginIndex);
			ASSERTMSG(0,temp);
			pos.x = 25000;
			pos.y = 0;
			pos.z = 25000;
		}
		else
		{
			int rl = rand() % LoginPoint->MapLoginPointNum;
			pos.x = LoginPoint->CurPoint[rl].x;
			pos.y = 0;
			pos.z = LoginPoint->CurPoint[rl].z;
		}
	}

	int temp;
	temp = rand() % 500 - 250;
	RandPos.x = pos.x + temp;
	temp = rand() % 500 - 250;
	RandPos.z = pos.z + temp;
	RandPos.y = 0;

	ASSERT(!(pos.x == 0 && pos.z == 0));
}

void CGameResourceManager::GetRegenPointFromEventMap( VECTOR3& pos,VECTOR3& RandPos,WORD LoginIndex,WORD MapchangeIndex,DWORD dwTeam)
{
	if(MapchangeIndex)
	{
		MAPCHANGE_INFO* changeinfo = GetMapChangeInfo(MapchangeIndex);
		if(!changeinfo)
		{
			char temp[128];
			sprintf(temp,"MapChangePoint is abnormal mappoint:%d,loginpoin:%d",MapchangeIndex,LoginIndex);
			ASSERTMSG(0,temp);
			pos.x = 25000;
			pos.y = 0;
			pos.z = 25000;
		}
		else
		{
			if(changeinfo->MoveMapNum != GAMERESRCMNGR->GetLoadMapNum())
				MapchangeIndex = 0;
			else
			{
				pos.x = changeinfo->MovePoint.x;
				pos.y = 0;
				pos.z = changeinfo->MovePoint.z;
			}
		}
	}
	if(MapchangeIndex == 0)
	{
		LOGINPOINT_INFO* LoginPoint = GetLoginPointInfoFromMapNum(GAMERESRCMNGR->GetLoadMapNum());//LoginIndex);
		if(!LoginPoint)
		{
			char temp[128];
			sprintf(temp,"LoginPoint is abnormal mappoint:%d,loginpoin:%d",MapchangeIndex,LoginIndex);
			ASSERTMSG(0,temp);
			pos.x = 25000;
			pos.y = 0;
			pos.z = 25000;
		}
		else
		{
			int rl = dwTeam - 1;
			pos.x = LoginPoint->CurPoint[rl].x;
			pos.y = 0;
			pos.z = LoginPoint->CurPoint[rl].z;
		}
	}

	int temp;
	temp = rand() % 500 - 250;
	RandPos.x = pos.x + temp;
	temp = rand() % 500 - 250;
	RandPos.z = pos.z + temp;
	RandPos.y = 0;

	ASSERT(!(pos.x == 0 && pos.z == 0));
}

#endif

//-----------------------------------------------------------------------------------------------------------//
//		서버만 사용하는 부분
#ifdef _SERVER_RESOURCE_FIELD_
//-----------------------------------------------------------------------------------------------------------//
void CGameResourceManager::LoadRevivePoint()
{
	CMHFile file;
	WORD	mapnum = 0;
	float	posx = 0;
	float	posz = 0;

	if(!file.Init("./System/Resource/SafetyLoc.bin", "rb"))
		return;


	while(!file.IsEOF())
	{
		mapnum = file.GetWord();
		posx = file.GetFloat();
		posz = file.GetFloat();

		VECTOR3 vector3;

		vector3.x = posx;
		vector3.y = 0;
		vector3.z = posz;

		m_RevivePointMap[mapnum] = vector3;
	}

}

void CGameResourceManager::GetMonsterStats(WORD wMonsterKind, monster_stats * stats)
{
	const BASE_MONSTER_LIST* const baseMonsterList = GetMonsterListInfo(wMonsterKind);

	if(0 == baseMonsterList)
	{
		return;
	}

	stats->PhysicalAttackMin1 = baseMonsterList->AttackPhysicalMin;
	stats->PhysicalAttackMax1 = baseMonsterList->AttackPhysicalMax;
	stats->PhysicalDefense = baseMonsterList->PhysicalDefense;
}

void CGameResourceManager::SetLoadMapNum(WORD Mapnum)
{
	m_LoadingMapNum = Mapnum;
}

BOOL CGameResourceManager::IsInWareNpcPosition( VECTOR3* pvPos )
{
	float dist = 0.0f;
	STATIC_NPCINFO* pInfo = NULL;
	m_WareNpcTable.SetPositionHead();
	while( (pInfo = m_WareNpcTable.GetData() )!= NULL)
	{
		dist = CalcDistanceXZ( pvPos, &pInfo->vPos );
		if( dist <= 5000.0f )	return TRUE;
	}

	return FALSE;
}

//-----------------------------------------------------------------------------------------------------------//
#endif //_SERVER_RESOURCE_FIELD_
//-----------------------------------------------------------------------------------------------------------//
BOOL CGameResourceManager::LoadExpPoint()
{
	char filename[64];
	CMHFile file;
	sprintf(filename,"System/Resource/characterExpPoint.bin");	

	// 070411 LYW --- GameResourceManager : Check binary file.
	/*
	if(!file.Init(filename, "rb"))
		return FALSE;
		*/
	BOOL bResult = FALSE ;

	bResult = file.Init(filename, "rb") ;

	if( !bResult )
	{
		MessageBox(NULL,"Loading failed!! ./System/Resource/characterExpPoint.bin",0,0);

		return FALSE ;
	}


	int idx =0;
	while(1)
	{
		if(idx > MAX_CHARACTER_LEVEL_NUM-1) break;
		if(file.IsEOF())
			break;
		file.GetWord();												// 레벨
		// 080603 LYW --- GameResourceManager : 경험치 처리 (__int32)에서 (__int64)로 변경 처리. 
		//m_CharacterExpPoint[idx] = file.GetExpPoint();				// 경험치
		m_CharacterExpPoint[idx] = file.GetDwordEx() ;
		idx++;
	}

	file.Release();
	return TRUE;	
}

BOOL CGameResourceManager::LoadFishingExpPoint()
{
	char filename[64];
	CMHFile file;
	sprintf(filename,"System/Resource/FishingExpPoint.bin");	

	BOOL bResult = FALSE ;

	bResult = file.Init(filename, "rb") ;

	if( !bResult )
	{
		MessageBox(NULL,"Loading failed!! ./System/Resource/FishingExpPoint.bin",0,0);

		return FALSE ;
	}


	int idx =0;
	while(1)
	{
		if(idx > MAX_FISHING_LEVEL-1) break;
		if(file.IsEOF())
			break;
		file.GetWord();												// 레벨
		m_FishingExpPoint[idx] = file.GetExpPoint();				// 경험치
		m_FishingLevelUpRestrict[idx] = file.GetDword();			// 레벨제한
		m_FishingLevelUpReward[idx] = file.GetDword();				// 보상아이템
		idx++;
	}

	file.Release();
	return TRUE;	
}

BOOL CGameResourceManager::LoadPlayerxMonsterPoint()
{
	CMHFile file;
	int idx = 0;
	if( !file.Init( "System/Resource/PlayerxMonsterPoint.bin", "rb") )
		return FALSE;
	while(1)
	{
		if(idx > MAX_PLAYERLEVEL_NUM - 1) break;

		for(int levelgap = 0 ; levelgap < MAX_MONSTERLEVELPOINTRESTRICT_NUM + MONSTERLEVELRESTRICT_LOWSTARTNUM + 1 ; levelgap++)
		{
			m_PLAYERxMONSTER_POINT[idx][levelgap] = file.GetInt();
		}
		idx++;
	}
	file.Release();

	return TRUE;
}

EXPTYPE CGameResourceManager::GetPLAYERxMONSTER_POINT(LEVELTYPE level,int levelgap)
{
	ASSERT(0 < level && level < 255);	// 255 is temp Max Level of Mon
	ASSERT(-MONSTERLEVELRESTRICT_LOWSTARTNUM <= levelgap && 
		levelgap <= MAX_MONSTERLEVELPOINTRESTRICT_NUM );

	return m_PLAYERxMONSTER_POINT[level-1][levelgap+MONSTERLEVELRESTRICT_LOWSTARTNUM];
}

// 090227 ShinJS --- 클라이언트에서 StaticNpc.bin의 이동NPC 정보를 불러온다
void CGameResourceManager::LoadMoveNPCInfo()
{
	CMHFile file;
	if( !file.Init( "./System/Resource/StaticNpc.bin", "rb" ) )
		return;

	STATIC_NPCINFO Info;
	while( 1 )
	{
		if( file.IsEOF() )
			break;

		Info.MapNum = file.GetWord();
		Info.wNpcJob = file.GetWord();
		SafeStrCpy( Info.NpcName, file.GetString(), MAX_NAME_LENGTH+1 );
		Info.wNpcUniqueIdx = file.GetWord();
		Info.vPos.x = file.GetFloat();
		Info.vPos.y = 0.0f;
		Info.vPos.z = file.GetFloat();
		file.GetFloat();
		file.GetByte();

		// 이동NPC가 아닌경우
		if( 0 != Info.MapNum )
			continue;

		STATIC_NPCINFO* pInfo = new STATIC_NPCINFO;
		ASSERT( pInfo );
		if( !pInfo )	break;

		*pInfo = Info;

		m_htMoveNpcStaticInfo.Add( pInfo, pInfo->wNpcUniqueIdx );
	}

	file.Release();

	return;
}

void CGameResourceManager::LoadCharBaseStatusInfo()
{
	enum Block
	{
		BlockNone,
		BlockRace,
		BlockClass,
		BlockStatus,
	}
	blockType = BlockNone;

	DWORD dwOpenCnt = 0;
	BOOL bIsComment = FALSE;

	CharBaseStatusInfoKey curStatusKey;

	CMHFile file;
	file.Init( "./System/Resource/CharacterBaseStatus.bin", "rb" );

	while( ! file.IsEOF() )
	{
		char txt[ MAX_PATH ] = {0,};
		file.GetLine( txt, MAX_PATH );

		int txtLen = _tcslen( txt );

		// 중간 주석 제거
		for( int i=0 ; i<txtLen-1 ; ++i )
		{
			if( txt[i] == '/' && txt[i+1] == '/' )
			{
				txt[i] = 0;
				break;
			}
			else if( txt[i] == '/' && txt[i+1] == '*' )
			{
				txt[i] = 0;
				bIsComment = TRUE;
			}
			else if( txt[i] == '*' && txt[i+1] == '/' )
			{
				txt[i] = ' ';
				txt[i+1] = ' ';
				bIsComment = FALSE;
			}
			else if( bIsComment )
			{
				txt[i] = ' ';
			}
		}

		char buff[ MAX_PATH ] = {0,};
		SafeStrCpy( buff, txt, MAX_PATH );
		char* delimit = " \n\t=";
		char* markBegin = "{";
		char* markEnd = "}";
		char* token = strtok( buff, delimit );

		if( ! token )
		{
			continue;
		}
		else if( ! stricmp( "Human", token ) )
		{
			blockType = BlockRace;
			curStatusKey.first = RaceType_Human;
		}
		else if( ! stricmp( "Elf", token ) )
		{
			blockType = BlockRace;
			curStatusKey.first = RaceType_Elf;
		}
		else if( ! stricmp( "Devil", token ) )
		{
			// 마족은 클래스가 없다.
			blockType = BlockClass;
			curStatusKey.first = RaceType_Devil;
			curStatusKey.second = 4;
		}
		else if( ! strnicmp( markBegin, token, strlen( markBegin ) ) )
		{
			++dwOpenCnt;
		}
		else if( ! strnicmp( markEnd, token, strlen( markEnd ) ) )
		{
			--dwOpenCnt;

			switch( blockType )
			{
			case BlockRace:
				{
					blockType = BlockNone;
				}
				break;
			case BlockClass:
				{
					blockType = BlockRace;
				}
				break;
			}
		}

		switch( blockType )
		{
		case BlockRace:
			{
				if( ! stricmp( "Fighter", token ) )
				{
					blockType = BlockClass;
					curStatusKey.second = 1;
				}
				else if( ! stricmp( "Rogue", token ) )
				{
					blockType = BlockClass;
					curStatusKey.second = 2;
				}
				else if( ! stricmp( "Mage", token ) )
				{
					blockType = BlockClass;
					curStatusKey.second = 3;
				}
			}
			break;

		case BlockClass:
			{
				PlayerStat& stat = m_CharBaseStatusInfoMap[ curStatusKey ];
				
				if( ! stricmp( "str", token ) )
				{
					token = strtok( 0, delimit );
					if( !token )	break;
					stat.mStrength.mPlus = float( _tstof( token ) );
				}
				else if( ! stricmp( "dex", token ) )
				{
					token = strtok( 0, delimit );
					if( !token )	break;
					stat.mDexterity.mPlus = float( _tstof( token ) );
				}
				else if( ! stricmp( "vit", token ) )
				{
					token = strtok( 0, delimit );
					if( !token )	break;
					stat.mVitality.mPlus = float( _tstof( token ) );
				}
				else if( ! stricmp( "int", token ) )
				{
					token = strtok( 0, delimit );
					if( !token )	break;
					stat.mIntelligence.mPlus = float( _tstof( token ) );
				}
				else if( ! stricmp( "wis", token ) )
				{
					token = strtok( 0, delimit );
					if( !token )	break;
					stat.mWisdom.mPlus = float( _tstof( token ) );
				}
			}
			break;
		}
	}
}

const PlayerStat& CGameResourceManager::GetBaseStatus( RaceType race, BYTE byClass )
{
	static PlayerStat emptyPlayerStat;

	CharBaseStatusInfoKey key;
	key.first = race;
	key.second = byClass;

	CharBaseStatusInfoMap::const_iterator iter = m_CharBaseStatusInfoMap.find( key );
	if( iter == m_CharBaseStatusInfoMap.end() )
		return emptyPlayerStat;

	return iter->second;
}

//-----------------------------------------------------------------------------------------------------------//
//		클라이언트만 사용하는 부분
#ifdef _CLIENT_RESOURCE_FIELD_
//-----------------------------------------------------------------------------------------------------------//
void CGameResourceManager::DeletePreLoadData()
{
	//
	sPRELOAD_INFO* pPreLoadInfo = NULL;
	m_PreLoadDataTable.SetPositionHead();
	while( (pPreLoadInfo = m_PreLoadDataTable.GetData() )!= NULL)
	{
		delete pPreLoadInfo;		
	}
	m_PreLoadDataTable.RemoveAll();	

	PTRLISTSEARCHSTART(m_PreLoadEfflist, PRELOAD*, pEff);
	//		m_PreLoadEfflist.Remove(pEff);
	delete pEff;
	pEff = NULL;
	PTRLISTSEARCHEND
		m_PreLoadEfflist.RemoveAll();

	PTRLISTSEARCHSTART(m_PreLoadModlist, PRELOAD*, pMod);
	//		m_PreLoadModlist.Remove(pMod);
	delete pMod;
	pMod = NULL;
	PTRLISTSEARCHEND
		m_PreLoadModlist.RemoveAll();

	if(m_PreLoadItemlist)
		delete[] m_PreLoadItemlist;

	sPRELOAD_EFFECT_INFO* pPreLoadEffectInfo = NULL;
	m_PreLoadEffectTable.SetPositionHead();
	while( (pPreLoadEffectInfo = m_PreLoadEffectTable.GetData() )!= NULL)
	{
		PTRLISTSEARCHSTART(pPreLoadEffectInfo->Effect, PRELOAD*, pEffect);
		delete pEffect;
		pEffect = NULL;
		PTRLISTSEARCHEND
			pPreLoadEffectInfo->Effect.RemoveAll();
		delete pPreLoadEffectInfo;		
	}
	m_PreLoadEffectTable.RemoveAll();	

}


#include ".\Engine\EngineObject.h"		// for PreLoadObject()
void CGameResourceManager::PreLoadData()
{
	DIRECTORYMGR->SetLoadMode(eLM_Character);

	CEngineObject::PreLoadObject("H_Man.chx");
	CEngineObject::PreLoadObject("H_Woman.chx");
	CEngineObject::PreLoadObject("E_Man.chx");
	CEngineObject::PreLoadObject("E_Woman.chx");
	
	// 090527 ONS 신규종족 관련 처리 추가
	CEngineObject::PreLoadObject("D_Man.chx");
	CEngineObject::PreLoadObject("D_Woman.chx");

	DIRECTORYMGR->SetLoadMode(eLM_Root);
}

BOOL CGameResourceManager::LoadModFileList(MOD_LIST pModList[RaceType_Max][GENDER_MAX])
{
	CMHFile file;
	char filename[64];
	DWORD idx = 0;
	// 100128 ONS 마족 DualWeapon처리 추가 
	char buffer[MAX_OBJECTNAME_SIZE] = {0,};
	char RObjectFile[MAX_OBJECTNAME_SIZE] = {0,};
	m_DualWeaponModList.clear();

	//////////////////////////////////////////////////////////////////////////
	// 남자
	idx = 0;
	sprintf(filename,"System/Resource/ModList_H_M.bin");
	if(!file.Init(filename,"rb"))
		return FALSE;

	ZeroMemory(buffer, sizeof(buffer));
	// BaseObjectFile
	pModList[RaceType_Human][GENDER_MALE].BaseObjectFile = file.GetString();	

	pModList[RaceType_Human][GENDER_MALE].MaxModFile = file.GetDword();
	file.GetLine( buffer, sizeof( buffer ) );
	pModList[RaceType_Human][GENDER_MALE].ModFile = new StaticString[pModList[RaceType_Human][GENDER_MALE].MaxModFile];
	while(1)
	{
		if(idx >= pModList[RaceType_Human][GENDER_MALE].MaxModFile) break;
		// 100128 ONS 마족 DualWeapon처리 추가
		file.GetLine( buffer, sizeof( buffer ) );
		ZeroMemory(RObjectFile, sizeof(RObjectFile));
		ParseModFileName(buffer, RObjectFile, idx, RaceType_Human, GENDER_MALE);
		pModList[RaceType_Human][GENDER_MALE].ModFile[idx] = RObjectFile;

		++idx ;
	}		
	file.Release();


	//////////////////////////////////////////////////////////////////////////
	// 여자
	idx = 0;
	sprintf(filename,"System/Resource/ModList_H_W.bin");
	if(!file.Init(filename,"rb"))
		return FALSE;

	ZeroMemory(buffer, sizeof(buffer));
	// BaseObjectFile
	pModList[RaceType_Human][GENDER_FEMALE].BaseObjectFile = file.GetString();

	pModList[RaceType_Human][GENDER_FEMALE].MaxModFile = file.GetDword();
	file.GetLine( buffer, sizeof( buffer ) );
	pModList[RaceType_Human][GENDER_FEMALE].ModFile = new StaticString[pModList[RaceType_Human][GENDER_FEMALE].MaxModFile];
	while(1)
	{
		if(idx >= pModList[RaceType_Human][GENDER_FEMALE].MaxModFile) break;
		// 100128 ONS 마족 DualWeapon처리 추가
		file.GetLine( buffer, sizeof( buffer ) );
		ZeroMemory(RObjectFile, sizeof(RObjectFile));
		ParseModFileName(buffer, RObjectFile, idx, RaceType_Human, GENDER_MALE);
		pModList[RaceType_Human][GENDER_FEMALE].ModFile[idx] = RObjectFile;

		++idx;
	}		
	file.Release();


	//////////////////////////////////////////////////////////////////////////
	// 남자
	idx = 0;
	sprintf(filename,"System/Resource/ModList_E_M.bin");
	if(!file.Init(filename,"rb"))
		return FALSE;

	ZeroMemory(buffer, sizeof(buffer));
	// BaseObjectFile
	pModList[RaceType_Elf][GENDER_MALE].BaseObjectFile = file.GetString();	

	pModList[RaceType_Elf][GENDER_MALE].MaxModFile = file.GetDword();
	file.GetLine( buffer, sizeof( buffer ) );
	pModList[RaceType_Elf][GENDER_MALE].ModFile = new StaticString[pModList[RaceType_Elf][GENDER_MALE].MaxModFile];
	while(1)
	{
		if(idx >= pModList[RaceType_Elf][GENDER_MALE].MaxModFile) break;
		// 100128 ONS 마족 DualWeapon처리 추가
		file.GetLine( buffer, sizeof( buffer ) );
		ZeroMemory(RObjectFile, sizeof(RObjectFile));
		ParseModFileName(buffer, RObjectFile, idx, RaceType_Elf, GENDER_MALE);
		pModList[RaceType_Elf][GENDER_MALE].ModFile[idx] = RObjectFile;

		++idx;
	}		
	file.Release();


	//////////////////////////////////////////////////////////////////////////
	// 여자
	idx = 0;
	sprintf(filename,"System/Resource/ModList_E_W.bin");
	if(!file.Init(filename,"rb"))
		return FALSE;

	ZeroMemory(buffer, sizeof(buffer));
	// BaseObjectFile
	pModList[RaceType_Elf][GENDER_FEMALE].BaseObjectFile = file.GetString();

	pModList[RaceType_Elf][GENDER_FEMALE].MaxModFile = file.GetDword();
	file.GetLine( buffer, sizeof( buffer ) );
	pModList[RaceType_Elf][GENDER_FEMALE].ModFile = new StaticString[pModList[RaceType_Elf][GENDER_FEMALE].MaxModFile];
	while(1)
	{
		if(idx >= pModList[RaceType_Elf][GENDER_FEMALE].MaxModFile) break;
		// 100128 ONS 마족 DualWeapon처리 추가
		file.GetLine( buffer, sizeof( buffer ) );
		ZeroMemory(RObjectFile, sizeof(RObjectFile));
		ParseModFileName(buffer, RObjectFile, idx, RaceType_Elf, GENDER_MALE);
		pModList[RaceType_Elf][GENDER_FEMALE].ModFile[idx] = RObjectFile;

		++idx;
	}		
	file.Release();

	// 090504 ONS 신규종족 추가작업
	//////////////////////////////////////////////////////////////////////////
	// 남자
	idx = 0;
	sprintf(filename,"System/Resource/ModList_D_M.bin");
	if(!file.Init(filename,"rb"))
		return FALSE;

	ZeroMemory(buffer, sizeof(buffer));
	// BaseObjectFile
	pModList[RaceType_Devil][GENDER_MALE].BaseObjectFile = file.GetString();	
	pModList[RaceType_Devil][GENDER_MALE].MaxModFile = file.GetDword();
	file.GetLine( buffer, sizeof( buffer ) );
	pModList[RaceType_Devil][GENDER_MALE].ModFile = new StaticString[pModList[RaceType_Devil][GENDER_MALE].MaxModFile];
	while(1)
	{
		if(idx >= pModList[RaceType_Devil][GENDER_MALE].MaxModFile) break;

		// 100128 ONS 마족 DualWeapon처리 추가
		file.GetLine( buffer, sizeof( buffer ) );
		ZeroMemory(RObjectFile, sizeof(RObjectFile));
		ParseModFileName(buffer, RObjectFile, idx, RaceType_Devil, GENDER_MALE);
		pModList[RaceType_Devil][GENDER_MALE].ModFile[idx] = RObjectFile;

		++idx ;
	}		
	file.Release();


	//////////////////////////////////////////////////////////////////////////
	// 여자
	idx = 0;
	sprintf(filename,"System/Resource/ModList_D_W.bin");
	if(!file.Init(filename,"rb"))
		return FALSE;

	ZeroMemory(buffer, sizeof(buffer));
	// BaseObjectFile
	pModList[RaceType_Devil][GENDER_FEMALE].BaseObjectFile = file.GetString();
	pModList[RaceType_Devil][GENDER_FEMALE].MaxModFile = file.GetDword();
	file.GetLine( buffer, sizeof( buffer ) );
	pModList[RaceType_Devil][GENDER_FEMALE].ModFile = new StaticString[pModList[RaceType_Devil][GENDER_FEMALE].MaxModFile];
	while(1)
	{
		if(idx >= pModList[RaceType_Devil][GENDER_FEMALE].MaxModFile) break;
		// 100128 ONS 마족 DualWeapon처리 추가
		file.GetLine( buffer, sizeof( buffer ) );
		ZeroMemory(RObjectFile, sizeof(RObjectFile));
		ParseModFileName(buffer, RObjectFile, idx, RaceType_Devil, GENDER_FEMALE);
		pModList[RaceType_Devil][GENDER_FEMALE].ModFile[idx] = RObjectFile;

		++idx;
	}		
	file.Release();

	return TRUE;
}

// 100128 ONS DualWeapon의 경우 mod(chx)파일을 두개로 지정하여 양손에 각각 attach시킨다.
// 첫번째 파일명은 기존의 m_ModFileList에 저장하고, 두번째 파일명을 따로 관리하여 사용한다
void CGameResourceManager::ParseModFileName( char* pInputFile, char* pRObjectName, DWORD wIndex, BYTE byRaceType, BYTE byGender )
{
	const char* separator = " /";
	char* pString = strtok( pInputFile, separator );
	if( !pString )
	{
#ifdef _GMTOOL_
		char tempBuf[128] = {0, } ;
		sprintf(tempBuf, "Failed to ModList Parsing!!(Race:%d, Gender:%d, Index:%d)", byRaceType, byGender, wIndex ) ;
		MessageBox(NULL, tempBuf, "Load ModList", MB_OK);
#endif
		return;
	}
	strcpy(pRObjectName, pString);

	pString = strtok(NULL, separator);
	if( pString )
	{
		m_DualWeaponModList[std::make_pair(wIndex, byRaceType*10+byGender)] = std::string( pString );
	}
}

// 100128 ONS DualWeapon의 경우 왼쪽손에 Attach시킬 mod(chx)파일을 가져온다.
const char* CGameResourceManager::GetDualWeaponLObject( WORD wIndex, BYTE byRaceType, BYTE byGender )
{
	DualWeaponModListMap::iterator it = m_DualWeaponModList.find(std::make_pair(wIndex, byRaceType*10+byGender));
	return (m_DualWeaponModList.end() == it ) ? NULL : (it->second).c_str();
}

BOOL CGameResourceManager::LoadFaceModFileList(MOD_LIST pFaceModList[RaceType_Max][GENDER_MAX]) //pjscode
{
	//얼굴 관련
	CMHFile file;
	char filename[64];
	DWORD idx = 0;

	//////////////////////////////////////////////////////////////////////////
	// 남자
	idx = 0;
	sprintf(filename,"System/Resource/FaceList_H_M.bin");		
	if(!file.Init(filename,"rb"))
		return FALSE;

	pFaceModList[RaceType_Human][GENDER_MALE].MaxModFile = file.GetDword();
	pFaceModList[RaceType_Human][GENDER_MALE].ModFile = new StaticString[pFaceModList[RaceType_Human][GENDER_MALE].MaxModFile];

	while(1)
	{
		if(idx >= pFaceModList[RaceType_Human][GENDER_MALE].MaxModFile) break;
		pFaceModList[RaceType_Human][GENDER_MALE].ModFile[idx] = file.GetString();

		++idx;
	}		
	file.Release();


	//////////////////////////////////////////////////////////////////////////
	// 여자
	idx = 0;
	sprintf(filename,"System/Resource/FaceList_H_W.bin");
	if(!file.Init(filename,"rb"))
		return FALSE;

	pFaceModList[RaceType_Human][GENDER_FEMALE].MaxModFile = file.GetDword();
	pFaceModList[RaceType_Human][GENDER_FEMALE].ModFile = new StaticString[pFaceModList[RaceType_Human][GENDER_FEMALE].MaxModFile];


	while(1)
	{
		if(idx >= pFaceModList[RaceType_Human][GENDER_FEMALE].MaxModFile) break;
		pFaceModList[RaceType_Human][GENDER_FEMALE].ModFile[idx] = file.GetString();

		++idx;
	}		
	file.Release();


	//////////////////////////////////////////////////////////////////////////
	// 남자
	idx = 0;
	sprintf(filename,"System/Resource/FaceList_E_M.bin");		
	if(!file.Init(filename,"rb"))
		return FALSE;

	pFaceModList[RaceType_Elf][GENDER_MALE].MaxModFile = file.GetDword();
	pFaceModList[RaceType_Elf][GENDER_MALE].ModFile = new StaticString[pFaceModList[RaceType_Elf][GENDER_MALE].MaxModFile];

	while(1)
	{
		if(idx >= pFaceModList[RaceType_Elf][GENDER_MALE].MaxModFile) break;
		pFaceModList[RaceType_Elf][GENDER_MALE].ModFile[idx] = file.GetString();

		++idx;
	}		
	file.Release();


	//////////////////////////////////////////////////////////////////////////
	// 여자
	idx = 0;
	sprintf(filename,"System/Resource/FaceList_E_W.bin");
	if(!file.Init(filename,"rb"))
		return FALSE;

	pFaceModList[RaceType_Elf][GENDER_FEMALE].MaxModFile = file.GetDword();
	pFaceModList[RaceType_Elf][GENDER_FEMALE].ModFile = new StaticString[pFaceModList[RaceType_Elf][GENDER_FEMALE].MaxModFile];


	while(1)
	{
		if(idx >= pFaceModList[RaceType_Elf][GENDER_FEMALE].MaxModFile) break;
		pFaceModList[RaceType_Elf][GENDER_FEMALE].ModFile[idx] = file.GetString();

		++idx;
	}		
	file.Release();

	// 090504 ONS 신규종족 추가작업
	//////////////////////////////////////////////////////////////////////////
	// 남자
	idx = 0;
	sprintf(filename,"System/Resource/FaceList_D_M.bin");		
	if(!file.Init(filename,"rb"))
		return FALSE;

	pFaceModList[RaceType_Devil][GENDER_MALE].MaxModFile = file.GetDword();
	pFaceModList[RaceType_Devil][GENDER_MALE].ModFile = new StaticString[pFaceModList[RaceType_Devil][GENDER_MALE].MaxModFile];

	while(1)
	{
		if(idx >= pFaceModList[RaceType_Devil][GENDER_MALE].MaxModFile) break;
		pFaceModList[RaceType_Devil][GENDER_MALE].ModFile[idx] = file.GetString();

		++idx;
	}		
	file.Release();


	//////////////////////////////////////////////////////////////////////////
	// 여자
	idx = 0;
	sprintf(filename,"System/Resource/FaceList_D_W.bin");
	if(!file.Init(filename,"rb"))
		return FALSE;

	pFaceModList[RaceType_Devil][GENDER_FEMALE].MaxModFile = file.GetDword();
	pFaceModList[RaceType_Devil][GENDER_FEMALE].ModFile = new StaticString[pFaceModList[RaceType_Devil][GENDER_FEMALE].MaxModFile];


	while(1)
	{
		if(idx >= pFaceModList[RaceType_Devil][GENDER_FEMALE].MaxModFile) break;
		pFaceModList[RaceType_Devil][GENDER_FEMALE].ModFile[idx] = file.GetString();

		++idx;
	}		
	file.Release();

	return TRUE;
}

BOOL CGameResourceManager::LoadBodyModFileList(MOD_LIST pBodyModList[RaceType_Max][GENDER_MAX])
{
	CMHFile file;
	char filename[64];
	DWORD idx = 0;

	//////////////////////////////////////////////////////////////////////////
	// 남자
	idx = 0;
	sprintf(filename,"System/Resource/BodyList_H_M.bin");		
	if(!file.Init(filename,"rb"))
		return FALSE;
	pBodyModList[RaceType_Human][GENDER_MALE].MaxModFile = file.GetDword();
	pBodyModList[RaceType_Human][GENDER_MALE].ModFile = new StaticString[pBodyModList[RaceType_Human][GENDER_MALE].MaxModFile];

	while(1)
	{
		if(idx >= pBodyModList[RaceType_Human][GENDER_MALE].MaxModFile) break;
		pBodyModList[RaceType_Human][GENDER_MALE].ModFile[idx] = file.GetString();

		++idx;
	}		
	file.Release();


	//////////////////////////////////////////////////////////////////////////
	// 여자
	idx = 0;
	sprintf(filename,"System/Resource/BodyList_H_W.bin");
	if(!file.Init(filename,"rb"))
		return FALSE;

	pBodyModList[RaceType_Human][GENDER_FEMALE].MaxModFile = file.GetDword();
	pBodyModList[RaceType_Human][GENDER_FEMALE].ModFile = new StaticString[pBodyModList[RaceType_Human][GENDER_FEMALE].MaxModFile];


	while(1)
	{
		if(idx >= pBodyModList[RaceType_Human][GENDER_FEMALE].MaxModFile) break;
		pBodyModList[RaceType_Human][GENDER_FEMALE].ModFile[idx] = file.GetString();

		++idx;
	}		
	file.Release();

	//////////////////////////////////////////////////////////////////////////
	// 남자
	idx = 0;
	sprintf(filename,"System/Resource/BodyList_E_M.bin");		
	if(!file.Init(filename,"rb"))
		return FALSE;
	pBodyModList[RaceType_Elf][GENDER_MALE].MaxModFile = file.GetDword();
	pBodyModList[RaceType_Elf][GENDER_MALE].ModFile = new StaticString[pBodyModList[RaceType_Elf][GENDER_MALE].MaxModFile];

	while(1)
	{
		if(idx >= pBodyModList[RaceType_Elf][GENDER_MALE].MaxModFile) break;
		pBodyModList[RaceType_Elf][GENDER_MALE].ModFile[idx] = file.GetString();

		++idx;
	}		
	file.Release();


	//////////////////////////////////////////////////////////////////////////
	// 여자
	idx = 0;
	sprintf(filename,"System/Resource/BodyList_E_W.bin");
	if(!file.Init(filename,"rb"))
		return FALSE;

	pBodyModList[RaceType_Elf][GENDER_FEMALE].MaxModFile = file.GetDword();
	pBodyModList[RaceType_Elf][GENDER_FEMALE].ModFile = new StaticString[pBodyModList[RaceType_Elf][GENDER_FEMALE].MaxModFile];


	while(1)
	{
		if(idx >= pBodyModList[RaceType_Elf][GENDER_FEMALE].MaxModFile) break;
		pBodyModList[RaceType_Elf][GENDER_FEMALE].ModFile[idx] = file.GetString();

		++idx;
	}		
	file.Release();

	// 090504 ONS 신규종족 추가작업
	//////////////////////////////////////////////////////////////////////////
	// 남자
	idx = 0;
	sprintf(filename,"System/Resource/BodyList_D_M.bin");		
	if(!file.Init(filename,"rb"))
		return FALSE;
	pBodyModList[RaceType_Devil][GENDER_MALE].MaxModFile = file.GetDword();
	pBodyModList[RaceType_Devil][GENDER_MALE].ModFile = new StaticString[pBodyModList[RaceType_Devil][GENDER_MALE].MaxModFile];

	while(1)
	{
		if(idx >= pBodyModList[RaceType_Devil][GENDER_MALE].MaxModFile) break;
		pBodyModList[RaceType_Devil][GENDER_MALE].ModFile[idx] = file.GetString();

		++idx;
	}		
	file.Release();


	//////////////////////////////////////////////////////////////////////////
	// 여자
	idx = 0;
	sprintf(filename,"System/Resource/BodyList_D_W.bin");
	if(!file.Init(filename,"rb"))
		return FALSE;
	pBodyModList[RaceType_Devil][GENDER_FEMALE].MaxModFile = file.GetDword();
	pBodyModList[RaceType_Devil][GENDER_FEMALE].ModFile = new StaticString[pBodyModList[RaceType_Devil][GENDER_FEMALE].MaxModFile];


	while(1)
	{
		if(idx >= pBodyModList[RaceType_Devil][GENDER_FEMALE].MaxModFile) break;
		pBodyModList[RaceType_Devil][GENDER_FEMALE].ModFile[idx] = file.GetString();

		++idx;
	}		
	file.Release();

	return TRUE;
}


BOOL CGameResourceManager::LoadHairModFileList(MOD_LIST pHairModList[RaceType_Max][GENDER_MAX])
{
	CMHFile file;
	char filename[64];
	DWORD idx = 0;

	//////////////////////////////////////////////////////////////////////////
	// 남자
	idx = 0;
	sprintf(filename,"System/Resource/HairList_H_M.bin");		
	if(!file.Init(filename,"rb"))
		return FALSE;
	pHairModList[RaceType_Human][GENDER_MALE].MaxModFile = file.GetDword();
	pHairModList[RaceType_Human][GENDER_MALE].ModFile = new StaticString[pHairModList[RaceType_Human][GENDER_MALE].MaxModFile];

	while(1)
	{
		if(idx >= pHairModList[RaceType_Human][GENDER_MALE].MaxModFile) break;
		pHairModList[RaceType_Human][GENDER_MALE].ModFile[idx] = file.GetString();

		++idx;
	}		
	file.Release();


	//////////////////////////////////////////////////////////////////////////
	// 여자
	idx = 0;
	sprintf(filename,"System/Resource/HairList_H_W.bin");
	if(!file.Init(filename,"rb"))
		return FALSE;
	pHairModList[RaceType_Human][GENDER_FEMALE].MaxModFile = file.GetDword();
	pHairModList[RaceType_Human][GENDER_FEMALE].ModFile = new StaticString[pHairModList[RaceType_Human][GENDER_FEMALE].MaxModFile];


	while(1)
	{
		if(idx >= pHairModList[RaceType_Human][GENDER_FEMALE].MaxModFile) break;
		pHairModList[RaceType_Human][GENDER_FEMALE].ModFile[idx] = file.GetString();

		++idx;
	}		
	file.Release();

	//////////////////////////////////////////////////////////////////////////
	// 남자
	idx = 0;
	sprintf(filename,"System/Resource/HairList_E_M.bin");		
	if(!file.Init(filename,"rb"))
		return FALSE;
	pHairModList[RaceType_Elf][GENDER_MALE].MaxModFile = file.GetDword();
	pHairModList[RaceType_Elf][GENDER_MALE].ModFile = new StaticString[pHairModList[RaceType_Elf][GENDER_MALE].MaxModFile];

	while(1)
	{
		if(idx >= pHairModList[RaceType_Elf][GENDER_MALE].MaxModFile) break;
		pHairModList[RaceType_Elf][GENDER_MALE].ModFile[idx] = file.GetString();

		++idx;
	}		
	file.Release();


	//////////////////////////////////////////////////////////////////////////
	// 여자
	idx = 0;
	sprintf(filename,"System/Resource/HairList_E_W.bin");
	if(!file.Init(filename,"rb"))
		return FALSE;
	pHairModList[RaceType_Elf][GENDER_FEMALE].MaxModFile = file.GetDword();
	pHairModList[RaceType_Elf][GENDER_FEMALE].ModFile = new StaticString[pHairModList[RaceType_Elf][GENDER_FEMALE].MaxModFile];


	while(1)
	{
		if(idx >= pHairModList[RaceType_Elf][GENDER_FEMALE].MaxModFile) break;
		pHairModList[RaceType_Elf][GENDER_FEMALE].ModFile[idx] = file.GetString();

		++idx;
	}		
	file.Release();

	// 090504 ONS 신규종족 추가작업
	//////////////////////////////////////////////////////////////////////////
	// 남자
	idx = 0;
	sprintf(filename,"System/Resource/HairList_D_M.bin");		
	if(!file.Init(filename,"rb"))
		return FALSE;
	pHairModList[RaceType_Devil][GENDER_MALE].MaxModFile = file.GetDword();
	pHairModList[RaceType_Devil][GENDER_MALE].ModFile = new StaticString[pHairModList[RaceType_Devil][GENDER_MALE].MaxModFile];

	while(1)
	{
		if(idx >= pHairModList[RaceType_Devil][GENDER_MALE].MaxModFile) break;
		pHairModList[RaceType_Devil][GENDER_MALE].ModFile[idx] = file.GetString();

		++idx;
	}		
	file.Release();


	//////////////////////////////////////////////////////////////////////////
	// 여자
	idx = 0;
	sprintf(filename,"System/Resource/HairList_D_W.bin");
	if(!file.Init(filename,"rb"))
		return FALSE;
	pHairModList[RaceType_Devil][GENDER_FEMALE].MaxModFile = file.GetDword();
	pHairModList[RaceType_Devil][GENDER_FEMALE].ModFile = new StaticString[pHairModList[RaceType_Devil][GENDER_FEMALE].MaxModFile];


	while(1)
	{
		if(idx >= pHairModList[RaceType_Devil][GENDER_FEMALE].MaxModFile) break;
		pHairModList[RaceType_Devil][GENDER_FEMALE].ModFile[idx] = file.GetString();

		++idx;
	}		
	file.Release();
	return TRUE;
}

BOOL CGameResourceManager::LoadPetModList()
{
	CMHFile file;
	if(!file.Init("System/Resource/Pet_Costume.bin","rb"))
		return FALSE;

	mPetModList.MaxModFile = file.GetDword();
	mPetModList.ModFile = new StaticString[ mPetModList.MaxModFile ];

	for(DWORD i = 0; i < mPetModList.MaxModFile; ++i)
	{
		mPetModList.ModFile[i] = file.GetString();
	}		
	file.Release();

	return TRUE;
}

BOOL CGameResourceManager::LoadNpcChxList()
{
	CMHFile file;
	if(!file.Init("System/Resource/NpcChxList.bin","rb"))
		return FALSE;

	m_MaxNpcChxList = file.GetDword();
	m_NpcChxList = new CHXLIST[m_MaxNpcChxList];

	for(DWORD i = 0; i < m_MaxNpcChxList; ++i)
	{
		m_NpcChxList[i].FileName = file.GetString();
	}		
	file.Release();

	return TRUE;
}

char* CGameResourceManager::GetNpcChxName(DWORD index)
{
	ASSERT(index > 0 && index < m_MaxNpcChxList);
	return m_NpcChxList[index].FileName;
}

BOOL CGameResourceManager::LoadGameDesc()
{
	char filename[64];
	char value[64] = {0,};
	CMHFile file;
	sprintf(filename,"system/GameDesc.bin");	

	if(!file.Init(filename, "rb"))
		return FALSE;

	while(! file.IsEOF() )
	{
		strcpy(value,strupr(file.GetString()));

		if(strcmp(value,"*DISPWIDTH") == 0)
		{
			m_GameDesc.dispInfo.dwWidth = file.GetDword();
		}
		else if(strcmp(value,"*DISPHEIGHT") == 0)
		{
			m_GameDesc.dispInfo.dwHeight = file.GetDword();
		}
		else if(strcmp(value,"*BPS") == 0)
		{
			m_GameDesc.dispInfo.dwBPS = file.GetDword();
		}
		else if(strcmp(value,"*WINDOWMODE") == 0)
		{
			m_GameDesc.dispInfo.dispType = file.GetBool() ? WINDOW_WITH_BLT : FULLSCREEN_WITH_BLT;
		}
		else if(strcmp(value,"*SHADOW") == 0)
		{
			m_GameDesc.bShadow = file.GetByte();
		}
		else if(strcmp(value,"*MAXSHADOWLIGHT") == 0)
		{
			m_GameDesc.MaxShadowNum = file.GetDword();
		}
		else if(strcmp(value,"*SHADOWDETAIL") == 0)
		{
			m_GameDesc.MaxShadowTexDetail = file.GetDword();
		}
		else if(strcmp(value,"*DISTRIBUTESERVERIP") == 0)
		{
			file.GetString(m_GameDesc.DistributeServerIP);
		}
		else if(strcmp(value,"*DISTRIBUTESERVERPORT") == 0)
		{
			m_GameDesc.DistributeServerPort = file.GetWord();
		}
		else if(strcmp(value,"*AGENTSERVERPORT") == 0)
		{
			m_GameDesc.AgentServerPort = file.GetWord();
		}
		else if(strcmp(value,"*MOVEPOINT") == 0)
		{
			file.GetString(m_GameDesc.MovePoint);
		}
		else if(strcmp(value,"*FPS") == 0)
		{
			m_GameDesc.FramePerSec = file.GetDword();
			m_GameDesc.TickPerFrame = 1000/(float)m_GameDesc.FramePerSec;
		}
		else if(strcmp(value,"*CAMERAANGLEX") == 0)
		{
			m_GameDesc.CameraMinAngleX = file.GetFloat();
			m_GameDesc.CameraMaxAngleX = file.GetFloat();
		}
		else if(strcmp(value,"*CAMERADISTANCE") == 0)
		{
			m_GameDesc.CameraMinDistance = file.GetFloat();
			m_GameDesc.CameraMaxDistance = file.GetFloat();
		}
		else if(strcmp(value,"*GRAVITY") == 0)
		{
			gAntiGravity = 1/file.GetFloat();
		}		

		// Volume
		else if(strcmp(value,"*MASTERVOLUME") == 0)
		{
			m_GameDesc.MasterVolume = file.GetFloat();
		}
		else if(strcmp(value,"*SOUNDVOLUME") == 0)
		{
			m_GameDesc.SoundVolume = file.GetFloat();
		}
		else if(strcmp(value,"*BGMVOLUME") == 0)
		{
			m_GameDesc.BGMVolume = file.GetFloat();
		}
		//limit
		else if(strcmp(value,"*LIMITDAY") == 0)
		{
			m_GameDesc.LimitDay = file.GetDword();
		}
		else if(strcmp(value,"*LIMITID") == 0)
		{
			file.GetString(m_GameDesc.LimitID);
		}
		else if(strcmp(value,"*LIMITPWD") == 0)
		{
			file.GetString(m_GameDesc.LimitPWD);
		}
		else if(strcmp(value,"*WINDOWTITLE") == 0)
		{
			file.GetString(m_GameDesc.strWindowTitle);
		}
	}

	// 웅주, 070530 해상도, 창모드는 이 파일에서 읽는다(bin 파일에 쓸 방법이 현재는 없기 때문이다)
	{
		FILE* fp = fopen("system\\launcher.sav","r");

		if( fp )
		{
			TCHAR buffer[MAX_PATH] = {0};
			LPCTSTR separator = " =\r\n";

			while( fgets( buffer, sizeof( buffer ), fp ) )
			{
				const char* token = strtok( buffer, separator );

				if( 0 == token ||	// empty line
					';' == token[ 0 ] )	// comment
				{
					continue;
				}
				else if( ! strcmpi( "resolution",  token ) )
				{
					token = strtok( 0, separator );
					LPCTSTR token2 = strtok(0, separator);

					switch( atoi( token ) )
					{
					case 0:
						{
							m_GameDesc.dispInfo.dwWidth = 800;
							m_GameDesc.dispInfo.dwHeight = 600;
							break;
						}
					case 1:
						{
							m_GameDesc.dispInfo.dwWidth = 1024;
							m_GameDesc.dispInfo.dwHeight = 768;
							break;
						}
					case 2:
						{
							m_GameDesc.dispInfo.dwWidth = 1280;
							m_GameDesc.dispInfo.dwHeight = 1024;
							break;
						}
					default:
						{
							m_GameDesc.dispInfo.dwWidth = _ttoi(token);
							m_GameDesc.dispInfo.dwHeight = _ttoi(token2 ? token2 : "");
							break;
						}
					}
				}
				else if( ! strcmpi( "windowMode", token ) )
				{
					token = strtok( NULL, separator );

					m_GameDesc.dispInfo.dispType = ( ! strcmpi( "true", token ) ? WINDOW_WITH_BLT : FULLSCREEN_WITH_BLT );
				}
			}

			fclose( fp );
		}
	}

	gTickPerFrame = m_GameDesc.TickPerFrame;
	AUDIOMGR->SetMasterVolume(m_GameDesc.MasterVolume);
	AUDIOMGR->SetSoundVolume(m_GameDesc.SoundVolume);
	AUDIOMGR->SetBGMVolume(m_GameDesc.BGMVolume);

	return TRUE;
}

BOOL CGameResourceManager::LoadServerList()
{
	m_nMaxServerList = 0;

	char filename[64];
	char value[64] = {0,};
	int index = 0;
	CMHFile file;

	sprintf( filename, "system/ServerList.bin" );
	if( !file.Init( filename, "rb" ) )
		return FALSE;

	while( 1 )
	{
		if( file.IsEOF() )
			break;

		strcpy( value, strupr(file.GetString()) );

		if( strcmp( value,"*USERSERVER" ) == 0 )
		{
			++index;
			if( index > 0 )
			{
				m_ServerList[index-1].bEnter = file.GetBool();
				file.GetString( m_ServerList[index-1].DistributeIP );
				m_ServerList[index-1].DistributePort = file.GetWord();
				file.GetStringInQuotation( m_ServerList[index-1].ServerName );
				m_ServerList[index-1].ServerNo = file.GetWord();
				++m_nMaxServerList;
			}
		}

		else if( strcmp( value,"*TESTSERVER" ) == 0 && g_bTESTSERVER)
		{
			++index;
			if( index > 0 )
			{
				m_ServerList[index-1].bEnter = file.GetBool();
				file.GetString( m_ServerList[index-1].DistributeIP );
				m_ServerList[index-1].DistributePort = file.GetWord();
				file.GetStringInQuotation( m_ServerList[index-1].ServerName );
				m_ServerList[index-1].ServerNo = file.GetWord();
				++m_nMaxServerList;
			}
		}
		else if( strcmp( value, "*CHANNELNUM" ) == 0 )
		{
			int nSet = file.GetInt();
			int nCount = file.GetInt();
			if( nSet > 0 && nCount > 0 )
				CHARSELECT->SetMaxChannel( nSet, nCount );				
		}
	}

	file.Release();

	return TRUE;
}


char* CGameResourceManager::GetServerSetName()
{
	for(int i=0; i<m_nMaxServerList; ++i)
	{
		if( g_nServerSetNum == m_ServerList[i].ServerNo )
			return m_ServerList[i].ServerName;
	}
	return NULL;
}



#include "ItemManager.h"			// for LoadPreDataTable()
void CGameResourceManager::LoadPreDataTable()
{
	CMHFile file;
	if(!file.Init("./System/Resource/PreLoadData.bin", "rb"))
		return;
	char Token[64];	

	while( !file.IsEOF() )
	{
		file.GetString(Token);

		if(strcmp(Token, "#EFFECT") == 0)
		{
			PRELOAD* pEff = NULL;
			pEff = new PRELOAD;

			char buf[128];

			file.GetString(buf);
			pEff->FileName = buf;

			m_PreLoadEfflist.AddTail(pEff);
		}
		else if(strcmp(Token, "#MOD") == 0)
		{
			PRELOAD* pEff = NULL;
			pEff = new PRELOAD;

			char buf[128];

			file.GetString(buf);
			pEff->FileName = buf;

			m_PreLoadModlist.AddTail(pEff);
		}
		else if(strcmp(Token, "#MAPNUM") == 0)
		{
			sPRELOAD_INFO* pPreLoadInfo = new sPRELOAD_INFO;
			pPreLoadInfo->MapNum = file.GetInt();

			LoadPreMonsterData(
				pPreLoadInfo,
				MAPTYPE(pPreLoadInfo->MapNum));

			int Count = file.GetInt();
			int* Level = new int[Count];

			for(int i=0; i<Count; i++)
				Level[i] = file.GetInt();

			ITEMMGR->SetPreItemData(pPreLoadInfo, Level, Count);

			SAFE_DELETE_ARRAY(
				Level);

			m_PreLoadDataTable.Add(pPreLoadInfo, pPreLoadInfo->MapNum);
		}
		///////////////////////////////////////////////////////////////////////////////
		// 06. 04. PreLoadData 추가기능 - 이영준
		// 아이템 사전 로드기능 추가
		// #ITEM [수량] [아이템 인덱스]...
		else if(strcmp(Token, "#ITEM") == 0)
		{
			// 있으면 지워준다
			if(m_PreLoadItemlist)
				delete[] m_PreLoadItemlist;

			WORD Count = file.GetWord();

			if(Count > 0)
			{
				// 버퍼 생성
				m_PreLoadItemlist = new WORD[Count + 1];
				// 첫번째 자리에 수량을 넣어준다
				m_PreLoadItemlist[0] = Count;

				WORD ItemIdx = 0;

				for(int i = 1; i < Count + 1; i++)
				{		
					ItemIdx = file.GetWord();
					m_PreLoadItemlist[i] = ItemIdx;
				}
			}
		}
		///////////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////
		// 06. 05. PreLoadData 추가기능 - 이영준
		// 맵별 이펙트 사전 로드기능 추가
		// #MAPEFFECT [수량] [파일이름]...
		else if(strcmp(Token, "#MAPEFFECT") == 0)
		{
			sPRELOAD_EFFECT_INFO* pPreLoadInfo = new sPRELOAD_EFFECT_INFO;

			pPreLoadInfo->MapNum = file.GetInt();
			pPreLoadInfo->Count = file.GetInt();

			for(int i = 0; i < pPreLoadInfo->Count; i++)
			{
				PRELOAD* pEff = NULL;
				pEff = new PRELOAD;

				char buf[128];

				file.GetString(buf);
				pEff->FileName = buf;

				pPreLoadInfo->Effect.AddTail(pEff);
			}

			m_PreLoadEffectTable.Add(pPreLoadInfo, pPreLoadInfo->MapNum);
		}
		///////////////////////////////////////////////////////////////////////////////

	}
}


void CGameResourceManager::LoadPreMonsterData(sPRELOAD_INFO* pPreLoadInfo, MAPTYPE MapNum)
{
	CMHFile file;
	char filename[256];
	sprintf(filename,"System/Resource/Monster_%02d.bin",MapNum);
	if(!file.Init(filename,"rb"))
		return ;

	char buff[256];
	while(1)
	{
		if(file.IsEOF())
		{
			break;
		}
		file.GetString(buff);
		strcpy(buff, _strupr(buff));

		CMD_ST(buff)			
			CMD_CS("#ADD")
			file.GetByte();
		file.GetDword();
		int kind = file.GetDword();
		if(kind)
		{
			// 같은게 있는지 찾고
			for(int i=0; i<pPreLoadInfo->Count[ePreLoad_Monster]; i++)
				if(kind == pPreLoadInfo->Kind[ePreLoad_Monster][i])		break;

			// 없으면 새로운 몬스터 추가
			if(i == pPreLoadInfo->Count[ePreLoad_Monster])
			{
				pPreLoadInfo->Kind[ePreLoad_Monster][i] = kind;
				++pPreLoadInfo->Count[ePreLoad_Monster];

				//
				if(pPreLoadInfo->Count[ePreLoad_Monster] >= MAX_KIND_PERMAP)
					break;
			}
		}
		CMD_EN
	}
}


#ifdef _TESTCLIENT_
BOOL CGameResourceManager::LoadTestClientInfo()
{
	char filename[64];
	char value[64] = {0,};
	sprintf(filename,"./TestClient.ini");
	CMHFile file;
	if(!file.Init(filename, "rt"))
		return FALSE;

	while(1)
	{
		if(file.IsEOF())
			break;

		strcpy(value,strupr(file.GetString()));

		if(strcmp(value,"*STARTPOSITION") == 0)
		{
			m_TestClientInfo.x = file.GetFloat();
			m_TestClientInfo.z = file.GetFloat();
		}		
		else if(strcmp(value,"*BOSSMONSTERKIND") == 0)
		{
			m_TestClientInfo.BossMonsterKind = file.GetWord();

		}
		else if(strcmp(value,"*BOSSMONSTERNUM") == 0)
		{
			m_TestClientInfo.BossMonsterNum = file.GetWord();
		}
		else if(strcmp(value,"*BOSSMONSTERFILE") == 0)
		{
			file.GetString(m_TestClientInfo.BossInfofile);
		}		
		else if(strcmp(value,"*EFFECT") == 0)
		{
			m_TestClientInfo.Effect = FindEffectNum(file.GetString());
		}
		else if(strcmp(value,"*SKILL") == 0)
		{
			m_TestClientInfo.SkillIdx = file.GetWord();
		}
		else if(strcmp(value,"*WEAPON") == 0)
		{
			m_TestClientInfo.WeaponIdx = file.GetWord();
		}
		else if(strcmp(value,"*SHOES") == 0)
		{
			m_TestClientInfo.ShoesIdx = file.GetWord();		
		}
		else if(strcmp(value,"*HAT") == 0)
		{
			m_TestClientInfo.HatIdx = file.GetWord();		
		}
		else if(strcmp(value,"*DRESS") == 0)
		{
			m_TestClientInfo.DressIdx = file.GetWord();		
		}
		else if(strcmp(value,"*SHIELD") == 0)
		{
			m_TestClientInfo.ShieldIdx = file.GetWord();		
		}
		else if(strcmp(value,"*GENDER") == 0)
		{
			m_TestClientInfo.Gender = file.GetByte();
		}
		else if(strcmp(value,"*RACE") == 0)
		{
			m_TestClientInfo.Race = file.GetByte();
		}
		else if(strcmp(value,"*JOB") == 0)
		{
			m_TestClientInfo.Job = file.GetByte();
		}

		else if(strcmp(value,"*MONSTERNUM") == 0)
		{
			m_TestClientInfo.MonsterNum = file.GetWord();
		}
		else if(strcmp(value,"*MONSTEREFFECT") == 0)
		{
			m_TestClientInfo.MonsterEffect = FindEffectNum(file.GetString());
		}
		else if(strcmp(value,"*MONSTERKIND") == 0)
		{
			m_TestClientInfo.MonsterKind = file.GetWord();
		}		
		else if(strcmp(value,"*MAP") == 0)
		{
			m_TestClientInfo.Map = file.GetByte();
		}
		else if(strcmp(value,"*CHARACTERNUM") == 0)
		{
			m_TestClientInfo.CharacterNum = file.GetWord();
		}
		else if(strcmp(value,"*HAIR") == 0)
		{
			m_TestClientInfo.HairType = file.GetByte();
		}
		else if(strcmp(value,"*FACE") == 0)
		{
			m_TestClientInfo.FaceType = file.GetByte();
		}
		else if(strcmp(value,"*LIGHTEFFECT") == 0)
		{
			m_TestClientInfo.LightEffect = file.GetBool();
		}
		//trustpak
		else if(strcmp(value, "*TEST_CHARACTER_NUM") == 0)
		{
			m_TestClientInfo.dwTestCharacterNo = file.GetDword();
		}
		else if(strcmp(value, "*TEST_CHARACTER_INFO") == 0)
		{
			static int s_iTestCharacterCount = 0;

			if (MAX_TEST_CHARACTER_NO > s_iTestCharacterCount)
			{
				m_TestClientInfo.aTestCharacterInfo[s_iTestCharacterCount].byGender		= file.GetByte();
				m_TestClientInfo.aTestCharacterInfo[s_iTestCharacterCount].fX			= file.GetFloat();
				m_TestClientInfo.aTestCharacterInfo[s_iTestCharacterCount].fY			= file.GetFloat();
				m_TestClientInfo.aTestCharacterInfo[s_iTestCharacterCount].fZ			= file.GetFloat();
				m_TestClientInfo.aTestCharacterInfo[s_iTestCharacterCount].fAngle		= DEGTORAD( file.GetFloat() );

				++s_iTestCharacterCount;
			}			
		}
		//
	}

	file.Release();



	return TRUE;
}
#endif

//-----------------------------------------------------------------------------------------------------------//
#endif //_CLIENT_RESOURCE_FIELD_
//-----------------------------------------------------------------------------------------------------------//

// 06. 05 HIDE NPC - 이영준
#ifdef _SERVER_RESOURCE_FIELD_
BOOL CGameResourceManager::LoadHideNpcList()
{
	CMHFile file;
	if(!file.Init("System/Resource/HideNpcList.bin","rb"))
		return FALSE;

	WORD mapnum = GetLoadMapNum();
	WORD index;

	while(!file.IsEOF())
	{
		// 현재 맵에 해당되는 NPC만 읽어들인다
		if(file.GetWord() == mapnum)
		{
			index = file.GetWord();

			HIDE_NPC_INFO* info = new HIDE_NPC_INFO;

			memset(info, 0, sizeof(HIDE_NPC_INFO));

			info->UniqueIdx = index;

			m_HideNpcTable.Add(info, index);
		}
	}

	return TRUE;
}

BOOL CGameResourceManager::LoadSkillMoney()
{
	CMHFile file;
	if(!file.Init("./System/Resource/SkillMoney.bin","rb"))
		return FALSE;

	while( !file.IsEOF() )
	{
		DWORD dwSkillIdx	= file.GetDword();
		if( dwSkillIdx == 0 )
		{
			g_Console.LOG( 4, "LoadSkillMoney() Error!!" );
			break;
		}

		DWORD dwSP			= file.GetDword();
		DWORD dwMoney		= file.GetDword();

        SKILL_MONEY* pSkillMoney = new SKILL_MONEY;
		pSkillMoney->SkillIdx	= dwSkillIdx;
		pSkillMoney->SP			= dwSP;
		pSkillMoney->Money		= dwMoney;

		m_htSkillMoney.Add( pSkillMoney, dwSkillIdx );
	}

	return TRUE;
}


DWORD CGameResourceManager::GetSkillMoney( DWORD SkillIdx )
{
	const SKILL_MONEY* const skillMoney = m_htSkillMoney.GetData(
		SkillIdx);

	return skillMoney ? skillMoney->Money : 0;
}

// 090227 ShinJS --- 이동NPC 정보 추가
void CGameResourceManager::AddMoveNpcInfo( WORD wNpcIdx, WORD wNpcKindIdx, WORD wXPos, WORD wZPos, WORD wDir )
{
	STATIC_NPCINFO* pInfo = m_StaticNpcTable.GetData( wNpcIdx );
	if( !pInfo )
	{
		pInfo = new STATIC_NPCINFO;
		ASSERT( pInfo );
		if( !pInfo )	return;

		pInfo->MapNum = m_LoadingMapNum;
		pInfo->wNpcUniqueIdx = wNpcIdx;
		pInfo->wNpcJob = wNpcKindIdx;
		pInfo->vPos.x = wXPos;
		pInfo->vPos.z = wZPos;
		pInfo->wDir = wDir;	

		// NPC Table에 이동NPC 추가
		m_StaticNpcTable.Add( pInfo, pInfo->wNpcUniqueIdx );

		// 이동NPC에 정보 추가
		m_htMoveNpcStaticInfo.Add( pInfo, pInfo->wNpcUniqueIdx );
	}
	else
	{
		pInfo->MapNum = m_LoadingMapNum;
		pInfo->wNpcUniqueIdx = wNpcIdx;
		pInfo->wNpcJob = wNpcKindIdx;
		pInfo->vPos.x = wXPos;
		pInfo->vPos.z = wZPos;
		pInfo->wDir = wDir;	
	}
}

// 090227 ShinJS --- 이동NPC 정보 제거
void CGameResourceManager::RemoveMoveNpcInfo( WORD wNpcUniqueIdx )
{
	// NPC Table에 이동 NPC 제거
	m_StaticNpcTable.Remove( wNpcUniqueIdx );

	// 이동NPC 정보 제거
	m_htMoveNpcStaticInfo.Remove( wNpcUniqueIdx );
}

// 090227 ShinJS --- StaticNpc Table에 해당 NPC 제거
void CGameResourceManager::RemoveStaticNpcInfo( WORD wNpcUniqueIdx )
{
	// NPC Table에 이동 NPC 제거
	m_StaticNpcTable.Remove( wNpcUniqueIdx );
}

#endif
