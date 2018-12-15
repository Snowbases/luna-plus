// ScriptTestMode.cpp - iros
//////////////////////////////////////////////////////////////////////////

#include "stdAfx.h"
#include "ScriptTestMode.h"

//////////////////////////////////////////////////////////////////////////
// cScriptOper
cScriptOper::cScriptOper()
: m_nParam(0)
{
}

cScriptOper::~cScriptOper()
{
	OperVecItr operItr;

	for (operItr=m_vecGlobalOper.begin(); operItr!=m_vecGlobalOper.end(); ++operItr)
		SAFE_DEL(*operItr);

	for (operItr=m_vecChildOper.begin(); operItr!=m_vecChildOper.end(); ++operItr)
		SAFE_DEL(*operItr);
}

cScriptOper* cScriptOper::FindGlobalOper(const char* str)
{
	ASSERT(str);

	OperVecItr operItr;
	for (operItr=m_vecGlobalOper.begin(); operItr!=m_vecGlobalOper.end(); ++operItr)
	{
		if ((*operItr)->m_operName == str)
		{
			return (*operItr);
		}
	}

	return NULL;
}

int cScriptOper::FindChildOperIdx(const char* str)
{
	ASSERT(str);

	OperVecItr operItr;
	int idx = 0;

	for (operItr=m_vecGlobalOper.begin(); operItr!=m_vecGlobalOper.end(); ++operItr)
	{
		if ((*operItr)->m_operName == str)
		{
			return idx;
		}

		++idx;
	}

	return -1;
}

cScriptOper* cScriptOper::FindChildOper(const char* str)
{
	ASSERT(str);

	OperVecItr operItr;
	for (operItr=m_vecChildOper.begin(); operItr!=m_vecChildOper.end(); ++operItr)
	{
		if ((*operItr)->m_operName == str)
		{
			return (*operItr);
		}
	}

	return NULL;
}

bool cScriptOper::ParamTest(const CStringArray& strArr, cScriptOper** childOper, CStringArray& listLastErrStr)
{
	const char* token;
	token = strArr.GetAt(0);

	// OperName 다시 채크.
	ASSERT(m_operName == token);

	int nToken, numParam = 0;
	list<UINT>::iterator paramItr = m_listParam.begin();
	UINT	paramType;
	CString errStr;

	cScriptOper*	subOper = NULL;
	CStringArray	subOperArr;

	for (nToken=1; nToken < strArr.GetSize(); ++nToken)
	{
		paramType = *paramItr;

		token = strArr.GetAt(nToken);

		// Line에서 SubOper 뒤에오는 Param은 전부 Sub쪽에서 계산 되므로.. 주의하자.
		// SubOper
		cScriptOper* findOper = FindChildOper(token);

		if (findOper)
		{
			if (subOper)
			{
				cScriptOper* cOper = NULL;
				subOper->ParamTest(subOperArr, &cOper, listLastErrStr);
				subOperArr.RemoveAll();
			}

			if (paramType != eType_CHILD_OPER_STR)
			{
				subOper = findOper;
				subOperArr.Add(token);
				continue;
			}
		}

		if (subOper)
		{
			subOperArr.Add(token);
			continue;
		}

		// Param
		++numParam;

		if (paramItr == m_listParam.end())
		{
			// Err Param Num is over
			errStr.Format(" error : [ %s ] 's Param num is not %d ", strArr.GetAt(0), numParam);
			listLastErrStr.Add(errStr);

			return false;
		}

		
		if (paramType == eType_CHILD_OPER_STR)
		{
			if (!findOper)
			{
				// Err Can't Find ChildUnit
				errStr.Format(" error : Can't Find [ %s ] Oper ", token);
				listLastErrStr.Add(errStr);
			}
			*childOper = findOper;
		}
		else
		{
			if (!CheckType(token, paramType))
			{
				// Err Type Invalid;
				errStr.Format(" error : Param[%d] [ %s ] is must %s", numParam, token, g_ParamTypeStr[paramType]);
				listLastErrStr.Add(errStr);
			}
		}

		if (paramType == eType_NUM_INT)
		{
			int num = atoi(token);

			int i;
			for (i=0; i<num; ++i)
			{
				token = strArr.GetAt(++nToken);

				if (token)
				{
					if (!CheckType(token, eType_INT))
					{
						// Err Type Invalid;
						errStr.Format(" error : Param[%d] [ %s ] is must %s", nToken, token, g_ParamTypeStr[eType_INT]);
						listLastErrStr.Add(errStr);
					}
				}
				else
				{
					errStr.Format(" error : [ %s ] is must %d num param", m_operName.c_str(), num);
					listLastErrStr.Add(errStr);
				}
			}
		}
		else if (paramType == eType_UNLIMIT_INT)
		{
			++nToken;
			while (nToken < strArr.GetSize())
			{
				token = strArr.GetAt(nToken);

				if (!CheckType(token, eType_INT))
				{
					// Err Type Invalid;
					errStr.Format(" error : Param[%d] [ %s ] is must %s", nToken, token, g_ParamTypeStr[eType_INT]);
					listLastErrStr.Add(errStr);
				}
				++nToken;
			}
			break;
		}

		++paramItr;
	}

	if (numParam != static_cast<int>(m_listParam.size()))
	{
		// Err Param Num
		int paramNum = (int)(m_listParam.size());
		errStr.Format(" error : [ %s ] is must %d num param", m_operName.c_str(), paramNum);
		listLastErrStr.Add(errStr);

		return false;
	}

	if (listLastErrStr.GetSize() > 0)
		return false;

	return true;
}

bool cScriptOper::LoadOperElem(TiXmlElement* operElem)
{
	ASSERT(operElem);

	m_operName = operElem->Attribute("name");
	operElem->QueryIntAttribute("nParam", &m_nParam);

	// Load ParamList
	TiXmlElement* paramTypeElem = operElem->FirstChildElement("PARAM");
	while (paramTypeElem)
	{
		UINT paramType = StrToParamType(paramTypeElem->Attribute("name"));
		if (paramType < eType_QNT)
		{
			m_listParam.push_back(paramType);
		}
		else
		{
			// Invalid TYPE Err
		}

		// NEXT
		paramTypeElem = paramTypeElem->NextSiblingElement("PARAM");
	}

	if (m_nParam != static_cast<int>(m_listParam.size()))
	{
		// ParamNum Err
	}
	// Load GrobalOper
	TiXmlElement* globalOperElem = operElem->FirstChildElement("CHILDBASEOPER");
	while (globalOperElem)
	{
		string name = globalOperElem->Attribute("name");

		if (name.length()>0)
		{
			cScriptOper* oper = new cScriptOper;

			oper->LoadOperElem(globalOperElem);

			m_vecGlobalOper.push_back(oper);
		}
		else
		{
			// Unit Name Err
		}

		// NEXT
		globalOperElem = globalOperElem->NextSiblingElement("CHILDBASEOPER");
	}


	//// Load ChildUnit
	//TiXmlElement* unitElem = operElem->FirstChildElement("CHILDUNIT");
	//while (unitElem)
	//{
	//	string name = unitElem->Attribute("name");

	//	if (name.length()>0)
	//	{
	//		cChildUnit* unit = new cChildUnit;

	//		unit->LoadUnitElem(unitElem);

	//		m_vecUnit.push_back(unit);
	//	}
	//	else
	//	{
	//		// Unit Name Err
	//	}

	//	// NEXT
	//	unitElem = unitElem->NextSiblingElement("CHILDUNIT");
	//}

	// Load ChildOper
	TiXmlElement* childOperElem = operElem->FirstChildElement("OPER");
	while (childOperElem)
	{
		string name = childOperElem->Attribute("name");

		if (name.length()>0)
		{
			cScriptOper* oper = new cScriptOper;

			oper->LoadOperElem(childOperElem);

			m_vecChildOper.push_back(oper);
		}
		else
		{
			// Unit Name Err
		}

		// NEXT
		childOperElem = childOperElem->NextSiblingElement("OPER");
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// cScriptTestMode
//////////////////////////////////////////////////////////////////////////

cScriptTestMode::cScriptTestMode()
	: m_curLevel(0)
{
	ZeroMemory(m_SelOper, sizeof(m_SelOper));
}
//////////////////////////////////////////////////////////////////////////
cScriptTestMode::~cScriptTestMode()
{
	OperVecItr itr;
	for (itr=m_vecOper.begin(); itr!=m_vecOper.end(); ++itr)
		SAFE_DEL(*itr);

	for (itr=m_vecGlobalOper.begin(); itr!=m_vecGlobalOper.end(); ++itr)
		SAFE_DEL(*itr);
}
//////////////////////////////////////////////////////////////////////////
cScriptOper* cScriptTestMode::FindGlobalOper(const char* str)
{
	ASSERT(str);

	OperVecItr operItr;
	for (operItr=m_vecGlobalOper.begin(); operItr!=m_vecGlobalOper.end(); ++operItr)
	{
		if ((*operItr)->m_operName == str)
		{
			return (*operItr);
		}
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////////
cScriptOper* cScriptTestMode::FindChildOper(const char* str)
{
	ASSERT(str);

	OperVecItr operItr;
	for (operItr=m_vecOper.begin(); operItr!=m_vecOper.end(); ++operItr)
	{
		if ((*operItr)->m_operName == str)
		{
			return (*operItr);
		}
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////////
void cScriptTestMode::StartTest()
{
	m_curLevel = 0;

	ZeroMemory(m_SelOper, sizeof(m_SelOper));

	m_LastErrStrArr.RemoveAll();
}
//////////////////////////////////////////////////////////////////////////
void cScriptTestMode::EndTest()
{
}
//////////////////////////////////////////////////////////////////////////
void cScriptTestMode::StartGroup()
{
	ASSERT(m_curLevel < MAX_CHILD_OPER_LEVEL);

	if (m_SelOper[m_curLevel].curOper)
	{
		m_SelOper[++m_curLevel].curOper = NULL;
	}
}
//////////////////////////////////////////////////////////////////////////
void cScriptTestMode::EndGroup()
{
	if (m_curLevel > 0)
		m_SelOper[m_curLevel--].Init();
}
//////////////////////////////////////////////////////////////////////////
bool cScriptTestMode::CommentTest(const char* str)
{
	ASSERT(str);

	list<string>::iterator itr;
	for (itr=m_listCommentStr.begin(); itr!=m_listCommentStr.end(); ++itr)
	{
		string& comment = (*itr);

		if (strlen(str) < comment.length())
			continue;

		size_t i;
		bool bComment = false;
		for (i=0; i<comment.length(); ++i)
		{
			if (comment[i] != str[i])
			{
				bComment = false;
				break;
			}
			bComment = true;
		}

		if (bComment)
			return true;
	}

	return false;
}
bool cScriptTestMode::CommentTest(const CStringArray& strArr)
{
	ASSERT(strArr.GetSize() > 0);

	const char* str = strArr.GetAt(0);

	list<string>::iterator itr;
	for (itr=m_listCommentStr.begin(); itr!=m_listCommentStr.end(); ++itr)
	{
		string& comment = (*itr);

		if (strlen(strArr[0]) < comment.length())
			continue;

		size_t i;
		bool bComment = false;
		for (i=0; i<comment.length(); ++i)
		{
			if (comment[i] != str[i])
			{
				bComment = false;
				break;
			}
			bComment = true;
		}

		if (bComment)
			return true;
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////
bool cScriptTestMode::OperTest(const CStringArray& strArr)
{
	OperVecItr itr;

	const char* token;
	CString errStr;

	token = strArr.GetAt(0);

	cScriptOper* findOper = NULL;

	if (m_curLevel > 0)
	{
		// 미리 선택된 Oper
		if (m_SelOper[m_curLevel-1].childOper)
		{
			findOper = m_SelOper[m_curLevel-1].childOper->FindChildOper(token);
			
			if (!findOper)
				findOper = m_SelOper[m_curLevel-1].curOper->FindGlobalOper(token);

			if (findOper)
			{
				if (!findOper->ParamTest(strArr, &m_SelOper[m_curLevel].childOper, m_LastErrStrArr))
				{
					return false;
				}
			}
			else
			{
				// Err Can't Find Oper From Child Unit
				errStr.Format(" error : Can't Find [ %s ] Child From [ %s ] ", token, m_SelOper[m_curLevel-1].curOper->m_operName.c_str());
				m_LastErrStrArr.Add(errStr);

				return false;
			}
		}
		else
		{
			if (m_SelOper[m_curLevel-1].curOper)
				findOper = m_SelOper[m_curLevel-1].curOper->FindChildOper(token);

			if (!findOper && m_curLevel == 1)
			{
				findOper = FindGlobalOper(token);
			}

			if (findOper)
			{
				m_SelOper[m_curLevel].curOper = findOper;

				if (!findOper->ParamTest(strArr, &m_SelOper[m_curLevel].childOper, m_LastErrStrArr))
				{
					return false;
				}
			}
			else
			{
				// Err Can't Find Child Unit
				errStr.Format(" error : Can't Find [ %s ] Child Oper ", token);
				m_LastErrStrArr.Add(errStr);

				return false;
			}

			return false;
		}
	}
	else
	{
		// MainOper
		findOper = FindChildOper(token);

		if (findOper)
		{
			m_SelOper[0].curOper = findOper;
			if (!findOper->ParamTest(strArr, &(m_SelOper[0].childOper), m_LastErrStrArr))
			{
				return false;
			}
		}
		else
		{
			// err Can't Find Oper
			errStr.Format(" error : Can't Find ' %s ' Operator ", token , token);
			m_LastErrStrArr.Add(errStr);


			return false;
		}
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool cScriptTestMode::TestLine(const CStringArray& strArr)
{
	m_LastErrStrArr.RemoveAll();

	if (CommentTest(strArr))
		return true;

	if (OperTest(strArr))
		return true;


	return false;
}
//////////////////////////////////////////////////////////////////////////
bool cScriptTestMode::LoadElem(TiXmlElement* parentElem)
{
	ASSERT(parentElem);

	// comment Str 정의
	TiXmlElement* commentStrElem = parentElem->FirstChildElement("COMMENT");
	while (commentStrElem)
	{
		string buffer = commentStrElem->Attribute("name");
		if (buffer.length() > 0)
			m_listCommentStr.push_back(buffer);

		// NEXT
		commentStrElem = commentStrElem->NextSiblingElement("COMMENT");
	}

	// Load GrobalOper
	TiXmlElement* globalOperElem = parentElem->FirstChildElement("CHILDBASEOPER");
	while (globalOperElem)
	{
		string name = globalOperElem->Attribute("name");

		if (name.length()>0)
		{
			cScriptOper* oper = new cScriptOper;

			oper->LoadOperElem(globalOperElem);

			m_vecGlobalOper.push_back(oper);
		}
		else
		{
			// Unit Name Err
		}

		// NEXT
		globalOperElem = globalOperElem->NextSiblingElement("CHILDBASEOPER");
	}

	// operator Load
	TiXmlElement* operElem = parentElem->FirstChildElement("OPER");
	while (operElem)
	{
		cScriptOper* oper = new cScriptOper;

		if (!oper->LoadOperElem(operElem))
		{
			// Load Oper Err
			SAFE_DEL(oper);
		}

		m_vecOper.push_back(oper);

		// NEXT
		operElem = operElem->NextSiblingElement("OPER");
	}
	//

	return true;
}
//////////////////////////////////////////////////////////////////////////