#include "StdAfx.h"
#include "conductinfo.h"
#include "MHFile.h"

cConductInfo::cConductInfo(void)
{
}

cConductInfo::~cConductInfo(void)
{}

BOOL cConductInfo::InitConductInfo( CMHFile* pFile )
{
	ASSERT(pFile->IsInited());
	ASSERT(pFile->IsEOF() == FALSE);

	while( !pFile->IsEOF() )
	{
		char buf[MAX_PATH] = {0,};
		pFile->GetString( buf );
		if( buf[0] == '/' && buf[1] == '/' )
		{
			pFile->GetLineX( buf, MAX_PATH );
			continue;
		}
		
		m_pConductInfo.ConductIdx = (WORD)atoi( buf );
		break;
	}

	

	SafeStrCpy(
		m_pConductInfo.ConductName,
		pFile->GetString(),
		_countof(m_pConductInfo.ConductName));	
	m_pConductInfo.ConductTootipIdx = pFile->GetWord();
	m_pConductInfo.ConductKind = pFile->GetWord();
	m_pConductInfo.ConductPos = pFile->GetWord();
	m_pConductInfo.HighImage = pFile->GetInt();
	m_pConductInfo.MotionIdx = pFile->GetInt();
	m_pConductInfo.bInvalidOnVehicle = pFile->GetBool();

	return TRUE;
}

char* cConductInfo::GetConductName()
{
	return m_pConductInfo.ConductName ;
}

WORD cConductInfo::GetConductIdx()
{
	return m_pConductInfo.ConductIdx ;
}

WORD cConductInfo::GetConductTooltipIdx()
{
	return m_pConductInfo.ConductTootipIdx ;
}

WORD cConductInfo::GetConductKind()
{
	return m_pConductInfo.ConductKind ;
}

WORD cConductInfo::GetConductPos()
{
	return m_pConductInfo.ConductPos ;
}

int cConductInfo::GetHighImage()
{
	return m_pConductInfo.HighImage ;
}

int cConductInfo::GetMotionIdx()
{
	return m_pConductInfo.MotionIdx ;
}

BOOL cConductInfo::IsInvalidOnVehicle() const
{
	return m_pConductInfo.bInvalidOnVehicle;
}