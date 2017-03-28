/**************************************************************************************************/
/*!
\file	Core.h
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

#pragma once

#include "../System/ISystem.h"

namespace Scuttlemeyer
{
	class CoreEngine
	{
	public:
		/// <summary>
		/// Engine constructor.
		/// </summary>
		/// <returns></returns>
		CoreEngine(void);

		/// <summary>
		/// Engine destructor.
		/// </summary>
		/// <returns></returns>
		~CoreEngine(void);

		/// <summary>
		/// Initializes all systems currently added to the engine.
		/// </summary>
		/// <returns></returns>
		void Initialize(void);

		/// <summary>
		/// The engine update loop.  Runs until the program exits.
		/// </summary>
		/// <returns></returns>
		void GameLoop(void);

		/// <summary>
		/// Adds a system to the engine.
		/// </summary>
		/// <param name="system">The system to add to the engine.</param>
		/// <returns></returns>
		void AddSystem(ISystem* system);

		/// <summary>
		/// Removes all systems currently added to the engine.
		/// </summary>
		/// <returns></returns>
		void DeleteSystems(void);

		/// <summary>
		/// Relays a sent message to the appropriate systems.
		/// </summary>
		/// <param name="msg">The message to broadcast.</param>
		/// <returns></returns>
		void BroadcastMessage(Message* msg);
	private:
		Timer mTimer;
		std::vector<ISystem*> mSystems;
	};
}