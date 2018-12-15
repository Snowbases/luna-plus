#include "StdAfx.h"
#include "CameraConfiguration.h"
#include "Input/UserInput.h"
#include "../4DyuchiGRX_common/IGeometry.h"
#include "Engine/GraphicEngine.h"

CCameraConfiguration::CCameraConfiguration( Type type )
{
	SetType( type );
	Load( TRUE );
}

CCameraConfiguration::~CCameraConfiguration()
{}

void CCameraConfiguration::SetType( Type type )
{
	mConfiguration.mType = type;
	// 090529 LUJ, 타입이 변경될 경우, 설정 파일을 다시 읽어오기 위해 초기화한다
	ZeroMemory( &mFileStatus, sizeof( mFileStatus ) );
}

BOOL CCameraConfiguration::Process()
{	
	if( Adjust() ||
		Load( TRUE ) )
	{
		UpdateCamera();
		return TRUE;
	}

	return FALSE;
}

BOOL CCameraConfiguration::Adjust()
{
	if( g_UserInput.GetKeyboard()->GetKeyPressed( KEY_SCROLL ) )
	{
		mConfiguration.mIsAdjustable = ! mConfiguration.mIsAdjustable;
	}

	static float plusVariation;
	static float percentVariation = 1;
	VECTOR3 position( mConfiguration.mCameraPosition );
	VECTOR3 rotation( mConfiguration.mCameraRotation );

	if( FALSE == mConfiguration.mIsAdjustable )
	{
		return FALSE;
	}
	else if( g_UserInput.GetKeyboard()->GetKeyPressed( KEY_F ) )
	{
		Load( FALSE );
		return TRUE;
	}
	else if( g_UserInput.GetKeyboard()->GetKeyPressed( KEY_SHIFT ) )
	{
		plusVariation += 10.0f;
		percentVariation += 0.1f;
#ifdef _GMTOOL_
		cprintf(
			"variation adjusted. plus: %0.1f, percent: %0.1f",
			plusVariation,
			percentVariation );
#endif
	}
	else if( g_UserInput.GetKeyboard()->GetKeyPressed( KEY_RSHIFT ) )
	{
		plusVariation = max( 0, plusVariation - 10.0f );
		percentVariation = max( 0, percentVariation - 0.5f );
#ifdef _GMTOOL_
		cprintf(
			"variation adjusted. plus: %0.1f, percent: %0.1f",
			plusVariation,
			percentVariation );
#endif
	}
	else if( g_UserInput.GetKeyboard()->GetKeyPressed( KEY_W ) )
	{
		position.y += 10.0f + plusVariation;
	}
	else if( g_UserInput.GetKeyboard()->GetKeyPressed( KEY_S ) )
	{
		position.y -= 10.0f + plusVariation;
	}
	if( g_UserInput.GetKeyboard()->GetKeyPressed( KEY_Q ) )
	{
		position.z += 10.0f + plusVariation;
	}
	else if( g_UserInput.GetKeyboard()->GetKeyPressed( KEY_E ) )
	{
		position.z -= 10.0f + plusVariation;
	}
	if( g_UserInput.GetKeyboard()->GetKeyPressed( KEY_A ) )
	{
		position.x -= 10.0f + plusVariation;
	}
	else if( g_UserInput.GetKeyboard()->GetKeyPressed( KEY_D ) )
	{
		position.x += 10.0f + plusVariation;
	}
	else if( g_UserInput.GetKeyboard()->GetKeyPressed( KEY_J ) )
	{
		rotation.y -= 0.05f * percentVariation;
	}
	else if( g_UserInput.GetKeyboard()->GetKeyPressed( KEY_L ) )
	{
		rotation.y += 0.05f * percentVariation;
	}
	else if( g_UserInput.GetKeyboard()->GetKeyPressed( KEY_I ) )
	{
		rotation.x -= 0.05f * percentVariation;
	}
	else if( g_UserInput.GetKeyboard()->GetKeyPressed( KEY_K ) )
	{
		rotation.x += 0.05f * percentVariation;
	}
	else if( g_UserInput.GetKeyboard()->GetKeyPressed( KEY_U ) )
	{
		rotation.z -= 0.05f * percentVariation;
	}
	else if( g_UserInput.GetKeyboard()->GetKeyPressed( KEY_O ) )
	{
		rotation.z += 0.05f * percentVariation;
	}

	// 090529 LUJ, 값이 바뀌었는지 확인한다
	if( position == mConfiguration.mCameraPosition &&
		rotation == mConfiguration.mCameraRotation )
	{
		return FALSE;
	}

	mConfiguration.mCameraPosition = position;
	mConfiguration.mCameraRotation = rotation;

	I4DyuchiGXGeometry* const geometry = g_pExecutive->GetGeometry();

	if( geometry )
	{
		geometry->SetCameraPos( &position, 0 );
		geometry->SetCameraAngleRad( &rotation, 0 );
	}

#ifdef _GMTOOL_
	cprintf(
		"camera adjusted. position( %0.1f, %0.1f, %0.1f ), rotation( %0.1f, %0.1f, %0.1f )\n",
		position.x,
		position.y,
		position.z,
		rotation.x,
		rotation.y,
		rotation.z );
#endif
	return TRUE;
}

BOOL CCameraConfiguration::Load( BOOL isUpdateRecently )
{
	struct stat fileStatus = { 0 };

	if( stat( "system/camera.cfg", &fileStatus ) )
	{
		return FALSE;
	}
	else if( isUpdateRecently )
	{
		if( 0 == memcmp( &mFileStatus, &fileStatus, sizeof( fileStatus ) ) )
		{
			return FALSE;
		}
	}

	mFileStatus = fileStatus;
	FILE* const file = fopen( "system/camera.cfg", "rb" );

	if( 0 == file )
	{
		return FALSE;
	}

	int openCount = 0;
	enum Type type = TypeNone;

	while( 0 == feof( file ) )
	{
		char text[ MAX_PATH ] = { 0 };
		fgets( text, sizeof( text ) / sizeof( *text ), file );
		const char* const seperator = " \t,=\n\r";

		for(
			const char* keyword = strtok( text, seperator );
			0 < keyword;
			keyword = strtok( 0, seperator ) )
		{
			const char* const commentSymbol = "//";

			if( 0 == strnicmp( commentSymbol, keyword, strlen( commentSymbol ) ) )
			{
				continue;
			}
			else if( 0 == strcmpi( "{", keyword ) )
			{
				++openCount;
			}
			else if( 0 == strcmpi( "}", keyword ) )
			{
				if( 0 == --openCount )
				{
					type = TypeNone;
				}
			}
			else if( 0 == strcmpi( "login", keyword ) )
			{
				type = TypeLogin;
			}
			else if( 0 == strcmpi( "select", keyword ) )
			{
				type = TypeSelectPlayer;
			}
			else if( 0 == strcmpi( "make", keyword ) )
			{
				type = TypeMakePlayer;
			}

			// 090529 LUJ, 현재 처리 중인 타입만 값을 읽는다
			if( mConfiguration.mType != type )
			{
				continue;
			}
			else if( 0 == strcmpi( "cameraPosition", keyword ) )
			{
				const char* const x = strtok( 0, seperator );
				const char* const y = strtok( 0, seperator );
				const char* const z = strtok( 0, seperator );
				VECTOR3& position = mConfiguration.mCameraPosition;
				position.x = float( atof( x ? x : "" ) );
				position.y = float( atof( y ? y : "" ) );
				position.z = float( atof( z ? z : "" ) );
			}
			else if( 0 == strcmpi( "cameraRotation", keyword ) )
			{
				const char* const x = strtok( 0, seperator );
				const char* const y = strtok( 0, seperator );
				const char* const z = strtok( 0, seperator );
				VECTOR3& rotation = mConfiguration.mCameraRotation;
				rotation.x = float( atof( x ? x : "" ) );
				rotation.y = float( atof( y ? y : "" ) );
				rotation.z = float( atof( z ? z : "" ) );
			}
			else if( 0 == strcmpi( "player1", keyword ) )
			{
				const char* const x = strtok( 0, seperator );
				const char* const y = strtok( 0, seperator );
				const char* const z = strtok( 0, seperator );
				VECTOR3& position = mConfiguration.mPlayerPosition1;
				position.x = float( atof( x ? x : "" ) );
				position.y = float( atof( y ? y : "" ) );
				position.z = float( atof( z ? z : "" ) );
			}
			else if( 0 == strcmpi( "player2", keyword ) )
			{
				const char* const x = strtok( 0, seperator );
				const char* const y = strtok( 0, seperator );
				const char* const z = strtok( 0, seperator );
				VECTOR3& position = mConfiguration.mPlayerPosition2;
				position.x = float( atof( x ? x : "" ) );
				position.y = float( atof( y ? y : "" ) );
				position.z = float( atof( z ? z : "" ) );
			}
			else if( 0 == strcmpi( "player3", keyword ) )
			{
				const char* const x = strtok( 0, seperator );
				const char* const y = strtok( 0, seperator );
				const char* const z = strtok( 0, seperator );
				VECTOR3& position = mConfiguration.mPlayerPosition3;
				position.x = float( atof( x ? x : "" ) );
				position.y = float( atof( y ? y : "" ) );
				position.z = float( atof( z ? z : "" ) );
			}
			else if( 0 == strcmpi( "player4", keyword ) )
			{
				const char* const x = strtok( 0, seperator );
				const char* const y = strtok( 0, seperator );
				const char* const z = strtok( 0, seperator );
				VECTOR3& position = mConfiguration.mPlayerPosition4;
				position.x = float( atof( x ? x : "" ) );
				position.y = float( atof( y ? y : "" ) );
				position.z = float( atof( z ? z : "" ) );
			}
			else if( 0 == strcmpi( "player5", keyword ) )
			{
				const char* const x = strtok( 0, seperator );
				const char* const y = strtok( 0, seperator );
				const char* const z = strtok( 0, seperator );
				VECTOR3& position = mConfiguration.mPlayerPosition5;
				position.x = float( atof( x ? x : "" ) );
				position.y = float( atof( y ? y : "" ) );
				position.z = float( atof( z ? z : "" ) );
			}
			else if( 0 == strcmpi( "humanPosition", keyword ) )
			{
				const char* const x = strtok( 0, seperator );
				const char* const y = strtok( 0, seperator );
				const char* const z = strtok( 0, seperator );
				VECTOR3& position = mDisplayConfigurationForMake.mHuman.mPlayerPosition;
				position.x = float( atof( x ? x : "" ) );
				position.y = float( atof( y ? y : "" ) );
				position.z = float( atof( z ? z : "" ) );
			}
			else if( 0 == strcmpi( "humanScale", keyword ) )
			{
				const char* const scale = strtok( 0, seperator );
				const float value = float( atof( scale ? scale : "" ) );
				VECTOR3& scaleVector = mDisplayConfigurationForMake.mHuman.mScale;
				scaleVector.x = value;
				scaleVector.y = value;
				scaleVector.z = value;
			}
			else if( 0 == strcmpi( "humanCameraPosition", keyword ) )
			{
				const char* const x = strtok( 0, seperator );
				const char* const y = strtok( 0, seperator );
				const char* const z = strtok( 0, seperator );
				VECTOR3& position = mDisplayConfigurationForMake.mHuman.mCameraPosition;
				position.x = float( atof( x ? x : "" ) );
				position.y = float( atof( y ? y : "" ) );
				position.z = float( atof( z ? z : "" ) );
			}
			else if( 0 == strcmpi( "humanCameraRotation", keyword ) )
			{
				const char* const x = strtok( 0, seperator );
				const char* const y = strtok( 0, seperator );
				const char* const z = strtok( 0, seperator );
				VECTOR3& position = mDisplayConfigurationForMake.mHuman.mCameraRotation;
				position.x = float( atof( x ? x : "" ) );
				position.y = float( atof( y ? y : "" ) );
				position.z = float( atof( z ? z : "" ) );
			}
			else if( 0 == strcmpi( "elfPosition", keyword ) )
			{
				const char* const x = strtok( 0, seperator );
				const char* const y = strtok( 0, seperator );
				const char* const z = strtok( 0, seperator );
				VECTOR3& position = mDisplayConfigurationForMake.mElf.mPlayerPosition;
				position.x = float( atof( x ? x : "" ) );
				position.y = float( atof( y ? y : "" ) );
				position.z = float( atof( z ? z : "" ) );
			}
			else if( 0 == strcmpi( "elfScale", keyword ) )
			{
				const char* const scale = strtok( 0, seperator );
				const float value = float( atof( scale ? scale : "" ) );
				VECTOR3& scaleVector = mDisplayConfigurationForMake.mElf.mScale;
				scaleVector.x = value;
				scaleVector.y = value;
				scaleVector.z = value;
			}
			else if( 0 == strcmpi( "elfCameraPosition", keyword ) )
			{
				const char* const x = strtok( 0, seperator );
				const char* const y = strtok( 0, seperator );
				const char* const z = strtok( 0, seperator );
				VECTOR3& position = mDisplayConfigurationForMake.mElf.mCameraPosition;
				position.x = float( atof( x ? x : "" ) );
				position.y = float( atof( y ? y : "" ) );
				position.z = float( atof( z ? z : "" ) );
			}
			else if( 0 == strcmpi( "elfCameraRotation", keyword ) )
			{
				const char* const x = strtok( 0, seperator );
				const char* const y = strtok( 0, seperator );
				const char* const z = strtok( 0, seperator );
				VECTOR3& position = mDisplayConfigurationForMake.mElf.mCameraRotation;
				position.x = float( atof( x ? x : "" ) );
				position.y = float( atof( y ? y : "" ) );
				position.z = float( atof( z ? z : "" ) );
			}
			else if( 0 == strcmpi( "devilPosition", keyword ) )
			{
				const char* const x = strtok( 0, seperator );
				const char* const y = strtok( 0, seperator );
				const char* const z = strtok( 0, seperator );
				VECTOR3& position = mDisplayConfigurationForMake.mDevil.mPlayerPosition;
				position.x = float( atof( x ? x : "" ) );
				position.y = float( atof( y ? y : "" ) );
				position.z = float( atof( z ? z : "" ) );
			}
			else if( 0 == strcmpi( "devilScale", keyword ) )
			{
				const char* const scale = strtok( 0, seperator );
				const float value = float( atof( scale ? scale : "" ) );
				VECTOR3& scaleVector = mDisplayConfigurationForMake.mDevil.mScale;
				scaleVector.x = value;
				scaleVector.y = value;
				scaleVector.z = value;
			}
			else if( 0 == strcmpi( "devilCameraPosition", keyword ) )
			{
				const char* const x = strtok( 0, seperator );
				const char* const y = strtok( 0, seperator );
				const char* const z = strtok( 0, seperator );
				VECTOR3& position = mDisplayConfigurationForMake.mDevil.mCameraPosition;
				position.x = float( atof( x ? x : "" ) );
				position.y = float( atof( y ? y : "" ) );
				position.z = float( atof( z ? z : "" ) );
			}
			else if( 0 == strcmpi( "devilCameraRotation", keyword ) )
			{
				const char* const x = strtok( 0, seperator );
				const char* const y = strtok( 0, seperator );
				const char* const z = strtok( 0, seperator );
				VECTOR3& position = mDisplayConfigurationForMake.mDevil.mCameraRotation;
				position.x = float( atof( x ? x : "" ) );
				position.y = float( atof( y ? y : "" ) );
				position.z = float( atof( z ? z : "" ) );
			}
		}
	}

	// 090529 LUJ, 생성 화면은 종족마다 카메라, 플레이어 위치가 다르다.
	//	따라서 카메라 변경에 사용되는 값에 대입해줘야 한다. 그렇지 않으면 키보드로 카메라 위치가 변경되지 않는다.
	if( TypeMakePlayer == mConfiguration.mType )
	{
		SetRace( mDisplayConfigurationForMake.mRaceIndex );
	}

	fclose( file );	
#ifdef _GMTOOL
	cprintf(
		"%02d:%02d:%02d camera.txt is loaded successfully(camera:%0.1f, %0.1f, %0.1f / eye: %0.1f, %0.1f, %0.1f)\n",
		time->tm_hour,
		time->tm_min,
		time->tm_sec,
		mConfiguration.mCameraPosition.x,
		mConfiguration.mCameraPosition.y,
		mConfiguration.mCameraPosition.z,
		mConfiguration.mCameraRotation.x,
		mConfiguration.mCameraRotation.y,
		mConfiguration.mCameraRotation.z );
#endif
	return TRUE;
}

const VECTOR3& CCameraConfiguration::GetPlayerPosition( DWORD standIndex ) const
{
	static VECTOR3 cameraPosition;
	cameraPosition = mConfiguration.mCameraPosition;

	switch( standIndex )
	{
	case 0:
		{
			cameraPosition = cameraPosition + mConfiguration.mPlayerPosition1;
			break;
		}
	case 1:
		{
			cameraPosition = cameraPosition + mConfiguration.mPlayerPosition2;
			break;
		}
	case 2:
		{
			cameraPosition = cameraPosition + mConfiguration.mPlayerPosition3;
			break;
		}
	case 3:
		{
			cameraPosition = cameraPosition + mConfiguration.mPlayerPosition4;
			break;
		}
	case 4:
		{
			cameraPosition = cameraPosition + mConfiguration.mPlayerPosition5;
			break;
		}
	}

	return cameraPosition;
}

const VECTOR3& CCameraConfiguration::GetRacePosition( DWORD raceIndex ) const
{
	static VECTOR3 playerPosition;
	playerPosition = GetCameraPositionByRace( raceIndex );

	switch( raceIndex )
	{
	case 0:
		{
			playerPosition = playerPosition + mDisplayConfigurationForMake.mHuman.mPlayerPosition;
			break;
		}
	case 1:
		{
			playerPosition = playerPosition + mDisplayConfigurationForMake.mElf.mPlayerPosition;
			break;
		}
	case 2:
		{
			playerPosition = playerPosition + mDisplayConfigurationForMake.mDevil.mPlayerPosition;
			break;
		}
	}

	return playerPosition;
}

const VECTOR3& CCameraConfiguration::GetCameraPositionByRace( DWORD raceIndex ) const
{
	switch( raceIndex )
	{
	case 0:
		{
			return mDisplayConfigurationForMake.mHuman.mCameraPosition;
		}
	case 1:
		{
			return mDisplayConfigurationForMake.mElf.mCameraPosition;
		}
	case 2:
		{
			return mDisplayConfigurationForMake.mDevil.mCameraPosition;
		}
	}

	static const VECTOR3 emptyValue = {0};
	return emptyValue;
}

const VECTOR3& CCameraConfiguration::GetCameraRotationByRace( DWORD raceIndex ) const
{
	switch( raceIndex )
	{
	case 0:
		{
			return mDisplayConfigurationForMake.mHuman.mCameraRotation;
		}
	case 1:
		{
			return mDisplayConfigurationForMake.mElf.mCameraRotation;
		}
	case 2:
		{
			return mDisplayConfigurationForMake.mDevil.mCameraRotation;
		}
	}

	static const VECTOR3 emptyValue = {0};
	return emptyValue;
}

void CCameraConfiguration::SetRace( DWORD raceIndex )
{
	mDisplayConfigurationForMake.mRaceIndex = raceIndex;

	switch( raceIndex )
	{
	case 0:
		{
			mConfiguration.mCameraPosition = mDisplayConfigurationForMake.mHuman.mCameraPosition;
			mConfiguration.mCameraRotation = mDisplayConfigurationForMake.mHuman.mCameraRotation;
			break;
		}
	case 1:
		{
			mConfiguration.mCameraPosition = mDisplayConfigurationForMake.mElf.mCameraPosition;
			mConfiguration.mCameraRotation = mDisplayConfigurationForMake.mElf.mCameraRotation;
			break;
		}
	case 2:
		{
			mConfiguration.mCameraPosition = mDisplayConfigurationForMake.mDevil.mCameraPosition;
			mConfiguration.mCameraRotation = mDisplayConfigurationForMake.mDevil.mCameraRotation;
			break;
		}
	}
}

const VECTOR3& CCameraConfiguration::GetScale( DWORD raceIndex ) const
{
	switch( raceIndex )
	{
	case 0:
		{
			return mDisplayConfigurationForMake.mHuman.mScale;
		}
	case 1:
		{
			return mDisplayConfigurationForMake.mElf.mScale;
		}
	case 2:
		{
			return mDisplayConfigurationForMake.mDevil.mScale;
		}
	}

	static const VECTOR3 defaultValue = { 1, 1, 1 };
	return defaultValue;
}

void CCameraConfiguration::UpdateCamera()
{
	if( 0 == g_pExecutive )
	{
		return;
	}

	I4DyuchiGXGeometry* const geometry = g_pExecutive->GetGeometry();

	if( 0 == geometry )
	{
		return;
	}

	geometry->SetCameraPos( &mConfiguration.mCameraPosition, 0 );
	geometry->SetCameraAngleRad( &mConfiguration.mCameraRotation, 0 );
}