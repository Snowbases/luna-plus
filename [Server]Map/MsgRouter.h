#pragma once

class MSG_OBJECT
{
public:
	WORD	msgID;
	DWORD	src;
	DWORD	dest;
	DWORD	start;
	DWORD	endDelay;
	DWORD	startDelay;
	DWORD	dwData;
};

class CMsgRouter  
{
	cLooseLinkedList<MSG_OBJECT> m_pMsgList;
	CPool<MSG_OBJECT> m_MsgPool;

public:
	CMsgRouter();
	virtual ~CMsgRouter();
	void SendMsg(WORD msgID, DWORD src, DWORD dest, DWORD delay, DWORD dwData);
	void MsgLoop();

	void DeliveryMsgTo(MSG_OBJECT * obj, CObject * pSrcObj, CObject * pDestObj);
	
private:
	void DeliveryMsg(MSG_OBJECT * obj);
};