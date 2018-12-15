#include "StdAfx.h"
#include "Pool.h"

namespace Trigger
{
	CPool::CPool(void) :
	mAllocatedSizeAtOneTime( 10 )
	{
	}

	CPool::~CPool(void)
	{
		// 객체 소멸 시 할당한 공간을 삭제한다
		for( MemorySet::const_iterator it = mAllocatedMemorySet.begin();
			mAllocatedMemorySet.end() != it;
			++it )
		{
			free( *it );
		}
	}

	CPool& CPool::GetInstance()
	{
		static CPool triggerPool;

		return triggerPool;
	}

	void* CPool::Allocate( size_t size )
	{
		// 할당받을 크기에 해당하는 메모리 셋을 가져온다
		MemorySet& memorySet = mPoolMemoryMap[ size ];
		
		// 할당할 메모리가 없을 경우 큰 덩어리로 할당을 받고, 메모리 셋에 넣어서 반환하기 편하게 한다
		if( memorySet.empty() )
		{
			// 큰 덩어리로 할당받고, 싱글턴 객체가 소멸할 때 쉽게 free할 수 있도록 컨테이너에 넣는다
			void* const allocatedMemory = malloc( mAllocatedSizeAtOneTime * size );
			mAllocatedMemorySet.insert( allocatedMemory );

			// 반환하기 쉽도록 할당 단위로 쪼개서 컨테이너에 넣는다.
			for( size_t i = 0; i < mAllocatedSizeAtOneTime; ++i )
			{
				char* const chunk = ( char* )allocatedMemory + i * size;
				memorySet.insert( ( void* )chunk );
			}
		}

		// 메모리 셋에 든 값을 반환하고, 컨테이너에서 삭제한다
		void* const memory = *( memorySet.begin() );
		memorySet.erase( memory );

		return memory;
	}

	void CPool::Deallocate( void* chunk, size_t size )
	{
		// 반환한 메모리를 재사용할 수 있도록 컨테이너에 넣는다
		MemorySet& memorySet = mPoolMemoryMap[ size ];
		memorySet.insert( chunk );
	}

	void* CPoolObject::operator new( size_t size )
	{
		return CPool::GetInstance().Allocate( size );
	}

	void* CPoolObject::operator new[]( size_t size )
	{ 
		return CPool::GetInstance().Allocate( size );
	}

	void CPoolObject::operator delete( void* memory, size_t size )
	{
		CPool::GetInstance().Deallocate( memory, size );
	}

	void CPoolObject::operator delete[] ( void* memory, size_t size )
	{
		CPool::GetInstance().Deallocate( memory, size );
	}
}