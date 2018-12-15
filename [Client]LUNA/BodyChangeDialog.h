/*
080414 LUJ, 캐릭터 외양 바꾸는 창
*/
#pragma once

#include "interface/cDialog.h"
#include "MHCamera.h"

class	CItem;

class CBodyChangeDialog : public  cDialog
{
public:
	CBodyChangeDialog();
	virtual ~CBodyChangeDialog();

	void Linking();
	
	virtual void OnActionEvent( LONG id, void* p, DWORD we );
	
	virtual void SetActive( BOOL isActive, CItem& );
	virtual void SetActive( BOOL isActive );
	virtual void Render();
	
private:	
	class CMenu
	{
	private:		
		// 080414 LUJ, 키: 메뉴 값
		typedef std::map< BYTE, std::string >	ItemList;

		ItemList					mItemList;
		ItemList::const_iterator	mIterator;

		DWORD	mDefaultValue;

	public:
		CMenu()
		{
			// 080728 LUJ, 초기화 함수를 분리
			Clear();
		}

		void Add( BYTE value, const char* text )
		{
			mItemList.insert( std::make_pair( value, text ) );
		}

		void Increase()
		{
			++mIterator;

			if( mIterator == mItemList.end() )
			{
				mIterator = mItemList.begin();
			}
		}

		void Decrease()
		{
			if( mIterator == mItemList.begin() )
			{
				mIterator = mItemList.end();
			}

			--mIterator;	
		}

		const char* GetText() const
		{
			return mIterator == mItemList.end() ? "?" : mIterator->second.c_str();
		}

		BYTE GetValue() const
		{
			return mIterator == mItemList.end() ? 0 : mIterator->first;
		}

		DWORD GetSize() const
		{
			return mItemList.size();
		}

		void SetDefault( BYTE value )
		{
			mDefaultValue = value;

			ItemList::const_iterator it = mItemList.begin();
			
			for(
				;
				mItemList.end() != it;
				++it )
			{
				if( value == it->first )
				{
					mIterator = it;
					break;
				}
			}
		}

		// 080728 LUJ, 초기화
		void Clear()
		{
			mItemList.clear();

			mDefaultValue	= 0;
			mIterator		= mItemList.begin();
		}
	};

	CMenu mHairMenu;
	CMenu mFaceMenu;

	CHARACTERCHANGE_INFO mCharacterChangeInfo;

	// 080414 LUJ, 외양 변경에 사용한 아이템
	ITEMBASE mUsedItem;

	// 080414 LUJ, 카메라가 자동으로 작동하지 않는 시간
	DWORD	mStopTime;

	// 080414 LUJ,	문자열. 캐릭터 생성 관련한 문자열은 CharMake_List.bin에 저장되어 있다. 따라서 여기를 우선적으로 조회하되
	//				없는 경우 리소스 매니저를 참조하도록 한다
	struct MenuText
	{
		RaceType	mRace;
		DWORD		mGender;
		DWORD		mValue;
		PART_TYPE	mPart;
		std::string	mText;
	};

	typedef std::list< MenuText >	MenuTextList;
	MenuTextList					mMenuTextList;

	// 080414 LUJ, MenuTextList에서 조건에 해당하는 문자열을 반환한다
	const char* GetText( RaceType, PART_TYPE, DWORD gender, DWORD value ) const;

	// 080414 LUJ, 카메라 정보를 복구하기 위해 사용
	MHCAMERADESC mStoredCameraDescription;

	// 080414 LUJ, 자동 회전 지연 시간
	DWORD	mAutoRotataionDelayTime;
};
