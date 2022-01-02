#include "Timer.h"

Timer::Timer()
{
	mSecondsPerCount = 0.0;
	mDeltaTime = -1.0;
	mBaseTime = 0;
	mPausedTime = 0;
	mStopTime = 0;
	mPreviousTime = 0;
	mCurrentTime = 0;
	mPaused = false;

	__int64 countsPerSecond;

	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond);
	mSecondsPerCount = 1.0 / (double)countsPerSecond;
}

float Timer::DeltaTime()
{
	return (float)mDeltaTime;
}

void Timer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	if (mPaused)
	{
		mPausedTime += startTime - mStopTime;

		mPreviousTime = startTime;
		mStopTime = 0;
		mPaused = false;
	}
}

void Timer::Stop()
{
	if (!mPaused)
	{
		__int64 currentTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

		mStopTime = currentTime;
		mPaused = true;
	}
}

void Timer::Reset()
{
	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	mBaseTime = currentTime;
	mPreviousTime = currentTime;
	mStopTime = 0;
	mPaused = false;
}

void Timer::Tick()
{
	if (mPaused)
	{
		mDeltaTime = 0.0;
		return;
	}

	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	mCurrentTime = currentTime;

	mDeltaTime = (mCurrentTime - mPreviousTime) * mSecondsPerCount;

	mPreviousTime = mCurrentTime;
}

float Timer::TotalTime()
{
	if (mPaused)
	{
		return (float)((mStopTime - mPausedTime - mBaseTime) * mSecondsPerCount);
	}
	else
	{
		return (float)((mCurrentTime - mPausedTime - mBaseTime) * mSecondsPerCount);
	}
}