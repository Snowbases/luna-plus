#pragma once

#define CONSIGNMENTMGR USINGTON(CConsignmentMgr)

class CConsignmentMgr
{
public:
	CConsignmentMgr(void);
	virtual ~CConsignmentMgr(void);


	void	NetworkMsgParser(DWORD dwConnectionIndex, BYTE Protocol, void* pMsg, DWORD dwLength);
};

EXTERNGLOBALTON(CConsignmentMgr)