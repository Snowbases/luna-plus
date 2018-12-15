/*
080401 LUJ, 로그 등 공통되는 창 기능에 대한 추상 클래스
*/
#pragma once


class CListCtrl;
class CProgressCtrl;
class CStatic;
class CButton;
class CclientApp;


class CCommand
{
	// 080401 LUJ, 추상 클래스를 생성할 수 없도록 생성자/소멸자를 protected로 만든다
protected:
	inline CCommand( CclientApp& application, const TCHAR* title )	:
	mApplication( application ),
		mTickCount( 0 ),
		mTitle( title )
	{}

	inline virtual ~CCommand() {};

public:
	virtual void Find(
		DWORD			serverIndex,
		const TCHAR*	beginTime,
		const TCHAR*	endTime )	= 0;

	virtual void Parse(
		const MSGROOT*,
		CListCtrl&,
		CProgressCtrl&,
		CStatic&,
		CButton&		findButton,
		CButton&		stopButton )	const = 0;

	virtual void Stop		( DWORD serverIndex )					const = 0;
	virtual void Initialize	( CListCtrl& )							const = 0;
	virtual void SaveToExcel( DWORD serverIndex, const CListCtrl& )	const = 0;

protected:
	CclientApp& mApplication;
	CString		mTitle;
	DWORD		mTickCount;	

public:
	inline const TCHAR* GetTitle() const { return mTitle; }
};
