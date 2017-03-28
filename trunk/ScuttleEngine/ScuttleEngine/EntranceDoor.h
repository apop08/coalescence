/******************************************************************************/
/*!
\file   EntranceDoor.h
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM250
\date   3/24/2012
\brief  
	Logic control unit for level entrance doors.  Based on Kevin's work in BlockWin.
\copyright 
“All content © 2012 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#pragma once

#include "ILogical.h"
#include "VMath.h"

namespace Scuttlemeyer
{
	class EntranceDoor : public ILogical
	{
	public:
		EntranceDoor();
		virtual void Initialize();
		virtual void Update(float dt);
		void Activate();
		void Deactivate();
		virtual Mid::MessageIdType SendMessage(Message* message);
		virtual std::string ToString();
		virtual void ToBar();
		~EntranceDoor();
	};
}