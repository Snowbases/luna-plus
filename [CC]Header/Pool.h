/*
081107 LUJ, STL 자료구조를 이용한 풀 자료구조. 기존 CMemoryPoolTempl 클래스를 대체하기 위해 만들어졌다.
기존 자료 구조는 할당보다 해제를 많이 할 경우 포인터 인덱스가 잘못되는 버그가 있다. 이 버그가
발생하면 메모리를 더 이상 할당받을 수 없게된다.
*/
#pragma once

#include <hash_set>
#include <vector>
#include <tchar.h>

template< typename Data >
class CPool
{
public:
	CPool()
	{}

	CPool(size_t baseSize, size_t increaseSize, LPCTSTR name)
	{
		Init(
			baseSize,
			increaseSize,
			name);
	}

	virtual ~CPool()
	{
		Release();
	}

	void Release()
	{
		for( MemoryList::const_iterator it = mMemoryList.begin();
			mMemoryList.end() != it;
			++it )
		{
			Data* data = *it;
			delete[] data;
		}

		mMemoryList.clear();
		mReservedDataSet.clear();
		mAllocatedDataSet.clear();
	}

	void Init( size_t baseSize, size_t increaseSize, const TCHAR* name )
	{
		mSetting.mBaseSize		= baseSize;
		mSetting.mIncreaseSize	= increaseSize;
		_tcsncpy( mSetting.mName, name, sizeof( mSetting.mName ) / sizeof( *mSetting.mName ) - 1 );

		Increase( baseSize );
	}

	void Free( Data* const info )
	{
		Deallocate( info );
	}

	Data* const Alloc()
	{
		return Allocate();
	}

private:
	void Increase( size_t size )
	{
		Data* dataArray = new Data[ size ];
		mMemoryList.push_back( dataArray );

		for( Data* data = dataArray + size; data-- != dataArray; )
		{
			mReservedDataSet.insert( data );
		}
	}

	Data* const Allocate()
	{
		if( mReservedDataSet.empty() )
		{
			Increase( mSetting.mIncreaseSize );

			// 081117 LUJ, 용량을 늘렸는데도 할당 가능 공간이 없으면 진행 불가
			if( mReservedDataSet.empty() )
			{
				return 0;
			}
		}

		Data* const data = *( mReservedDataSet.begin() );

		mAllocatedDataSet.insert( data );
		mReservedDataSet.erase( data );

		return data;
	}

	void Deallocate( Data* const data )
	{	
		// 090225 LUJ, 할당되었던 메모리만 해제해야한다
		if( mAllocatedDataSet.erase( data ) )
		{
			mReservedDataSet.insert( data );
		}
	}

	bool IsAllocated( const Data* data ) const
	{
		return mAllocatedDataSet.end() != mAllocatedDataSet.find( const_cast< Data* >( data ) );
	}

	bool IsReserved( const Data* data ) const
	{
		return mReservedDataSet.end() != mReservedDataSet.find( const_cast< Data* >( data ) );
	}

	typedef stdext::hash_set< Data* > DataSet;
	// 081117 LUJ, 할당된 포인터 셋
	DataSet	mAllocatedDataSet;
	// 081117 LUJ, 사용 가능한 포인터 셋
	DataSet	mReservedDataSet;
	typedef std::list< Data* > MemoryList;
	MemoryList mMemoryList;
	
	struct Setting
	{
		size_t	mBaseSize;
		size_t	mIncreaseSize;
		TCHAR	mName[ 100 ];

		Setting()
		{
			memset( this, 0, sizeof( *this ) );
		}
	}
	mSetting;
};