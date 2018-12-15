// ScriptTest.cpp - iros
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ScriptTest.h"

cScriptTest g_ScriptTest;


//////////////////////////////////////////////////////////////////////////
// cScriptTest

cScriptTest::cScriptTest()
{
	m_bLoad = false;
}

cScriptTest::~cScriptTest()
{
	ScriptTestModeVecItr itr;
	for (itr=m_vecScript.begin(); itr!=m_vecScript.end(); ++itr)
		SAFE_DEL(*itr);

	m_bLoad = false;
}

cScriptTestMode* cScriptTest::GetScriptTestMode(UINT mode)
{
	if (mode >= m_vecScript.size())
		return NULL;

	return m_vecScript[mode];
}

bool cScriptTest::LoadXml(const char* filePath)
{
	TiXmlDocument doc(filePath);

	bool loadOkay = doc.LoadFile();

	if ( !loadOkay )
	{
		CString str;
		str.Format( _T("Could not load test file '%s'. Error='%s'. Exiting.\n"), filePath, doc.ErrorDesc() );

		AfxMessageBox(str);
	}

	UINT codepage = GetACP();

	TiXmlDeclaration* decl = doc.FirstChild()->ToDeclaration();
	const char* enc = decl->Encoding();

	if (enc)
	{
		if (0 == *enc || 0 == _stricmp(enc, "UTF-8") || 0 == _stricmp(enc, "UTF8"))
			codepage = CP_UTF8;
	}

	TiXmlElement* rootElem = doc.FirstChildElement("TestScript");
	if (NULL == rootElem)
		return false;

	float version;
	rootElem->QueryFloatAttribute("VERSION", &version);

	string name;
	TiXmlElement* modeElem = rootElem->FirstChildElement();
	while (modeElem)
	{
		name = modeElem->Value();
		if (name.length() > 0)
		{
			cScriptTestMode* mode = new cScriptTestMode;

			mode->m_modeName = name;
			mode->LoadElem(modeElem);

			m_vecScript.push_back(mode);
		}

		// NEXT
		modeElem = modeElem->NextSiblingElement();
	}

	return (m_bLoad = true);
}