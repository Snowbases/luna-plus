#ifndef __CLISTDIALOG_EX__
#define __CLISTDIALOG_EX__

#include "./interface/cListDialog.h"										// 리스트 다이얼로그 헤더를 불러온다.

class cListDialogEx : public cListDialog									// 리스트 다이얼로그 클래스를 상속받은 리스트 다이얼로그 확장 클래스.
{
public:
	cListDialogEx();														// 생성자 함수.
	~cListDialogEx();														// 소멸자 함수.

	virtual void ListMouseCheck( LONG x, LONG y, DWORD we );				// 리스트 마우스 체크 함수.
	virtual void Render();													// 렌더 함수.
};

#endif //__CLISTDIALOG_EX__