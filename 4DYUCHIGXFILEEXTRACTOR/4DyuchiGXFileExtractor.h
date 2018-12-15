// 4DyuchiGXFileExtractor.h : main header file for the 4DYUCHIGXFILEEXTRACTOR application
//

#if !defined(AFX_4DYUCHIGXFILEEXTRACTOR_H__7A703D2A_3597_4560_A568_77E1E3964631__INCLUDED_)
#define AFX_4DYUCHIGXFILEEXTRACTOR_H__7A703D2A_3597_4560_A568_77E1E3964631__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CMy4DyuchiGXFileExtractorApp:
// See 4DyuchiGXFileExtractor.cpp for the implementation of this class
//

class CMy4DyuchiGXFileExtractorApp : public CWinApp
{
public:
	CMy4DyuchiGXFileExtractorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMy4DyuchiGXFileExtractorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMy4DyuchiGXFileExtractorApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_4DYUCHIGXFILEEXTRACTOR_H__7A703D2A_3597_4560_A568_77E1E3964631__INCLUDED_)
