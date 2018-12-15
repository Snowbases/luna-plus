#pragma once


#include "interface/cDialog.h"

class cComboBox;
class cButton;


class CGuildWarehouseRankDialog  : public cDialog  
{
public:
	CGuildWarehouseRankDialog();
	virtual ~CGuildWarehouseRankDialog();
	virtual void SetActive(BOOL val);
	void Linking();
	
	virtual void OnActionEvent( LONG id, void* p, DWORD event );


private:
	cComboBox*	m_pRankComboBox;
};