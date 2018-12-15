#pragma once
#include <stack>

class CHistory;

class CHistoryManager
{
public:
	CHistoryManager(void);
	virtual ~CHistoryManager(void);
	void Undo();
	void Redo();
	void ClearUndo();
	void ClearRedo();
	bool IsUndoEmpty() const;
	bool IsRedoEmpty() const;
	template< class Unit >
	inline Unit& Allocate()
	{
		Unit* const unit = new Unit;
		mUndoStack.push(unit);

		return *unit;
	}

protected:
	// 작업한 내역이 저장된다
	std::stack< CHistory* > mUndoStack;
	// 작업 취소한 내역이 저장된다
	std::stack< CHistory* > mRedoStack;
};
