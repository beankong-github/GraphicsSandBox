#include "pch.h"
#include "GameTimer.h"


GameTimer::GameTimer()
	: mSecondPerCount(0.0)
	, mDeltatTime(-1.0)
	, mBaseTime(0.0)
	, mPausedTime(0)
	, mPrevTime(0)
	, mCurrentTime (0)
	, mStopTime(0)
	, mStopped(false)
{
}

float GameTimer::TotalTime() const
{
}
float GameTimer::DeltaTime() const
{
}

void GameTimer::Reset()
{

}
void GameTimer::Start()
{
}
void GameTimer::Stop()
{
}
void GameTimer::Tick()
{

}