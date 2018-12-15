#pragma once


// CFileListView 뷰입니다.

class CFileListView : public CListView
{
	DECLARE_DYNCREATE(CFileListView)

protected:
	CFileListView();           // 동적 만들기에 사용되는 protected 생성자입니다.
	virtual ~CFileListView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
};


