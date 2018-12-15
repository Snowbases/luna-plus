#include "StdAfx.h"
#include "Manager.h"
#include "ObjectManager.h"
#include "MHAudioManager.h"
#include "..\MHCamera.h"
#include "GameIn.h"
#include "MoveManager.h"
#include "DungeonMgr.h"
#include "TriggerTimerDlg.h"
#include "WeatherManager.h"
#include "..\TileManager.h"
#include "..\MHMap.h"
#include "..\cMsgBox.h"
#include "..\interface\cWindowManager.h"
#include "..\WindowIDEnum.h"
#include "..\ChatManager.h"
#include "..\ObjectStateManager.h"
#include "..\Input\UserInput.h"
#include "..\CristalEmpireBonusDialog.h"

void MessageBoxEvent(LONG windowIndex, LPVOID eventThrower, DWORD windowEvent)
{
	Trigger::CManager::GetInstance().OnActionEvent(
		windowIndex,
		eventThrower,
		windowEvent);
}

namespace Trigger
{
	CManager::CManager(void)
	{}

	CManager::~CManager(void)
	{
		for(EngineEffectContainer::iterator iterator = mEngineEffectContainer.begin();
			mEngineEffectContainer.end() != iterator;
			++iterator)
		{
			CEngineEffect* engineEffect = iterator->second;
			SAFE_DELETE(engineEffect);
		}
	}

	CManager& CManager::GetInstance()
	{
		static CManager instance;
		return instance;
	}

	void CManager::NetworkMsgParse(BYTE Protocol,void* pMsg)
	{
		switch(Protocol)
		{
		case MP_TRIGGER_ADDEFFECT_ACK:
			{
				MSG_DWORD5* pmsg = (MSG_DWORD5*)pMsg;
				DWORD dwObjectIndex = pmsg->dwObjectID;
				WORD  wEffectIndex = (WORD)pmsg->dwData1;
				BOOL  bMapEffect = (BOOL)pmsg->dwData2;

				if(bMapEffect)
				{
					// 맵에 붙이는 이펙트
					VECTOR3 Pos;
					Pos.x = (float)pmsg->dwData3;
					Pos.y = (float)pmsg->dwData4;
					Pos.z = (float)pmsg->dwData5;
					OBJECTEFFECTDESC desc( (WORD)pmsg->dwData1, EFFECT_FLAG_ABSOLUTEPOS, &Pos );
					HERO->AddObjectEffect( pmsg->dwData1, &desc, 1 );
				}
				else
				{
					AttachEffect(dwObjectIndex, wEffectIndex);
				}				
			}
			break;

		case MP_TRIGGER_REMOVEEFFECT_ACK:
			{
				MSG_DWORD2* pmsg = (MSG_DWORD2*)pMsg;
				DWORD dwObjectIndex = pmsg->dwObjectID;
				WORD  wEffectIndex = (WORD)pmsg->dwData1;
				BOOL  bMapEffect = (BOOL)pmsg->dwData2;

				if(bMapEffect)
				{
					HERO->RemoveObjectEffect(wEffectIndex);
				}
				else
				{
					DetatchEffect(dwObjectIndex, wEffectIndex);
				}
			}
			break;

		case MP_TRIGGER_PLAYSOUND_ACK:
			{
				MSG_WORD2* pmsg = (MSG_WORD2*)pMsg;
				BOOL bIsBGM = (BOOL)pmsg->wData1;
				WORD wSoundNum = pmsg->wData2;
				
				if( bIsBGM )
					AUDIOMGR->SmoothChangeBGM( (SNDIDX)wSoundNum );
				else
					AUDIOMGR->Play(
					wSoundNum,
					gHeroID);
			}
			break;
		case MP_TRIGGER_CAMERA_SHAKE_ACK:
			{
				MSG_WORD3* pmsg = (MSG_WORD3*)pMsg;
				const WORD ShakeNum		= pmsg->wData1;
				const WORD ShakeAmp		= pmsg->wData2;
				const WORD ShakeTimes	= pmsg->wData3;

				if(ShakeNum < 1 || ShakeAmp < 0 || ShakeTimes < 1)
					break;

				CAMERA->SetShaking(ShakeNum, ShakeAmp, ShakeTimes);
			}
			break;

		case MP_TRIGGER_CAMERA_FILTER_ATTACH:
			{
				MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg;
				// 100119 ShinJS --- 카메라 필터 설정값 추가(반복설정, FadeIn 설정, Fade 배속[기준값=100])
				const DWORD dwFilterIdx = pmsg->dwData1;
				const BOOL bNoRepeat = (BOOL)pmsg->dwData2;
				const BOOL bFadeIn = (BOOL)pmsg->dwData3;
				const float fFadeTime = pmsg->dwData4 / 1000.f;

				CAMERA->AttachCameraFilter( dwFilterIdx, bNoRepeat, bFadeIn, fFadeTime ) ;
			}
			break;

		case MP_TRIGGER_CAMERA_FILTER_DETACH:
			{
				MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
				// 100119 ShinJS --- 카메라 필터 설정값 추가(FadeOut 설정, Fade 배속[기준값=100])
				const DWORD dwFilterIdx = pmsg->dwData1;
				const BOOL bFadeOut = (BOOL)pmsg->dwData2;
				const float fFadeTime = pmsg->dwData3 / 1000.f;

				CAMERA->DetachCameraFilter( dwFilterIdx, bFadeOut, fFadeTime ) ;
			}
			break;

		case MP_TRIGGER_CAMERA_FILTER_MOVE_ACK:
			{
				MSG_DWORD6* pmsg = (MSG_DWORD6*)pMsg;
				const DWORD dwFilterIdx = pmsg->dwData1;
				const BOOL bNoRepeat = (BOOL)pmsg->dwData2;
				const BOOL bFadeIn = (BOOL)pmsg->dwData3;
				const float fFadeTime = pmsg->dwData4 / 1000.f;

				VECTOR3 vecPos={0,};
				vecPos.x = (float)pmsg->dwData5;
				vecPos.z = (float)pmsg->dwData6;

				// Hero를 정지 시킨다(EndMove 처리)
				if( HERO->GetState() == eObjectState_Move )
					OBJECTSTATEMGR->EndObjectState( HERO, eObjectState_Move );

				MOVE_INFO* pMoveInfo = HERO->GetBaseMoveInfo();
				pMoveInfo->m_bEffectMoving = FALSE;
				pMoveInfo->bMoving = FALSE;
				pMoveInfo->InitTargetPosition();
				HERO->ClearWayPoint();
				HERO->SetFollowPlayer( 0 );

				// 탈것을 소유한 경우 정지 시킨다.
				CVehicle* pVehicle = (CVehicle*)OBJECTMGR->GetObject( HERO->GetVehicleID() );

				if( pVehicle &&
					pVehicle->GetObjectKind() == eObjectKind_Vehicle &&
					pVehicle->GetOwnerIndex() == gHeroID)
				{
					MOVEMGR->EndMove( pVehicle );
					pVehicle->SetFollow( FALSE );
				}

				// Filter Play동안 이동 입력 처리를 막는다.
				g_UserInput.SetAllowMouseClick( FALSE );

				CCameraFilterObject& cameraFilterObject = CAMERA->AttachCameraFilter( dwFilterIdx, bNoRepeat, bFadeIn, fFadeTime, 0, eCAMERA_FILTER_ACTION_MOVE );
				cameraFilterObject.SetFilterActionMovePos( vecPos );
			}
			break;

		case MP_TRIGGER_TIMER_START:
			{
				MSG_DWORD4* pmsg = (MSG_DWORD4*)pMsg;

				GAMEIN->GetTriggerTimerDlg()->Start((eTimerType)pmsg->dwData1, pmsg->dwData3, pmsg->dwData4);

				if(eTimerState_Run != (eTimerState)pmsg->dwData2)
				{
					GAMEIN->GetTriggerTimerDlg()->SetState((eTimerState)pmsg->dwData2);
				}
			}
			break;

		case MP_TRIGGER_TIMER_STATE:
			{
				MSG_DWORD* pmsg = (MSG_DWORD*)pMsg;
				GAMEIN->GetTriggerTimerDlg()->SetState((eTimerState)pmsg->dwData);
			}
			break;
		case MP_TRIGGER_ANIMATION_BEGIN_ACK:
			{
				TESTMSG* const message = (TESTMSG*)pMsg;
				CObject* const object = OBJECTMGR->GetObject(message->dwObjectID);

				if(0 == object)
				{
					break;
				}

				// object를 정지 시킨다
				if( object->GetState() == eObjectState_Move )
					OBJECTSTATEMGR->EndObjectState( object, eObjectState_Move );

				MOVE_INFO* pMoveInfo = object->GetBaseMoveInfo();
				pMoveInfo->m_bEffectMoving = FALSE;
				pMoveInfo->bMoving = FALSE;
				pMoveInfo->InitTargetPosition();

				if( object->GetID() == HEROID )
				{
					HERO->ClearWayPoint();
					HERO->SetFollowPlayer( 0 );
				}


				DWORD effectNumber = 0; 

				// 100924 ShinJS ,로 여러개 입력시 랜덤하게 선택하여 실행하도록 한다.
				const LPCTSTR delimit = "/";
				char receivedFileName[MAX_PATH]={0};
				SafeStrCpy( receivedFileName, message->Msg, sizeof(receivedFileName)/sizeof(*receivedFileName) );
				
				if( strtok( message->Msg, delimit ) != NULL )
				{
					std::vector< std::string > beffnameList;
					char* beffname = strtok( receivedFileName, delimit );
					beffnameList.push_back( beffname );

					while( (beffname = strtok( 0, delimit )) != NULL )
					{
						beffnameList.push_back( beffname );
					}

					int randpos = rand() % beffnameList.size();
					const std::string& randbeffname = beffnameList[ randpos ];
					effectNumber = FindEffectNum( randbeffname.c_str() );
				}
				else
				{
					effectNumber = FindEffectNum( receivedFileName );
				}

				OBJECTEFFECTDESC description(effectNumber);
				object->AddObjectEffect(
					effectNumber,
					&description,
					1);
			}
			break;

		case MP_TRIGGER_WEATHER_CHANGE_ACK:
			{
				MSG_DWORD3* pmsg = (MSG_DWORD3*)pMsg;
				DWORD dwWeatherHashCode = pmsg->dwData1;
				BOOL bOn = (pmsg->dwData2 ? TRUE : FALSE);
				WORD wIntensity = (WORD)pmsg->dwData3;

				if( bOn )
				{
					WEATHERMGR->EffectOn( dwWeatherHashCode, wIntensity );
				}
				else
				{
					WEATHERMGR->EffectOff();
				}
			}
			break;
		case MP_TRIGGER_ENGINE_EFFECT_ACK:
			{
				const MSG_NAME_DWORD5* const message = (MSG_NAME_DWORD5*)pMsg;
				LPCTSTR fileName = message->Name;
				const float positionX = float(message->dwData1);
				const float positionZ = float(message->dwData2);
				const int motionIndex = message->dwData3;
                const float angle = message->dwData4;
				const BOOL isLoop = message->dwData5;

				VECTOR3 position = {0};
				position.x = positionX;
				position.z = positionZ;
				g_pExecutive->GXMGetHFieldHeight(
					&(position.y),
					position.x,
					position.z);

				AddEngineEffect(
					fileName,
					position,
					motionIndex,
					angle,
					isLoop);
			}
			break;
		case MP_TRIGGER_COLLISION_TILE_ACK:
			{
				const MSG_NAME_DWORD5* const message = (MSG_NAME_DWORD5*)pMsg;
				LPCTSTR name = message->Name;
				RECT rect = {0};
				rect.left = message->dwData1;
				rect.top = message->dwData2;
				rect.right = message->dwData3;
				rect.bottom = message->dwData4;
				const float angle = LOWORD(message->dwData5);
				const BOOL isCollide = HIWORD(message->dwData5);

				SetCollisionTile(
					name,
					rect,
					angle,
					isCollide);
			}
			break;
		case MP_TRIGGER_SELECT_FLAG_ACK:
			{
				const MSG_NAME_DWORD5* const message = (MSG_NAME_DWORD5*)pMsg;
				LPCTSTR name = message->Name;
				const DWORD valueAccept = message->dwData1;
				const DWORD valueReject = message->dwData2;
				const DWORD messageIndex = LOWORD(message->dwData3);
				const DWORD waitSecond = HIWORD(message->dwData3);
				const DWORD ownerType = message->dwData4;
				const DWORD ownerIndex = message->dwData5;

				ZeroMemory(
					&mFlag,
					sizeof(mFlag));
				mFlag.mValueAccept = valueAccept;
				mFlag.mValueReject = valueReject;
				mFlag.mOwnerType = ownerType;
				mFlag.mOwnerIndex = ownerIndex;
				mFlag.mHashCode = GetHashCodeFromTxt(
					name);

				PutMessageBox(
					messageIndex,
					waitSecond);
			}
			break;
		case MP_TRIGGER_ITEM_OPTION_REQUEST_ACK:
			{
				CCristalEmpireBonusDialog* const dialog = (CCristalEmpireBonusDialog*)WINDOWMGR->GetWindowForID(
					CI_BGSTATIC_0);

				if(0 == dialog)
				{
					break;
				}

				const MSG_ITEM_OPTION* const message = (MSG_ITEM_OPTION*)pMsg;

				dialog->Ready(
					*message);
				dialog->SetAbsXYAtCenter();
				dialog->SetActive(
					TRUE);
			}
			break;
		case MP_TRIGGER_ITEM_OPTION_APPLY_ACK:
			{
				CCristalEmpireBonusDialog* const dialog = (CCristalEmpireBonusDialog*)WINDOWMGR->GetWindowForID(
					CI_BGSTATIC_0);

				if(0 == dialog)
				{
					break;
				}

				const MSG_ITEM_OPTION* const message = (MSG_ITEM_OPTION*)pMsg;

				dialog->Finish(
					*message);
			}
			break;
		case MP_TRIGGER_ADD_SHOCKED_TIME_ACK:
			{
				const MSG_DWORD2* const message = (MSG_DWORD2*)pMsg;
				CObject* object = OBJECTMGR->GetObject(message->dwData1);
				if( object == NULL )
					break;

				object->SetShockTime( message->dwData2 );
			}
			break;
		case MP_TRIGGER_DROP_OBJECT_ON_ACK:
			{
				const MSG_INT2* const message = (MSG_INT2*)pMsg;
				CObject* object = OBJECTMGR->GetObject(message->dwObjectID);
				if( object == NULL )
					break;

				const float fGravityAcceleration = message->nData1 / 1000.f;
				const DWORD dwValidTime = (DWORD)max( message->nData2, 0 );

				// object를 정지 시킨다(EndMove 처리)
				if( object->GetState() == eObjectState_Move )
					OBJECTSTATEMGR->EndObjectState( object, eObjectState_Move );

				MOVE_INFO* pMoveInfo = object->GetBaseMoveInfo();
				pMoveInfo->m_bEffectMoving = FALSE;
				pMoveInfo->bMoving = FALSE;
				pMoveInfo->InitTargetPosition();

				if( object->GetID() == HEROID )
				{
					HERO->ClearWayPoint();
					HERO->SetFollowPlayer( 0 );
				}

				object->ApplyGravity( TRUE, fGravityAcceleration, dwValidTime );
			}
			break;
		case MP_TRIGGER_DROP_OBJECT_OFF_ACK:
			{
				const MSGBASE* const message = (MSGBASE*)pMsg;
				CObject* object = OBJECTMGR->GetObject(message->dwObjectID);
				if( object == NULL )
					break;

				object->ApplyGravity( FALSE, 0.f, 0 );
			}
			break;
		}
	}

	void CManager::AttachEffect( DWORD dwObjectIndex, WORD wEffectIndex )
	{
		CObject* pObject = OBJECTMGR->GetObject(dwObjectIndex);
		if(! pObject)
			return;

		OBJECTEFFECTDESC desc( wEffectIndex );
		pObject->AddObjectEffect( wEffectIndex, &desc, 1 );

		stEffectInfo& effectInfo = mEffectInfoContainer[dwObjectIndex];
		effectInfo.dwObjectIndex = dwObjectIndex;
		effectInfo.dwEffectIndex = wEffectIndex;
	}

	void CManager::DetatchEffect( DWORD dwObjectIndex, WORD wEffectIndex )
	{
		CObject* pObject = OBJECTMGR->GetObject(dwObjectIndex);
		if(! pObject)
			return;

		pObject->RemoveObjectEffect(wEffectIndex);
		mEffectInfoContainer.erase(dwObjectIndex);
	}

	void CManager::RegenEffect( DWORD dwObjectIndex )
	{
		const EffectInfoContainer::const_iterator iterator = mEffectInfoContainer.find(dwObjectIndex);

		if(mEffectInfoContainer.end() == iterator)
		{
			return;
		}

		CObject* const pObject = OBJECTMGR->GetObject(dwObjectIndex);

		if(0 == pObject)
		{
			return;
		}

		const stEffectInfo& effectInfo = iterator->second;
		OBJECTEFFECTDESC desc(effectInfo.dwEffectIndex);
		pObject->AddObjectEffect(
			effectInfo.dwEffectIndex,
			&desc,
			1);
	}

	void CManager::RemoveEngineEffect()
	{
		// 100111 LUJ, 부하 감소를 위해 매 프로세스에 한번만 처리한다
		if(mEngineEffectQueue.empty())
		{
			return;
		}

		const DWORD hashCode = mEngineEffectQueue.front();
		mEngineEffectQueue.pop();

		const EngineEffectContainer::iterator iterator = mEngineEffectContainer.find(hashCode);

		if(mEngineEffectContainer.end() == iterator)
		{
			return;
		}

		CEngineEffect* engineEffect = iterator->second;

		// 100111 LUJ, 모션이 종료된 이펙트를 제거한다
		if(FALSE == engineEffect->IsRepeat() &&
			engineEffect->IsEndMotion())
		{
			SAFE_DELETE(engineEffect);
			mEngineEffectContainer.erase(hashCode);
			return;
		}

		mEngineEffectQueue.push(hashCode);
	}

	void CManager::Process()
	{
		RemoveEngineEffect();
	}

	void CManager::AddEngineEffect(LPCTSTR fileName, const VECTOR3& position, int motionIndex, float angle, BOOL isRepeat)
	{
		const DWORD hashCode = GetHashCodeFromTxt(fileName);
		CEngineEffect* engineEffect = 0;
		EngineEffectContainer::iterator iterator = mEngineEffectContainer.find(hashCode);

		if(mEngineEffectContainer.end() == iterator)
		{
			engineEffect = new CEngineEffect;
			mEngineEffectContainer.insert(
				std::make_pair(hashCode, engineEffect));

			TCHAR path[MAX_PATH] = {0};
			_stprintf(
				path,
				"data/3dData/monster/%s",
				fileName);
			engineEffect->Init(
				path,
				0,
				eEngineObjectType_Effect,
				0,
				WORD(-1));
			mEngineEffectQueue.push(hashCode);
		}
		else
		{
			engineEffect = iterator->second;
		}

		engineEffect->SetEngObjAngle(DEGTORAD(angle));
		engineEffect->ApplyHeightField(TRUE);
		engineEffect->SetEngObjPosition((VECTOR3*)&position);
		engineEffect->ChangeMotion(
			motionIndex,
			isRepeat);
		engineEffect->SetRepeat(isRepeat);
	}

	void CManager::SetCollisionTile(LPCTSTR name, const RECT& definedArea, float angle, BOOL isCollide)
	{
#ifdef _GMTOOL_
		// 100111 LUJ, 타일의 충돌 속성을 변경하므로, 보기 쉽도록 디버그 정보를 표시해주자
		MAP->SetRenderTileData(TRUE);
#endif
		RECT area = definedArea;
		const DWORD hashCode = GetHashCodeFromTxt(name);
		
		if(definedArea.left > definedArea.right)
		{
			area.left = definedArea.right;
			area.right = definedArea.left;
		}

		if(definedArea.top > definedArea.bottom)
		{
			area.top = definedArea.bottom;
			area.bottom = definedArea.top;
		}

		// 101111 LUJ, 같은 이름으로 지정된 충돌 타일을 제거한다
		RestoreTile(hashCode);
		TilePositionSet& tilePositionSet = mBlockContainer[hashCode];
		CTileManager* const tileManager = MAP->GetTileManager();

		const SIZE size = {
			area.right - area.left,
			area.bottom - area.top
		};
		const POINT centerPosition = {
			size.cx / 2,
			size.cy / 2};
		const float cosignTheta = cos(DEGTORAD(angle));
		const float signTheta = sin(DEGTORAD(angle));

		typedef void (CTileManager::*FunctionPointer)(CObject*, VECTOR3*, AREATILE);
		FunctionPointer functionPointer = (isCollide ? &(CTileManager::AddTileAttrByAreaData) : &(CTileManager::RemoveTileAttrByAreaData));

		for(int left = centerPosition.x * -1;
			left < centerPosition.x;
			left += TILECOLLISON_DETAIL)
		{
			for(int top = centerPosition.y * -1;
				top < centerPosition.y;
				top += TILECOLLISON_DETAIL)
			{
				VECTOR3 position = {
					left * cosignTheta - top * signTheta + centerPosition.x + area.left,
					0,
					left * signTheta + top * cosignTheta + centerPosition.y + area.top};
				(tileManager->*functionPointer)(
					0,
					&position,
					SKILLAREA_ATTR_BLOCK);
				tilePositionSet.insert(
					TilePosition(position.x, position.z));
			}
		}
	}

	void CManager::RestoreTile(HashCode hashCode)
	{
		BlockContainer::iterator blockContainer = mBlockContainer.find(hashCode);

		if(mBlockContainer.end() == blockContainer)
		{
			return;
		}

		CTileManager* const tileManager = MAP->GetTileManager();
		TilePositionSet& tilePositionSet = blockContainer->second;

		for(TilePositionSet::const_iterator tileIterator = tilePositionSet.begin();
			tilePositionSet.end() != tileIterator;
			++tileIterator)
		{
			const TilePosition& tilePosition = *tileIterator;

			VECTOR3 position = {tilePosition.first, 0, tilePosition.second};
			tileManager->RemoveTileAttrByAreaData(
				0,
				&position,
				SKILLAREA_ATTR_BLOCK);
		}

		tilePositionSet.clear();
	}

	void CManager::RestoreTile()
	{
		for(BlockContainer::const_iterator blockContainer = mBlockContainer.begin();
			mBlockContainer.end() != blockContainer;
			++blockContainer)
		{
			RestoreTile(blockContainer->first);
		}
	}

	void CManager::PutMessageBox(DWORD messageIndex, DWORD waitSecond)
	{
		cMsgBox* const messageBox = WINDOWMGR->MsgBox(
			MBI_NOTICE,
			MBT_YESNO,
			CHATMGR->GetChatMsg(messageIndex));

		if(0 == messageBox)
		{
			return;
		}

		messageBox->SetMBFunc(
			::MessageBoxEvent);
		messageBox->SetVisibleTime(
			waitSecond * 1000);
	}

	void CManager::OnActionEvent(LONG windowIndex, LPVOID windowControl, DWORD windowEvent)
	{
		DWORD value = 0;

		switch(windowEvent)
		{
		case MBI_YES:
			{
				value = mFlag.mValueAccept;
				break;
			}
		case MBI_NO:
			{
				value = mFlag.mValueReject;
				break;
			}
		}

		MSG_DWORD4 message;
		ZeroMemory(
			&message,
			sizeof(message));
		message.Category = MP_TRIGGER;
		message.Protocol = MP_TRIGGER_SET_FLAG_SYN;
		message.dwObjectID = gHeroID;
		message.dwData1 = mFlag.mHashCode;
		message.dwData2 = value;
		message.dwData3 = mFlag.mOwnerIndex;
		message.dwData4 = mFlag.mOwnerType;
		NETWORK->Send(
			&message,
			sizeof(message));
	}
}