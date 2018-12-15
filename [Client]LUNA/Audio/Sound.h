// Sound.h: interface for the CSound class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOUND_H__A1029C72_F3F7_42D1_A178_C0AC2139F3A0__INCLUDED_)
#define AFX_SOUND_H__A1029C72_F3F7_42D1_A178_C0AC2139F3A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSoundItem;
#include "SoundLib.h"

class CSound  
{
	ISoundEffect* m_pSoundEffect;
	BOOL m_bLoop;
	DWORD mObjectIndex;
	float m_EachSoundVolume;

	float m_MaxDist;
	float m_MinDist;

public:
	CSound(const CSoundItem&, DWORD objectIndex);
	virtual ~CSound();

	void Play();
	BOOL IsPlaying();
	void Stop();
	void SetVolume(float fVolume);

	BOOL Process();

	float GetEachSoundVolume()	{	return m_EachSoundVolume;	}
};

#endif // !defined(AFX_SOUND_H__A1029C72_F3F7_42D1_A178_C0AC2139F3A0__INCLUDED_)
