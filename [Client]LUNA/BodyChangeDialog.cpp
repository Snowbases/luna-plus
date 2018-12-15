#include "stdafx.h"
#include "BodyChangeDialog.h"
#include "WindowIDEnum.h"
#include "ObjectManager.h"
#include "ItemManager.h"
#include "Item.h"
#include "interface/cStatic.h"
#include "cResourceManager.h"
#include "ExitManager.h"
#include "input/UserInput.h"
#include "MoveManager.h"
#include "ObjectStateManager.h"
#include "cMsgBox.h"
#include "ChatManager.h"
#include "cWindowManager.h"


CBodyChangeDialog::CBodyChangeDialog() :
mStopTime( 0 ),
mAutoRotataionDelayTime( 5000 )
{
	ZeroMemory( &mCharacterChangeInfo,		sizeof( mCharacterChangeInfo ) );
	ZeroMemory( &mUsedItem,					sizeof( mUsedItem ) );
	ZeroMemory( &mStoredCameraDescription,	sizeof( mStoredCameraDescription ) );

	// 080414 LUJ, 메뉴 문자열 리스트를 설정한다. 설정 이유는 헤더 파일 참조
	{
		CMHFile file;

		if( ! file.Init( "system/resource/CharMake_List.bin", "rb" ) )
		{
			return;
		}

		char line[ MAX_PATH * 5 ] = { 0 };

		while( ! file.IsEOF() )
		{
			file.GetLine( line, sizeof( line ) );

			MenuText menuText;
			menuText.mRace		= RaceType_Max;
			menuText.mPart		= ePartType_None;
			menuText.mGender	= UINT_MAX;
			menuText.mValue		= UINT_MAX;
			
			const char* separator = " \t";

			// 080414 LUJ, 첫번째는 개수
			const char* token = strtok( line, separator );

			// 080414 LUJ, 두번째는 타입
			token = strtok( 0, separator );

			if( ! token )
			{
				continue;
			}
			else if( ! stricmp( "CM_ST_HMFACE", token ) )
			{
				menuText.mRace		= RaceType_Human;
				menuText.mPart		= ePartType_Face;
				menuText.mGender	= GENDER_MALE;
			}
			else if( ! stricmp( "CM_ST_HMHAIR", token ) )
			{
				menuText.mRace		= RaceType_Human;
				menuText.mPart		= ePartType_Hair;
				menuText.mGender	= GENDER_MALE;
			}
			else if( ! stricmp( "CM_ST_HWMFACE", token ) )
			{
				menuText.mRace		= RaceType_Human;
				menuText.mPart		= ePartType_Face;
				menuText.mGender	= GENDER_FEMALE;
			}
			else if( ! stricmp( "CM_ST_HWMHAIR", token ) )
			{
				menuText.mRace		= RaceType_Human;
				menuText.mPart		= ePartType_Hair;
				menuText.mGender	= GENDER_FEMALE;
			}
			else if( ! stricmp( "CM_ST_EMFACE", token ) )
			{
				menuText.mRace		= RaceType_Elf;
				menuText.mPart		= ePartType_Face;
				menuText.mGender	= GENDER_MALE;
			}
			else if( ! stricmp( "CM_ST_EMHAIR", token ) )
			{
				menuText.mRace		= RaceType_Elf;
				menuText.mPart		= ePartType_Hair;
				menuText.mGender	= GENDER_MALE;
			}
			else if( ! stricmp( "CM_ST_EWMFACE", token ) )
			{
				menuText.mRace		= RaceType_Elf;
				menuText.mPart		= ePartType_Face;
				menuText.mGender	= GENDER_FEMALE;
			}
			else if( ! stricmp( "CM_ST_EWMHAIR", token ) )
			{
				menuText.mRace		= RaceType_Elf;
				menuText.mPart		= ePartType_Hair;
				menuText.mGender	= GENDER_FEMALE;
			}
			// 090504 ONS 신규종족관련 처리 추가
			else if( ! stricmp( "CM_ST_DMFACE", token ) )
			{
				menuText.mRace		= RaceType_Devil;
				menuText.mPart		= ePartType_Face;
				menuText.mGender	= GENDER_MALE;
			}
			else if( ! stricmp( "CM_ST_DMHAIR", token ) )
			{
				menuText.mRace		= RaceType_Devil;
				menuText.mPart		= ePartType_Hair;
				menuText.mGender	= GENDER_MALE;
			}
			else if( ! stricmp( "CM_ST_DWMFACE", token ) )
			{
				menuText.mRace		= RaceType_Devil;
				menuText.mPart		= ePartType_Face;
				menuText.mGender	= GENDER_FEMALE;
			}
			else if( ! stricmp( "CM_ST_DWMHAIR", token ) )
			{
				menuText.mRace		= RaceType_Devil;
				menuText.mPart		= ePartType_Hair;
				menuText.mGender	= GENDER_FEMALE;
			}

			// 080414 LUJ, 지정되지 않은 예약어
			else
			{
				continue;
			}

			for(
				token = strtok( 0, separator );
				token;
				token = strtok( 0, separator ) )
			{

				menuText.mText	= token;

				const char* value1 = strtok( 0, separator );

				// 080414 LUJ, 현 시점에서는 의미없는 값
				const char* value2 = strtok( 0, separator );

				if( ! value1 || ! value2 )
				{
					break;
				}

				menuText.mValue	= atoi( value1 );

				mMenuTextList.push_back( menuText );
			}
		}
	}
}

CBodyChangeDialog::~CBodyChangeDialog()
{}


void CBodyChangeDialog::Linking()
{}


void CBodyChangeDialog::OnActionEvent( LONG id, void* p, DWORD we )
{
	CHero* hero = OBJECTMGR->GetHero();

	if( ! hero )
	{
		return;
	}

	// 080414 LUJ, 이벤트가 있으면 자동 회전 시간을 지연시킨다
	mStopTime = gCurTime + mAutoRotataionDelayTime;

	CHARACTERCHANGE_INFO info( mCharacterChangeInfo );

	switch( id )
	{
	case BODY_CHANGE_HAIR_LEFT:
		{
			mHairMenu.Decrease();
			info.HairType	= mHairMenu.GetValue();
			info.FaceType	= mFaceMenu.GetValue();

			hero->SetCharChangeInfo( &info );

			{
				cStatic* textStatic = ( cStatic* )GetWindowForID( BODY_CHANGE_HAIR_TEXT );

				if( textStatic )
				{
					textStatic->SetStaticText( mHairMenu.GetText() );
				}
			}

			break;
		}
	case BODY_CHANGE_HAIR_RIGHT:
		{
			mHairMenu.Increase();
			info.HairType	= mHairMenu.GetValue();
			info.FaceType	= mFaceMenu.GetValue();

			hero->SetCharChangeInfo( &info );

			{
				cStatic* textStatic = ( cStatic* )GetWindowForID( BODY_CHANGE_HAIR_TEXT );

				if( textStatic )
				{
					textStatic->SetStaticText( mHairMenu.GetText() );
				}
			}

			break;
		}
	case BODY_CHANGE_FACE_LEFT:
		{
			mFaceMenu.Decrease();
			info.HairType	= mHairMenu.GetValue();
			info.FaceType	= mFaceMenu.GetValue();

			hero->SetCharChangeInfo( &info );

			{
				cStatic* textStatic = ( cStatic* )GetWindowForID( BODY_CHANGE_FACE_TEXT );

				if( textStatic )
				{
					textStatic->SetStaticText( mFaceMenu.GetText() );
				}
			}

			break;
		}
	case BODY_CHANGE_FACE_RIGHT:
		{
			mFaceMenu.Increase();
			info.HairType	= mHairMenu.GetValue();
			info.FaceType	= mFaceMenu.GetValue();

			hero->SetCharChangeInfo( &info );

			{
				cStatic* textStatic = ( cStatic* )GetWindowForID( BODY_CHANGE_FACE_TEXT );

				if( textStatic )
				{
					textStatic->SetStaticText( mFaceMenu.GetText() );
				}
			}

			break;
		}
	case BODY_CHANGE_SUBMIT:
		{
			const ITEM_INFO* info = ITEMMGR->GetItemInfo( mUsedItem	.wIconIdx );

			if( ! info )
			{
				break;
			}

			char text[ MAX_PATH ] = { 0 };

			sprintf(
				text,
				CHATMGR->GetChatMsg( 1244 ),
				info->ItemName );

			WINDOWMGR->MsgBox( MBI_USEBODYCHANGEITEM, MBT_YESNO, text );

			// 080930 LUJ, 메시지 박스 선택 전까지 창을 잠근다
			SetDisable( TRUE );
			break;
		}			
	case MBI_USEBODYCHANGEITEM:
		{
			// 080930 LUJ, 선택 후에는 창 잠금을 해제한다
			SetDisable( FALSE );

			if( MBI_NO == we )
			{
				break;
			}

			info.HairType	= mHairMenu.GetValue();
			info.FaceType	= mFaceMenu.GetValue();

			// 080414 LUJ, 현재 값과 다르지 않다면 처리할 이유 없음
			if( ! memcmp( &info, &mCharacterChangeInfo, sizeof( info ) ) )
			{
				return;
			}

			// 080414 LUJ, 서버에 메시지 전달
			{
				MSG_CHARACTER_CHANGE message;
				ZeroMemory( &message, sizeof( message ) );

				message.dwObjectID	= hero->GetID();
				message.Category	= MP_ITEM;
				message.Protocol	= MP_ITEM_SHOPITEM_CHARCHANGE_SYN;
				message.mItem		= mUsedItem;
				message.mInfo		= info;

				NETWORK->Send( &message, sizeof( message ) );
			}

			SetDisable( TRUE );
			EXITMGR->SendExitMsg( eEK_CHARSEL );

			// 080414 LUJ, 자동 접속 종료되는 동안 회전하도록 한다
			mStopTime = gCurTime;
			break;
		}
	case CMI_CLOSEBTN:
		{
			SetActive( FALSE );
			break;
		}
	}

	// 080414 LUJ, 현재값과 같으면 확인 버튼 비활성화
	// 080930 LUJ, 메시지 박스 선택 때 처리하면 버튼이 사라져버린다.
	if( MBI_USEBODYCHANGEITEM != id )
	{
		cWindow* window = GetWindowForID( BODY_CHANGE_SUBMIT );

		if( window )
		{
			// 080930 LUJ, 선택된 메뉴 항목이 유효한지 반환하는 내부 클래스
			class 
			{
			public:
				BOOL operator()( const CMenu& faceMenu, const CMenu& hairMenu, const ITEMBASE& usedItem ) const
				{
					const CItemManager::BodyChangeScriptListMap&				scriptListMap	= ITEMMGR->GetBodyChangeScriptListMap();
					const CItemManager::BodyChangeScriptListMap::const_iterator	map_it			= scriptListMap.find( usedItem.wIconIdx );
	
					if( scriptListMap.end() == map_it )
					{
						return FALSE;
					}
	
					BOOL isValidHair	= FALSE;
					BOOL isValidFace	= FALSE;
	
					const CItemManager::BodyChangeScriptList& scriptList = map_it->second;
	
					for(	CItemManager::BodyChangeScriptList::const_iterator list_it = scriptList.begin();
							scriptList.end() != list_it;
							++list_it )
					{
						const CItemManager::BodyChangeScript& script = *list_it;
	
						if( script.mPart	== ePartType_Hair	&&
							script.mValue	== hairMenu.GetValue() )
						{
							isValidHair = TRUE;
						}
						else if(	script.mPart	== ePartType_Face	&&
									script.mValue	== faceMenu.GetValue() )
						{
							isValidFace = TRUE;
						}
					}
	
					return isValidFace && isValidHair;
				}
			}
			IsValid;
	
			const BOOL	isValid		= IsValid( mFaceMenu, mHairMenu, mUsedItem );
			const BOOL	isDifferent	= memcmp( &info, &mCharacterChangeInfo, sizeof( info ) );
			
			window->SetActive( isValid && isDifferent );
		}
	}

	cDialog::OnActionEvent( id, p, we );
}


void CBodyChangeDialog::SetActive( BOOL isActive, CItem& item )
{
	CHero* hero = OBJECTMGR->GetHero();

	if( ! hero )
	{
		return;
	}

	typedef CItemManager::BodyChangeScriptListMap BodyChangeScriptListMap;

	const BodyChangeScriptListMap&			scriptListMap	= ITEMMGR->GetBodyChangeScriptListMap();
	BodyChangeScriptListMap::const_iterator	map_it			= scriptListMap.find( item.GetItemIdx() );

	// 080414 LUJ, 아이템에 해당하는 외양 변경 스크립트가 있는지 조사
	if( scriptListMap.end() == map_it )
	{
		return;
	}

	// 080728 LUJ, 전 미용실 아이템의 항목이 남아있으므로 초기화해야한다.
	mFaceMenu.Clear();
	mHairMenu.Clear();

	// 080414 LUJ, 리스트를 바탕으로 플레이어에 해당하는 내용을 구성한다
	{
		typedef CItemManager::BodyChangeScriptList	BodyChangeScriptList;
		typedef CItemManager::BodyChangeScript		BodyChangeScript;

		const BodyChangeScriptList& scriptList = map_it->second;

		for(
			BodyChangeScriptList::const_iterator list_it = scriptList.begin();
			scriptList.end() != list_it;
			++list_it )
		{
			const BodyChangeScript& script = *list_it;

			if( script.mRace	!= hero->GetRace() ||
				script.mGender	!= hero->GetGender() )
			{
				continue;
			}

			const char* text = GetText(
				script.mRace,
				script.mPart,
				script.mGender,
				script.mValue );

			// 080414 LUJ, 문자열이 없으면 리소스 매니저에서 읽어온다
			// 080428 LUJ, 빈 문자열를 잘못 체크하는 문제 수정
			if( ! text || ! *text )
			{
				text = RESRCMGR->GetMsg( script.mText );
			}

			switch( script.mPart )
			{
			case ePartType_Face:
				{
					mFaceMenu.Add(
						BYTE( script.mValue ),
						text );
					break;
				}
			case ePartType_Hair:
				{
					mHairMenu.Add(
						BYTE( script.mValue ),
						text );
					break;
				}
			}
		}
	}

	if( ! mHairMenu.GetSize() &&
		! mFaceMenu.GetSize() )
	{
		return;
	}
    
	// 080414 LUJ, 이전 정보를 저장하고, 기본값을 현재 설정으로 해놓는다
	{
		const CHARACTER_TOTALINFO* info = hero->GetCharacterTotalInfo();

		if( ! info )
		{
			return;
		}

		ZeroMemory( &mCharacterChangeInfo, sizeof( mCharacterChangeInfo ) );

		mCharacterChangeInfo.FaceType	= info->FaceType;
		mCharacterChangeInfo.HairType	= info->HairType;
		mCharacterChangeInfo.Gender		= info->Gender;
		mCharacterChangeInfo.Height		= info->Height;
		mCharacterChangeInfo.Width		= info->Width;

		mHairMenu.Add( info->HairType, "..." );
		mFaceMenu.Add( info->FaceType, "..." );

		mHairMenu.SetDefault( info->HairType );
		mFaceMenu.SetDefault( info->FaceType );
	}

	// 080414 LUJ, 아이템 정보 저장 및 비활성화
	{
		item.SetLock( TRUE );

		mUsedItem = item.GetItemBaseInfo();
	}

	// 080414 LUJ, 초기 값을 설정하기 위해 이벤트를 임의로 발생시킨다. 왼쪽으로 갔다 다시 오른쪽으로 오면 원래 초기값
	{
		OnActionEvent( BODY_CHANGE_HAIR_LEFT,	0, 0 );
		OnActionEvent( BODY_CHANGE_HAIR_RIGHT,	0, 0 );

		OnActionEvent( BODY_CHANGE_FACE_LEFT,	0, 0 );
		OnActionEvent( BODY_CHANGE_FACE_RIGHT,	0, 0 );

		// 080414 LUJ, 이벤트가 있을 때마다 자동 회전을 지연시킨다. 최초에는 자동 회전되도록 하자
		mStopTime = gCurTime;
	}	

	// 080414 LUJ, 플레이어를 움직일 수 없도록 한다
	{
		hero->DisableAutoAttack();
		hero->SetNextAction( 0 );

		MOVEMGR->HeroMoveStop();
		OBJECTSTATEMGR->StartObjectState( hero, eObjectState_Exit );
	}

	// 080414 LUJ, 창의 위치를 자동 배치한다
	{
		const DISPLAY_INFO& screen	= GAMERESRCMNGR->m_GameDesc.dispInfo;
		const LONG			space	= 100;
		const LONG			x		= screen.dwWidth - GetWidth() - space;
		const LONG			y		= ( screen.dwHeight - GetHeight() ) / 2;

		SetAbsXY( x, y );
	}

	// 080414 LUJ, 카메라 정보도 저장해두고 창이 닫힐 때 복구한다
	{
		const MHCAMERADESC* description = CAMERA->GetCameraDesc();

		if( description )
		{
			mStoredCameraDescription = *description;

			const VECTOR3& vector1 = description->m_CameraPos;

			VECTOR3 vector2 = { 0 };
			hero->GetPosition( &vector2 );

			// 080414 LUJ, 3차원 공간 상의 두 점 간의 거리를 구한다
			const double distance =
				sqrt( pow( vector1.x, 2.0f ) + pow( vector1.y, 2.0f ) + pow( vector1.z, 2.0f ) ) -
				sqrt( pow( vector2.x, 2.0f ) + pow( vector2.y, 2.0f ) + pow( vector2.z, 2.0f ) );

			// 080414 LUJ, 카메라를 최대한 당긴다
			CAMERA->ZoomDistance( 0, -1.0f * abs( float( distance ) ), 1000 );
		}
	}

	// 080414 LUJ, 종족/성별 표시
	{
		std::string race;
		std::string gender;

		switch( hero->GetRace() )
		{
		case RaceType_Human:
			{
				race = RESRCMGR->GetMsg( 247 );
				break;
			}
		case RaceType_Elf:
			{
				race = RESRCMGR->GetMsg( 248 );
				break;
			}
		// 090504 ONS 신규종족관련 처리 추가
		case RaceType_Devil:
			{
				race = RESRCMGR->GetMsg( 1129 );
				break;
			}
		}

		switch( hero->GetGender() )
		{
		case GENDER_FEMALE:
			{
				gender = RESRCMGR->GetMsg( 30 );
				break;
			}
		case GENDER_MALE:
			{
				gender = RESRCMGR->GetMsg( 29 );
				break;
			}
		}

		cStatic* titleStatic = ( cStatic* )GetWindowForID( BODY_CHANGE_TITLE_TEXT );

		if( titleStatic )
		{
			titleStatic->SetStaticText( ( race + " " + gender ).c_str() );
		}
	}

	SetActive( TRUE );
}


void CBodyChangeDialog::SetActive( BOOL isActive )
{
	CHero* hero = OBJECTMGR->GetHero();

	if( ! hero )
	{
		return;
	}
	else if( ! isActive )
	{
		hero->SetCharChangeInfo( &mCharacterChangeInfo );

		// 080414 LUJ, 아이템 잠금 해제
		{
			CItem* item = ITEMMGR->GetItem( mUsedItem.dwDBIdx );

			if( item )
			{
				item->SetLock( FALSE );
			}
		}

		// 080414 LUJ, 플레이어를 움직일 수 있도록 한다
		OBJECTSTATEMGR->StartObjectState( hero, eObjectState_None );
		
		// 080414 LUJ, 저장된 카메라와 현재 카메라의 간격을 알아내서 원위치시킨다
		{
			const MHCAMERADESC* description = CAMERA->GetCameraDesc();

			if( description )
			{
				VECTOR3 vector1 = { 0 };
				hero->GetPosition( &vector1 );

				const VECTOR3& vector2 = mStoredCameraDescription.m_CameraPos;

				// 080414 LUJ, 3차원 공간 상의 두 점 간의 거리를 구한다
				const double distance =
					sqrt( pow( vector1.x, 2.0f ) + pow( vector1.y, 2.0f ) + pow( vector1.z, 2.0f ) ) -
					sqrt( pow( vector2.x, 2.0f ) + pow( vector2.y, 2.0f ) + pow( vector2.z, 2.0f ) );

				// 080414 LUJ, 카메라를 원 위치시킨다
				CAMERA->ZoomDistance( 0, abs( float( distance ) ), 1000 );
			}
		}

		ITEMMGR->SetDisableDialog( FALSE, eItemTable_Inventory );
	}

	cDialog::SetActive( isActive );
}


void CBodyChangeDialog::Render()
{
	if( ! IsActive() )
	{
		return;
	}

	// 080414 LUJ, 오른쪽 마우스 버튼이 눌려있으면 일정 시간 움직이지 않도록 한다
	if( g_UserInput.GetMouse()->RButtonPressed() )
	{
		mStopTime = gCurTime + mAutoRotataionDelayTime;
	}
	else if( gCurTime > mStopTime  )
	{
		CAMERA->MouseRotate( -1, 0 );
	}

	cDialog::Render();
}


const char* CBodyChangeDialog::GetText( RaceType race, PART_TYPE part, DWORD gender, DWORD value ) const
{
	for(
		MenuTextList::const_iterator it = mMenuTextList.begin();
		mMenuTextList.end() != it;
		++it )
	{
		const MenuText& menuText = *it;

		if( menuText.mGender	!= gender	||
			menuText.mRace		!= race		||
			menuText.mPart		!= part		||
			menuText.mValue		!= value )
		{
			continue;
		}

		return menuText.mText.c_str();
	}

	return "";
}