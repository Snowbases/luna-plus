// FamilyMarkManager.h: interface for the CFamilyMarkManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FAMILYMARKMANAGER_H__5F7B3564_F660_4AB8_91B1_023A697C6DFE__INCLUDED_)
#define AFX_FAMILYMARKMANAGER_H__5F7B3564_F660_4AB8_91B1_023A697C6DFE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#define FAMILYMARKMGR CFamilyMarkManager::GetInstance()

class CFamilyMark;
class CFamilyMarkImg;

class CFamilyMarkManager  
{
	CYHHashTable<CFamilyMark> m_MarkTable;

	CFamilyMarkImg* m_pLoadingImg;

	CFamilyMarkImg* LoadMarkImg(MARKNAMETYPE markname);	
	void SendFamilyMarkImgRequest(MARKNAMETYPE MarkName);
	void SendFamilyMarkImgUpdate(DWORD FamilyIdx,char* pImgData);
	BOOL ReadBitInfoFromFile(char* filename,char* imgData);
	
	BOOL LoadMarkInfo();
public:
	MAKESINGLETON(CFamilyMarkManager);
	CFamilyMarkManager();
	virtual ~CFamilyMarkManager();

	BOOL Init();
	void Release();

	CFamilyMark* GetFamilyMark(MARKNAMETYPE markname);

	void OnRecvFamilyMarkImg(MSG_FAMILYMARK_IMG* pmsg);

	BOOL ChangeFamilyMarkSyn(DWORD FamilyIdx,char* filename);
	void UpdateFamilyMarkImg(DWORD FamilyIdx,MARKNAMETYPE markname,char* pImgData);

	CFamilyMark* LoadFamilyMarkImg( MARKNAMETYPE markname );
};
#endif // !defined(AFX_FAMILYMARKMANAGER_H__5F7B3564_F660_4AB8_91B1_023A697C6DFE__INCLUDED_)
