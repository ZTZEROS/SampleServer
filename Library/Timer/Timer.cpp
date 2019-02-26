#include "FirstOrderLibrary.h"
#include "Timer.h"

Timer TIMER;

Timer::Timer() {}
Timer::~Timer() {}

bool Timer::Initialize()
{
	FrameRate = TIMER_FRAME_RATE;

	QueryPerformanceFrequency(&PerformanceFrequency);

	QueryPerformanceCounter(&CurrentPerformanceCounter);
	PreviousPerformanceCounter = CurrentPerformanceCounter;
	PostPerformanceCounter.QuadPart = CurrentPerformanceCounter.QuadPart + PerformanceFrequency.QuadPart;

	FramePeriod.QuadPart = PerformanceFrequency.QuadPart / FrameRate;
	FrameError.QuadPart = PerformanceFrequency.QuadPart - FramePeriod.QuadPart * FrameRate;
	FrameDelay.QuadPart = 0;

	CurrentLoopCount = 0;
	CurrentUpdateCount = 0;
	CurrentRenderCount = 0;

	Updatable = 0;
	Renderable = 0;

	timeBeginPeriod(1);

	return 0;
}

bool Timer::Update()
{
	LARGE_INTEGER currentElapsedCounter;

	QueryPerformanceCounter(&CurrentPerformanceCounter);

	currentElapsedCounter.QuadPart = (CurrentPerformanceCounter.QuadPart - PreviousPerformanceCounter.QuadPart);

	++CurrentLoopCount;

	if (FramePeriod.QuadPart < currentElapsedCounter.QuadPart)
	{
		Updatable = 1;
		//Renderable = 1;

		PreviousPerformanceCounter.QuadPart += FramePeriod.QuadPart;
	}
	else
	{
		Updatable = 0;
		//Renderable = 0;
	}

	if (PostPerformanceCounter.QuadPart < CurrentPerformanceCounter.QuadPart)
	{
		//Updatable = 1;
		Renderable = 1;

		PostPerformanceCounter.QuadPart += PerformanceFrequency.QuadPart;
	}
	else
	{
		//Updatable = 0;
		Renderable = 0;
	}

	return 0;
}

bool Timer::Render()
{
	if(Renderable)
	{
		printf(
			"CurrentLoopCount : %d, "
			"CurrentUpdateCount : %d, "
			"CurrentRenderCount : %d "
			CRLF
			,
			CurrentLoopCount,
			CurrentUpdateCount,
			CurrentRenderCount
			);

		CurrentLoopCount = 0;
		CurrentUpdateCount = 0;
		CurrentRenderCount = 0;
	}

	return 0;
}

bool Timer::Termimalize()
{
	timeEndPeriod(1);

	return 0;
}



BOOL Timer::CheckUpdate() //Updatablity
{
	if (Updatable) ++CurrentUpdateCount;

	return Updatable;
}

BOOL Timer::CheckRender() //Renderability
{
	if (Renderable) ++CurrentRenderCount;

	return Renderable;
}

BOOL Timer::CheckElapsedTime()
{
	LARGE_INTEGER currentElapsedCounter;
	LARGE_INTEGER currentPerformanceCounter;

	PreviousPerformanceCounter = CurrentPerformanceCounter;
	QueryPerformanceCounter(&CurrentPerformanceCounter);

	currentElapsedCounter.QuadPart = (CurrentPerformanceCounter.QuadPart - PreviousPerformanceCounter.QuadPart);

	if (PostPerformanceCounter.QuadPart < CurrentPerformanceCounter.QuadPart)
	{
		CurrentUpdateCount = 0;
		CurrentRenderCount = 0;
		PostPerformanceCounter.QuadPart += PerformanceFrequency.QuadPart;
	}

	++CurrentUpdateCount;

	if (FramePeriod.QuadPart < FrameDelay.QuadPart)
	{
		FrameDelay.QuadPart -= FramePeriod.QuadPart;

		return 0;
	}
	else
	{
		++CurrentRenderCount;

		if (FramePeriod.QuadPart < currentElapsedCounter.QuadPart)
		{
			FrameDelay.QuadPart += currentElapsedCounter.QuadPart - FramePeriod.QuadPart;
		}
		else
		{
			//busy waiting //deprecated
			//do
			//{
			//	QueryPerformanceCounter(&currentPerformanceCounter);
			//}
			//while (currentPerformanceCounter.QuadPart < CurrentPerformanceCounter.QuadPart + (FramePeriod.QuadPart - currentElapsedCounter.QuadPart));

			Sleep(1000 * (FramePeriod.QuadPart - currentElapsedCounter.QuadPart) / PerformanceFrequency.QuadPart);
		}

		CurrentPerformanceCounter.QuadPart += (FramePeriod.QuadPart - currentElapsedCounter.QuadPart);

		return 1;
	}
}