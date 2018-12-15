// ScriptTest.h - iros
//////////////////////////////////////////////////////////////////////////

#ifndef _SCRIPT_TEST_
#define _SCRIPT_TEST_

#include <tinyxml.h>
#include <list>
#include <vector>

#include "ScriptTestStr.h"
#include "ScriptTestMode.h"

//
typedef vector<cScriptTestMode*>	ScriptTestModeVec;
typedef ScriptTestModeVec::iterator	ScriptTestModeVecItr;

class cScriptTest
{
public:
	cScriptTest();
	virtual ~cScriptTest();

	bool LoadXml(const char* filePath);
	cScriptTestMode* GetScriptTestMode(UINT mode);

public:
	bool m_bLoad;

	ScriptTestModeVec m_vecScript;
};

extern cScriptTest g_ScriptTest;

inline cScriptTestMode* GetScriptTestMode(UINT mode)
{
	if (!g_ScriptTest.m_bLoad)
		return NULL;

	return g_ScriptTest.GetScriptTestMode(mode);
};

#endif //_SCRIPT_TEST_