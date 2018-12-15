#pragma once

#include "stdafx.h"
#include <initguid.h>

interface ICode : public IUnknown
{
	virtual void __stdcall					Encode(char* pExtCode,DWORD* pdwCodeLen,char* pMsg,DWORD dwLen) = 0;
	virtual BOOL __stdcall					Decode(char* pMsg,DWORD* pdwLen) = 0;
};
