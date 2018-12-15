// EffectDesc.cpp: implementation of the CEffectDesc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EffectDesc.h"
#include "GameResourceManager.h"

#include "EffectUnitDesc.h"
#include "Effect.h"

#include "..\MHFile.h"
#include "LightEffectUnitDesc.h"
#include "ObjectEffectUnitDesc.h"
#include "AnimationEffectUnitDesc.h"
#include "DamageEffectUnitDesc.h"
#include "CameraEffectUnitDesc.h"
#include "SoundEffectUnitDesc.h"
#include "MoveEffectUnitDesc.h"


#include "EffectTrigger.h"
#include "EffectTriggerOnUnitDesc.h"
#include "EffectTriggerOffUnitDesc.h"
#include "EffectTriggerMoveUnitDesc.h"
#include "EffectTriggerAttachUnitDesc.h"
#include "EffectTriggerDetachUnitDesc.h"
#include "EffectTriggerCameraRotateUnitDesc.h"
#include "EffectTriggerCameraAngleUnitDesc.h"
#include "EffectTriggerCameraZoomUnitDesc.h"
#include "EffectTriggerChangeCameraUnitDesc.h"
#include "EffectTriggerSetBaseMotionUnitDesc.h"
#include "EffectTriggerCameraShakeUnitDesc.h"
#include "EffectTriggerLinkUnitDesc.h"
#include "EffectTriggerFadeOutUnitDesc.h"
#include "EffectTriggerAnimateUnitDesc.h"
#include "EffectTriggerIllusionUnitDesc.h"
#include "EffectTriggerGravityMoveUnitDesc.h"


#include "..\Engine\EngineObject.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEffectDesc::CEffectDesc()
{
	m_MaxEffectUnitDesc = 0;
	m_MaxEffectTriggerDesc = 0;
	m_ppEffectUnitDescArray = NULL;
	m_ppEffectTriggerUnitDesc = NULL;
	m_EffectEndTime = 10000;
	m_bRepeat = FALSE;
	m_NextEffect = 0;
	m_OperatorAnimationTime = 0;
	// 090623 LUJ, 정보를 파일에서 읽어들였는지 여부를 나타내는 플래그
	mIsUnloaded = TRUE;
	m_EffectKind = eEffect_Max;
}

CEffectDesc::~CEffectDesc()
{
	Release();
}

void CEffectDesc::Release()
{
	if(m_ppEffectUnitDescArray)
	{
		for(DWORD n=0;n<m_MaxEffectUnitDesc;++n)
		{
			CEffectUnitDesc* unitDesc = m_ppEffectUnitDescArray[n];

			SAFE_DELETE( unitDesc );
		}

		delete [] m_ppEffectUnitDescArray;
	}
	if(m_ppEffectTriggerUnitDesc)
	{
		for(DWORD n=0;n<m_MaxEffectTriggerDesc;++n)
		{
			CEffectTriggerUnitDesc* unitDesc = m_ppEffectTriggerUnitDesc[ n ];
			SAFE_DELETE( unitDesc );
		}

		delete [] m_ppEffectTriggerUnitDesc;
	}

	m_ppEffectUnitDescArray = NULL;
	m_ppEffectTriggerUnitDesc = NULL;
}

BOOL CEffectDesc::LoadEffectDesc( eEffectDescKind EffectKind,CMHFile* pFile )
{
	m_EffectKind = EffectKind;

	if( ! pFile->IsInited() )
	{
		m_MaxEffectUnitDesc = 0;
		m_MaxEffectTriggerDesc = 0;

		return FALSE;
	}

	m_FileName = pFile->GetFileName();

	char token[ 128 ];
	DWORD TriggerNum = 0;

	//////////////////////////////////////////////////////////////////////////
	// Damage Percent 검증용
	BOOL bDamageUnit = FALSE;
	float Percent = 0;

	while(! pFile->IsEOF() )
	{
		pFile->GetString( token );

		if( std::string( "#REPEAT" ) == token )
		{
			m_bRepeat = pFile->GetBool();
		}
		else if( std::string( "#NEXTEFFECT" ) == token )
		{
			m_NextEffect = FindEffectNum( pFile->GetString() );
		}
		else if( std::string( "#EFFECTENDTIME" ) == token )
		{
			pFile->GetString( token );
			DWORD time;

			if( token[0] == 'f' || token[0] == 'F' )
			{
				if( 2 > strlen( token ) )
				{
					MessageBox(NULL,"f다음에 숫자를 붙여써주세요!!!!",NULL,NULL);//pjslocal
					return FALSE;
				}

				const float frame = (float)atof( &token[1] );
				const float tickPerFrame = GAMERESRCMNGR->m_GameDesc.TickPerFrame;
				time = ( DWORD )( tickPerFrame * frame );		// 프레임일경우 시간으로 변환
			}
			else
			{
				time = atoi( token );
			}

			m_EffectEndTime = time;
		}
		else if( std::string( "#MAXEFFECTUNIT" ) == token )
		{
			m_MaxEffectUnitDesc = pFile->GetDword();
			m_ppEffectUnitDescArray = new CEffectUnitDesc*[m_MaxEffectUnitDesc];
			ZeroMemory( m_ppEffectUnitDescArray, sizeof(CEffectUnitDesc*)*m_MaxEffectUnitDesc );
		}
		else if( std::string( "#NEWEFFECTUNIT" ) == token )
		{
			const DWORD unitnum = pFile->GetDword();

			if(m_MaxEffectUnitDesc <= unitnum)
			{
				MessageBox(NULL,"Check EffectUnit Num and MaxUnitNum!!!",0,0);
				return FALSE;
			}

			CEffectUnitDesc*& desc = m_ppEffectUnitDescArray[ unitnum ];

			if( desc )
			{
				std::string message = std::string( m_FileName ) + ": " + std::string( token ) + ": unitnum이 중복되었습니다. 중복된 효과는 무시됩니다";
				MessageBox( 0, message.c_str(), 0, 0 );
				continue;
			}

			pFile->GetString( token );

			if( std::string( "LIGHT" ) == token )
			{
				BOOL bDO = pFile->GetBool();
				desc = new CLightEffectUnitDesc(bDO);
				desc->ParseScript(pFile);
			}
			else if( std::string( "OBJECT" ) == token )
			{
				BOOL bDO = pFile->GetBool();
				desc = new CObjectEffectUnitDesc(bDO);
				desc->ParseScript(pFile);
			}
			else if( std::string( "ANIMATION" ) == token )
			{
				BOOL bDO = pFile->GetBool();
				desc = new CAnimationEffectUnitDesc(bDO);
				desc->ParseScript(pFile);
			}
			else if( std::string( "DAMAGE" ) == token )
			{
				BOOL bDO = pFile->GetBool();
				desc = new CDamageEffectUnitDesc(bDO);
				desc->ParseScript(pFile);
			}
			else if( std::string( "CAMERA" ) == token )
			{
				BOOL bDO = pFile->GetBool();
				desc = new CCameraEffectUnitDesc(bDO, eCameraEffectUnitType_AngleDist);
				desc->ParseScript(pFile);
			}
			else if( std::string( "CAMERAFILTER" ) == token )
			{
				BOOL bDO = pFile->GetBool();
				desc = new CCameraEffectUnitDesc(bDO, eCameraEffectUnitType_Filter);
				desc->ParseScript(pFile);
			}
			else if( std::string( "SOUND" ) == token )
			{
				BOOL bDO = pFile->GetBool();
				desc = new CSoundEffectUnitDesc(bDO);
				desc->ParseScript(pFile);
			}
			else if( std::string( "MOVE" ) == token )
			{
				BOOL bDO = pFile->GetBool();
				desc = new CMoveEffectUnitDesc(bDO);
				desc->ParseScript(pFile);
			}
			else
			{
				// 웅주, 현재 스크립트에 사용되지 않는 키워드가 상당히 많이 있음... MOVE, SOUNDNUM 등...
				//assert( 0 && "not defined keyword" );
			}
		}
		else if( std::string( "#MAXTRIGGER" ) == token )
		{
			m_MaxEffectTriggerDesc		= pFile->GetDword();
			m_ppEffectTriggerUnitDesc	= new CEffectTriggerUnitDesc*[m_MaxEffectTriggerDesc];
			ZeroMemory( m_ppEffectTriggerUnitDesc, sizeof(CEffectTriggerUnitDesc*)*m_MaxEffectTriggerDesc );
		}
		else if( std::string( "#TRIGGER" ) == token )
		{
			pFile->GetString( token );

			DWORD time;

			if(token[0] == 'f' || token[0] == 'F')
			{
				const float frame			= (float)atof(&token[1]);
				const float tickPerFrame	= GAMERESRCMNGR->m_GameDesc.TickPerFrame;
				time = (DWORD)( tickPerFrame * frame );		// 프레임일경우 시간으로 변환
			}
			else
			{
				time = atoi( token );
			}

			if(m_MaxEffectTriggerDesc <= TriggerNum)
			{
				MessageBox(NULL,"Check TriggerNum and MaxTriggerNum!!!",0,0);
				return FALSE;
			}

			CEffectTriggerUnitDesc*& desc = m_ppEffectTriggerUnitDesc[ TriggerNum ];

			if( desc )
			{
				std::string message = std::string( m_FileName ) + ": " + std::string( token ) + ": TriggerNum이 중복되었습니다. 중복된 효과는 무시됩니다";
				MessageBox( 0, message.c_str(), 0, 0 );
				return FALSE;
			}

			const DWORD unitnum = pFile->GetDword();
			pFile->GetString( token );

			if( std::string( "ON") == token )
			{
				desc = new CEffectTriggerOnUnitDesc(time,unitnum);

#ifdef _TESTCLIENT_
				if(	m_MaxEffectUnitDesc > unitnum &&
					m_ppEffectUnitDescArray[unitnum]->m_EffectUnitType == eEffectUnit_Damage )
				{
					CDamageEffectUnitDesc* unitDesc = ( CDamageEffectUnitDesc* )( m_ppEffectUnitDescArray[unitnum] );
					assert( unitDesc );

					Percent += unitDesc->GetDamageRate();
					bDamageUnit = TRUE;
				}
#endif
			}
			else if( std::string( "OFF") == token )
			{
				desc = new CEffectTriggerOffUnitDesc(time,unitnum);
			}
			else if( std::string( "MOVE") == token )
			{
				desc = new CEffectTriggerMoveUnitDesc(time,unitnum);
			}
			else if( std::string( "GRAVITYMOVE" ) == token )
			{
				desc = new CEffectTriggerGravityMoveUnitDesc(time,unitnum);
			}
			else if( std::string( "ATTACH" ) == token )
			{
				desc = new CEffectTriggerAttachUnitDesc(time,unitnum);
			}
			else if( std::string( "DETACH" ) == token )
			{
				desc = new CEffectTriggerDetachUnitDesc(time,unitnum);
			}
			else if( std::string( "CAMERAROTATE" ) == token )
			{
				desc = new CEffectTriggerCameraRotateUnitDesc(time,unitnum);
			}
			else if( std::string( "CAMERAZOOM" ) == token )
			{
				desc = new CEffectTriggerCameraZoomUnitDesc(time,unitnum);
			}
			else if( std::string( "CAMERAANGLE" ) == token )
			{
				desc = new CEffectTriggerCameraAngleUnitDesc(time,unitnum);
			}
			else if( std::string( "CAMERACHANGE" ) == token )
			{
				desc = new CEffectTriggerChangeCameraUnitDesc(time,unitnum);
			}
			else if( std::string( "SETBASEMOTION" ) == token )
			{
				desc = new CEffectTriggerSetBaseMotionUnitDesc(time,unitnum);
			}
			else if( std::string( "CAMERASHAKE" ) == token )
			{
				desc = new CEffectTriggerCameraShakeUnitDesc(time,unitnum);
			}
			else if( std::string( "LINK" ) == token )
			{
				desc = new CEffectTriggerLinkUnitDesc(time,unitnum);
			}
			else if( std::string( "FADEOUT" ) == token )
			{
				desc = new CEffectTriggerFadeOutUnitDesc(time,unitnum);
			}
			else if( std::string( "FADEIN" ) == token )
			{
				desc = new CEffectTriggerFadeOutUnitDesc(time,unitnum);
			}
			else if( std::string( "ANIMATE" ) == token )
			{
				desc = new CEffectTriggerAnimateUnitDesc(time,unitnum);
//				desc->ParseScript( pFile );
			}
			else if( std::string( "ILLUSION" ) == token )
			{
				desc = new CEffectTriggerIllusionUnitDesc(time,unitnum);

			}
			else
			{
				//assert( 0 && "not defined keyword" );
			}

			if( desc )
			{
				desc->ParseScript( pFile );
				++TriggerNum;
			}
		}
	}

	if(m_MaxEffectTriggerDesc != TriggerNum)
	{
		MessageBox(NULL,"MaxTriggerNum is too BIG!!!",0,0);
		return FALSE;
	}

	if(bDamageUnit && (0.99 > Percent || Percent > 1.01))
	{
#ifdef _TESTCLIENT_
		MessageBox(NULL,"데미지 Percent의 합이 100이 아닙니다!!!",0,0);
		return FALSE;
#endif
	}

	return TRUE;
}

BOOL CEffectDesc::GetEffect(EFFECTPARAM* pParam,CEffect* pOutEffect)
{
	// 090623 LUJ, 속도 향상을 위해 필요 시에 읽어들인다
	if( mIsUnloaded )
	{
		Release();

		CMHFile file;
		file.Init( m_FileName, "rb" );
		LoadEffectDesc( m_EffectKind, &file );
		mIsUnloaded = FALSE;
	}

	pOutEffect->Init(	m_MaxEffectUnitDesc,pParam,
						this,
						m_MaxEffectTriggerDesc,
						m_ppEffectTriggerUnitDesc );

	for(DWORD n = 0; n < m_MaxEffectUnitDesc; ++n )
	{
		CEffectUnitDesc* effectUnitDesc = m_ppEffectUnitDescArray[ n ];

		if( ! effectUnitDesc )
		{			
			// 이 오류는 이펙트 스크립트의 #MAXEFFECTUNIT가 정의된 이펙트 유닛보다 클 경우 발생한다
			// 심각한 오류는 아니지만 쓰지 않는 메모리를 할당하기 때문에 제거하는 것이 바람직하다.
			// 어떤 스크립트에서 그런 문제가 있는지 알려면 다음과 같이 하자.
			//
			// 이 함수의 피호출자를 보면 이펙트 번호(코드에서는 EffectDescNum)가 설정되어 있다.
			// 이 번호로 LIST_*.befl 중 하나를 열고(종족, 성별로 다른 이펙트가 정의되어 있다)
			// 번호로 어떤 이펙트 파일을 참조하는지 확인하자. 그 파일을 열어 #NEWEFFECTUNIT으로 
			// 생성된 이펙트 개수가 #MAXEFFECTUNIT와 동일한지 확인하고 아니면 맞게 고쳐주자.
			assert( 0 && "effectUnitDesc is null. MaxEffectUnitDesc is wrong. See this effect script." );
			return FALSE;
		}

		CEffectUnit* pEffectUnit = effectUnitDesc->GetEffectUnit(pParam);

		//////////////////////////////////////////////////////////////////////////
		// virtual 바인딩 시점 문제 때문에 CEffectUnit의 생성자에서 여기로 옮김 [4/29/2003]
		if( effectUnitDesc->IsDangledToOperator() )
		{
			TARGETSET Targetset;
			Targetset.pTarget = pParam->m_pOperator;
			pEffectUnit->InitEffect(1,&Targetset);
		}
		else if( pParam->m_nTargetNum )
		{
			pEffectUnit->InitEffect(pParam->m_nTargetNum,pParam->m_pTargetSet);
		}
		
		pOutEffect->SetEffectUnit(n, pEffectUnit);
	}

	return TRUE;
}

DWORD CEffectDesc::GetOperatorAnimatioEndTime(CEngineObject* pEngineObject)
{	
	if( m_OperatorAnimationTime )
	{
		return m_OperatorAnimationTime;
	}

	for(DWORD n = 0; n < m_MaxEffectUnitDesc; ++n )
	{
		CEffectUnitDesc* effectUnitDesc = m_ppEffectUnitDescArray[n];

		if(		effectUnitDesc->m_EffectUnitType != eEffectUnit_Animation ||
			!	effectUnitDesc->IsDangledToOperator() )
		{
			continue;
		}

		CAnimationEffectUnitDesc* pDesc = (CAnimationEffectUnitDesc*)effectUnitDesc;

		const DWORD AniTime = pEngineObject->GetAnimationTime(pDesc->GetMotionNum());

		for(DWORD trig=0;trig<m_MaxEffectTriggerDesc;++trig)
		{
			CEffectTriggerUnitDesc* desc = m_ppEffectTriggerUnitDesc[trig];

			if( desc &&
				desc->GetUnitNum() == n )
			{
				const DWORD AniStartTime = desc->GetStartTime();

				if(AniStartTime + AniTime >= m_OperatorAnimationTime)
				{
					m_OperatorAnimationTime = AniStartTime + AniTime;
					break;
				}
			}
		}
	}

	return m_OperatorAnimationTime;
}

// 090623 LUJ, 이펙트 종류와 정보를 읽을 파일 이름을 저장한다
void CEffectDesc::SetData( LPCTSTR fileName, eEffectDescKind kind )
{
	m_EffectKind = kind;
	m_FileName = fileName;
}