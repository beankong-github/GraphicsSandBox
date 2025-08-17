#pragma once
#include "engine.h"

class Engine_API GameTimer
{
public:
	GameTimer();

	float TotalTime() const;
	float DeltaTime() const;

	void Reset();
	void Start();
	void Stop();
	void Tick();

private:
	double mSecondPerCount;
	double mDeltatTime;

	UINT64 mBaseTime;
	UINT64 mPausedTime;
	UINT64 mStopTime;
	UINT64 mPrevTime;
	UINT64 mCurrentTime;

	bool mStopped;
};
