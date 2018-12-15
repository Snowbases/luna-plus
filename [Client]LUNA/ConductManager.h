#pragma once

class cConductInfo ;
class cConductBase ;

class cImage ;

#define CONDUCTMGR	cConductManager::GetInstance()

#define MAX_CONDUCT_COUNT	100

#define CONDUCT_FILE_NAME	"System/Resource/ConductList.bin"
#define EMOTICON_FILE_NAME	"System/Resource/Emoticon.bin"

// 070528 LYW --- ConductManager : Add struct for emoticon list.
struct EMOTICON
{
	WORD index ;

	char command[32] ;

	WORD effectNum ;
} ;

class cConductManager
{
	CYHHashTable<cConductInfo>	m_ConductInfoTable ;

	cConductInfo*				m_pConductInfo ;
public:
	cConductManager(void);
	virtual ~cConductManager(void);

	GETINSTANCE(cConductManager) ;

	void InitInfoTable() ;
	void ReleaseInfoTable() ;

	void LoadConductInfoList() ;

	cConductInfo* GetConductInfo( DWORD conductIdx ) ;

	WORD GetConductTooltipInfo( WORD conductInfo ) ;

	void RegistConduct() ;

	cConductBase* NewConductIcon( BYTE bIndex ) ;

	void GetIconImage(DWORD Conductldx, cImage * pLowImage , cImage * pHighImage) ;

	void SetToolTipIcon(cConductBase *pConduct) ;

	// 070220 LYW --- ConductManager : Add function to use conduct icon.
	BOOL OnConductCommand( DWORD idx ) ;

	// 070220 LYW --- ConductManager : Add function to use life skill.
	void LifeSkillCommand( DWORD idx ) ;
	// 070220 LYW --- ConductManager : Add function to use express emotion.
	void ExpressEmotion( DWORD idx ) ;

	// 070425 LYW --- ConductManager : Add function to use emoticon.
	void EmoticonCommand( DWORD idx ) ;
	void PetEmoticonCommand( DWORD idx ) ;

	// 070228 LYW --- ConductManager : Add function to send message to server.
	void SendToServer( int motionNum ) ;

	// 070228 LYW --- ConductManager : Add function to parsing message.
	void NetworkMsgParse( BYTE Protocol, void* pMsg );

	CYHHashTable<cConductInfo>* GetConductTable() { return &m_ConductInfoTable ; }

///////////////////////////////////////
//	EMOTICON PART
///////////////////////////////////////
private :
	cPtrList	m_EmoticonList ;

public :
	void	LoadEmoticonList() ;

	cPtrList* GetEmoticonList() { return &m_EmoticonList ; }
};
