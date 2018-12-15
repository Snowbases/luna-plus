#pragma once
//=================================================================================================
//	FILE		: cChangeNameDialog.h
//	PURPOSE		: Dialog interface to change name of hero.
//	PROGRAMMER	: Yongs Lee
//	DATE		: December 26, 2007
//	ADDITION	:
//	ATTENTION	:
//=================================================================================================





//=========================================================================
//		필요한 헤더파일을 불러온다.
//=========================================================================
#include "./Interface/cDialog.h"





//=========================================================================
//		필수 정의 파트.
//=========================================================================
#define CN_MIN_NAME_LENGTH		4
#define CN_MAX_NAME_LENGTH		17





//=========================================================================
//		필요한 컨트롤 인터페이스를 선언한다.
//=========================================================================
class cButton ;
class cEditBox ;






//=========================================================================
//		이름 변경 다이얼로그 클래스 정의
//=========================================================================
class cChangeNameDialog : public cDialog
{
	cEditBox* m_pNameBox ;

	cButton* m_pChangeBtn ;
	cButton* m_pCancelBtn ;
public:
	cChangeNameDialog(void);
	virtual ~cChangeNameDialog(void);

	void Linking() ;

	virtual void OnActionEvent( LONG id, void* p, DWORD event ) ;

	void Clear_NameBox() ;
	
	// 091125 ONS 변경할 캐릭터명을 반환한다.
	char* GetChangeName();
};
