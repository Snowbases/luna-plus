#include "stdafx.h"
#include "WindowIDEnum.h"
#include "cBrowser.h"
#include ".\cHousingWebDlg.h"
#include <Wincrypt.h>
#include "ObjectManager.h"
#include "MacroManager.h"

cHousingWebDlg::cHousingWebDlg(void)
{
	m_pBrowser = NULL;
}

cHousingWebDlg::~cHousingWebDlg(void)
{
}

void cHousingWebDlg::Linking()
{
	m_pBrowser = (cBrowser*)GetWindowForID( ISD_BROWSER );
	SetAlwaysTop( TRUE );
}

void cHousingWebDlg::SetActive( BOOL val )
{
	cDialog::SetActiveRecursive( val );

	if(val == FALSE)
	{
		MACROMGR->ForceSetFocus();
	}
}

void cHousingWebDlg::Navigate( char* pURL )
{
	m_pBrowser->Navigate( pURL );
}

extern int g_nServerSetNum;
void cHousingWebDlg::OpenMiniHomePage(DWORD dwOwnerUserIndex)
{
	if(IsActive())
	{
		SetActive(FALSE);
		return;
	}

	if(0 == dwOwnerUserIndex)
		return;

	char csBuffer[1024];
	char csBufferMsg[1024];
	char csDigest[1024];
	DWORD dwTickCount = GetTickCount();
	const DWORD dwSecretNum = 17;		//웹서버에서도 동일한 암호숫자 상수값 사용함. 변경시 웹서버에서도 수정되어야 함.

	sprintf(
		csBuffer,
		"%|%d%|%s%|%d%|%s%|%d",
		dwTickCount,
		g_szHeroIDName,
		g_nServerSetNum,
		HERO->GetObjectName(),
		dwOwnerUserIndex);
	sprintf(
		csBufferMsg,
		 "%|%d%|%s%|%d%|%s%|%d",
		dwTickCount + dwSecretNum,
		g_szHeroIDName,
		g_nServerSetNum,
		HERO->GetObjectName(),
		dwOwnerUserIndex);

	HCRYPTPROV hCryptProv = 0;
	HCRYPTHASH hHash = 0;
	BYTE bHash[0x7f] = {0};
	DWORD dwHashLen= 16; // The MD5 algorithm always returns 16 bytes. 
	DWORD cbContent= strlen(csBuffer);
	BYTE* pbContent= (BYTE*)csBuffer;


	if(CryptAcquireContext(&hCryptProv, 
		NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_MACHINE_KEYSET)) 
	{

		if(CryptCreateHash(hCryptProv, 
			CALG_MD5,	// algorithm identifier definitions see: wincrypt.h
			0, 0, &hHash)) 
		{
			if(CryptHashData(hHash, pbContent, cbContent, 0))
			{

				if(CryptGetHashParam(hHash, HP_HASHVAL, bHash, &dwHashLen, 0)) 
				{
					// Make a string version of the numeric digest value
					//csDigest.Empty();
					memset( csDigest, 0, sizeof(char)*1024 );
					//CString tmp;
					char tmp[32];
					for (int i = 0; i<16; i++)
					{
						sprintf( tmp, "%02x", bHash[i] );
						strcat( csDigest, tmp );
					}

				}
				//else csDigest=_T("Error getting hash param"); 
			}
			//else csDigest=_T("Error hashing data"); 
		}
		//else csDigest=_T("Error creating hash"); 
	}
	//else csDigest=_T("Error acquiring context"); 


	CryptDestroyHash(hHash); 
	CryptReleaseContext(hCryptProv, 0); 

	char URL[1024] = {0,};
	sprintf( URL, "%s%s%|%s", "http://lunaplus.enpang.com/Housing/default.asp?utt=", csBufferMsg, csDigest);

	Navigate( URL );
	SetActive( TRUE );
}