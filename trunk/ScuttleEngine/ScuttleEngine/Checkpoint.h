/******************************************************************************/
/*!
\file   Checkpoint.h
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM250
\date   1/23/2012
\brief  
	Logic control unit for checkpoints.
\copyright 
“All content © 2012 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#pragma once

#include "ILogical.h"
#include "Engine.h"

namespace Scuttlemeyer
{
	class Checkpoint : public ILogical
	{
	public:
		Checkpoint();
		virtual void Initialize();
		virtual void Serialize(TextLoader& stream);
		virtual void Update(float dt);
		virtual Mid::MessageIdType SendMessage(Message* message);
		virtual std::string ToString();
		virtual void ToBar();
		~Checkpoint();
	};
}
