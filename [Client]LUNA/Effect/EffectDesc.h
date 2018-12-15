// EffectDesc.h: interface for the CEffectDesc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EFFECTDESC_H__CF7C295E_CC07_4B6B_9FB0_7F7B75076E3E__INCLUDED_)
#define AFX_EFFECTDESC_H__CF7C295E_CC07_4B6B_9FB0_7F7B75076E3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CEffect;
class CEffectUnitDesc;
class CEffectTriggerUnitDesc;
class CMHFile;
class CEngineObject;
struct EFFECTPARAM;
enum eEffectDescKind;

class CEffectDesc  
{
	eEffectDescKind m_EffectKind;
	BOOL mIsUnloaded;
	StaticString m_FileName;

	DWORD m_MaxEffectUnitDesc;
	DWORD m_MaxEffectTriggerDesc;
	DWORD m_EffectEndTime;
	CEffectUnitDesc** m_ppEffectUnitDescArray;
	CEffectTriggerUnitDesc** m_ppEffectTriggerUnitDesc;
	
	BOOL m_bRepeat;
	DWORD m_NextEffect;
	
	DWORD m_OperatorAnimationTime;

public:
	CEffectDesc();
	virtual ~CEffectDesc();
	BOOL LoadEffectDesc( eEffectDescKind, CMHFile* );
	BOOL GetEffect( EFFECTPARAM*, CEffect* );
	DWORD GetEffectEndTime() const { return m_EffectEndTime; }
	int GetMaxEffectUnitDesc() const { return m_MaxEffectUnitDesc; }
	int GetMaxEffectTriggerDesc() const { return m_MaxEffectTriggerDesc; }
	inline eEffectDescKind GetEffectKind() const { return m_EffectKind; }
	inline LPCTSTR GetFileName() { return LPCTSTR( m_FileName ) ? m_FileName : ""; }
	// 090623 LUJ, 파일을 읽기 위한 기본 정보를 설정한다
	void SetData( LPCTSTR fileName, eEffectDescKind );
	DWORD GetOperatorAnimatioEndTime( CEngineObject* );
	BOOL IsRepeat() const { return m_bRepeat; }
	DWORD GetNextEffect() const { return m_NextEffect; }

	// 091106 ShinJS --- Beff File 재로드를 위해 추가
	void Release();
	void SetLoadFalg( BOOL bUnLoaded ) { mIsUnloaded = bUnLoaded; }
};

#endif // !defined(AFX_EFFECTDESC_H__CF7C295E_CC07_4B6B_9FB0_7F7B75076E3E__INCLUDED_)
