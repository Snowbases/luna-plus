#pragma once
//-------------------------------------------------------------------------------------------------
//	NAME		: AppearPlayer.h
//	DESC		: 캐릭터의 모습 처리를 관리하는 클래스.
//	PROGRAMMER	: Yongs Lee
//	DATE		: February 05, 2009
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
//		PREPROCESSOR PART.
//-------------------------------------------------------------------------------------------------
enum GENDER_TYPE
{
	eMAN	= 0,
	eWOMAN,
} ;

// iros --- 어떤 FLAG를 넘겨줘야 할까 ;;;
enum CHANGE_APPEAR_FLAG
{
	eCHAPPEAR_NONE = 0x00,
	eCHAPPEAR_MOD = 0x01,
	eCHAPPEAR_PART  = 0x02,
	eCHAPPEAR_WEAPON  = 0x04,
	eCHAPPEAR_SCALE  = 0x08,
	eCHAPPEAR_EFFECT  = 0x10,
	eCHAPPEAR_OPTION  = 0x20,
	eCHAPPEAR_INIT = 0xFF,
};

//-------------------------------------------------------------------------------------------------
//		INCLUDE PART.
//-------------------------------------------------------------------------------------------------
class CEngineObject ;
class CPlayer ;

struct stFLAG_SHOWPART
{
	BYTE IsShowFace ;
	BYTE IsShowHelmet ;
	BYTE IsShowGlasses ;
	BYTE IsShowMask ;
} ;





//-------------------------------------------------------------------------------------------------
//		THE CLASS CAPPEARPLAYER
//-------------------------------------------------------------------------------------------------
class CAppearPlayer
{
private :
	stFLAG_SHOWPART			m_stShowFlag ;

	BOOL					m_IsForEffect ;

	CPlayer*				m_pPlayer ;
	CEngineObject*			m_pEngineObject ;

	CHARACTER_TOTALINFO*	m_pTotalInfo ;

private :
	void InitData() ;
	void InitShowFlag() ;

	BYTE InitMOD() ;
	BYTE InitHead() ;
	BYTE ChangePart() ;
	BYTE AttachPart() ;
	BYTE InitScale() ;
	BYTE InitPos() ;
	BYTE AppearWeapone() ;

	// 2009.02.20 nsoh 코스튬 모자 착용시 미표시 처리
	BYTE InitHair();

public:
	CAppearPlayer(void) ;
	~CAppearPlayer(void) ;

	void SetForEffect(BOOL isForEffect)	;
	void SetPlayer( CPlayer* pPlayer ) ;
	void SetEngineObject( CEngineObject* pEngineObject)	;

	void SetBaseAppearPlayer();
	void RenewalAll();
	void Renewal(DWORD changeFlag);
	//void Renewal();

	// 090530 ONS 신규종족 무기아이템에서 2개의 MOD파일명을 얻는 함수
	BOOL GetModFromChx(char* pFilename, char* RObj, char* LObj);
};
