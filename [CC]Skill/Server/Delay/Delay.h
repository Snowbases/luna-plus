#pragma once

enum DELAY_TYPE
{
	DELAY_NONE,
	DELAY_TIME,
	DELAY_SCOPE,
	DELAY_MAX,
};

class cDelay
{
	BOOL			mbStart;
	DWORD			mStart;
	DWORD			mDelay;
    DWORD			mRemain;
	float			mRate;

public:
	cDelay(void);
	virtual ~cDelay(void);

	void			Init( DWORD time );
	void			Start();
	DWORD			Check();
	void			End();

	BOOL	IsStart();
	DWORD	GetStartTime();
	DWORD	GetDelayTime();
	DWORD	GetRemainTime();
	float	GetRemainRate();
};
