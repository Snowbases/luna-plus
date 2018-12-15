#pragma once
#include "Action.h"

namespace Trigger
{
	class CActionCameraFilterAttach :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionCameraFilterAttach;
		}
	};

	class CActionCameraFilterDetach :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionCameraFilterDetach;
		}
	};

	class CActionCameraFilterMove :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionCameraFilterMove;
		}
	};

	class CActionCameraShake :
		public CAction
	{
	public:
		virtual void DoAction();
		static CAction* Clone()
		{
			return new CActionCameraShake;
		}
	};
}