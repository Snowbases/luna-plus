#include "stdafx.h"
#include "4DyuchiGXMapEditor.h"
#include "4DyuchiGXMapEditorDoc.h"
#include "Tool.h"

IMPLEMENT_DYNCREATE(CMy4DyuchiGXMapEditorDoc, CDocument)

BEGIN_MESSAGE_MAP(CMy4DyuchiGXMapEditorDoc, CDocument)
	//{{AFX_MSG_MAP(CMy4DyuchiGXMapEditorDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CMy4DyuchiGXMapEditorDoc::CMy4DyuchiGXMapEditorDoc()
{}

CMy4DyuchiGXMapEditorDoc::~CMy4DyuchiGXMapEditorDoc()
{}

BOOL CMy4DyuchiGXMapEditorDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	if(0 == g_pTool)
	{
		static CTool tool;
		g_pTool = &tool;

		POSITION position = GetFirstViewPosition();
		g_pTool->Initialize(GetNextView(position));
	}

	return TRUE;
}