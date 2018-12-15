#include "stdafx.h"
#include "HistoryManager.h"
#include "History.h"

CHistoryManager::CHistoryManager(void)
{
}

CHistoryManager::~CHistoryManager(void)
{
	ClearUndo();
	ClearRedo();
}

void CHistoryManager::Undo()
{
	if(mUndoStack.empty())
	{
		return;
	}

	CHistory* const history = mUndoStack.top();
	mUndoStack.pop();

	if(0 == history)
	{
		return;
	}

	history->Undo();
	mRedoStack.push(history);
}

void CHistoryManager::Redo()
{
	if(mRedoStack.empty())
	{
		return;
	}

	CHistory* const history = mRedoStack.top();
	mRedoStack.pop();

	if(0 == history)
	{
		return;
	}

	history->Redo();
	mUndoStack.push(history);
}

void CHistoryManager::ClearUndo()
{
	while(false == mUndoStack.empty())
	{
		const CHistory* const history = mUndoStack.top();
		delete history;
		mUndoStack.pop();
	}
}

void CHistoryManager::ClearRedo()
{
	while(false == mRedoStack.empty())
	{
		const CHistory* const history = mRedoStack.top();
		delete history;
		mRedoStack.pop();
	}
}

bool CHistoryManager::IsUndoEmpty() const
{
	return mUndoStack.empty();
}

bool CHistoryManager::IsRedoEmpty() const
{
	return mRedoStack.empty();
}