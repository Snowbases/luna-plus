// ScriptTestMode.h - iros
//////////////////////////////////////////////////////////////////////////

#ifndef _SCRIPT_TEST_MODE_
#define _SCRIPT_TEST_MODE_

#include <tinyxml.h>
#include <list>
#include <vector>

#include "ScriptTestStr.h"

using namespace std;

class cScriptOper;
class cScriptTestMode;

class cScriptOper
{
public:
	cScriptOper();
	virtual ~cScriptOper();

	bool			LoadOperElem(TiXmlElement* operElem);

	cScriptOper*	FindGlobalOper(const char* str);
	cScriptOper*	FindChildOper(const char* str);
	int				FindChildOperIdx(const char* str);

	bool			ParamTest(const CStringArray& str, cScriptOper** childOper, CStringArray& listLastErrStr);

public:
	typedef vector<cScriptOper*>			OperVec;
	typedef OperVec::iterator				OperVecItr;

	string				m_operName;
	int					m_nParam;
	list<UINT>			m_listParam;

	OperVec				m_vecGlobalOper;
	OperVec				m_vecChildOper;

};

//
struct stSelOper
{
	cScriptOper* curOper;
	cScriptOper* childOper;

	stSelOper() : curOper(NULL), childOper(NULL) { }
	void Init() { curOper = NULL; childOper = NULL; }
};

#define MAX_CHILD_OPER_LEVEL	10
//
class cScriptTestMode
{
public:
	cScriptTestMode();
	virtual ~cScriptTestMode();

	bool LoadElem(TiXmlElement* parentElem);

	cScriptOper* FindGlobalOper(const char* str);
	cScriptOper* FindChildOper(const char* str);

	void StartTest();
	void EndTest();

	void StartGroup();
	void EndGroup();

	bool CommentTest(const char* str);
	bool CommentTest(const CStringArray& str);
	bool OperTest(const CStringArray& str);

	bool TestLine(const CStringArray& str);

	CStringArray& GetLastErr() { return m_LastErrStrArr; }

public:

	typedef vector<cScriptOper*>	OperVec;
	typedef OperVec::iterator		OperVecItr;

	string				m_modeName;
	list<string>		m_listCommentStr;
	OperVec				m_vecOper;
	OperVec				m_vecGlobalOper;

	int					m_curLevel;
	stSelOper			m_SelOper[MAX_CHILD_OPER_LEVEL];

	CStringArray		m_LastErrStrArr;
};


#endif //_SCRIPT_TEST_MODE_