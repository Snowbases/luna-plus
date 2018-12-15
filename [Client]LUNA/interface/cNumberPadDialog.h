#pragma once
//=================================================================================================
//	FILE		: cNumberPadDialog.h
//	PURPOSE		: Dialog interface to create some numbers.
//	PROGRAMMER	: Yongs Lee
//	DATE		: January 9, 2008
//	ADDITION	:
//	ATTENTION	: 현재, 비밀 번호의 입력 용으로, 키보드가 아닌, 마우스를 사용하여 처리하기 위해, 
//				  숫자 입력 인터페이스가 필요하다. 또한 추후 숫자 입력의 필요성이 생길 수 있으므로,
//				  필요한 곳에서 임시 작성용이 아닌, 하나의 인터페이스를 추가하여, 사용할 수 있도록 
//				  한다. 지금은 꼭 필요한 기능만 구현하지만, 추구 필요한기능은 추가하여 사용하도록.
//=================================================================================================





//=========================================================================
//		필요한 헤더파일을 불러온다.
//=========================================================================
#include "./Interface/cDialog.h"





//=========================================================================
//		필수 정의 파트.
//=========================================================================
#define MAX_CIPHER		4													// 최대 자리수를 정의한다.
//char strSecurityPW[16] ; 메시지 구조체의 크기도 꼭 변경해 준다.
// Distributeserver의 BOOL LoginCheckQuery() 함수 안에 임시 버퍼도 MAX_CIPHER+1로 맞춰준다.




//=========================================================================
//		필요한 컨트롤 인터페이스를 선언한다.
//=========================================================================
class cButton ;
class cStatic ;

enum NUMBER_BUTTONS
{
	e_Btn_0 = 0,
	e_Btn_1,
	e_Btn_2,
	e_Btn_3,
	e_Btn_4,
	e_Btn_5,
	e_Btn_6,
	e_Btn_7,
	e_Btn_8,
	e_Btn_9,
	e_Btn_BackSpace,

	e_Btn_Max,
} ;





//=========================================================================
//		숫자 패드 다이얼로그 클래스 정의
//=========================================================================
class cNumberPadDialog : public cDialog
{
	int		m_nNumber[MAX_CIPHER] ;											// 입력 숫자.
	int		m_nCipherCount ;												// 자리수 카운트. 0 = 1단위 숫자, 1 = 10단위 숫자, ....

	char	m_NumberStr[MAX_CIPHER+1] ;										// 숫자를 문자열로 담을 버퍼.

	cStatic*	m_pNumberBox ;												// 숫자 박스 컨트롤.

	cButton*	m_pNumberBtn[e_Btn_Max] ;									// 숫자 버튼들 컨트롤.

public:
	cNumberPadDialog(void) ;												// 생성자 함수.
	virtual ~cNumberPadDialog(void) ;										// 소멸자 함수.

	void Initialize() ;														// 초기화 함수.

	void Linking() ;														// 하위 컨트롤 링크 함수.

	virtual void OnActionEvent( LONG id, void* p, DWORD event ) ;			// 액션 이벤트 처리 함수.

	void	IncreaseUnit(int nNumber) ;										// 숫자 단위를 증가하는 함수.
	void	DecreaseUnit() ;												// 숫자 단위를 감소하는 함수.

	void	PrintNumberToNumberBox() ;										// 숫자 박스에 숫자를 출력하는 함수.

	int		GetNumber(int nCipher) ;										// 숫자를 리턴하는 함수.
	char*	GetNumberStr() { return m_NumberStr ; }							// 숫자 문자열을 리턴하는 함수.

	int		GetMaxCipherCount() { return MAX_CIPHER ; }						// 최대 자리수를 리턴하는 함수.
	int		GetCurCipherCount() { return m_nCipherCount ; }					// 현재 자리수를 리턴하는 함수.

	virtual void SetActive( BOOL val ) ;									// 활성화 여부를 세팅하는 함수.
};
