/**************************************************************************************************/
/*!
\file	KillZone.h
\par	Project: Coalescence
\date	2/29/2012
\authors
Primary  : Lincoln Tosi

\brief
Logic to kill the player if they enter one of these zones while playing.

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#pragma once

#include "ILogical.h"

namespace Scuttlemeyer
{
	class KillZone : public ILogical
	{
	public:
		KillZone();
		virtual void Initialize();
		virtual void Update(float dt);
		void Activate();
		void Deactivate();
		virtual Mid::MessageIdType SendMessage(Message* message);
		virtual std::string ToString();
		virtual void ToBar();
		~KillZone();
	private:
		bool CheckExtents();
	};
}
