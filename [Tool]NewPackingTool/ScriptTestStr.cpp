// ScriptTestStr.cpp - iros
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScriptTestStr.h"


char g_seps[] = " \t\n\0";

char g_ParamTypeStr[eType_QNT][50] =
{
	"BOOL",
	"INT",
	"FLOAT",
	"FRAME",
	"STRING",
	"CHILD_OPER_STR",
	"NUM_INT",
	"UNLIMIT_INT"
};

UINT StrToParamType(const char* str)
{
	ASSERT(str);
	if (strlen(str) == 0)
		return eType_QNT;

	UINT i;

	for (i=0; i<eType_QNT; ++i)
	{
		if ( !_tcscmp(str, g_ParamTypeStr[i]) )
			return i;
	}

	return eType_QNT;
}

bool IsBoolStr(const TCHAR* /*token*/)
{
	return true;
}

bool IsIntStr(const TCHAR* token)
{
	ASSERT(token);

	int len = (int)_tcslen(token);

	int i;
	for (i=0; i<len; ++i)
	{
		if (i==0 && token[i] == '-')
			continue;

		if ( token[i] < '0' || token[i] > '9')
			return false;
	}

	return true;
}

bool IsFloatStr(const TCHAR* token)
{
	ASSERT(token);

	int len = (int)_tcslen(token);

	int i;
	for (i=0; i<len; ++i)
	{
		if (i==0 && token[i] == '-')
			continue;

		if ( token[i] < '0' || token[i] > '9')
			if (token[i] != '.')
				return false;
	}

	return true;
}

bool IsFrameStr(const TCHAR* token)
{
	ASSERT(token);

	if(token[0] == 'f' || token[0] == 'F')
	{
		return IsIntStr(&token[1]);
	}
	else
	{
		return IsIntStr(token);
	}
}

bool CheckType(const TCHAR* token, UINT type)
{
	switch (type)
	{
		case eType_BOOL :
			return IsBoolStr(token);
		case eType_INT :
			return IsIntStr(token);
		case eType_FLOAT :
			return IsFloatStr(token);
		case eType_FRAME :
			return IsFrameStr(token);
		case eType_STRING :
			return true; //(IsStr(token))
		case eType_CHILD_OPER_STR :
			return true; 
		case eType_NUM_INT:
			return IsIntStr(token);
		case eType_UNLIMIT_INT:
			return true;
		default:
			break;
	}

	return TRUE;
}