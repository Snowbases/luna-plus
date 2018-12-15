// MHAudioManager.cpp: implementation of the CMHAudioManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MHAudioManager.h"
#include "SoundItem.h"
#include "Sound.h"
#include "MHFile.h"

extern BOOL g_bActiveApp;

IMilesSoundLib*		g_pSoundLib;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
GLOBALTON(CMHAudioManager);
CMHAudioManager::CMHAudioManager()
{
	g_pSoundLib	= NULL;
	m_SoundTable.Initialize(32);

	m_MasterVolume = 1;
	m_BGMVolume = 1;
	m_SoundVolume = 1;

	m_idxLastBGM = 0;

	m_bIsBGMPlaying = FALSE;
	m_bSmoothChangeBGM = FALSE;
	m_ChangeBGMIdx = WORD(-1);
	m_bChangedBGM = FALSE;
	m_fCurChangeBGMVolume = 0.0f;
	m_fSavedBGMVolume = 0.0f;
}

CMHAudioManager::~CMHAudioManager()
{
	
}


BOOL CMHAudioManager::Initialize( HWND hWnd )
{
	m_bIsBGMPlaying = FALSE;

	// 사운드 라이브러리 생성.
	BOOL	bCreated	=	CreateSoundLib( &g_pSoundLib);
	if( bCreated == FALSE)
	{
//		_asm nop;
		return FALSE;
	}
	
	// 초기화. 파일 최대 갯수 100개, 소리 객체 200개, eax 시도.
	SOUND_ERROR_CODE	sResult	=	g_pSoundLib->Init( 500, 500, "system/miles", FALSE, TRUE);
	
	g_pSoundLib->Set3DSpeakerType(SOUND_SPEAKER_TYPE_STEREO);

//	JSD - 일단 두가지 경우만 체크함
//	마일즈 초기화에 실패하면 SOUND_ERROR_MILES_STARTUP_FAILED,
//	BGM드라이버를 찾지 못할 경우, 사운드카드가 없다면 SOUND_ERROR_BGMDRIVER_STARTUP_FAILED,
	if( sResult == SOUND_ERROR_BGMDRIVER_STARTUP_FAILED ||	
		sResult == SOUND_ERROR_MILES_STARTUP_FAILED )
	{
//		Terminate();
		m_bIsBGMPlaying = FALSE;
		g_pSoundLib = NULL;
		return FALSE;
	}
//
	if( sResult != SOUND_ERROR_NOERROR )
	{
//		_asm nop;
		Terminate();
		return FALSE;
	}
	
	LoadList();

	return TRUE;
}

void CMHAudioManager::LoadList()
{
	CMHFile file;
	file.Init(
		"Data/Sound/SoundList.bin",
		"rb");

	while(FALSE == file.IsEOF())
	{
		TCHAR text[MAX_PATH] = {0};
		file.GetLine(
			text,
			_countof(text));
		LPCTSTR textSeperator = "\t";
		LPCTSTR textIndex = _tcstok(
			text,
			textSeperator);
		LPCTSTR textFileName = _tcstok(
			0,
			textSeperator);
		LPCTSTR textIsLoop = _tcstok(
			0,
			textSeperator);
		LPCTSTR textIsStreaming = _tcstok(
			0,
			textSeperator);
		LPCTSTR textMinimumDistance = _tcstok(
			0,
			textSeperator);
		LPCTSTR textMaximumDistance = _tcstok(
			0,
			textSeperator);
		LPCTSTR textVolume = _tcstok(
			0,
			textSeperator);

		if(0 == textFileName)
		{
			continue;
		}

		TCHAR drive[_MAX_DRIVE] = {0};
		TCHAR directory[_MAX_DIR] = {0};
		TCHAR fileName[_MAX_FNAME] = {0};
		TCHAR extension[_MAX_EXT] = {0};

		_tsplitpath(
			textFileName,
			drive,
			directory,
			fileName,
			extension);

		if(0 == _tcsicmp("null", fileName))
		{
			continue;
		}

		TCHAR soundFile[MAX_PATH] = {0};
		_sntprintf(
			soundFile,
			_countof(soundFile),
			"Data/Sound/%s",
			textFileName);

		FILE* const fp = fopen(
			soundFile,
			"rb");

		if(0 == fp)
		{
			_tcprintf(
				"SoundList.bin: %s could not open\n",
				soundFile);
			continue;
		}

		fclose(
			fp);

		CSoundItem& soundItem = mSoundItemContainer[SNDIDX(_ttoi(textIndex ? textIndex : ""))];
		soundItem.Init(
			soundFile,
			_ttoi(textIsLoop ? textIsLoop : ""),
			_ttoi(textIsStreaming ? textIsStreaming : ""),
			float(_tstof(textMinimumDistance ? textMinimumDistance : "")),
			float(_tstof(textMaximumDistance ? textMaximumDistance : "")),
			float(_tstof(textVolume ? textVolume : "")));
	}
}

void CMHAudioManager::Terminate()
{
	m_bIsBGMPlaying = FALSE;

	m_SoundTable.SetPositionHead();

	while(CSound* sound = m_SoundTable.GetData())
	{
		sound->Stop();
		SAFE_DELETE(
			sound);
	}

	m_SoundTable.RemoveAll();
	mSoundItemContainer.clear();

	if( g_pSoundLib )
	{
		g_pSoundLib->Delete();
		g_pSoundLib = NULL;
	}
}

CSound*	CMHAudioManager::Play(SNDIDX idx, DWORD objectIndex)
{
	if(FALSE == m_bEffectSoundOn)
	{
		return 0;
	}
	else if(0 == g_pSoundLib)
	{
		return 0;
	}
	else if(0 == m_SoundVolume)
	{
		return 0;
	}
	else if(mSoundItemContainer.end() == mSoundItemContainer.find(idx))
	{
		return 0;
	}

	CSound* const pSound = mSoundItemContainer[idx].CreateSound(
		objectIndex);

	if(pSound == 0)
		return 0;
	
	pSound->Play();
	pSound->SetVolume(GetSoundVolume(pSound->GetEachSoundVolume()));

	m_SoundTable.Add(pSound,(DWORD)pSound);
	return pSound;
}
void	CMHAudioManager::Stop( CSound* pSound )
{
	if(g_pSoundLib == NULL)
		return;

	if(CSound* const sound = m_SoundTable.GetData(DWORD(pSound)))
	{
		pSound->Stop();
		m_SoundTable.Remove(
			DWORD(pSound));
		delete sound;
	}
}

void	CMHAudioManager::SetVolume( CSound* pSound, float val )
{
	if(g_pSoundLib == NULL)
		return;

	ASSERT(pSound);
	if( pSound )
		pSound->SetVolume(val);
}

void	CMHAudioManager::PlayBGM( SNDIDX idx )
{
	if(m_idxLastBGM == idx)
	{
		return;
	}

	m_idxLastBGM = idx;

	if( idx == 0 )			return;
	if( !g_bActiveApp )		return;
	if( !m_bBGMSoundOn )	return;
	if( !g_pSoundLib )		return;

	m_bIsBGMPlaying = TRUE;
	g_pSoundLib->PlayBGM(
		LPTSTR(GetFileName(idx)));
	g_pSoundLib->SetBGMVolume(
		GetBGMVolume());
}
void	CMHAudioManager::MuteBGM( BOOL val )
{
	if(g_pSoundLib == NULL)
		return;

	if( val )
	{
		g_pSoundLib->SetBGMVolume(0);
		mLastBGMVolume = GetBGMVolume();
	}
	else
	{
		g_pSoundLib->SetBGMVolume(mLastBGMVolume);
	}
}

void	CMHAudioManager::StopBGM()
{
	if(g_pSoundLib == NULL)
		return;

	m_bIsBGMPlaying = FALSE;
	m_idxLastBGM = 0;
	g_pSoundLib->StopBGM();
}

LPCTSTR CMHAudioManager::GetFileName(SNDIDX idx) const
{
	SoundItemContainer::const_iterator iterator = mSoundItemContainer.find(
		idx);

	if(mSoundItemContainer.end() == iterator)
	{
		return "";
	}

	const CSoundItem& soundItem = iterator->second;

	return soundItem.GetFileName();
}

void CMHAudioManager::Process()
{
	if(g_pSoundLib == NULL)
		return;

	// 091014 ShinJS --- BGM 부드럽게 변경하기
	if( m_bSmoothChangeBGM )
	{
		static DWORD dwLastProcTime;

		// 0.2초마다 검사
        if( gCurTime - dwLastProcTime > 200 )
		{
			// BGM이 변경된 경우
			if( m_bChangedBGM )
			{
				// BGM Volume 10%씩 증가
				m_fCurChangeBGMVolume += 10.0f;

				// BGM 변경 완료
				if( m_fCurChangeBGMVolume >= 100.0f )
				{
					m_fCurChangeBGMVolume = 100.0f;
					m_bSmoothChangeBGM = FALSE;
				}
			}
			// 아직 변경되지 않은 경우
			else
			{
				// BGM Volume 5%씩 감소
				m_fCurChangeBGMVolume -= 5.0f;

				// BGM 변경
				if( m_fCurChangeBGMVolume <= 0.0f )
				{
					m_fCurChangeBGMVolume = 0.0f;
					m_bChangedBGM = TRUE;
					PlayBGM( m_ChangeBGMIdx );
					m_ChangeBGMIdx = WORD(-1);
				}
			}

			float fVol = m_fSavedBGMVolume * m_fCurChangeBGMVolume * 0.01f;
			SetBGMVolume( fVol );

			dwLastProcTime = gCurTime;
		}
	}

	m_SoundTable.SetPositionHead();

	for(CSound* sound = m_SoundTable.GetData();
		0 < sound;
		sound = m_SoundTable.GetData())
	{
		if(FALSE == sound->Process())
		{
			continue;
		}

		sound->Stop();
		m_SoundTable.Remove(
			DWORD(sound));
		SAFE_DELETE(
			sound);
	}
}
void CMHAudioManager::StopAll()
{
	m_SoundTable.SetPositionHead();

	for(CSound* sound = m_SoundTable.GetData();
		0 < sound;
		sound = m_SoundTable.GetData())
	{
		sound->Stop();
		SAFE_DELETE(
			sound);
	}

	m_SoundTable.RemoveAll();
}
void CMHAudioManager::SetListenerPosition(VECTOR3* pPos,float fAngleRad)
{	
	if(g_pSoundLib == NULL)
		return;

	VECTOR3 odir,dir;
	odir.x = 0;
	odir.y = 0;
	odir.z = -1;
	TransToRelatedCoordinate(&dir,&odir,fAngleRad);//-gPHI/2);
	g_pSoundLib->SetListener(pPos,&dir);
	/*
	VECTOR3 odir,dir;
	odir.x = 0;
	odir.y = 0;
	odir.z = -1;
	TransToRelatedCoordinate(&dir,&odir,fAngleRad);
	g_pSoundLib->SetListener(pPos,&dir);
	*/
}

void CMHAudioManager::SetMasterVolume(float fVal)
{
	if(fVal < 0)
		m_MasterVolume = 0;
	else if(fVal > 1)
		m_MasterVolume = 1;
	else
		m_MasterVolume = fVal;

	if(g_pSoundLib == NULL)
		return;
	
	ApplyVolumeChange();
}
void CMHAudioManager::SetBGMVolume(float fVal)
{
	if(fVal < 0)
		m_BGMVolume = 0;
	else if(fVal > 1)
		m_BGMVolume = 1;
	else
		m_BGMVolume = fVal;

	if(g_pSoundLib == NULL)
		return;

	ApplyBGMVolumeChange();
}
void CMHAudioManager::SetSoundVolume(float fVal)
{
	if(fVal < 0)
		m_SoundVolume = 0;
	else if(fVal > 1)
		m_SoundVolume = 1;
	else
		m_SoundVolume = fVal;

	if(g_pSoundLib == NULL)
		return;	

	ApplyVolumeChange();
}

void CMHAudioManager::ApplyVolumeChange()
{
	if(0 == g_pSoundLib)
	{
		return;
	}

	m_SoundTable.SetPositionHead();

	for(CSound* sound = m_SoundTable.GetData();
		0 < sound;
		sound = m_SoundTable.GetData())
	{
		sound->SetVolume(
			GetSoundVolume(sound->GetEachSoundVolume()));
	}
}


void CMHAudioManager::ApplyBGMVolumeChange()
{
	if(g_pSoundLib && m_bIsBGMPlaying)
	{		
		g_pSoundLib->SetBGMVolume(GetBGMVolume());
	}
}



float CMHAudioManager::GetBGMVolume()
{
	if(g_pSoundLib == NULL)
		return 0.0f;
	return m_MasterVolume * m_BGMVolume;	
}
float CMHAudioManager::GetSoundVolume(float fEachSoundVolume)
{
	if(g_pSoundLib == NULL)
		return 0.0f;
	return m_MasterVolume * m_SoundVolume * fEachSoundVolume;
}

//KES
void	CMHAudioManager::PlayLastBGM()
{
	if( !m_bBGMSoundOn )
		return;

	if(g_pSoundLib == NULL)
		return;

	m_bIsBGMPlaying = TRUE;

	g_pSoundLib->PlayBGM(
		LPTSTR(GetFileName(m_idxLastBGM)));
	g_pSoundLib->SetBGMVolume(
		GetBGMVolume());
}

void CMHAudioManager::SmoothChangeBGM( SNDIDX idx )
{
	// BGM이 같은 경우 변경하지 않는다.
	if( m_ChangeBGMIdx == idx )
	{
		return;
	}
	else if( !m_bSmoothChangeBGM )
	{
		m_fCurChangeBGMVolume = 100.0f;
		m_fSavedBGMVolume = GetBGMVolume();
	}

	m_bSmoothChangeBGM = TRUE;
	m_bChangedBGM = FALSE;
	m_ChangeBGMIdx = idx;
}