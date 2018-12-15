#pragma once

class CMy4DyuchiGXMapEditorView : public CView
{

protected: // create from serialization only
	CMy4DyuchiGXMapEditorView();
	DECLARE_DYNCREATE(CMy4DyuchiGXMapEditorView)

// Attributes
public:
	CMy4DyuchiGXMapEditorDoc* GetDocument();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMy4DyuchiGXMapEditorView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMy4DyuchiGXMapEditorView();

// Generated message map functions
protected:
	//{{AFX_MSG(CMy4DyuchiGXMapEditorView)
	afx_msg void OnPaint();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSectionInput();
	afx_msg void OnUpdateSectionInput(CCmdUI* pCmdUI);
	afx_msg void OnSectionInfo();
	afx_msg void OnUpdateSectionInfo(CCmdUI* pCmdUI);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnCreateNpc();
	afx_msg void OnUpdateCreateNpc(CCmdUI* pCmdUI);
	afx_msg void OnLoadTtb();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMovespotInput();
	afx_msg void OnUpdateMovespotInput(CCmdUI* pCmdUI);
	afx_msg void OnMovespotInfo();
	afx_msg void OnUpdateMovespotInfo(CCmdUI* pCmdUI);
	afx_msg void OnStartspotInfo();
	afx_msg void OnUpdateStartspotInfo(CCmdUI* pCmdUI);
	afx_msg void OnStartspotInput();
	afx_msg void OnUpdateStartspotInput(CCmdUI* pCmdUI);
	afx_msg void OnSaveTtb();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};