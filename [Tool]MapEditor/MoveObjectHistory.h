#pragma once
#include "History.h"

class CMoveObjectHistory : public CHistory
{
public:
	CMoveObjectHistory();
	virtual ~CMoveObjectHistory(void);
	virtual void Undo();
	virtual void Redo();
	void SetValue(const GXOBJECT_PROPERTY&, GXOBJECT_HANDLE);

private:
	void RefreshProperty();
	void Run();

	struct Parameter
	{
		GXOBJECT_PROPERTY mProperty;
		GXOBJECT_HANDLE mHandle;
	}
	mParameter;
};
