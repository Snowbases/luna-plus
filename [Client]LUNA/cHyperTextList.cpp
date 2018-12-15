#include "StdAfx.h"
#include "cHyperTextList.h"
#include "MHFile.h"

cHyperTextList::cHyperTextList()
{
	LoadScript();
}

cHyperTextList::~cHyperTextList()
{}

void cHyperTextList::LoadScript()
{
	CMHFile file;
	file.Init("Data/Script/Npc/Npc_HyperText.bin", "rb");
	
	while(FALSE == file.IsEOF())
	{
		TCHAR buffer[MAX_PATH] = {0};
		file.GetLine(
			buffer,
			sizeof(buffer) / sizeof(*buffer));

		LPCTSTR seperator = " \t";
		LPCTSTR token = _tcstok(buffer, seperator);

		if(0 == token)
		{
			continue;
		}

		const DWORD dialogIndex = _ttoi(token);
		TCHAR textLine[MAX_PATH] = {0};

		for(token = _tcstok(0, seperator);;)
		{
			_tcsncat(
				textLine,
				token,
				sizeof(textLine) / sizeof(*textLine) - _tcslen(textLine));

			token = _tcstok(0, seperator);

			if(0 == token)
			{
				break;
			}

			LPCTSTR textSpace = " ";
			_tcsncat(
				textLine,
				textSpace,
				sizeof(textLine) / sizeof(*textLine) - _tcslen(textLine));
		}

		DIALOGUE& dialog = mHyperTextMap[dialogIndex];
		ZeroMemory(
			&dialog,
			sizeof(dialog));
		SafeStrCpy(
			dialog.str,
			textLine,
			sizeof(dialog.str) / sizeof(*dialog.str));
	}
}

DIALOGUE* cHyperTextList::GetHyperText(DWORD dialogIndex)
{
	HyperTextMap::iterator iterator = mHyperTextMap.find(dialogIndex);

	return mHyperTextMap.end() == iterator ? 0 : &(iterator->second);
}