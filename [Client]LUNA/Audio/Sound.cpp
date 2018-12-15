// Sound.cpp: implementation of the CSound class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Sound.h"
#include "SoundItem.h"
#include "ObjectManager.h"

extern IMilesSoundLib* g_pSoundLib;

CSound::CSound(const CSoundItem& pSItem, DWORD objectIndex)
{
	SOUND_ERROR_CODE herr = g_pSoundLib->CreateSoundEffect(&m_pSoundEffect,pSItem.m_hSoundFile);
	if(herr != SOUND_ERROR_NOERROR)
	{
		// »ç¿îµå »ý¼º ½ÇÆÐ
		//ASSERT(0);
		m_pSoundEffect = NULL;
	}

	m_bLoop = pSItem.m_bLoop;
	mObjectIndex = objectIndex;
	m_MaxDist = pSItem.m_MaxDist;
	m_MinDist = pSItem.m_MinDist;	
	m_EachSoundVolume = pSItem.m_fVolume;	
}

CSound::~CSound()
{
	if(m_pSoundEffect)
		m_pSoundEffect->Release();
}

void CSound::Play()
{
	//ASSERT(m_pSoundEffect);
	if(m_pSoundEffect == NULL)
		return;

	m_pSoundEffect->Play(m_bLoop);	
	m_pSoundEffect->SetMaxMinDistance(m_MaxDist,m_MinDist);

	if(CObject* const object = OBJECTMGR->GetObject(mObjectIndex))
	{
		VECTOR3 pos = {0};
		object->GetPosition(&pos);
		m_pSoundEffect->SetPosition(&pos) ;
	}
}
BOOL CSound::IsPlaying()
{
	//ASSERT(m_pSoundEffect);
	if(m_pSoundEffect)
		return m_pSoundEffect->IsAlive();
	
	return FALSE;
}
void CSound::Stop()
{
	//ASSERT(m_pSoundEffect);
	if(m_pSoundEffect)
		m_pSoundEffect->Stop();
}
void CSound::SetVolume(float fVolume)
{
	//ASSERT(m_pSoundEffect);
	if(m_pSoundEffect)
		m_pSoundEffect->SetVolume(fVolume);
}

BOOL CSound::Process()
{
	if(m_pSoundEffect == NULL)
		return TRUE;
	if(m_pSoundEffect->IsAlive() == FALSE)
		return TRUE;

	return FALSE;
}