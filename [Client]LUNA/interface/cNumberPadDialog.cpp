//=================================================================================================
//	FILE		: cNumberPadDialog.cpp
//	PURPOSE		: Implemention part of number pad interface.
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
#include "stdafx.h"
#include "../WindowIDEnum.h"
#include "../cMsgBox.h" 

#include "./Interface/cWindowManager.h"

#include "./Interface/cNumberPadDialog.h"
#include "./Interface/cStatic.h"








//=========================================================================
//	NAME : cNumberPadDialog
//	DESC : Construct function.
//=========================================================================
cNumberPadDialog::cNumberPadDialog(void)
{
	int count = 0 ;															// 하위 for문을 위한 카운트 변수를 선언한다.

	for(count = 0 ; count < MAX_CIPHER ; ++count )
	{
		m_nNumber[count] = 0 ;												// 입력 숫자를 0으로 초기화 한다.
	}

	memset(m_NumberStr, 0, sizeof(m_NumberStr)) ;							// 숫자 문자열을 초기화 한다.

	m_nCipherCount	= -1 ;													// 자리수 카운트를 0으로 초기화 한다.

	m_pNumberBox	= NULL ;												// 숫자 박스 컨트롤을 초기화 한다.

	for(count = 0 ; count < e_Btn_Max ; ++count )
	{
		m_pNumberBtn[count] = NULL ;										// 숫자 버튼들 컨트롤을 초기화 한다.
	}
}





//=========================================================================
//	NAME : ~cNumberPadDialog
//	DESC : Descruct function.
//=========================================================================
cNumberPadDialog::~cNumberPadDialog(void)
{
}





//=========================================================================
//	NAME : Initialize
//	DESC : The function to initialize all to default.
//=========================================================================
void cNumberPadDialog::Initialize() 	
{
	for(int count = 0 ; count < MAX_CIPHER ; ++count )
	{
		m_nNumber[count] = 0 ;												// 입력 숫자를 0으로 초기화 한다.
	}

	m_nCipherCount	= -1 ;													// 자리수 카운트를 0으로 초기화 한다.

	m_pNumberBox->SetStaticText("") ;										// 숫자 입력란을 초기화 한다.

	cStatic* pSecurityBox = NULL ;
	pSecurityBox = (cStatic*)WINDOWMGR->GetWindowForIDEx(MT_SECURITYPW) ;	// 보안 스태틱을 불러온다.

	if( !pSecurityBox ) return ;											// 보안 스태틱이 유효한지 체크한다.

	pSecurityBox->SetStaticText("") ;										// 보안 스태틱을 초기화 한다.
}





//=========================================================================
//	NAME : Linking
//	DESC : The function to link all child controls.
//=========================================================================
void cNumberPadDialog::Linking()
{
	m_pNumberBox = (cStatic*)GetWindowForID(NUMBERPAD_NUMBERBOX) ;				// 숫자 박스 컨트롤을 링크한다.

	for(int count = 0 ; count < e_Btn_Max ; ++count )							// 하위 버튼 컨드롤들을 링크한다.
	{
		m_pNumberBtn[count] = (cButton*)GetWindowForID(NUMBERPAD_0+count) ;
	}

	m_pNumberBox->SetActive(FALSE) ;											// 숫자 창을 비활성화 시킨다.
}





//=========================================================================
//	NAME : OnActionEvent
//	DESC : The function to management action event.
//=========================================================================
void cNumberPadDialog::OnActionEvent( LONG id, void* p, DWORD event ) 			
{
	if( id == NUMBERPAD_BACKSPACE )													// 백 스페이스 버튼이 눌렸다면,
	{
		DecreaseUnit() ;															// 숫자를 감소시키는 함수를 호출한다.
		return ;																	// return 처리를 한다.
	}	

	int nNumber = 0 ;																// 현재 눌려진 번호를 담을 변수를 선언하고 초기화 한다.

	switch( id )																	// 인자로 넘어온 아이디를 확인한다.
	{
	case NUMBERPAD_0 :	nNumber = 0 ;	break ;										// 눌려진 버튼 대로 숫자를 세팅한다.
	case NUMBERPAD_1 :	nNumber = 1 ;	break ;
	case NUMBERPAD_2 :	nNumber = 2 ;	break ;
	case NUMBERPAD_3 :	nNumber = 3 ;	break ;
	case NUMBERPAD_4 :	nNumber = 4 ;	break ;
	case NUMBERPAD_5 :	nNumber = 5 ;	break ;
	case NUMBERPAD_6 :	nNumber = 6 ;	break ;
	case NUMBERPAD_7 :	nNumber = 7 ;	break ;
	case NUMBERPAD_8 :	nNumber = 8 ;	break ;
	case NUMBERPAD_9 :	nNumber = 9 ;	break ;
	}

	IncreaseUnit(nNumber) ;															// 숫자를 증가시키는 함수를 호출한다.
}





//=========================================================================
//	NAME : IncreaseUnit
//	DESC : The function to increase unitcount and calculate number.
//=========================================================================
void cNumberPadDialog::IncreaseUnit(int nNumber) 
{
	++m_nCipherCount ;																// 자리수를 증가한다.
	
	if( m_nCipherCount >= MAX_CIPHER ) 
	{
		m_nCipherCount = (MAX_CIPHER-1) ;			// 오버 된 자리수를 보정한다.
		return ;
	}

	m_nNumber[m_nCipherCount] = nNumber ;											// 해당하는 자리수에 숫자를 세팅한다.

	PrintNumberToNumberBox() ;														// 숫자박스에 숫자를 출력한다.
}





//=========================================================================
//	NAME : DecreaseUnit
//	DESC : The function to decrease unitcount and calulate number.
//=========================================================================
void cNumberPadDialog::DecreaseUnit() 
{
	if( m_nCipherCount <= -1 ) return ;												// 자리수를 확인한다.

	m_nNumber[m_nCipherCount] = 0 ;													// 해당하는 자리수의 숫자를 초기화 한다.
	--m_nCipherCount ;																// 자리수를 감소한다.

	PrintNumberToNumberBox() ;														// 숫자박스에 수자를 출력한다.

	if( m_nCipherCount <= -1 ) m_nCipherCount = -1 ;								// 자리수를 보정한다.
}




//=========================================================================
//	NAME : PrintNumberToNumberBox
//	DESC : The function to print numbers to number box.
//=========================================================================
void cNumberPadDialog::PrintNumberToNumberBox() 
{
	cStatic* pSecurityBox = (cStatic*)WINDOWMGR->GetWindowForIDEx(MT_SECURITYPW) ;

	if( !pSecurityBox ) return ;

	memset(m_NumberStr, 0, sizeof(m_NumberStr)) ;									// 숫자 문자열을 초기화 한다.

	//m_pNumberBox->SetStaticText("") ;												// 숫자 박스를 초기화 한다.

	pSecurityBox->SetStaticText("") ;

	char securityStr[MAX_CIPHER+1] = {0,} ;

	char pNumber[MAX_CIPHER+1] ;

	for( int count = -1 ; count < m_nCipherCount ; ++count )
	{
		memset(pNumber, 0, sizeof(pNumber)) ;
		itoa(m_nNumber[count+1], pNumber, 10) ; //보안번호 10진수이므로  
		strcat(m_NumberStr, pNumber) ;
		strcat(securityStr, "*") ;
	}

	//switch(m_nCipherCount)															// 자리수를 확인하고, 문자열로 숫자를 나타낸다.
	//{
	//case 0 :
	//	{
	//		sprintf(m_NumberStr, "%d", m_nNumber[0]) ;									
	//		sprintf(securityStr, "*", m_nNumber[0]) ;	
	//	}
	//	break ;

	//case 1 :
	//	{
	//		sprintf(m_NumberStr, "%d%d", m_nNumber[0], m_nNumber[1]) ;
	//		sprintf(securityStr, "**", m_nNumber[0], m_nNumber[1]) ;
	//	}
	//	break ;

	//case 2 :
	//	{
	//		sprintf(m_NumberStr, "%d%d%d", m_nNumber[0], m_nNumber[1], m_nNumber[2]) ;
	//		sprintf(securityStr, "***", m_nNumber[0], m_nNumber[1], m_nNumber[2]) ;
	//	}
	//	break ;

	//case 3 :
	//	{
	//		sprintf(m_NumberStr, "%d%d%d%d", m_nNumber[0], m_nNumber[1], m_nNumber[2], m_nNumber[3]) ;
	//		sprintf(securityStr, "****", m_nNumber[0], m_nNumber[1], m_nNumber[2], m_nNumber[3]) ;
	//	}
	//	break ;

	//default : return ;
	//}

	//m_pNumberBox->SetStaticText(m_NumberStr) ;										// 현재 숫자로 숫자 박스를 세팅한다.

	pSecurityBox->SetStaticText(securityStr) ;
}




//=========================================================================
//	NAME : GetNumber
//	DESC : The function to return number of selected cipher.
//=========================================================================
int cNumberPadDialog::GetNumber(int nCipher)
{
	if( nCipher <= 0 || nCipher >= MAX_CIPHER ) return 0 ;							// 자리수의 유효성을 체크한다.

	if( m_nCipherCount <= 0 ) return 0 ;											// 버튼 입력이 없었을 경우, 0 리턴 처리를 한다.

	return m_nNumber[m_nCipherCount] ;												// 해당하는 숫자를 리턴한다.
}





//=========================================================================
//	NAME : SetActive
//	DESC : The function to setting active or not this dialog.
//=========================================================================
void cNumberPadDialog::SetActive( BOOL val ) 
{
	if( val == TRUE )																// 활성화 할 때는, 
	{
		Initialize() ;																// 모든 정보를 초기화 해준다.
	}

	cDialog::SetActive( val ) ;
}