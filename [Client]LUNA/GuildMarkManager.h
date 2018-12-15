#pragma once


#define GUILDMARKMGR CGuildMarkManager::GetInstance()


class cImageSelf;
class CGuildMark;
class CGuildMarkImg;

class CGuildMarkManager  
{
	CYHHashTable<CGuildMark> m_MarkTable;

	CGuildMarkImg* m_pLoadingImg;

	CGuildMarkImg* LoadMarkImg(MARKNAMETYPE markname);	
	//void SendGuildMarkImgRequest(MARKNAMETYPE MarkName);
	//void SendGuildMarkImgUpdate(DWORD GuildIdx,char* pImgData);
	BOOL ReadBitInfoFromFile(char* filename,char* imgData);
	
	BOOL LoadMarkInfo();
public:
	static CGuildMarkManager* GetInstance();

	//MAKESINGLETON(CGuildMarkManager);
	CGuildMarkManager();
	virtual ~CGuildMarkManager();

	BOOL Init();
	void Release();

	CGuildMark* GetGuildMark(MARKNAMETYPE markname);

	//void OnRecvGuildMarkImg(MSG_GUILDMARK_IMG* pmsg);

	BOOL ChangeGuildMarkSyn(DWORD GuildIdx, const char* filename);
	void UpdateGuildMarkImg(DWORD GuildIdx,MARKNAMETYPE markname, const char* pImgData);

	CGuildMark* LoadGuildMarkImg( MARKNAMETYPE markname );

	
private:
	//typedef std::list< cImageSelf* >	ImageList;
	//ImageList							mImageList;

	const char * const mDirectory;
};