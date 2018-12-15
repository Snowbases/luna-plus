#pragma once

#include <WinSock2.h>

BOOL StartAcceptThread(SOCKET sckListen,HWND hWnd,UINT uiMsg);
void EndAcceptThread();