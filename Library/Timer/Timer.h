#pragma once

class Timer
{
public:
	UINT FrameRate;

	LARGE_INTEGER PerformanceFrequency;

	LARGE_INTEGER PreviousPerformanceCounter;
	LARGE_INTEGER CurrentPerformanceCounter;
	LARGE_INTEGER PostPerformanceCounter;
	
	LARGE_INTEGER FramePeriod;
	LARGE_INTEGER FrameDelay;
	LARGE_INTEGER FrameError;

	UINT CurrentLoopCount;
	UINT CurrentUpdateCount;
	UINT CurrentRenderCount;

	BOOL Updatable;
	BOOL Renderable;

public:
	Timer();
	virtual ~Timer();

	bool Initialize();
	bool Update();
	bool Render();
	bool Termimalize();



	BOOL CheckUpdate();
	BOOL CheckRender();
	BOOL CheckElapsedTime();



	UINT GetFrameRate() { return FrameRate; }

	LARGE_INTEGER GetPerformanceFrequency() { return PerformanceFrequency; }

	LARGE_INTEGER GetFramePeriod() { return FramePeriod; }
	LARGE_INTEGER GetFrameDelay() { return FrameDelay; }
	LARGE_INTEGER GetFrameError() { return FrameError; }

	UINT GetCurrentLoopCount() { return CurrentLoopCount; }
	UINT GetCurrentUpdateCount() { return CurrentUpdateCount; }
	UINT GetCurrentRenderCount() { return CurrentRenderCount; }

	BOOL GetUpdatable() { return Updatable; }
	BOOL GetRenderable() { return Renderable; }
};

extern Timer TIMER;