#include "StdAfx.h"
#include "..\[CC]Header\GameResourceManager.h"
#include "MoveManager.h"
#include "ObjectStateManager.h"
#include "UserTable.h"
#include "Player.h"
#include "Pet.h"
#include "CharMove.h"
#include "PartyManager.h"
#include "Party.h"
#include "PackedData.h"
#include "..\[CC]Skill\Server\Manager\SkillManager.h"
#include "..\[CC]Skill\Server\Info\ActiveSkillInfo.h"
#include "Network.h"
#include "LootingManager.h"
#include "MapDBMsgParser.h"
#include "Vehicle.h"
#include "PetManager.h"

CMoveManager::CMoveManager()
{}

CMoveManager::~CMoveManager()
{}

CMoveManager& CMoveManager::GetInstance()
{
	static CMoveManager instance;

	return instance;
}

void CMoveManager::NetworkMsgParse( MSGBASE* const pMsg, DWORD dwLength )
{
	switch( pMsg->Protocol )
	{
	case MP_MOVE_TARGET:
		{
			MOVE_TARGETPOS* pmsg = (MOVE_TARGETPOS*)pMsg;
			CPlayer* pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwMoverID);
			if(pPlayer == NULL)
				return;
			if(pPlayer->GetState() == eObjectState_Die)
			{
				MSG_DWORD msg;
				msg.Category = MP_CHAR;
				msg.Protocol = MP_CHAR_YOUAREDIED;
				pPlayer->SendMsg(&msg,sizeof(msg));
				return;
			}

			if(pPlayer->GetState() == eObjectState_Immortal )
			{
				OBJECTSTATEMGR_OBJ->EndObjectState( pPlayer, eObjectState_Immortal, 0 );
			}
			if(pmsg->tpos.PosNum >= MAX_CHARTARGETPOSBUF_SIZE)
			{
				CCharMove::CorrectPlayerPosToServer(pPlayer);
				return;
			}
			//
			VECTOR3 Start;
			pmsg->GetStartPos(&Start);

			VECTOR3* pCurPos = CCharMove::GetPosition(pPlayer);
			if(CalcDistanceXZ(pCurPos,&Start) >= 1000)
			{				
				CCharMove::CorrectPlayerPosToServer(pPlayer);
				return;
			}
			pmsg->GetTargetInfo((MOVE_INFO *)pPlayer->GetMoveInfo());
			
			VECTOR3 colpos = {0};

			if(g_pServerSystem->GetMap()->CollisionCheck(&Start,pPlayer->GetMoveInfo()->GetTargetPosition(pPlayer->GetMoveInfo()->GetCurTargetPosIdx()),&colpos,pPlayer))
			{
				CCharMove::CorrectPlayerPosToServer(pPlayer);
				return;
			}

			CCharMove::StartMoveEx(pPlayer,gCurTime,&Start,pPlayer->GetMoveInfo()->GetTargetPosition(pPlayer->GetMoveInfo()->GetCurTargetPosIdx()));
			pmsg->AddedMoveSpeed = pPlayer->GetMoveInfo()->AddedMoveSpeed;

			// 071008 웅주, 상태값 변경
			{
				OBJECTSTATEMGR_OBJ->StartObjectState( pPlayer, eObjectState_Move, 0 );
			}

			// RaMa
			CParty* pParty = PARTYMGR->GetParty(pPlayer->GetPartyIdx());
			if(pParty)
			{
				pParty->SendMemberPos(pPlayer->GetID(), pPlayer->GetMoveInfo());
			}

			PACKEDDATA_OBJ->QuickSend(pPlayer,pmsg,dwLength);
			break;
		}
	case MP_MOVE_ONETARGET:
		{
			MOVE_ONETARGETPOS* const pmsg = (MOVE_ONETARGETPOS*)pMsg;
			CPlayer* const pPlayer = (CPlayer*)g_pUserTable->FindUser(pmsg->dwMoverID);

			if(pPlayer == 0)
			{
				break;
			}
			else if(eObjectKind_Player != pPlayer->GetObjectKind())
			{
				break;
			}
			else if(pPlayer->GetInited() == FALSE)
			{
				break;
			}
			else if(pPlayer->GetInitedGrid() == FALSE)
			{
				break;
			}
			else if(pPlayer->GetState() == eObjectState_Die)
			{
				MSG_DWORD msg;
				msg.Category = MP_CHAR;
				msg.Protocol = MP_CHAR_YOUAREDIED;
				pPlayer->SendMsg(&msg,sizeof(msg));
				return;
			}

			if(pPlayer->GetState() == eObjectState_Immortal )
			{
				OBJECTSTATEMGR_OBJ->EndObjectState( pPlayer, eObjectState_Immortal, 0 );
			}

			VECTOR3 Start;
			pmsg->GetStartPos(&Start);

			VECTOR3* pCurPos = CCharMove::GetPosition(pPlayer);
			if(CalcDistanceXZ(pCurPos,&Start) >= 1000)
			{				
				CCharMove::CorrectPlayerPosToServer(pPlayer);

				CVehicle* const vehicle = ( CVehicle* )g_pUserTable->FindUser( pPlayer->GetSummonedVehicle() );

				if( vehicle )
				{
					vehicle->Recall( *pCurPos );
				}

				return;
			}

			pmsg->GetTargetInfo((MOVE_INFO *)pPlayer->GetMoveInfo());
			
			VECTOR3 colpos = {0};

			if(g_pServerSystem->GetMap()->CollisionCheck(&Start,pPlayer->GetMoveInfo()->GetTargetPosition(pPlayer->GetMoveInfo()->GetCurTargetPosIdx()),&colpos,pPlayer))
			{
				CCharMove::CorrectPlayerPosToServer(pPlayer);
				return;
			}

			BASEMOVE_INFO* const baseMoveInfo = pPlayer->GetMoveInfo();
			VECTOR3 targetPosition = *( baseMoveInfo->GetTargetPosition( baseMoveInfo->GetCurTargetPosIdx() ) );

			CCharMove::StartMoveEx(
				pPlayer,
				gCurTime,
				&Start,
				&targetPosition );
			OBJECTSTATEMGR_OBJ->StartObjectState(
				pPlayer,
				eObjectState_Move,
				0 );

			// 090316 LUJ, 탈것에 탑승 상태인 경우 이동시켜준다
			{
				CVehicle* const vehicleObject = ( CVehicle* )g_pUserTable->FindUser( pPlayer->GetMountedVehicle() );

				if( vehicleObject &&
					vehicleObject->GetObjectKind() == eObjectKind_Vehicle )
				{
					const MSG_VEHICLE_ERROR::Error error = vehicleObject->Move( pPlayer->GetID(), Start, targetPosition );

					if( MSG_VEHICLE_ERROR::ErrorNone != error )
					{
						MSG_VEHICLE_ERROR message;
						ZeroMemory( &message, sizeof( message ) );
						message.Category = MP_VEHICLE;
						message.Protocol = MP_VEHICLE_ERROR_ACK;
						message.mError = error;
						pPlayer->SendMsg( &message, sizeof( message ) );
						return;
					}
				}
			}

			CParty* pParty = PARTYMGR->GetParty(pPlayer->GetPartyIdx());
			if(pParty)
			{
				pParty->SendMemberPos(pPlayer->GetID(), pPlayer->GetMoveInfo());
			}

			MOVE_ONETARGETPOS_FROMSERVER msg;
			SetProtocol(&msg,MP_MOVE,MP_MOVE_ONETARGET);
			msg.AddedMoveSpeed = pPlayer->GetMoveInfo()->AddedMoveSpeed;
			msg.dwObjectID = pPlayer->GetID();
			msg.SetStartPos(&Start);
			// 081203 LUJ, 플레이어 이동 속도에 따라 종점 좌표를 설정한다
			msg.SetTargetPos( 0 < pPlayer->GetMoveSpeed() ? pPlayer->GetMoveInfo()->GetTargetPosition( 0 ) : &Start );

			// 091214 ShinJS --- 이동처리를 이전상태로 되돌린다.
			PACKEDDATA_OBJ->QuickSendExceptObjectSelf(pPlayer,&msg,msg.GetSize());
			break;
		}
	case MP_MOVE_STOP:
		{
			MOVE_POS* pmsg = (MOVE_POS*)pMsg;
			VECTOR3 pos;
			pmsg->cpos.Decompress(&pos);
			CPlayer* pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwMoverID);
			if(pPlayer == NULL)
				return;

			// 상태 변경
			//if( !OBJECTSTATEMGR_OBJ->StartObjectState(pPlayer, eObjectState_None, 0) )
			//	return;
			//if(pPlayer->GetState() == eObjectState_Move)
			OBJECTSTATEMGR_OBJ->EndObjectState(pPlayer,eObjectState_Move);

			//////////////////////////////////////////////////////////////////////////
			// 클라이언트들의 충돌 체크(클라이언트들에서 하기 때문에 필요없지만... 확인용)
			// 클라이언트의 해킹등을 방지하기 위해..
			VECTOR3 colpos;
			VECTOR3* pCurPos = CCharMove::GetPosition(pPlayer);

			if(g_pServerSystem->GetMap()->CollisionCheck(pCurPos,&pos,&colpos,pPlayer))
			{
				CCharMove::CorrectPlayerPosToServer(pPlayer);
				return;
			}
			//////////////////////////////////////////////////////////////////////////
			// 정상적인 이동인지 확인하고 틀리면 Correct를 해준다.
			if(CCharMove::CheckVaildStop(pPlayer,gCurTime,&pos) == TRUE)
			{
				CVehicle* const vehicle = ( CVehicle* )g_pUserTable->FindUser( pPlayer->GetMountedVehicle() );

				if( vehicle &&
					vehicle->GetObjectKind() == eObjectKind_Vehicle &&
					pPlayer->GetSummonedVehicle() == pPlayer->GetMountedVehicle() )
				{
					const MSG_VEHICLE_ERROR::Error error = vehicle->EndMove( pPlayer->GetID(), pos );

					if( MSG_VEHICLE_ERROR::ErrorNone != error )
					{
						MSG_VEHICLE_ERROR message;
						ZeroMemory( &message, sizeof( message ) );
						message.Category = MP_VEHICLE;
						message.Protocol = MP_VEHICLE_ERROR_ACK;
						message.mError = error;
						pPlayer->SendMsg( &message, sizeof( message ) );
						return;
					}
				}
				else
				{
					CCharMove::EndMove(pPlayer,gCurTime,&pos);
				}
			}

			// 091214 ShinJS --- 이동처리를 이전상태로 되돌린다.
			PACKEDDATA_OBJ->QuickSendExceptObjectSelf(pPlayer,pmsg,dwLength);
			break;
		}
	case MP_MOVE_PET_MOVE:
		{
			MOVE_ONETARGETPOS* pmsg = ( MOVE_ONETARGETPOS* )pMsg;
			CObject* pTarget = ( CObject* )g_pUserTable->FindUser( pmsg->dwObjectID );
			CPet* pPet = ( CPet* )g_pUserTable->FindUser( pmsg->dwMoverID );

			if( !pTarget || !pPet )
				return;

			if( pTarget->GetInited() == FALSE || pTarget->GetInitedGrid() == FALSE )
				return;

			if( pPet->GetInited() == FALSE || pPet->GetInitedGrid() == FALSE )
				return;

			VECTOR3 Start;
			pmsg->GetStartPos( &Start );

			VECTOR3* pCurPos = CCharMove::GetPosition( pPet );
			if( CalcDistanceXZ( pCurPos, &Start) >= 1000 )
			{				
				CCharMove::CorrectPlayerPosToServer( pPet );
				return;
			}

			pmsg->GetTargetInfo( (MOVE_INFO*)pPet->GetMoveInfo() );

			CCharMove::StartMoveEx( pPet, gCurTime, &Start, pPet->GetMoveInfo()->GetTargetPosition( pPet->GetMoveInfo()->GetCurTargetPosIdx() ) );

			MOVE_ONETARGETPOS_FROMSERVER msg;
			SetProtocol( &msg, MP_MOVE,MP_MOVE_PET_MOVE );
			msg.AddedMoveSpeed = pPet->GetMoveInfo()->AddedMoveSpeed;
			msg.dwObjectID = pPet->GetID();
			msg.SetStartPos( &Start );
			msg.SetTargetPos( pPet->GetMoveInfo()->GetTargetPosition( 0 ) );

			PACKEDDATA_OBJ->QuickSendExceptObjectSelf( pPet, &msg,msg.GetSize() );
			break;
		}
	case MP_MOVE_PET_STOP:
		{
			MOVE_POS* pmsg = (MOVE_POS*)pMsg;
			VECTOR3 pos;
			pmsg->cpos.Decompress(&pos);

			CPlayer* pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwObjectID);
			CPet* pPet = ( CPet* )g_pUserTable->FindUser( pmsg->dwMoverID );

			if( !pPlayer || !pPet )
				return;

			VECTOR3 colpos;
			VECTOR3* pCurPos = CCharMove::GetPosition( pPet );

			if( g_pServerSystem->GetMap()->CollisionCheck( pCurPos, &pos, &colpos, pPet ) )
			{
				CCharMove::CorrectPlayerPosToServer( pPet );
				return;
			}
			if( CCharMove::CheckVaildStop( pPet, gCurTime, &pos ) == TRUE )
				CCharMove::EndMove( pPet, gCurTime, &pos );

			PACKEDDATA_OBJ->QuickSendExceptObjectSelf( pPet, pmsg, dwLength );
			break;
		}
	case MP_MOVE_WALKMODE:
		{
			MSGBASE* pmsg = (MSGBASE*)pMsg;
			CPlayer* const pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwObjectID);

			if(0 == pPlayer)
			{
				break;
			}
			else if(eObjectKind_Player != pPlayer->GetObjectKind())
			{
				break;
			}

			CCharMove::SetWalkMode(pPlayer);

			// 090428 ShinJS --- 탈것 소유시 탈것의 MoveMode도 변경시켜 준다
			CVehicle* pVehicle = (CVehicle*)g_pUserTable->FindUser( pPlayer->GetSummonedVehicle() );
			if( pVehicle && pVehicle->GetObjectKind() == eObjectKind_Vehicle )
			{
				CCharMove::SetWalkMode( pVehicle );
			}

			CPet* const petObject = PETMGR->GetPet(
				pPlayer->GetPetItemDbIndex());

			if(petObject)
			{
				CCharMove::SetWalkMode(
					petObject);
			}
	
			PACKEDDATA_OBJ->QuickSend(pPlayer,pmsg,dwLength);
			break;
		}
	case MP_MOVE_RUNMODE:
		{
			MSGBASE* pmsg = (MSGBASE*)pMsg;
			CPlayer* const pPlayer = (CPlayer *)g_pUserTable->FindUser(pmsg->dwObjectID);

			if(0 == pPlayer)
			{
				break;
			}
			else if(eObjectKind_Player != pPlayer->GetObjectKind())
			{
				break;
			}

			CCharMove::SetRunMode(pPlayer);

			// 090428 ShinJS --- 탈것 소유시 탈것의 MoveMode도 변경시켜 준다
			CVehicle* pVehicle = (CVehicle*)g_pUserTable->FindUser( pPlayer->GetSummonedVehicle() );
			if( pVehicle && pVehicle->GetObjectKind() == eObjectKind_Vehicle )
			{
				CCharMove::SetRunMode( pVehicle );
			}

			CPet* const petObject = PETMGR->GetPet(
				pPlayer->GetPetItemDbIndex());

			if(petObject)
			{
				CCharMove::SetRunMode(
					petObject);
			}

			PACKEDDATA_OBJ->QuickSend(pPlayer,pmsg,dwLength);
			break;
		}
	case MP_MOVE_EFFECTMOVE:
		{
			MOVE_POS* pmsg = (MOVE_POS*)pMsg;
			CObject* pObject = g_pUserTable->FindUser(pmsg->dwMoverID);
			if(pObject == NULL)
				return;

			if( pObject->IsEffectMove() == FALSE )
				return;

			VECTOR3 pos;
			pmsg->cpos.Decompress(&pos);

			if( pObject->GetObjectKind() == eObjectKind_Player )
			{
				VECTOR3* pCurPos = CCharMove::GetPosition( pObject );
				if( CalcDistanceXZ( &pos, pCurPos ) >= 1500 )
				{
					CCharMove::CorrectPlayerPosToServer( pObject );
					return;
				}
			}

			if(CheckValidPosition(pos) == FALSE)
			{
				return;
			}

			CCharMove::SetPosition(pObject,&pos);
			break;
		}
		// 080930 LYW --- MapNetworkMsgParser : 페이드 무브 추가.
	case MP_FADEMOVE_SYN:
		{
			// 원본 메시지 변환.
			MOVE_POS* pmsg = NULL ;
			pmsg = (MOVE_POS*)pMsg ;

			if( !pmsg ) return ;


			// 오브젝트 받기.
			CObject* pObject = NULL ;
			pObject = g_pUserTable->FindUser(pmsg->dwMoverID) ;

			if( !pObject ) return ;


			// 위치 변경하기.
			VECTOR3 pos ;
			pmsg->cpos.Decompress(&pos) ;

			if( !CheckValidPosition(pos) ) return ;

			CCharMove::SetPosition(pObject,&pos) ;


			// 성공 결과 전송하기.
			pmsg->Protocol = MP_FADEMOVE_ACK ;
			((CPlayer*)pObject)->SendMsg(pmsg,sizeof(MOVE_POS)) ;
			break;
		}
		// 081031 LUJ, 플레이어를 소환할 수 없음
	case MP_MOVE_RECALL_ASK_NACK_TO_SERVER:
		{
			const MSG_RECALL_NACK*	m				= ( MSG_RECALL_NACK* )pMsg;
			const MSG_RECALL_SYN	storedMessage	= PopRecallMessage( m->mKey );

			CPlayer* recallPlayer = ( CPlayer* )g_pUserTable->FindUser( storedMessage.mRecallPlayer.mIndex );

			if( !	recallPlayer	||
					recallPlayer->GetObjectKind() != eObjectKind_Player )
			{
				break;
			}

			MSG_RECALL_NACK message( *m );
			message.Protocol = MP_MOVE_RECALL_NACK;
			SafeStrCpy(
				message.mRecallPlayerName,
				storedMessage.mRecallPlayer.mName,
				sizeof( message.mRecallPlayerName ) );
			SafeStrCpy(
				message.mTargetPlayerName,
				storedMessage.mTargetPlayer.mName,
				sizeof( message.mTargetPlayerName ) );

			recallPlayer->SendMsg(
				&message,
				sizeof( message ) );
			break;
		}
		// 081031 LUJ, 리콜 대상이 있는 맵에서 소환 요청을 처리한다
	case MP_MOVE_RECALL_ASK_SYN_TO_SERVER:
		{
			const MSG_RECALL_SYN*	m				= ( MSG_RECALL_SYN* )pMsg;
			CPlayer*				targetPlayer	= ( CPlayer* )g_pUserTable->FindUser( m->mTargetPlayer.mIndex );

			if( !	targetPlayer ||
					targetPlayer->GetObjectKind() != eObjectKind_Player )
			{
				MSG_RECALL_NACK message(
					MP_MOVE,
					MP_MOVE_RECALL_ASK_NACK_TO_SERVER,
					MSG_RECALL_NACK::TypePlayerIsNotExist,
					m->mRecallPlayer.mIndex,
					m->mKey );
				PACKEDDATA_OBJ->SendToMapServer(
					m->mRecallPlayer.mMapType,
					&message,
					sizeof( message ) );
				break;
			}

			MSG_RECALL_NACK::Type errorType = MSG_RECALL_NACK::TypeNone;

			if(g_pServerSystem->IsNoRecallMap(*targetPlayer))
			{
				errorType = MSG_RECALL_NACK::TypePlayerLocateOnBlockedMap;
			}

			// 081031 LUJ, 플레이어 상태를 검사한다
			switch( targetPlayer->GetState() )
			{
			case eObjectState_Deal:
				{
					errorType = MSG_RECALL_NACK::TypeTargetIsDealing;
					break;
				}
			case eObjectState_Exchange:
				{
					errorType = MSG_RECALL_NACK::TypeTargetIsExchanging;
					break;
				}
			case eObjectState_Die:
				{
					errorType = MSG_RECALL_NACK::TypeTargetDied;
					break;
				}
			case eObjectState_StreetStall_Guest:
				{
					errorType = MSG_RECALL_NACK::TypeTargetIsGuestInStall;
					break;
				}
			case eObjectState_StreetStall_Owner:
				{
					errorType = MSG_RECALL_NACK::TypeTargetIsOwnerInStall;
					break;
				}
			default:
				{
					if( targetPlayer->IsPKMode() )
					{
						errorType = MSG_RECALL_NACK::TypeTargetIsInPlayerKillMode;
					}
					else if( LOOTINGMGR->IsLootedPlayer( targetPlayer->GetID() ) )
					{
						errorType = MSG_RECALL_NACK::TypeTargetIsLooting;
					}
					else if( targetPlayer->GetAutoNoteIdx() )
					{
						errorType = MSG_RECALL_NACK::TypeTargetDoAutoNote;
					}
					else if( targetPlayer->IsShowdown() )
					{
						errorType = MSG_RECALL_NACK::TypeTargetIsOnShowdown;
					}
					else if( targetPlayer->GetObjectBattleState() )
					{
						errorType = MSG_RECALL_NACK::TypeTargetIsOnBattle;
					}

					break;
				}
			}

			// 081031 LUJ, 리콜이 불가능한 경우 요청 측에 다시 알린다
			if( MSG_RECALL_NACK::TypeNone != errorType )
			{
				MSG_RECALL_NACK message(
					MP_MOVE,
					MP_MOVE_RECALL_ASK_NACK_TO_SERVER,
					errorType,
					m->mRecallPlayer.mIndex,
					m->mKey );
				PACKEDDATA_OBJ->SendToMapServer(
					m->mRecallPlayer.mMapType,
					&message,
					sizeof( message ) );
				break;
			}

			// 081031 LUJ, 서버에 저장해둔다
			SetRecallMessage(
				m->mKey,
				*m );

			// 081031 LUJ, 해당 플레이어에게 전송한다.
			{
				MSG_RECALL_SYN message( *m );
				message.Protocol = MP_MOVE_RECALL_ASK_SYN;

				targetPlayer->SendMsg(
					&message,
					sizeof( message ) );
			}

			break;
		}
		// 081031 LUJ, 대상 플레이어가 소환에 대해 승낙/거절을 선택함
	case MP_MOVE_RECALL_ASK_ACK:
	case MP_MOVE_RECALL_ASK_NACK:
		{
			const MSG_DWORD*	m				= ( MSG_DWORD* )pMsg;
			CPlayer*			targetPlayer	= ( CPlayer* )g_pUserTable->FindUser( m->dwObjectID );

			if( !	targetPlayer ||
					targetPlayer->GetObjectKind() != eObjectKind_Player )
			{
				break;
			}

			const DWORD				key				= m->dwData;
			const MSG_RECALL_SYN&	storedMessage	= GetRecallMessage( key );

			MSG_DWORD message;
			ZeroMemory( &message, sizeof( message ) );
			message.Category	= MP_MOVE;
			message.Protocol	= MP_MOVE_RECALL_ASK_ACK_TO_SERVER;
			message.dwData		= ( MP_MOVE_RECALL_ASK_ACK == m->Protocol );
			message.dwObjectID	= key;

			PACKEDDATA_OBJ->SendToMapServer(
				storedMessage.mRecallPlayer.mMapType,
				&message,
				sizeof(message));
			break;
		}
		// 081031 LUJ, 대상 플레이어가 리콜한 플레이어에게 응답함
	case MP_MOVE_RECALL_ASK_ACK_TO_SERVER:
		{
			// 081031 LUJ, 수치 검사를 위해 패밀리, 길드 포인트를 쿼리한다(맵 서버에 저장되어 있지 않은 정보)
			
			const MSG_DWORD*		m						= ( MSG_DWORD* )pMsg;
			const DWORD				key						= m->dwObjectID;
			const BOOL				targetPlayerRejected	= ( ! m->dwData );
			// 081031 LUJ, 정보 전송 시 메모리가 깨진다. 따라서 전송이 불가능하도록 상수로 선언되었다
			const MSG_RECALL_SYN&	storedMessage			= GetRecallMessage( key );
			const BOOL				timeOver				= ( ( gCurTime - key ) > storedMessage.mLimitMiliSecond );

			CPlayer* recallPlayer = ( CPlayer* )g_pUserTable->FindUser( storedMessage.mRecallPlayer.mIndex );

			if( !	recallPlayer ||
					recallPlayer->GetObjectKind() != eObjectKind_Player )
			{
				MSG_RECALL_NACK message(
					MP_MOVE,
					MP_MOVE_RECALL_NACK_TO_SERVER,
					MSG_RECALL_NACK::TypePlayerIsNotExist,
					storedMessage.mTargetPlayer.mIndex,
					key );
				PACKEDDATA_OBJ->SendToMapServer(
					storedMessage.mRecallPlayer.mMapType,
					&message,
					sizeof( message ) );
				break;
			}
			else if( targetPlayerRejected )
			{
				MSG_RECALL_SYN message( storedMessage );
				message.Protocol = MP_MOVE_RECALL_ASK_NACK;

				recallPlayer->SendMsg(
					&message,
					sizeof( message ) );
				break;
			}
			else if( timeOver )
			{
				// 081031 LUJ, 리콜 대상에게 알린다
				MSG_RECALL_NACK message(
					MP_MOVE,
					MP_MOVE_RECALL_NACK_TO_SERVER,
					MSG_RECALL_NACK::TypeTimeOver,
					storedMessage.mTargetPlayer.mIndex,
					key );

				PACKEDDATA_OBJ->SendToMapServer(
					storedMessage.mTargetPlayer.mMapType,
					&message,
					sizeof(message));

				// 081031 LUJ, 리콜 사용자에게도 알린다
				message.Protocol = MP_MOVE_RECALL_NACK;
				recallPlayer->SendMsg(
					&message,
					sizeof( message ) );
				break;
			}

			// 081031 LUJ, 소요 스킬 사용
			{
				const BOOL skillNeedsToRecall = ( UINT_MAX != storedMessage.mSkillIndex );
				const cActiveSkillInfo*	const activeSkillInfo = SKILLMGR->GetActiveInfo( storedMessage.mSkillIndex );

				if( skillNeedsToRecall &&
					activeSkillInfo )
				{
					switch( activeSkillInfo->GetInfo().mConsumePoint.mType )
					{
					case ACTIVE_SKILL_INFO::ConsumePoint::TypeFamily:
					case ACTIVE_SKILL_INFO::ConsumePoint::TypeGuild:
						{
							g_DB.FreeMiddleQuery(
								RRecallDataLoad,
								key,
								"EXEC dbo.MP_MOVE_RECALL_DATA_SELECT %d, %d",
								recallPlayer->GetGuildIdx(),
								recallPlayer->GetFamilyIdx() );
							return;
						}
					}
				}
			}

			Recall( key );
			break;
		}
		// 081031 LUJ, 대상자를 특정 맵과 위치로 소환한다
	case MP_MOVE_RECALL_SYN_TO_SERVER:
		{
			const MSG_DWORD2*		m				= ( MSG_DWORD2* )pMsg;
			const DWORD				key				= m->dwObjectID;
			const MSG_RECALL_SYN	storedMessage	= GetRecallMessage( key );

			VECTOR3 movePosition = { 0 };
			movePosition.x	= float( m->dwData1 );
			movePosition.z	= float( m->dwData2 );

			CPlayer* targetPlayer = ( CPlayer* )g_pUserTable->FindUser( storedMessage.mTargetPlayer.mIndex );

			// 090701 LUJ, 초기화가 된 플레이어만 소환될 수 있다
			if( !	targetPlayer ||
					targetPlayer->GetInited() == FALSE ||
					targetPlayer->GetObjectKind() != eObjectKind_Player )
			{
				MSG_RECALL_NACK message(
					MP_MOVE,
					MP_MOVE_RECALL_NACK_TO_SERVER,
					MSG_RECALL_NACK::TypeTargetIsNotExist,
					storedMessage.mRecallPlayer.mIndex,
					key );
				PACKEDDATA_OBJ->SendToMapServer(
					storedMessage.mRecallPlayer.mMapType,
					&message,
					sizeof( message ) );
				return;
			}

			targetPlayer->UpdateLogoutToDB( FALSE );
			targetPlayer->SetMapMoveInfo(
				storedMessage.mRecallPlayer.mMapType,
				DWORD( movePosition.x ),
				DWORD( movePosition.z ) );

			// 081218 LUJ, 값을 미리 복사해둔다
			const DWORD		playerIndex		= targetPlayer->GetID();
			const DWORD		userIndex		= targetPlayer->GetUserID();
			const MONEYTYPE	inventoryMoney	= targetPlayer->GetMoney( eItemTable_Inventory );
			const MONEYTYPE	storageMoney	= targetPlayer->GetMoney( eItemTable_Storage );
			const MAPTYPE	mapType			= storedMessage.mRecallPlayer.mMapType;
			const WORD		channelIndex	= WORD( storedMessage.mChannelIndex );

			g_pServerSystem->RemovePlayer( storedMessage.mTargetPlayer.mIndex );

			// 081218 LUJ, 업데이트보다 맵 이동이 빨리 진행될 경우 업데이트되지 않은 정보가 오게된다.
			//			이를 막기 위해 프로시저 처리 완료 후 진행한다
			UpdatePlayerOnMapOut(
				playerIndex,
				userIndex,
				inventoryMoney,
				storageMoney,
				mapType,
				channelIndex );

			// 081031 LUJ, 전 서버에 알린다
			{
				MSG_WORD messageToChannel;
				ZeroMemory( &messageToChannel, sizeof( messageToChannel ) );
				messageToChannel.Category	= MP_USERCONN;
				messageToChannel.Protocol	= MP_USERCONN_CHANGE_CHANNEL_SYN;
				messageToChannel.dwObjectID	= storedMessage.mTargetPlayer.mIndex;
				messageToChannel.wData		= WORD( storedMessage.mChannelIndex );
				g_Network.Broadcast2AgentServer(
					( char* )&messageToChannel,
					sizeof( messageToChannel ) );
			}

			// 081031 LUJ, 성공을 알린다
			{
				MSGBASE message;
				ZeroMemory( &message, sizeof( message ) );
				message.Category	= MP_MOVE;
				message.Protocol	= MP_MOVE_RECALL_ACK_TO_SERVER;
				message.dwObjectID	= key;

				PACKEDDATA_OBJ->SendToMapServer(
					storedMessage.mRecallPlayer.mMapType,
					&message,
					sizeof( message ) );
			}

			// 081031 LUJ, 같은 맵에 위치할 경우, 성공 처리를 한 후 삭제할 것이다.
			if( storedMessage.mRecallPlayer.mMapType != storedMessage.mTargetPlayer.mMapType )
			{
				PopRecallMessage( key );
			}

			break;
		}
		// 081031 LUJ, 성공하였음을 알린다
	case MP_MOVE_RECALL_ACK_TO_SERVER:
		{
			const MSGBASE*			m				= ( MSGBASE* )pMsg;
			const DWORD				key				= m->dwObjectID;
			const MSG_RECALL_SYN	storedMessage	= PopRecallMessage( key );

			CPlayer* recallPlayer = ( CPlayer* )g_pUserTable->FindUser( storedMessage.mRecallPlayer.mIndex );

			if(	!	recallPlayer ||
					recallPlayer->GetObjectKind() != eObjectKind_Player )
			{
				break;
			}

			MSG_RECALL_SYN message( storedMessage );
			message.Protocol = MP_MOVE_RECALL_ACK;

			recallPlayer->SendMsg(
				&message,
				sizeof( message	 ) );
			break;
		}
		// 081031 LUJ, 리콜 사용한 플레이어 측 문제로 소환 실패
		//
		//	알림:	스킬 사용 후 실패한 경우 사용한 리소스를 복구하지
		//			못한다. 필요한 경우 추가되어야 한다.
	case MP_MOVE_RECALL_NACK_TO_SERVER:
		{
			const MSG_RECALL_NACK*	m				= ( MSG_RECALL_NACK* )pMsg;
			const MSG_RECALL_SYN	storedMessage	= PopRecallMessage( m->mKey );

			CPlayer* player = ( CPlayer* )g_pUserTable->FindUser( m->dwObjectID );

			if( !	player	||
					player->GetObjectKind() != eObjectKind_Player )
			{
				break;
			}

			// 081031 LUJ, 대상에게 메시지 전송
			{
				MSG_RECALL_NACK message( *m );
				message.Protocol = MP_MOVE_RECALL_NACK;
				SafeStrCpy(
					message.mRecallPlayerName,
					storedMessage.mRecallPlayer.mName,
					sizeof( message.mRecallPlayerName ) );
				SafeStrCpy(
					message.mTargetPlayerName,
					storedMessage.mTargetPlayer.mName,
					sizeof( message.mTargetPlayerName ) );
				player->SendMsg(
					&message,
					sizeof( message ) );
			}

			break;
		}
		// 090316 LUJ, 스스로 쫓아오는 상태
	case MP_MOVE_VEHICLE_BEGIN_SYN:
		{
			MOVE_ONETARGETPOS* receivedMessage = ( MOVE_ONETARGETPOS* )pMsg;
			const DWORD playerIndex = receivedMessage->dwObjectID;
			CPlayer* const playerObject = ( CPlayer* )g_pUserTable->FindUser( playerIndex );

			if( 0 == playerObject || playerObject->GetObjectKind() != eObjectKind_Player )
			{
				break;
			}

			CVehicle* const vehicleObject = ( CVehicle* )g_pUserTable->FindUser( playerObject->GetSummonedVehicle() );

			if( 0 == vehicleObject || vehicleObject->GetObjectKind() != eObjectKind_Vehicle )
			{
				break;
			}
			else if( playerIndex != vehicleObject->GetOwnerIndex() )
			{
				break;
			}

			VECTOR3 vehiclePosition = *( CCharMove::GetPosition( vehicleObject ) );
			VECTOR3 startPosition = { 0 };
			receivedMessage->GetStartPos( &startPosition );

			if( 1000 < CalcDistanceXZ( &vehiclePosition, &startPosition ) )
			{
				CCharMove::CorrectPlayerPosToServer( vehicleObject );
				break;
			}

			MOVE_INFO* const vehicleMoveInfo = ( MOVE_INFO* )vehicleObject->GetMoveInfo();
			receivedMessage->GetTargetInfo( vehicleMoveInfo );

			vehicleObject->Move(
				playerIndex,
				startPosition,
				*( vehicleMoveInfo->GetTargetPosition( vehicleMoveInfo->GetCurTargetPosIdx() ) ) );

			MOVE_ONETARGETPOS_FROMSERVER message;
			ZeroMemory( &message, sizeof( message ) );
			message.Category = MP_MOVE;
			message.Protocol = MP_MOVE_VEHICLE_BEGIN_ACK;
			message.AddedMoveSpeed = vehicleMoveInfo->AddedMoveSpeed;
			message.dwObjectID = vehicleObject->GetID();
			message.SetStartPos( &startPosition );
			message.SetTargetPos( vehicleMoveInfo->GetTargetPosition( 0 ) );
			PACKEDDATA_OBJ->QuickSendExceptObjectSelf( vehicleObject, &message, message.GetSize() );
			break;
		}
	case MP_MOVE_VEHICLE_RECALL_SYN:
		{
			MOVE_POS* const receviedMessage = ( MOVE_POS* )pMsg;
			const DWORD playerIndex = receviedMessage->dwObjectID;
			const DWORD vehicleIndex = receviedMessage->dwMoverID;
			VECTOR3 recallPosition = { 0 };
			receviedMessage->cpos.Decompress( &recallPosition );

			CObject* const playerObject = g_pUserTable->FindUser( playerIndex );
			CVehicle* const vehicleObject = ( CVehicle* )g_pUserTable->FindUser( vehicleIndex );

			if( 0 == playerObject || playerObject->GetObjectKind() != eObjectKind_Player )
			{
				break;
			}
			else if( 0 == vehicleObject || vehicleObject->GetObjectKind() != eObjectKind_Vehicle )
			{
				break;
			}
			else if( playerIndex != vehicleObject->GetOwnerIndex() )
			{
				break;
			}

			VECTOR3 playerPosition = { 0 };
			playerObject->GetPosition( &playerPosition );
			VECTOR3 vehiclePosition = { 0 };
			vehicleObject->GetPosition( &vehiclePosition );

			if( 3000 > CalcDistanceXZ( &playerPosition, &vehiclePosition ) )
			{
				break;
			}

			vehicleObject->Recall( recallPosition );
			break;
		}
	}
}

// 081031 LUJ, 처리할 리콜 정보를 저장한다
void CMoveManager::SetRecallMessage( DWORD key, const MSG_RECALL_SYN& message )
{
	mRecallMessageMap[ key ] = message;
}

// 081031 LUJ, 서버에 저장된 리콜 정보를 반환한다
const MSG_RECALL_SYN& CMoveManager::GetRecallMessage( DWORD key ) const
{
	const RecallMessageMap::const_iterator it = mRecallMessageMap.find( key );

	if( mRecallMessageMap.end() == it )
	{
		static MSG_RECALL_SYN emptyMessage;

		return emptyMessage;
	}

	return it->second;
}

// 081031 LUJ, 리콜 정보가 있다면 반환후 컨테이너에서 삭제한다
MSG_RECALL_SYN CMoveManager::PopRecallMessage( DWORD key )
{
	MSG_RECALL_SYN message( GetRecallMessage( key ) );
	mRecallMessageMap.erase( key );

	return message;
}

// 081031 LUJ, 특정 플레이어를 소환한다
void CMoveManager::RequestToRecall( const MSG_RECALL_SYN& inputMessage )
{
	// 081031 LUJ, 상태 검사
	{
		CPlayer* player = ( CPlayer* )g_pUserTable->FindUser( inputMessage.mRecallPlayer.mIndex );

		if(	!	player ||
				player->GetObjectKind() != eObjectKind_Player )
		{
			return;
		}

		MSG_RECALL_NACK::Type errorType = MSG_RECALL_NACK::TypeNone;

		// 081031 LUJ, 상태 체크
		switch( player->GetState() )
		{
		case eObjectState_Die:
			{
				errorType = MSG_RECALL_NACK::TypePlayerDied;
				break;
			}
		case eObjectState_Deal:
			{
				errorType = MSG_RECALL_NACK::TypePlayerIsDealing;
				break;
			}
		case eObjectState_Exchange:
			{
				errorType = MSG_RECALL_NACK::TypePlayerIsExchanging;
				break;
			}
		case eObjectState_StreetStall_Guest:
			{
				errorType = MSG_RECALL_NACK::TypePlayerIsGuestInStall;
				break;
			}
		case eObjectState_StreetStall_Owner:
			{
				errorType = MSG_RECALL_NACK::TypePlayerIsOwnerInStall;
				break;
			}
		default:
			{
				if( player->IsShowdown() )
				{
					errorType = MSG_RECALL_NACK::TypePlayerIsOnShowdown;
				}
				else if( player->IsPKMode() )
				{
					errorType = MSG_RECALL_NACK::TypePlayerIsInPlayerKillMode;
				}

				break;
			}
		}

		if( MSG_RECALL_NACK::TypeNone != errorType )
		{
			MSG_RECALL_NACK message(
				MP_MOVE,
				MP_MOVE_RECALL_NACK,
				errorType,
				player->GetID(),
				inputMessage.mKey );
			player->SendMsg(
				&message,
				sizeof( message ) );
			return;
		}
	}	

	MSG_RECALL_SYN message( inputMessage );
	// 081031 LUJ, 해킹을 방지하기 위해 카운트 값을 키값으로 사용
	message.mKey		= gCurTime;

	// 081031 LUJ, 해킹 방지를 위해 서버에 저장한다
	SetRecallMessage(
		message.mKey,
		message );
	PACKEDDATA_OBJ->SendToMapServer(
		message.mTargetPlayer.mMapType,
		&message,
		sizeof( message ) );
}

// 081031 LUJ, 플레이어를 소환한다
void CMoveManager::Recall( DWORD key, DWORD guildScore, DWORD familyPoint )
{
	const MSG_RECALL_SYN& storedMessage = GetRecallMessage( key );

	CPlayer* recallPlayer = ( CPlayer* )g_pUserTable->FindUser( storedMessage.mRecallPlayer.mIndex );

	if( !	recallPlayer ||
			recallPlayer->GetObjectKind() != eObjectKind_Player )
	{
		MSG_RECALL_NACK message(
			MP_MOVE,
			MP_MOVE_RECALL_NACK_TO_SERVER,
			MSG_RECALL_NACK::TypePlayerIsNotExist,
			storedMessage.mTargetPlayer.mIndex,
			key );
		PACKEDDATA_OBJ->SendToMapServer(
			storedMessage.mRecallPlayer.mMapType,
			&message,
			sizeof( message ) );
		return;
	}
	
	MSG_RECALL_NACK::Type	errorType			= MSG_RECALL_NACK::TypeNone;
	const BOOL				skillNeedsToRecall	= ( UINT_MAX != storedMessage.mSkillIndex );

	// 081031 LUJ, 스킬이 
	if( skillNeedsToRecall )
	{
		const cActiveSkillInfo* const activeSkillInfo = SKILLMGR->GetActiveInfo( storedMessage.mSkillIndex );

		if( ! activeSkillInfo )
		{
			errorType = MSG_RECALL_NACK::TypePlayerHasNoSkill;
		}

		switch(activeSkillInfo->GetInfo().mConsumePoint.mType)
		{
		case ACTIVE_SKILL_INFO::ConsumePoint::TypeFamily:
			{
				if( familyPoint < activeSkillInfo->GetInfo().mConsumePoint.mValue )
				{
					errorType = MSG_RECALL_NACK::TypeSkillNeedsMoreFamilyPoint;
				}

				break;
			}
		case ACTIVE_SKILL_INFO::ConsumePoint::TypeGuild:
			{
				if( guildScore < activeSkillInfo->GetInfo().mConsumePoint.mValue )
				{
					errorType = MSG_RECALL_NACK::TypeSkillNeedsMoreGuildScore;
				}

				break;
			}
		}

		if( MSG_RECALL_NACK::TypeNone == errorType )
		{
			MAINTARGET mainTarget;
			ZeroMemory( &mainTarget, sizeof( mainTarget ) );
			mainTarget.dwMainTargetID	= recallPlayer->GetID();

			MSG_SKILL_START_SYN skillMessage;
			ZeroMemory( &skillMessage, sizeof( skillMessage ) );
			const VECTOR3 dirVector = {0.f, 0.f, -1.f};
			skillMessage.InitMsg(
				storedMessage.mSkillIndex,
				&mainTarget,
				dirVector,
				recallPlayer->GetID() );

			// 081031 LUJ, 스킬 사용
			switch( SKILLMGR->OnSkillStartSyn( &skillMessage ) )
			{
			case SKILL_SUCCESS:
				{
					break;
				}
			case SKILL_COOLING:
				{
					errorType = MSG_RECALL_NACK::TypePlayerFailedUsingSkill;
					break;
				}
			case SKILL_INFO_INVALUED:
				{
					errorType = MSG_RECALL_NACK::TypeSkillIsNotExist;
					break;
				}
			default:
				{
					errorType = MSG_RECALL_NACK::TypePlayerFailedUsingSkill;
					break;
				}
			}
		}
	}

	// 081031 LUJ, 에러가 발생한 경우 오류 메시지 전송
	if( MSG_RECALL_NACK::TypeNone != errorType )
	{
		// 081031 LUJ, 리콜 사용자에게도 알린다
		{
			MSG_RECALL_NACK message(
				MP_MOVE,
				MP_MOVE_RECALL_NACK,
				errorType,
				recallPlayer->GetID(),
				key );
			SafeStrCpy(
				message.mRecallPlayerName,
				storedMessage.mRecallPlayer.mName,
				sizeof( message.mRecallPlayerName ) );
			SafeStrCpy(
				message.mTargetPlayerName,
				storedMessage.mTargetPlayer.mName,
				sizeof( message.mTargetPlayerName ) );
			recallPlayer->SendMsg(
				&message,
				sizeof( message ) );
		}

		// 081031 LUJ, 대상에게 알린다
		{
			MSG_RECALL_NACK message(
				MP_MOVE,
				MP_MOVE_RECALL_NACK_TO_SERVER,
				errorType,
				storedMessage.mTargetPlayer.mIndex,
				key );
			PACKEDDATA_OBJ->SendToMapServer(
				storedMessage.mTargetPlayer.mMapType,
				&message,
				sizeof(message));
		}

		return;
	}

	// 081031 LUJ, 대상을 소환한다
	{
		VECTOR3 recallPlayerPosition = { 0 };
		recallPlayer->GetPosition( &recallPlayerPosition );

		const float range		= float( storedMessage.mRandomPositionRange );
		const float randomRange	= float( rand() % int( range ) - ( range / 2.0f ) );

		MSG_DWORD2 message;
		ZeroMemory( &message, sizeof( message ) );
		message.Category	= MP_MOVE;
		message.Protocol	= MP_MOVE_RECALL_SYN_TO_SERVER;
		message.dwObjectID	= key;
		message.dwData1		= DWORD( recallPlayerPosition.x + randomRange );
		message.dwData2		= DWORD( recallPlayerPosition.z + randomRange );

		PACKEDDATA_OBJ->SendToMapServer(
			storedMessage.mTargetPlayer.mMapType,
			&message,
			sizeof(message));
	}
}
