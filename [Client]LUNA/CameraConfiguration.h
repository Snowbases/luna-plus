/*
090529 LUJ, 로그인, 선택, 생성 화면을 위한 카메라 설정을 관리한다
*/
#pragma once

#include <sys/stat.h>
#include <time.h>

class CCameraConfiguration
{
public:
	enum Type
	{
		TypeNone,
		TypeLogin,
		TypeSelectPlayer,
		TypeMakePlayer,
	};

	CCameraConfiguration( Type type = TypeNone );
	virtual ~CCameraConfiguration();
	void SetType( Type type );
	BOOL Process();
	void UpdateCamera();
	const VECTOR3& GetPlayerPosition( DWORD standIndex ) const;
	const VECTOR3& GetRacePosition( DWORD raceIndex ) const;
	const VECTOR3& GetCameraPositionByRace( DWORD raceIndex ) const;
	const VECTOR3& GetCameraRotationByRace( DWORD raceIndex ) const;
	const VECTOR3& GetScale( DWORD raceIndex ) const;
	void SetRace( DWORD raceIndex );

private:
	BOOL Adjust();
	BOOL Load( BOOL isUpdateRecently );

private:
	// 090529 LUJ, 설정 파일이 업데이트될 경우 즉시 읽어오기 위해 최근에 저장된 시각을 저장한다.
	struct stat mFileStatus;
	struct Configuration
	{
		VECTOR3 mCameraPosition;
		VECTOR3 mCameraRotation;
		VECTOR3 mPlayerPosition1;
		VECTOR3 mPlayerPosition2;
		VECTOR3 mPlayerPosition3;
		VECTOR3 mPlayerPosition4;
		VECTOR3 mPlayerPosition5;
		Type mType;
		BOOL mIsAdjustable;

		Configuration() :
		mType( TypeNone ),
		mIsAdjustable( FALSE )
		{
			ZeroMemory( &mCameraPosition, sizeof( mCameraPosition ) );
			ZeroMemory( &mCameraRotation, sizeof( mCameraRotation ) );
			ZeroMemory( &mPlayerPosition1, sizeof( mPlayerPosition1 ) );
			ZeroMemory( &mPlayerPosition2, sizeof( mPlayerPosition2 ) );
			ZeroMemory( &mPlayerPosition3, sizeof( mPlayerPosition3 ) );
			ZeroMemory( &mPlayerPosition4, sizeof( mPlayerPosition4 ) );
			ZeroMemory( &mPlayerPosition5, sizeof( mPlayerPosition5 ) );
		}
	}
	mConfiguration;
	struct DisplayConfigurationForMake
	{
		struct DisplayConfiguration
		{
			VECTOR3 mPlayerPosition;
			VECTOR3 mCameraPosition;
			VECTOR3 mCameraRotation;
			VECTOR3 mScale;

			DisplayConfiguration()
			{
				ZeroMemory( &mPlayerPosition, sizeof( mPlayerPosition ) );
				ZeroMemory( &mCameraPosition, sizeof( mCameraPosition ) );
				ZeroMemory( &mCameraRotation, sizeof( mCameraRotation ) );
				ZeroMemory( &mScale, sizeof( mScale ) );
			}
		};

		DisplayConfiguration mHuman;
		DisplayConfiguration mElf;
		DisplayConfiguration mDevil;
		DWORD mRaceIndex;

		DisplayConfigurationForMake() :
		mRaceIndex( UINT_MAX )
		{}
	}
	mDisplayConfigurationForMake;
};