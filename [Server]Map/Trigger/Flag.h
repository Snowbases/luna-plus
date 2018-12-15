#pragma once

#include "Pool.h"

namespace Trigger
{
	enum eOwner;

	class CFlag : public CPoolObject
	{
	public:
		void SetValue(DWORD);
		DWORD GetValue() const;
	}
}