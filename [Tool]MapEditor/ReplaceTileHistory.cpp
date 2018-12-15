#include "stdafx.h"
#include "ReplaceTileHistory.h"
#include "Tool.h"

CReplaceTileHistory::CReplaceTileHistory()
{}

void CReplaceTileHistory::SetValue(DWORD index, LPCTSTR oldPath, LPCTSTR newPath)
{
	TCHAR path[MAX_PATH] = {0};
	GetCurrentDirectory(
		sizeof(path) / sizeof(*path),
		path);

	mParameter.mIndex = index;
	mParameter.mOldPath = CString(path) + _T("\\") + oldPath;
	mParameter.mNewPath = newPath;
}

CReplaceTileHistory::~CReplaceTileHistory(void)
{}

void CReplaceTileHistory::Undo()
{
	// 존재하는 파일인지 검사해보고 없으면 기본 파일로 복구해야함!

	g_pTool->UpdateTile(
		mParameter.mOldPath,
		mParameter.mIndex);
}

void CReplaceTileHistory::Redo()
{
	// 존재하는 파일인지 검사해보고 없으면 기본 파일로 복구해야함!

	g_pTool->UpdateTile(
		mParameter.mNewPath,
		mParameter.mIndex);
}