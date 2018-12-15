#pragma once

class CMy4DyuchiGXMapEditorDoc : public CDocument
{
protected: // create from serialization only
	CMy4DyuchiGXMapEditorDoc();
	DECLARE_DYNCREATE(CMy4DyuchiGXMapEditorDoc)

// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMy4DyuchiGXMapEditorDoc)
	public:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMy4DyuchiGXMapEditorDoc();

// Generated message map functions
protected:
	//{{AFX_MSG(CMy4DyuchiGXMapEditorDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};