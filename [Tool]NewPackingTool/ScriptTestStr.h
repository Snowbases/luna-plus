// ScriptTestStr.h - iros
//////////////////////////////////////////////////////////////////////////

#ifndef _SCRIPT_TEST_STR_
#define _SCRIPT_TEST_STR_

extern char g_seps[];

enum eParamType
{
	eType_BOOL,
	eType_INT,
	eType_FLOAT,
	eType_FRAME,
	eType_STRING,
	eType_CHILD_OPER_STR,
	eType_NUM_INT,
	eType_UNLIMIT_INT,
	eType_QNT,
};
extern char g_ParamTypeStr[eType_QNT][50];

UINT StrToParamType(const char* str);
bool CheckType(const char* token, UINT type);

#endif //_SCRIPT_TEST_STR_