#pragma once

// MList 에 찍기위한 구조체
struct MData
{
	CString Group;
	CString MonsterIdx;
	CString MonsterKind;
	CString x;
	CString z;
	CString Flag;
};

class CTile;
class MToolMouseStatus;
class CRegenToolDoc;
class CRectangle;

class CRegenToolView : public CFormView
{
	VECTOR3 mClickedPositionOnField;
	CFont m_Font;
	IDIFontObject* m_pFontObject;
	MToolMouseStatus* mMouseStatus;

	BOOL m_bSelectedObjectMove;
	CPoint m_RBtnDownPos;
	BOOL m_bCanUsePopupMenu;
	
	// 카메라 이동처리
	BOOL m_bIsMovingCameraSmoothly;
	BOOL m_bIsMovingCameraY;
	VECTOR3 m_vecMoveTargetPos;
	VECTOR3 m_vecPopupMenuPos;
	const std::auto_ptr< CRectangle > mRectangle;
	typedef std::set< GXOBJECT_HANDLE > HandleContainer;
	HandleContainer mSelectedHandleContainer;

	typedef std::map< GXOBJECT_HANDLE, CPoint > SelectedHandleMoveInfo;
	SelectedHandleMoveInfo m_SelectedObjectMoveInfo;

protected: // create from serialization only
	CRegenToolView();
	DECLARE_DYNCREATE(CRegenToolView)
	void SetDefaultCamera();
	void PutPosition(CPoint&);
	void PutPopup(CPoint&);
	VECTOR3 GetFieldPositionByCursor( CPoint& point );
	void MoveCameraSmoothly();

public:
	//{{AFX_DATA(CRegenToolView)
	enum { IDD = IDD_REGENTOOL_FORM };
	//}}AFX_DATA
	
// Attributes
public:
	CRegenToolDoc* GetDocument();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegenToolView)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	void InitEngine();
	void InsertMonster(VECTOR3* TargetPos);
	CPoint CalcSelectPoint(CPoint point);
	GXOBJECT_HANDLE GetSelectedObjectHandle(CPoint point);
	void RenderFont( LONG x, LONG y, DWORD dwColor, LPCTSTR, ...);
	void RenderSelectedObjectInfo();
	void AfterRender();
	void SelectObject(const CRect&);
	void SetSelectedObjectHandle( GXOBJECT_HANDLE handle );
	const GXOBJECT_HANDLE GetSelectedObjectHandle() const { return mSelectedHandleContainer.empty() == TRUE ? NULL : *mSelectedHandleContainer.begin(); }
	void SetCameraPos( GXOBJECT_HANDLE handle, BOOL bMoveCameraY=FALSE, BOOL bMoveDirectly=FALSE );
	void SetCameraPos( VECTOR3 vecTargetPos, BOOL bMoveCameraY=FALSE, BOOL bMoveDirectly=FALSE );

	void StartMoveOfSelectedObjects();
	void MovingOfSelectedObjects( const CPoint& mousepoint );
	void EndMoveOfSelectedObjects();

	virtual ~CRegenToolView();

// Generated message map functions
protected:
	//{{AFX_MSG(CRegenToolView)
	CComboBox m_MapKind;
	CButton mFogButton;
	CButton mMapObjectButton;
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSelchangeMapcombo();
	afx_msg void OnFileSave();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedCheckFogToggle();
	
	afx_msg void OnBnClickedCheckMapObjectToggle();
	afx_msg void OnFileSaveAs();
	afx_msg void OnMenuObjectAdd();
	afx_msg void OnMenumObjectRemove();
	afx_msg void OnMenuObjectClose();
	afx_msg void OnMenuObjectMove();
	afx_msg void OnMenuCameraMove();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // debug version in RegenToolView.cpp
inline CRegenToolDoc* CRegenToolView::GetDocument()
   { return (CRegenToolDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}