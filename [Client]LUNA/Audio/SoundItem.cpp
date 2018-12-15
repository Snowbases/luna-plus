// SoundItem.cpp: implementation of the CSoundItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SoundItem.h"

extern IMilesSoundLib*		g_pSoundLib;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSoundItem::CSoundItem()
{
	m_hSoundFile	= NULL;
	m_MinDist		= 10;
	m_MaxDist		= 100;
	m_fVolume		= 1;
}

CSoundItem::~CSoundItem()
{
	if(m_hSoundFile)
	{
		g_pSoundLib->ReleaseSoundEffectFile(m_hSoundFile);
		m_hSoundFile = NULL;
	}
}

void CSoundItem::Init(LPCTSTR fileName, BOOL bLoop, BOOL bStreaming, float MinDist, float MaxDist, float fVolume)
{
	_sntprintf(
		m_szFileName,
		_countof(m_szFileName),
		"%s",
		fileName);

	m_bLoop = bLoop;
	m_bStream = bStreaming;

	m_MinDist = MinDist;
	m_MaxDist = MaxDist;
	m_fVolume = fVolume;
	if(m_fVolume > 0)
		m_fVolume = 1;
	if(m_fVolume < 0)
		m_fVolume = 0;
}

CSound* CSoundItem::CreateSound(DWORD objectIndex)
{
	if(((LPCTSTR)m_szFileName) == NULL)
	{
//		ASSERTMSG(0,"Null.wav Referenced");
		return NULL;
	}

	if(m_hSoundFile == NULL)
	{
		SOUND_ERROR_CODE herr = g_pSoundLib->CreateSoundEffectFile(&m_hSoundFile,m_szFileName);
		if(herr != SOUND_ERROR_NOERROR)
		{
//			_asm int 3;
			return NULL;
		}
	}

	return new CSound(
		*this,
		objectIndex);
}