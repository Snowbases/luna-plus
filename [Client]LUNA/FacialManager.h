#pragma once

#define FACIALMGR USINGTON(CFacialManager)

class CPlayer ;

class CFacialManager
{
public:
	// desc_hseos_얼굴교체01
	// S 얼굴교체 추가 added by hseos 2007.06.08
	static DWORD			PLAYER_FACE_EYE_BLINK_OPEN_TIME;						// 얼굴 눈깜빡임 시 뜨고 있는 시간
	static DWORD			PLAYER_FACE_EYE_BLINK_OPEN_ADD_RND_TIME;				// 얼굴 눈깜빡임 시 뜨고 있는 추가 랜덤 시간
	static DWORD			PLAYER_FACE_EYE_BLINK_CLOSE_TIME;						// 얼굴 눈깜빡임 시 감고 있는 시간
	static DWORD			PLAYER_FACE_EYE_BLINK_CLOSE_ADD_RND_TIME;				// 얼굴 눈깜빡임 시 감고 있는 추가 랜덤 시간
	static DWORD			PLAYER_FACE_SHAPE_TIME;									// 얼굴 모양이 표시되는 시간 ( 애니메이션의 경우 눈깜빡임 처럼 추가필요)
	// E 얼굴교체 추가 added by hseos 2007.06.08

public:
	CFacialManager(void);
	~CFacialManager(void); 


	// desc_hseos_얼굴교체01
	// S 얼굴교체 추가 added by hseos 2007.06.08
	//						스크립트 파일 로드
	BOOL					LoadScriptFileData();														
	// E 얼굴교체 추가 added by hseos 2007.06.08

	void ChangeHeroFace(BYTE faceNum) ;
	void ChangePlayerFace(CPlayer* pPlayer, BYTE faceNum) ;
	void NetworkMsgParse( BYTE Protocol, void* pMsg );

	// 070312 LYW --- FacialManager : Add function for eye part.
	void CloseHeroEye() ;
	void ClosePlayerEye() ;
	void SendToServerFace(BYTE faceNum) ;
};
EXTERNGLOBALTON(CFacialManager)