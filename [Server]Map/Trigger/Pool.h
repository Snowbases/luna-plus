#pragma once

namespace Trigger
{
	class CPool
	{
	public:
		static CPool& GetInstance();
		void* Allocate( size_t size );
		void Deallocate( void* chunk, size_t size );

	private:
		CPool(void);
		virtual ~CPool(void);

	private:
		typedef std::set< void* > MemorySet;
		std::map< size_t, MemorySet > mPoolMemoryMap;
		MemorySet mAllocatedMemorySet;
		const size_t mAllocatedSizeAtOneTime;
	};

	class CPoolObject
	{
	public:
		CPoolObject(void) {}
		virtual ~CPoolObject(void) {}
	public:
		void* operator new( size_t size );
		void* operator new[]( size_t size );
		void operator delete( void* memory, size_t size );
		void operator delete[] ( void* memory, size_t size );
	};
}