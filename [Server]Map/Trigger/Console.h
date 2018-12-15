#pragma once

namespace Trigger
{
	class CConsole
	{
	public:
		CConsole();
		virtual ~CConsole();	
        void Put(LPCTSTR, ...) const;
	};
}