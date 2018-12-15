#include "Stdafx.h"
#include "ItemManager.h"
#include "GameResourceManager.h"
#include "AppearanceManager.h"
#include "AppearPlayer.h"
#include "Player.h"
#include "cHousingMgr.h"
#include "ChatManager.h"

CAppearPlayer::CAppearPlayer(void)
{
	// 캐릭터 표현 처리에 사용할 내부 변수들 초기화.
	InitData() ;
}

CAppearPlayer::~CAppearPlayer(void)
{}

void CAppearPlayer::SetForEffect(BOOL isForEffect)
{
	m_IsForEffect = isForEffect ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: SetPlayer
//	DESC		: 모습 표현 대상 Player 정보를 설정한다.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 05, 2009
//-------------------------------------------------------------------------------------------------
void CAppearPlayer::SetPlayer( CPlayer* pPlayer )
{
	// 함수 인자를 확인한다.
	if( !pPlayer ) return ;


	// Player 정보를 설정한다.
	m_pPlayer = pPlayer ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: SetEngineObject
//	DESC		: 모습 표현 대상 엔진 오브젝트 정보를 설정한다.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 05, 2009
//-------------------------------------------------------------------------------------------------
void CAppearPlayer::SetEngineObject( CEngineObject* pEngineObject)
{
	// 함수 인자를 확인한다.
	if( !pEngineObject ) return ;


	// 엔진 오브젝트 정보를 설정한다.
	m_pEngineObject = pEngineObject ;
}


// @@@ Iros 최소사양시 제공되는 박스맨 모습.
void CAppearPlayer::SetBaseAppearPlayer()
{
	// 모습을 갱신 할 Player와 엔진 오브젝트 정보의 유효성을 확인한다.
	if( !m_pPlayer || !m_pEngineObject ) return ;

	// 활성화 중이던 Effect를 종료한다.
	m_pPlayer->RemoveEffects() ;

	// 보여주기 여부를 담은 플래그 구조체를 초기화 한다.
	InitShowFlag() ;

	// MOD 파트 처리를 한다.
	if( !InitMOD() ) return ;

	MOD_LIST* pModList = &GAMERESRCMNGR->m_ModFileList[ m_pTotalInfo->Race ][ m_pTotalInfo->Gender ] ;

	int BoxHeadNum = 1722;
	int BoxBodyNum = 1723;
	int BoxHandNum = 1724;
	int BoxBootsNum = 1725;

	m_pEngineObject->ChangePart(eAppearPart_Face,"NULL.MOD") ;
	m_pPlayer->SetFaceObject( NULL ) ;

	m_pEngineObject->ChangePart(eAppearPart_Body, pModList->ModFile[ BoxBodyNum ]);
	m_pEngineObject->ChangePart(eAppearPart_Foot, pModList->ModFile[ BoxBootsNum ]);
	m_pEngineObject->ChangePart(eAppearPart_Hand, pModList->ModFile[ BoxHandNum ]);
	m_pEngineObject->ChangePart(eAppearPart_Hair, pModList->ModFile[ BoxHeadNum ]);

	// 스케일 처리를 한다.
	if( !InitScale() ) return ;

	// 위치 설정 처리를 한다.
	if( !InitPos() ) return ;

	APPEARANCEMGR->HideWeapon(m_pPlayer,m_pEngineObject) ;

	if(const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo(m_pPlayer->GetWearedItemIdx(eWearedItem_Weapon)))
	{
		int boxRhandWeapon = 1727;

		switch(itemInfo->Part3DType)
		{
		case ePartType_TwoBlade:
			{
				if(eWeaponType_BothDagger == itemInfo->WeaponType)
				{
					CEngineObject* pRightWeapon = new CEngineObject ;
					pRightWeapon->Init(
						pModList->ModFile[boxRhandWeapon],
						NULL,
						eEngineObjectType_Weapon) ;
					m_pEngineObject->AttachWeapon(
						pRightWeapon,
						RIGHT_HAND) ;

					CEngineObject* pLeftWeapon = new CEngineObject ;
					pLeftWeapon->Init(
						pModList->ModFile[boxRhandWeapon],
						NULL,
						eEngineObjectType_Weapon) ;
					m_pEngineObject->AttachWeapon(
						pLeftWeapon,
						LEFT_HAND) ;
				}

				break;
			}
		default:
			{
				CEngineObject* pWeapon = new CEngineObject ;

				if(eWeaponType_Bow == itemInfo->WeaponType)
				{
					int boxBowWeapon = 1728;

					pWeapon->Init(
						pModList->ModFile[boxBowWeapon],
						NULL,
						eEngineObjectType_Weapon) ;
					m_pEngineObject->AttachWeapon(
						pWeapon,
						LEFT_HAND);
				}
				else
				{
					pWeapon->Init(
						pModList->ModFile[boxRhandWeapon],
						NULL,
						eEngineObjectType_Weapon);
					m_pEngineObject->AttachWeapon(
						pWeapon,
						RIGHT_HAND);
				}

				break;
			}
		}
	}

	if(const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo(m_pPlayer->GetWearedItemIdx(eWearedItem_Shield)))
	{
		int boxShield = 1726;

		CEngineObject* pShield	= new CEngineObject ;
		pShield->Init(
			pModList->ModFile[boxShield],
			NULL,
			eEngineObjectType_Weapon) ;

		m_pEngineObject->AttachWeapon(
			pShield,
			LEFT_HAND);
	}

	m_pEngineObject->EnableSelfIllumin() ;
}


//-------------------------------------------------------------------------------------------------
//	NAME		: Renewal
//	DESC		: Player의 모습을 갱신한다.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 05, 2009
//-------------------------------------------------------------------------------------------------
void CAppearPlayer::RenewalAll()
{
	// 모습을 갱신 할 Player와 엔진 오브젝트 정보의 유효성을 확인한다.
	if( !m_pPlayer || !m_pEngineObject ) return ;

	// 활성화 중이던 Effect를 종료한다.
	m_pPlayer->RemoveEffects() ;

	// 보여주기 여부를 담은 플래그 구조체를 초기화 한다.
	InitShowFlag() ;

	// MOD 파트 처리를 한다.
	if( !InitMOD() ) return ;

	// 머리(헤어) 처리를 한다.
	if( !InitHair() ) return ;

	// 바뀌는 부분들 처리를 한다.
	if( !ChangePart() ) return ;

	// 붙여지는 부분들 처리를 한다.
	if( !AttachPart() ) return ;

	// 얼굴 파트 처리를 한다.
	if( !InitHead() ) return ;

	// 스케일 처리를 한다.
	if( !InitScale() ) return ;

	// 위치 설정 처리를 한다.
	if( !InitPos() ) return ;

	// 무기를 표현한다.
	if( !AppearWeapone() ) return ;

	// HERO 캐릭터는 자체 마우스 피킹이 안되도록 한다.
	if(m_pPlayer->GetID() == HEROID)
	{
		m_pEngineObject->DisablePick() ;
	}

	// 엔젠 오브젝트의 자체 라이트 효과 처리를 한다.
	m_pEngineObject->EnableSelfIllumin() ;

	m_pPlayer->ClearChangeParts();
}

void CAppearPlayer::Renewal(DWORD changeFlag)
{
	// 모습을 갱신 할 Player와 엔진 오브젝트 정보의 유효성을 확인한다.
	if( !m_pPlayer || !m_pEngineObject ) return ;

	m_pTotalInfo = m_pPlayer->GetCharacterTotalInfo() ;

	// 활성화 중이던 Effect를 종료한다.
	m_pPlayer->RemoveEffects() ;

	// 보여주기 여부를 담은 플래그 구조체를 초기화 한다.
	InitShowFlag() ;

	if (changeFlag & eCHAPPEAR_MOD)
	{
		// MOD 파트 처리를 한다.
		if( !InitMOD() ) return ;

		// 머리(헤어) 처리를 한다.
		if( !InitHair() ) return ;
	}

	if (changeFlag & eCHAPPEAR_MOD || changeFlag & eCHAPPEAR_PART)
	{
		// 바뀌는 부분들 처리를 한다.
		if( !ChangePart() ) return ;

		// 붙여지는 부분들 처리를 한다.
		if( !AttachPart() ) return ;
	}

	// 얼굴 파트 처리를 한다.
	if (changeFlag & eCHAPPEAR_MOD)
	{
		if( !InitHead() ) return ;
	}

	// 스케일 처리를 한다.
	if (changeFlag & eCHAPPEAR_MOD || changeFlag & eCHAPPEAR_SCALE)
	{
		if( !InitScale() ) return ;
	}

	// 위치 설정 처리를 한다.
	if( !InitPos() ) return ;

	if (changeFlag & eCHAPPEAR_WEAPON)
	{
		// 무기를 표현한다.
		if( !AppearWeapone() ) return ;
	}

	if (changeFlag & eCHAPPEAR_MOD)
	{
		// HERO 캐릭터는 자체 마우스 피킹이 안되도록 한다.
		if(m_pPlayer->GetID() == HEROID)
		{
			m_pEngineObject->DisablePick() ;
		}

		// 엔젠 오브젝트의 자체 라이트 효과 처리를 한다.
		m_pEngineObject->EnableSelfIllumin() ;
	}

	m_pPlayer->ClearChangeParts();
}





//-------------------------------------------------------------------------------------------------
//	NAME		: InitMOD
//	DESC		: 모습을 갱신 할 Player의 MOD 파트 처리를 한다.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 05, 2009
//-------------------------------------------------------------------------------------------------
BYTE CAppearPlayer::InitMOD()
{
	// 종족과 성별을 받는다.
	m_pTotalInfo = m_pPlayer->GetCharacterTotalInfo() ;
	if( !m_pTotalInfo ) goto ERREND ;

	ASSERT( m_pTotalInfo->Gender <= eWOMAN ) ;


	// MOD 정보를 받는다. ( 리소스 매니져에서 mod 파일 리스트를 노출 시킨 것은 좋지 않다고 생각함 )
	MOD_LIST* pModList = &GAMERESRCMNGR->m_ModFileList[ m_pTotalInfo->Race ][ m_pTotalInfo->Gender ] ;
	if( !pModList ) goto ERREND ;


	// 모델 정보 초기화
	m_pEngineObject->Init( pModList->BaseObjectFile, m_pPlayer, eEngineObjectType_Character ) ;
	// 090316 LUJ, 날개 정보를 초기화시켜주지 않으면 해제된 메모리를 사용하고, 클라이언트가 강제 종료된다
	m_pPlayer->SetWingObject( NULL );

	// 결과 리턴.
	return TRUE ;


// 비정상 종료 처리를 한다.
ERREND :
	InitData() ;
	return FALSE ;
}


//-------------------------------------------------------------------------------------------------
//	NAME		: InitHair
//	DESC		: 모습을 갱실 할 Player의 헤어 파트 처리를 한다.
//	PROGRAMMER	: nsoh
//	DATE		: February 20, 2009
//-------------------------------------------------------------------------------------------------
BYTE CAppearPlayer::InitHair() 
{
	if(0 == m_pTotalInfo)
	{
		InitData();
		return FALSE;
	}

	MOD_LIST& modList = GAMERESRCMNGR->m_HairModList[m_pTotalInfo->Race][m_pTotalInfo->Gender];

	if(modList.MaxModFile <= m_pTotalInfo->HairType)
	{
#ifdef _GMTOOL_
		_tcprintf(
			"[오류] 등록되지 않은 머리(%d)를 기본 표시합니다",
			m_pTotalInfo->HairType);
#endif
		m_pTotalInfo->HairType = 0;
	}

	m_pEngineObject->ChangePart(
		eAppearPart_Hair,
		modList.ModFile[m_pTotalInfo->HairType]);
	return TRUE;
}


//-------------------------------------------------------------------------------------------------
//	NAME		: InitHead
//	DESC		: 모습을 갱실 할 Player의 머리 파트 처리를 한다.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 05, 2009
//-------------------------------------------------------------------------------------------------
BYTE CAppearPlayer::InitHead()
{
	if(0 == m_pTotalInfo)
	{
		InitData();
		return FALSE;
	}

	m_pEngineObject->ChangePart(
		eAppearPart_Face,
		"NULL.MOD");
	m_pPlayer->SetFaceObject(
		0);

	if(FALSE == m_stShowFlag.IsShowFace)
	{
		return TRUE;
	}

	MOD_LIST& modList = GAMERESRCMNGR->m_FaceModList[m_pTotalInfo->Race][m_pTotalInfo->Gender];

	CEngineObject* const pFace = new CEngineObject;
	pFace->Init(
		0 == modList.ModFile ? "" : modList.ModFile[0],
		0,
		eEngineObjectType_Weapon);
	m_pEngineObject->AttachDress(
		pFace,
		"Bip01 Head");
	m_pPlayer->SetFaceObject(
		pFace);
	pFace->SetMaterialIndex(
		m_pTotalInfo->FaceType * 2 + 72);
	return TRUE;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: ChangePart
//	DESC		: 모습을 갱신 할 Player의 바뀌는 부분 처리를 한다.
//				: ChangePart를 사용할 경우 Attach된 오브젝트를 모두 해제한뒤
//				: ChangePart를 사용한뒤 다시 Attach를 사용해야 하므로
//				: Attach후 ChangePart를 사용할 경우 문제가 발생할 요지가 있다.
//				: 따라서 ChangePart를 사용해야 하는 장착 아이템을 먼저 처리한뒤
//				: Attach를 사용해야 하는 장착아이템을 처리한다.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 05, 2009
//-------------------------------------------------------------------------------------------------
BYTE CAppearPlayer::ChangePart() 
{
	// 캐릭터 토탈 정보 유효성을 확인한다.
	if( !m_pTotalInfo ) goto ERREND ;


	// MOD 정보를 받는다. ( 리소스 매니져에서 mod 파일 리스트를 노출 시킨 것은 좋지 않다고 생각함 )
	MOD_LIST* pModList = &GAMERESRCMNGR->m_ModFileList[ m_pTotalInfo->Race ][ m_pTotalInfo->Gender ] ;
	if( !pModList ) goto ERREND ;

	ITEM_INFO* pInfo = NULL;
	WORD PartType = 0;
	WORD PartModelNum = 0;
	DWORD* WearedItemIdx = m_pTotalInfo->WearedItemIdx;

	char HideParts[eWearedItem_Max] = {0};
	EncodeHidePartsFlag(
		m_pTotalInfo->HideFlag,
		HideParts);

	// ChangePart를 사용하는 부분	
	for(int n=0 ;n < eWearedItem_Max ; ++n)
	{
		if (/*m_pPlayer->GetChangePart(n) && */WearedItemIdx[n] && !HideParts[n])
		{
			// 아이템 정보를 받는다.
			pInfo = ITEMMGR->GetItemInfo(WearedItemIdx[n]) ;
			if( !pInfo ) continue ;

			// 파트 정보를 받는다.
			PartType		= pInfo->Part3DType ;
			PartModelNum	= pInfo->Part3DModelNum ;

			// 3D 파트 정보가 없으면 아이콘만 표시되는 녀석들이다.
			if( PartType == ePartType_None ) continue ;

			if( PartModelNum == 65535 ) continue ;

			// 파트 타입을 확인한다.
			switch( PartType )
			{
			case ePartType_Hair :
				{
					m_pEngineObject->ChangePart( eAppearPart_Hair, pModList->ModFile[ PartModelNum ] ) ;
				}
				break ;

			case ePartType_Body :
			case ePartType_Costume_Body :
				{
					m_pEngineObject->ChangePart( eAppearPart_Body, pModList->ModFile[ PartModelNum ] ) ;
				}
				break ;

			case ePartType_Hand :
			case ePartType_Costume_Hand :
				{
					m_pEngineObject->ChangePart( eAppearPart_Hand, pModList->ModFile[ PartModelNum ] ) ;
				}
				break ;

			case ePartType_Foot :
			case ePartType_Costume_Foot :
				{
					m_pEngineObject->ChangePart( eAppearPart_Foot, pModList->ModFile[ PartModelNum ] ) ;
				}
				break ;

			case ePartType_Head_Panda :
				{
					m_pEngineObject->ChangePart( eAppearPart_Hair, pModList->ModFile[ PartModelNum ] ) ;

					m_stShowFlag.IsShowGlasses  = FALSE ;
					m_stShowFlag.IsShowHelmet	= FALSE ;
					m_stShowFlag.IsShowMask		= FALSE ;
				}
				break ;

			case ePartType_Head_Chicken :
				{
					m_pEngineObject->ChangePart( eAppearPart_Hair, pModList->ModFile[ PartModelNum ] ) ;
					
					m_stShowFlag.IsShowGlasses  = TRUE ;
					m_stShowFlag.IsShowHelmet	= FALSE ;
					m_stShowFlag.IsShowMask		= TRUE ;
				}
				break ;

			case ePartType_Head_Tights :
				{
					m_pEngineObject->ChangePart( eAppearPart_Hair, pModList->ModFile[ PartModelNum ] ) ;

					m_stShowFlag.IsShowGlasses  = TRUE ;
					m_stShowFlag.IsShowHelmet	= TRUE ;
					m_stShowFlag.IsShowMask		= TRUE ;
				}
				break ;

			case ePartType_Head_Bag :
				{
					m_pEngineObject->ChangePart( eAppearPart_Hair, pModList->ModFile[ PartModelNum ] ) ;

					m_stShowFlag.IsShowGlasses  = FALSE ;
					m_stShowFlag.IsShowHelmet	= FALSE ;
					m_stShowFlag.IsShowMask		= TRUE ;
				}
				break ;

			case ePartType_Head_Mask :
				{
					m_stShowFlag.IsShowGlasses  = FALSE ;
					m_stShowFlag.IsShowHelmet	= TRUE ;
					m_stShowFlag.IsShowMask		= FALSE ;
				}
				break ;

			//090506 pdy 파츠 추가 
			case ePartType_Head_CatEar:
				{
					m_stShowFlag.IsShowFace		= TRUE ;
					m_stShowFlag.IsShowGlasses  = TRUE ;
					m_stShowFlag.IsShowHelmet	= TRUE ;
					m_stShowFlag.IsShowMask		= TRUE ;
				}
				break;

			default : break ;

			}
		}
	}

	// 결과 리턴.
	return TRUE ;

	// 비정상 종료 처리를 한다.
ERREND :
	InitData() ;
	return FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: AttachPart
//	DESC		: 모습을 갱신 할 Player에 붙여야 하는 파트 처리를 한다.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 05, 2009
//-------------------------------------------------------------------------------------------------
BYTE CAppearPlayer::AttachPart() 
{
	// 캐릭터 토탈 정보 유효성을 확인한다.
	if( !m_pTotalInfo ) goto ERREND ;


	// MOD 정보를 받는다. ( 리소스 매니져에서 mod 파일 리스트를 노출 시킨 것은 좋지 않다고 생각함 )
	MOD_LIST* pModList = &GAMERESRCMNGR->m_ModFileList[ m_pTotalInfo->Race ][ m_pTotalInfo->Gender ] ;
	if( !pModList ) goto ERREND ;

	ITEM_INFO* pInfo = NULL;
	WORD PartType = 0;
	WORD PartModelNum = 0;
	DWORD* WearedItemIdx = m_pTotalInfo->WearedItemIdx;

	char HideParts[eWearedItem_Max] = {0};
	EncodeHidePartsFlag(
		m_pTotalInfo->HideFlag,
		HideParts);

	// Attach를 사용하는 부분
	for( int n = 0 ; n < eWearedItem_Max ; ++n )
	{
		if(/*m_pPlayer->IsChangePart(n) &&*/ WearedItemIdx[ n ] && !HideParts[ n ])
		{
			pInfo = ITEMMGR->GetItemInfo(WearedItemIdx[ n ]) ;

			/// 아이템 정보 없다...
			if( !pInfo ) continue ;

			PartType	 = pInfo->Part3DType ;
			PartModelNum = pInfo->Part3DModelNum ;

			/// 3D 파트 정보가 없으면 아이콘만 표시되는 녀석들이다.
			if( PartType == ePartType_None ) continue ;

			if( PartModelNum == 65535 ) continue ;

			switch( PartType )
			{
			case ePartType_OneHand :
			case ePartType_TwoHand :
			case ePartType_TwoBlade :
				{
					// 100310 ShinJS --- 탈것 탑승시 보이지 않도록 함.
					if( m_pPlayer->IsGetOnVehicle() )
						break;

					if( m_pPlayer->GetState() != eObjectState_StreetStall_Owner )
					{
						if( !AppearWeapone() ) goto ERREND ;
					}
				}
				break ;

			case ePartType_Shield :
				{
					// 100310 ShinJS --- 탈것 탑승시 보이지 않도록 함.
					if( m_pPlayer->IsGetOnVehicle() )
						break;

					CEngineObject* pShield = NULL ;
					pShield = new CEngineObject ;
					pShield->Init(pModList->ModFile[PartModelNum],NULL,eEngineObjectType_Weapon) ;
					m_pEngineObject->AttachWeapon(pShield,"Lhand") ;	
				}
				break ;

			case ePartType_Helmet :
				{
					if( m_stShowFlag.IsShowHelmet )
					{
						CEngineObject* pHelmet = NULL ;
						pHelmet = new CEngineObject ;
						pHelmet->Init(pModList->ModFile[PartModelNum],NULL,eEngineObjectType_Weapon) ;
						m_pEngineObject->AttachDress(pHelmet,"Bone_head") ;
					}
				}
				break ;

			case ePartType_Head_Panda :
				{
					m_stShowFlag.IsShowFace = FALSE ;
				}
				break ;

			case ePartType_Mask :
				{
					if( m_stShowFlag.IsShowMask )
					{
						CEngineObject* pMask = NULL ;
						pMask = new CEngineObject ;
						pMask->Init(pModList->ModFile[PartModelNum],NULL,eEngineObjectType_Weapon) ;
						m_pEngineObject->AttachDress(pMask,"Bone_head") ;
					}
				}
				break ;

			case ePartType_Glasses :
				{
					if( m_stShowFlag.IsShowGlasses )
					{
						CEngineObject* pGlasses = NULL ;
						pGlasses = new CEngineObject ;
						pGlasses->Init(pModList->ModFile[PartModelNum],NULL,eEngineObjectType_Weapon) ;
						m_pEngineObject->AttachDress(pGlasses,"Bone_head") ;	
					}
				}
				break ;

			case ePartType_Wing :
			case ePartType_Wing2 :
				{
					CEngineObject* pWing = new CEngineObject ;
					pWing->Init(pModList->ModFile[PartModelNum],NULL,eEngineObjectType_Weapon) ;
					m_pEngineObject->AttachDress(pWing,"Bip01 Spine1") ;

					// 091028 특정 스킬사용시 눈모양이 변하거나, 날개 이동모션이 나오지않는 버그 수정 
					//if( !m_IsForEffect )
					//{
						m_pPlayer->SetWingObject( pWing ) ;
					//}
				}
				break ;
			//100406 pdy 파츠추가
			case ePartType_Pelvis:
				{
					CEngineObject* pTail = new CEngineObject ;
					pTail->Init(pModList->ModFile[PartModelNum],NULL,eEngineObjectType_Weapon) ;
					m_pEngineObject->AttachDress(pTail,"Bip01 Pelvis") ;
				}
				break;

			//090506 pdy 파츠 추가 
			case ePartType_Tail:
				{
					CEngineObject* pTail = new CEngineObject ;
					pTail->Init(pModList->ModFile[PartModelNum],NULL,eEngineObjectType_Weapon) ;
					m_pEngineObject->AttachDress(pTail,"Bip01") ;
				}
				break ;

			//090506 pdy 파츠 추가 
			case ePartType_Head_CatEar :
				{
					CEngineObject* pEar = new CEngineObject ;
					pEar->Init(pModList->ModFile[PartModelNum],NULL,eEngineObjectType_Weapon) ;
					m_pEngineObject->AttachDress(pEar,"Bone_head") ;
				}
				break;

			case ePartType_Head_Mask :
				{
					CEngineObject* pMask = NULL ;
					pMask = new CEngineObject ;
					pMask->Init(pModList->ModFile[PartModelNum],NULL,eEngineObjectType_Weapon) ;
					m_pEngineObject->AttachDress(pMask,"Bone_head") ;	
				}
				break ;

			default : break ;
			}
		}
	}

	if(0 == HideParts[eWearedItem_Costume_Head])
	{
		ITEM_INFO*	pHatInfo			=	NULL;
		ITEM_INFO*  pCustume_Head_Info	=	NULL;
		WORD	CustumeHeadPartsNum		=	NULL;	
		pHatInfo			=	ITEMMGR->GetItemInfo(WearedItemIdx[eWearedItem_Hat]);
		pCustume_Head_Info	=	ITEMMGR->GetItemInfo(WearedItemIdx[eWearedItem_Costume_Head]);
			
		if( pHatInfo != NULL && pCustume_Head_Info != NULL )
		{
			if( pHatInfo->Part3DType == ePartType_Head_Panda && pCustume_Head_Info->Part3DType != ePartType_Head_Panda )
			{
				CustumeHeadPartsNum	=	pCustume_Head_Info->Part3DModelNum;


				m_stShowFlag.IsShowFace		=	TRUE;
				m_stShowFlag.IsShowGlasses	=	TRUE;
				m_stShowFlag.IsShowMask		=	TRUE;
				m_stShowFlag.IsShowHelmet	=	FALSE;
			}
		}
	}

	// 직업 이펙트를 활성화 시킨다.
	m_pPlayer->StartJobEffect() ;

	
	// 결과 리턴.
	return TRUE ;

	// 비정상 종료 처리를 한다.
ERREND :
	InitData() ;
	return FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: InitScale
//	DESC		: 모습을 갱신 할 Player의 Scale 처리를 한다.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 05, 2009
//-------------------------------------------------------------------------------------------------
BYTE CAppearPlayer::InitScale() 
{
	// 캐릭터 토탈 정보 유효성을 확인한다.
	if( !m_pTotalInfo ) goto ERREND ;


	//스케일
	VECTOR3 scale ;	

	if( m_pTotalInfo->Width > 3.0f ) m_pTotalInfo->Width = 3.0f ;
	else if( m_pTotalInfo->Width < 0.3f ) m_pTotalInfo->Width = 0.3f ;

	if( m_pTotalInfo->Height > 3.0f ) m_pTotalInfo->Height = 3.0f ;
	else if( m_pTotalInfo->Height < 0.3f ) m_pTotalInfo->Height = 0.3f ;

	SetVector3( &scale, m_pTotalInfo->Width, m_pTotalInfo->Height, m_pTotalInfo->Width) ;

	m_pEngineObject->SetScale(&scale) ;


	// 결과 리턴.
	return TRUE ;

	// 비정상 종료 처리를 한다.
ERREND :
	InitData() ;
	return FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: InitPos
//	DESC		: 모습을 갱신 할 Player의 위치 처리를 한다.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 05, 2009
//-------------------------------------------------------------------------------------------------
BYTE CAppearPlayer::InitPos() 
{
	// 캐릭터 토탈 정보 유효성을 확인한다.
	if( !m_pTotalInfo ) goto ERREND ;


	m_pEngineObject->ApplyHeightField( TRUE ) ;

	MOVE_INFO* moveInfo = m_pPlayer->GetBaseMoveInfo() ;
	if( !moveInfo ) goto ERREND ;

	ROTATEINFO* rotateInfo = m_pPlayer->GetBaseRotateInfo() ;
	if( !rotateInfo ) goto ERREND ;


	m_pPlayer->SetPosition( &moveInfo->CurPosition ) ;

	if(HOUSINGMGR->IsHousingMap())
		m_pPlayer->SetAngle( HOUSINGMGR->GetStartAngle() ) ;
	else
		m_pPlayer->SetAngle( rotateInfo->Angle.ToRad() ) ;

	// 결과 리턴.
	return TRUE ;

	// 비정상 종료 처리를 한다.
ERREND :
	InitData() ;
	return FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: AppearWeapone
//	DESC		: 모습을 갱신 할 Player의 무기 표현 처리를 한다.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 05, 2009
//-------------------------------------------------------------------------------------------------
BYTE CAppearPlayer::AppearWeapone() 
{
	// 100310 ShinJS --- 탈것 탑승시 보이지 않도록 함.
	if( m_pPlayer->IsGetOnVehicle() )
		return TRUE;

	// 캐릭터 토탈 정보 유효성을 확인한다.
	if( !m_pTotalInfo ) goto ERREND ;

	MOD_LIST& pModList = GAMERESRCMNGR->m_ModFileList[ m_pTotalInfo->Race ][ m_pTotalInfo->Gender ] ;
	
	APPEARANCEMGR->HideWeapon(m_pPlayer,m_pEngineObject) ;

	if(const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo(m_pPlayer->GetWearedItemIdx(eWearedItem_Weapon)))
	{
		switch(itemInfo->Part3DType)
		{
		case ePartType_TwoBlade:
			{
				if(eWeaponType_BothDagger == itemInfo->WeaponType)
				{
					CEngineObject* pRightWeapon = new CEngineObject ;
					pRightWeapon->Init(
						pModList.ModFile[itemInfo->Part3DModelNum],
						NULL,
						eEngineObjectType_Weapon);
					m_pEngineObject->AttachWeapon(
						pRightWeapon,
						RIGHT_HAND);

					// 마족의 왼손무기를 modlist파일에서 추출하여 attach시킨다.
					if(const char* pLModFile = GAMERESRCMNGR->GetDualWeaponLObject(itemInfo->Part3DModelNum, m_pTotalInfo->Race, m_pTotalInfo->Gender))
					{
						CEngineObject* pLeftWeapon = new CEngineObject ;
						pLeftWeapon->Init(
							LPTSTR(pLModFile),
							NULL,
							eEngineObjectType_Weapon) ;
						m_pEngineObject->AttachWeapon(
							pLeftWeapon,
							LEFT_HAND) ;
					}
				}

				break;
			}
		default:
			{
				CEngineObject* pWeapon = new CEngineObject ;
				pWeapon->Init(
					pModList.ModFile[itemInfo->Part3DModelNum],
					NULL,
					eEngineObjectType_Weapon);

				m_pEngineObject->AttachWeapon(
					pWeapon,
					eWeaponType_Bow == itemInfo->WeaponType ? LEFT_HAND : RIGHT_HAND);

				break;
			}
		}
	}

	if(const ITEM_INFO* const itemInfo = ITEMMGR->GetItemInfo(m_pPlayer->GetWearedItemIdx(eWearedItem_Shield)))
	{
		CEngineObject* pShield	= new CEngineObject ;

		pShield->Init(
			pModList.ModFile[itemInfo->Part3DModelNum],
			NULL,
			eEngineObjectType_Weapon);
		m_pEngineObject->AttachWeapon(
			pShield,
			LEFT_HAND);
	}

	if(m_pTotalInfo->WeaponEnchant >= 3)
	{
		BYTE level = m_pTotalInfo->WeaponEnchant - 3;

		if(EnchantEffect* pEffect = ITEMMGR->GetItemEffect(m_pTotalInfo->WearedItemIdx[eWearedItem_Weapon]))
		{
			for( int i = 0; i < 3; i++ )
			{
				if( pEffect->Effect[ level ][ i ] == 0 ) continue ;

				TARGETSET set ;
				set.pTarget = m_pPlayer ;
				m_pPlayer->mWeaponEffect[ i ] = EFFECTMGR->StartEffectProcess( pEffect->Effect[ level ][ i ], m_pPlayer, &set, 0, m_pPlayer->GetID() ) ;
			}
		}
	}

	// 결과 리턴.
	return TRUE ;

	// 비정상 종료 처리를 한다.
ERREND :
	InitData() ;
	return FALSE ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: InitData
//	DESC		: 모습 갱신 처리에 필요로 한 정보들을 초기화 하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 05, 2009
//-------------------------------------------------------------------------------------------------
void CAppearPlayer::InitData()
{
	InitShowFlag() ;

	m_IsForEffect		= FALSE ;

	m_pPlayer			= NULL ;
	m_pEngineObject		= NULL ;

	m_pTotalInfo		= NULL ;
}





//-------------------------------------------------------------------------------------------------
//	NAME		: InitShowFlag
//	DESC		: 모습을 보여주는 초기화 플래그를 초기화 하는 함수.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 05, 2009
//-------------------------------------------------------------------------------------------------
void CAppearPlayer::InitShowFlag()
{
	memset( &m_stShowFlag, TRUE, sizeof(m_stShowFlag) ) ;
}
