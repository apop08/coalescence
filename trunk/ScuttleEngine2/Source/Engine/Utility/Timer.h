/**************************************************************************************************/
/*!
\file	Timer.h
\par	Project: ScuttleEngine 2
\date	5/16/12
\authors
Primary  : Matt Sutherlin
\brief
	Wrapper and logic for the performance counter.

\copyright 
	“All content © 2012 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/

#pragma once

#include "../../Precompiled.h"

namespace Scuttlemeyer
{
	class Timer
	{
	public:
		/// <summary>
		/// Timer constructor.
		/// </summary>
		/// <returns></returns>
		Timer(void) {
			LARGE_INTEGER lLI;
			QueryPerformanceFrequency(&lLI);

			mFreq = double(lLI.QuadPart);
		}

		/// <summary>
		/// Timer destructor.
		/// </summary>
		/// <returns></returns>
		~Timer(void) {};

		/// <summary>
		/// Starts the timer.  Calling it subsequently will restart the timer.
		/// </summary>
		/// <returns></returns>
		void StartTimer(void) {
			LARGE_INTEGER lLI;
			QueryPerformanceCounter(&lLI);

			mStartTime = lLI.QuadPart;

			mTime = 0.0;
		}

		/// <summary>
		/// Gets the current elapsed time.
		/// </summary>
		/// <returns>The time since the last call of StartTimer or GetTimer, whichever was most recent.</returns>
		double GetTime(void) {
			LARGE_INTEGER lLI;
			QueryPerformanceCounter(&lLI);

			double lTime = (double(lLI.QuadPart - mStartTime) / mFreq) - mTime;

			mTime = lTime;

			return lTime;
		}
	private:
		double mTime;
		double mFreq;
		__int64 mStartTime;
	};
}