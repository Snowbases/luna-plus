#pragma once

class cTimeDelay
{
protected:
	DWORD mStart;
	DWORD mDelay;
	DWORD mRemain;
	float mRate;
	BOOL mbStart;

public:
	cTimeDelay(void);
	virtual ~cTimeDelay(void);
	void Init(DWORD time) { mDelay = time; }
	void Start();
	DWORD Check();
	void End() { mStart = FALSE; }
	DWORD GetStartTime() const { return mStart; }
	DWORD GetDelayTime() const { return mDelay; }
	DWORD GetRemainTime() const { return mRemain; }
	float GetRemainRate() const { return mRate; }
	BOOL IsStart() const { return mbStart; }
};