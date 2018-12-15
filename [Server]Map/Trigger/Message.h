#pragma once

#include "Pool.h"

namespace Trigger
{
	enum eProperty;

	class CMessage : public CPoolObject
	{
	public:
		CMessage(void);
		virtual ~CMessage(void);
		typedef std::map< eProperty, long > PropertyMap;
		PropertyMap mPropertyMap;

		int GetValue(eProperty property) const
		{
			const PropertyMap::const_iterator iter = mPropertyMap.find(property);

			return iter == mPropertyMap.end() ? 0 : iter->second;
		}

		DWORD IsEmpty() const
		{
			return mPropertyMap.empty();
		}

		void AddValue(eProperty property, long value)
		{
			mPropertyMap[property] = value;
		}

		DWORD GetIndex() const { return mIndex; }
		void SetIndex( DWORD index ) { mIndex = index; }

	private:
		DWORD mIndex;
	};
}