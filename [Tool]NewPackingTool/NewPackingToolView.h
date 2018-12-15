// NewPackingToolView.h : iCNewPackingToolView 클래스의 인터페이스
//


#pragma once

#include "ScriptTest.h"
#include "ScriptTestStr.h"

class CNewPackingToolView : public CEditView
{
protected: // serialization에서만 만들어집니다.
	CNewPackingToolView();
	DECLARE_DYNCREATE(CNewPackingToolView)

// 특성
public:
	CNewPackingToolDoc* GetDocument() const;

// 작업
public:

// 재정의
	public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현
public:
	virtual ~CNewPackingToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 메시지 맵 함수를 생성했습니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	virtual void OnInitialUpdate();
	void	SelChange(int index);
	void	MoveLine(int line);
	int		GetCurIndex() { return m_curIndex; }

	int		CheckScript(UINT modeType);
	void	ScriptTest(UINT testType);

public:
	int		m_curIndex;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#ifndef _DEBUG  // NewPackingToolView.cpp의 디버그 버전
inline CNewPackingToolDoc* CNewPackingToolView::GetDocument() const
   { return reinterpret_cast<CNewPackingToolDoc*>(m_pDocument); }
#endif

