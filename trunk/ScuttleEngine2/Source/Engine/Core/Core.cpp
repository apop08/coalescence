/**************************************************************************************************/
/*!
\file	Core.cpp
\par	Project: ScuttleEngine 2
\date	5/16/12
\authors
Primary  : Matt Sutherlin
\brief
	Core engine.  Runs all other engine systems.

\copyright 
	“All content © 2012 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/

#include "../../Precompiled.h"
#include "Core.h"

namespace Scuttlemeyer
{
	CoreEngine::CoreEngine(void)
	{
	}

	CoreEngine::~CoreEngine(void)
	{
	}

	void CoreEngine::Initialize(void)
	{
		for(unsigned int i = 0; i < mSystems.size(); ++i)
		{
			mSystems[i]->Initialize();
		}
	}

	void CoreEngine::GameLoop(void)
	{
	}

	void CoreEngine::AddSystem(ISystem *system)
	{
		mSystems.push_back(system);
	}

	void CoreEngine::DeleteSystems(void)
	{
		for(unsigned int i = 0; i < mSystems.size(); ++i)
		{
			delete mSystems[mSystems.size() - i - 1];
		}
	}

	void CoreEngine::BroadcastMessage(Message *msg)
	{
	}
}