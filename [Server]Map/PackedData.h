#pragma once

class CObject;

#define PACKEDDATA_OBJ CPackedData::GetInstance()

class CPackedData  
{
	friend class CGeneralGridTable;
public:
	GETINSTANCE( CPackedData )

	CPackedData();
	virtual ~CPackedData();
	void AddObject( CObject* );
	void Send();
	void QuickSend( CObject*, MSGBASE*, int MsgLen );
	void QuickSendExceptObjectSelf( CObject*, MSGBASE*, int MsgLen );
	void SendToMapServer(MAPTYPE, MSGBASE*, WORD MsgLen) const;
	void SendToBroadCastMapServer(MSGBASE*, WORD MsgLen) const;
	MSGBASE* GetMsg() { return (MSGBASE*)m_SendData.Data; }
	int GetMsgLen()	{ return m_SendData.wRealDataSize; }

private:
	void Init( LPVOID, WORD length );

private:
	SEND_PACKED_DATA m_SendData;
	struct Data
	{
		cPtrList mIndexList;
		DWORD mAgentNumber;
	};
	CYHHashTable< Data > mData;
};