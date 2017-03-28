/**************************************************************************************************/
/*!
\file	ISystem.h
\par	Project: ScuttleEngine 2
\date	5/16/12
\authors
Primary  : Matt Sutherlin
\brief
	Interface for engine systems.

\copyright 
	“All content © 2012 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/

#pragma once

#include "Message.h"
#include "../Utility/Timer.h"

namespace Scuttlemeyer
{
	class ISystem
	{
	public:
		/// <summary>
		/// System destructor.
		/// </summary>
		/// <returns></returns>
		virtual ~ISystem() {};

		/// <summary>
		/// Initializes the system.  Should be called by the engine's Initialize.
		/// </summary>
		/// <returns></returns>
		virtual void Initialize() {};

		/// <summary>
		/// Updates the system.  Should be called by the engine's GameLoop.
		/// </summary>
		/// <param name="dt">The frame time of the current update loop.</param>
		/// <returns></returns>
		virtual void Update(float dt) = 0;

		/// <summary>
		/// Receives and processes messages sent from the core.
		/// </summary>
		/// <param name="msg">The message to process.</param>
		/// <returns></returns>
		virtual void SendMessage(Message* msg) {};

		/// <summary>
		/// Returns the name of the system.  Useful for identifying specific systems in the engine's system vector.
		/// </summary>
		/// <returns>The name of the system.</returns>
		virtual std::string GetName() = 0;
	protected:
		Timer mTimer;
	};
}