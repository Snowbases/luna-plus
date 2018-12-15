#pragma once

#define CONSIGNMENTMGR USINGTON(CConsignmentMgr)

class CConsignmentMgr
{
public:
	CConsignmentMgr(void);
	virtual ~CConsignmentMgr(void);


	void NetworkMsgParse(BYTE Protocol,void* pMsg);	
	void LoadRegistedList();
};

EXTERNGLOBALTON(CConsignmentMgr)
