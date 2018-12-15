// GMNotifyManager.h: interface for the CGMNotifyManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GMNOTIFYMANAGER_H__8AB34475_4213_4ADE_B123_5DF6F67CA06F__INCLUDED_)
#define AFX_GMNOTIFYMANAGER_H__8AB34475_4213_4ADE_B123_5DF6F67CA06F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define NOTIFYMGR USINGTON(CGMNotifyManager)


#define MAX_NOTIFYMSG_LENGTH			127												// 공지 메시지 길이
#define MAX_NOTIFYMSG_PER_LINE			64												// 라인 별 공지 메시지 길이
#define MAX_NOTIFYMSG_LINE				5												// 최대 공지 메시지 라인
#define NOTIFYMSG_LAST_TIME				10000											// 공지 메시지 시간

#define NOTIFYMSG_DEFAULT_X				200												// 공지 메시지 기본 X좌표
#define NOTIFYMSG_DEFAULT_Y				100												// 공지 메시지 기본 Y좌표
#define NOTIFYMSG_DEFAULT_WIDTH			624												// 공지 메시지 기본 가로사이즈
#define NOTIFYMSG_DEFAULT_LINESPACE		10												// 공지 메시지 기본 줄 간격
#define NOTIFYMSG_DEFAULT_FONT			0												// 공지 메시지 기본 폰트

struct sGMNOTIFY																		// GM공지 구조체
{
	DWORD	dwReceiveTime;																// 수락 시간
	DWORD	dwColor;																	// 색상
	int		nStrLen;																	// 문자열 길이
	char	strMsg[MAX_NOTIFYMSG_LENGTH+1];												// 공지 메시지 문자열
};

enum eNOTIFY_CLASS																		// 공지 메시지 타입
{
	eNTC_DEFAULT,																		// 기본 공지
	eNTC_EMERGENCY,																		// 긴급 공지
	eNTC_REMAINTIME,																	// 빌링															
	eNTC_TRACKING,																		// 트렉킹 공지?
	eNTC_COUNT,																			// 카운트 공지
};

const DWORD dwNotifyColor[eNTC_COUNT] = {												// 공지 타입에 따른 색상

	RGBA_MAKE(0, 255, 60, 0),
	RGBA_MAKE(0, 255, 60, 0),
	RGBA_MAKE(0, 255, 60, 0),
	RGBA_MAKE(255, 20, 20, 255),
};

class CGMNotifyManager																	// GM공지 매니져 클래스
{
protected:

	cPtrList	m_ListNotify;															// 공지 리스트
	int			m_nMaxLine;																// 최대 라인

	RECT		m_rcPos;																// 출력 영역 렉트
	int			m_nLineSpace;															// 라인 간격
	int			m_nFontIdx;																// 폰트 인덱스

	BOOL		m_bUseEventNotify;														// 이벤트 공지 사용여부를 담는 변수
	char		m_strEventNotifyTitle[32];												// 이벤트 공지 제목 문자열
	char		m_strEventNotifyContext[128];											// 이벤트 공지 내용 문자열

	BOOL		m_bNotifyChanged;														// 공지 변경 여부를 담는 변수
	
	BOOL		m_ApplyEventList[eEvent_Max];											// 이벤트 리스트의 적용여부를 담는 변수 배열

protected:
	void PushMsg( char* pMsg, DWORD dwColor );											// 메시지 추가 함수
	void PopMsg();																		// 메시지 반환 함수

public:
	CGMNotifyManager();																	// 공지 매니져 생성자 함수
	virtual ~CGMNotifyManager();														// 공재 매니져 소멸자 함수

	void Init( int nMaxLine );															// 초기화 함수
	void Release();																		// 해제 함수

	void AddMsg( char* pMsg, int nClass = eNTC_DEFAULT );								// 메시지 추가 함수
	void Render();																		// 렌더 함수

	void SetPosition( LONG lx, LONG ly, LONG lWidth );									// 위치 설정 함수
	void SetFont( int nFontIdx )		{ m_nFontIdx = nFontIdx; }						// 폰트 설정 함수
	void SetLineSpace( int nLineSpace ) { m_nLineSpace = nLineSpace; }					// 라인 간격 설정 함수

	void SetEventNotifyStr( char* pStrTitle, char* pStrContext );						// 이벤트 공지 메시지 세팅 함수
	void SetEventNotify( BOOL bUse );													// 이벤트 공지 사용 여부 세팅 함수
	BOOL IsEventNotifyUse()				{ return m_bUseEventNotify; }					// 이벤트 공지 사용 여부를 반환하는 함수
	char* GetEventNotifyTitle()			{ return m_strEventNotifyTitle; }				// 이벤트 공지 제목을 반환하는 함수
	char* GetEventNotifyContext()		{ return m_strEventNotifyContext; }				// 이벤트 공지 내용을 반환하는 함수

	void SetEventNotifyChanged( BOOL bChanged ) { m_bNotifyChanged = bChanged; }		// 이벤트 공지 변경 여부를 세팅하는 함수
	BOOL IsEventNotifyChanged()					{ return m_bNotifyChanged; }			// 이벤트 공지 변경 여부를 반환하는 함수

	void ResetEventApply()					{	memset( m_ApplyEventList, 0, sizeof(BOOL)*eEvent_Max );		}	// 이벤트 적용을 새로 고침하는 함수
	void SetEventApply( DWORD EventIdx )	{	m_ApplyEventList[EventIdx] = TRUE;		}	// 이벤트를 적용하는 함수
	BOOL IsApplyEvent( DWORD EventIdx )		{	return m_ApplyEventList[EventIdx];		}	// 이벤트 적용 여부를 반환하는 함수

	void LoadGameDesc() ;																// 게임 설정을 로딩하는 함수
};

EXTERNGLOBALTON(CGMNotifyManager)

#endif // !defined(AFX_GMNOTIFYMANAGER_H__8AB34475_4213_4ADE_B123_5DF6F67CA06F__INCLUDED_)
