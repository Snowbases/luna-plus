#pragma once

class cHyperTextList
{
	typedef DWORD DialogIndex;
	typedef stdext::hash_map< DialogIndex, DIALOGUE > HyperTextMap;
	HyperTextMap mHyperTextMap;

public:
	cHyperTextList();
	virtual ~cHyperTextList();
	DIALOGUE* GetHyperText(DWORD dialogIndex);

protected:
	void LoadScript();
};