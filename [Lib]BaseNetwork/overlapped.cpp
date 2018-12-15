#include "stdafx.h"
#include "overlapped.h"
#include "define.h"

DWORD COverlapped::ResetBuffer()
{
	if (!m_dwEntryPosition)
		goto lb_return;

	memcpy(m_pBuffer,m_pBuffer+m_dwEntryPosition,m_dwExistPacketSize);
	m_dwEntryPosition = 0;

lb_return:
	return (m_dwMaxBufferSize - (m_dwEntryPosition + m_dwExistPacketSize));
}

void COverlapped::SetSocket( SOCKET )
{
	m_socket = INVALID_SOCKET;
}