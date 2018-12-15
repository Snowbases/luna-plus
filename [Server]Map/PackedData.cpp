// PackedData.cpp: implementation of the CPackedData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PackedData.h"
#include "Network.h"
#include "ServerTable.h"
#include "Object.h"
#include "GridSystem.h"

CPackedData::CPackedData()
{
	mData.Initialize( 10 );
}

CPackedData::~CPackedData()
{
	mData.SetPositionHead();
	
	while(Data* data = mData.GetData())
	{
		delete data;
	}
}

void CPackedData::Init( void* pData,WORD MsgLen )
{
	ZeroMemory( &m_SendData, sizeof( m_SendData ) );
	m_SendData.Category = MP_PACKEDDATA;
	m_SendData.Protocol = MP_PACKEDDATA_NORMAL;
	m_SendData.wRealDataSize = MsgLen;
	memcpy(m_SendData.Data,pData,MsgLen);

	mData.SetPositionHead();

	while(Data* const data = mData.GetData())
	{
		data->mIndexList.RemoveAll();
	}
}

void CPackedData::AddObject(CObject* pObject)
{
	Data* data = mData.GetData( pObject->GetAgentNum() );

	if( 0 == data )
	{
		data = new Data;
		data->mAgentNumber = pObject->GetAgentNum();
		mData.Add( data, pObject->GetAgentNum() );
	}

	data->mIndexList.AddHead( LPVOID( pObject->GetID() ) );
}

void CPackedData::Send()
{
	if(0 == m_SendData.wRealDataSize)
	{
		return;
	}

	mData.SetPositionHead();

	while(Data* data = mData.GetData())
	{
		if( data->mIndexList.IsEmpty() )
		{
			continue;
		}

		LPSTR indexPosition = &m_SendData.Data[ m_SendData.wRealDataSize ];
		PTRLISTPOS position = data->mIndexList.GetHeadPosition();

		while(const DWORD objectIndex = DWORD(data->mIndexList.GetNext(position)))
		{
			memcpy( indexPosition, &objectIndex, sizeof( objectIndex ) );
			indexPosition += sizeof( objectIndex );
		}

		const DWORD dataLength = sizeof(m_SendData) - sizeof(m_SendData.Data) + indexPosition - m_SendData.Data;
		m_SendData.wReceiverNum = WORD( data->mIndexList.GetCount() );
		g_Network.Send2Server(
			data->mAgentNumber,
			LPSTR( &m_SendData ),
			dataLength );
	}
}

void CPackedData::QuickSendExceptObjectSelf(CObject* pObject,MSGBASE* pMsg,int MsgLen)
{
	if(pObject->GetInited() == FALSE)
		return;

	Init( pMsg, WORD( MsgLen ) );
		
	g_pServerSystem->GetGridSystem()->AddGridToPackedDataWithoutOne(
		pObject->GetGridPosition(),	pObject->GetID(),PACKEDDATA_OBJ, pObject);
	Send();
}

void CPackedData::QuickSend(CObject* pObject,MSGBASE* pMsg,int MsgLen)
{
	if(pObject->GetInited() == FALSE)
		return;

	GridPosition* pGridPosition = pObject->GetGridPosition();
	if(pGridPosition->x == 65535 || pGridPosition->z == 65535)
	{
//		g_Console.Log(eLogDisplay,4,"Not Inited Object Send %s",pObject->GetObjectName());
		return;
	}

	Init( pMsg, WORD( MsgLen ) );

	g_pServerSystem->GetGridSystem()->AddGridToPackedDataWithoutOne(
		pGridPosition, 0,PACKEDDATA_OBJ, pObject);	
	Send();
}

void CPackedData::SendToMapServer(MAPTYPE ToMapNum, MSGBASE* pMsg, WORD MsgLen) const
{
	static SEND_PACKED_TOMAPSERVER_DATA msg;
	ZeroMemory(
		&msg,
		sizeof(msg));
	msg.Category = MP_PACKEDDATA;
	msg.Protocol = MP_PACKEDDATA_TOMAPSERVER;
	msg.ToMapNum = ToMapNum;
	msg.wRealDataSize = MsgLen;
	memcpy(msg.Data,pMsg,MsgLen);

	g_Network.Send2AgentServer(
		(char*)&msg,
		msg.GetMsgLength());
}

void CPackedData::SendToBroadCastMapServer(MSGBASE* pMsg, WORD MsgLen) const
{
	static SEND_PACKED_TOMAPSERVER_DATA msg;
	ZeroMemory(
		&msg,
		sizeof(msg));
	msg.Category = MP_PACKEDDATA;
	msg.Protocol = MP_PACKEDDATA_TOBROADMAPSERVER;
	msg.ToMapNum = 0;
	msg.wRealDataSize = MsgLen;
	memcpy(msg.Data,pMsg,MsgLen);
	
	g_Network.Send2AgentServer(
		(char*)&msg,
		msg.GetMsgLength());
}