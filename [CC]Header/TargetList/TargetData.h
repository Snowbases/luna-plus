// TargetData.h: interface for the CTargetData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TARGETDATA_H__EE370754_13CF_4C16_AFF2_4AB36480B13E__INCLUDED_)
#define AFX_TARGETDATA_H__EE370754_13CF_4C16_AFF2_4AB36480B13E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// 080910 LUJ, 블록 추가
#define TARGET_FLAG_MANADAMAGE		0x2000
#define TARGET_FLAG_BLOCK			0x1000
#define TARGET_FLAG_DECISIVE		0x0800
#define TARGET_FLAG_DISPELFROMEVIL	0x0400
#define TARGET_FLAG_VAMPIRIC_MANA	0x0200
#define TARGET_FLAG_VAMPIRIC_LIFE	0x0100
#define TARGET_FLAG_STUN			0x0080
#define TARGET_FLAG_DODGE			0x0040
#define TARGET_FLAG_CRITICAL		0x0020
#define TARGET_FLAG_LIFEDAMAGE		0x0010
#define TARGET_FLAG_COUNTERDAMAGE	0x0004
#define TARGET_FLAG_HEALLIFE		0x0002
#define TARGET_FLAG_RECHARGE		0x0001


typedef DWORD TARGETDATA;
#define MAX_TARGET_DATA	8

struct RESULTINFO
{
	DWORD mSkillIndex;
	BOOL bDispelFromEvil;
	BOOL bDodge;
	BOOL bCritical;
	BOOL bDecisive;
	// 080910 LUJ, 블록 추가
	BOOL bBlock;
	TARGETDATA RealDamage;
	TARGETDATA CounterDamage;
	TARGETDATA ManaDamage;
	TARGETDATA HealLife;
	TARGETDATA RechargeMana;
	TARGETDATA StunTime;
	TARGETDATA Vampiric_Life;
	TARGETDATA Vampiric_Mana;
};

class CTargetData  
{
	DWORD TargetID;
	WORD TargetFlag;
	BYTE TargetKind;

	TARGETDATA TargetData[MAX_TARGET_DATA];
	
public:
	CTargetData();
	~CTargetData();

	DWORD GetTargetID()	{	return TargetID;	}
	WORD GetTargetFlag()	{	return TargetFlag;	}
	BYTE GetTargetKind()	{	return TargetKind;	}
	void SetTarget(DWORD id,WORD flag,BYTE kind);
	
	TARGETDATA GetTargetData(int n)	{	return TargetData[n];	}
	void SetTargetData(int n,TARGETDATA data);

	int GetBaseTargetDataSize()	{	return sizeof(CTargetData)-(sizeof(WORD)*MAX_TARGET_DATA);	}
	int GetOneDataSize()	{	return sizeof(TARGETDATA);	}
	int GetTargetDataSize(int datanum)	{	return GetBaseTargetDataSize() + GetOneDataSize()*datanum;	}
};

#endif // !defined(AFX_TARGETDATA_H__EE370754_13CF_4C16_AFF2_4AB36480B13E__INCLUDED_)
