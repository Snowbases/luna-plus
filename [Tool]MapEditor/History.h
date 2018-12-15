#pragma once

class CHistory
{
public:
	CHistory(void) {}
	virtual ~CHistory(void) {}
	virtual void Undo() = 0;
	virtual void Redo() = 0;
};
