// DirectoryManager.h: interface for the CDirectoryManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRECTORYMANAGER_H__366E8598_49B1_4150_BFDE_9772A132F74B__INCLUDED_)
#define AFX_DIRECTORYMANAGER_H__366E8598_49B1_4150_BFDE_9772A132F74B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum eLOADMODE
{
	eLM_Root,
	eLM_EffectScript,
	eLM_Ini,
	eLM_Sound,
	eLM_Resource,
	eLM_BGM,
	eLM_Character,
	eLM_Pet,
	eLM_Monster,
	eLM_Npc,
	eLM_Effect,
	eLM_Minimap,

	eLM_Map,
	eLM_Cloud,
	
	eLM_MapObject,

	eLM_NotChange,

	// desc_hseos_³óÀå½Ã½ºÅÛ_01
	// S ³óÀå½Ã½ºÅÛ Ãß°¡ added by hseos 2007.10.10
	eLM_Farm,
	// E ³óÀå½Ã½ºÅÛ Ãß°¡ added by hseos 2007.10.10

	eLM_Housing,

	eLM_Max,
};

#define LDP_EFFECTSCRIPT	"\\Data\\Script\\Direct"
#define LDP_INI				"\\Ini"
#define LDP_SOUND			"\\Data\\Sound"
#define LDP_RESOURCE		"\\System\\Resource"
#define LDP_BGM				"\\Data\\Sound\\BGM"
#define LDP_CHARACTER		"\\Data"
#define LDP_PET				"\\Data"
#define LDP_MONSTER			"\\Data"
#define LDP_NPC				"\\Data"
#define LDP_EFFECT			"\\Data"
#define LDP_MINIMAP			"\\Data\\Interface\\2DImage\\Minimap"
#define LDP_MAP				"\\Data"
#define LDP_CLOUD			"\\Data"
#define LDP_MAPOBJECT		"\\Data"
// desc_hseos_³óÀå½Ã½ºÅÛ_01
// S ³óÀå½Ã½ºÅÛ Ãß°¡ added by hseos 2007.10.10
#define LDP_FARM			"\\Data"
// E ³óÀå½Ã½ºÅÛ Ãß°¡ added by hseos 2007.10.10

#define LDP_HOUSING			"\\Data"

#define LD_EFFECTSCRIPT		"\\Data\\Script\\Direct"
#define LD_INI				"\\Ini"
#define LD_SOUND			"\\Data\\Sound"
#define LD_RESOURCE			"\\System\\Resource"
#define LD_BGM				"\\Data\\Sound\\BGM"
#define LD_CHARACTER		"\\Data\\3DData\\Character"
#define LD_PET				"\\Data\\3DData\\Pet"
#define LD_MONSTER			"\\Data\\3DData\\Monster"
#define LD_NPC				"\\Data\\3DData\\Npc"
#define LD_EFFECT			"\\Data\\3DData\\Effect"
#define LD_MINIMAP			"\\Data\\Interface\\2DImage\\Minimap"
#define LD_MAP				"\\Data\\3DData\\Map"
#define LD_CLOUD			"\\Data\\3DData\\Cloud"
#define LD_MAPOBJECT		"\\Data\\3DData\\MapObject"
// desc_hseos_³óÀå½Ã½ºÅÛ_01
// S ³óÀå½Ã½ºÅÛ Ãß°¡ added by hseos 2007.10.10
#define LD_FARM				"\\Data\\3DData\\Farm"
// E ³óÀå½Ã½ºÅÛ Ãß°¡ added by hseos 2007.10.10
#define LD_HOUSING			"\\Data\\3DData\\Housing"

#define DIRECTORYMGR	USINGTON(CDirectoryManager)

class CDirectoryManager  
{
	StaticString m_RootDirectory;
	StaticString m_Directory[eLM_Max];
	
	DWORD m_CurMode;

public:
//	//MAKESINGLETON(CDirectoryManager);

	CDirectoryManager();
	virtual ~CDirectoryManager();

	void Init();
	void Release();

	void SetLoadMode(DWORD LoadMode);
	char * GetFullDirectoryName(DWORD mode);
};
EXTERNGLOBALTON(CDirectoryManager)
#endif // !defined(AFX_DIRECTORYMANAGER_H__366E8598_49B1_4150_BFDE_9772A132F74B__INCLUDED_)
