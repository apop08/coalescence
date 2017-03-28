/******************************************************************************/
/*!
\file   TutBox.h
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   11/20/2011
\brief  
	Logic control unit for tutorial boxes.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#pragma once

#include "ILogical.h"
#include "VMath.h"

namespace Scuttlemeyer
{
	class TutBox : public ILogical
	{
	public:
		TutBox();
		virtual void Initialize();
		virtual void Serialize(TextLoader& stream);
		virtual void Update(float dt);
		virtual Mid::MessageIdType SendMessage(Message* message);
		virtual std::string ToString();
		virtual void ToBar();
		~TutBox();
	private:
		Vec2 mTriggerPosition;
		float mTriggerRadius;
	};
}