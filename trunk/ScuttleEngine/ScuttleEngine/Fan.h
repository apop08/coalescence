/******************************************************************************/
/*!
\file   Fan.h
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM250
\date   4/04/2012
\brief  
	Logic control unit for on/off fans.
\copyright 
“All content © 2012 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#pragma once

#include "ILogical.h"

namespace Scuttlemeyer
{
	class Fan : public ILogical
	{
	public:
		Fan();
		virtual void Initialize();
		virtual void Serialize(TextLoader& stream);
		virtual void Update(float dt);
		void Activate();
		void Deactivate();
		virtual Mid::MessageIdType SendMessage(Message* message);
		virtual std::string ToString();
		virtual void ToBar();
		~Fan();
	private:
		bool mBStarted;
		float mDefRegPower;
		bool mBActivateIsActivate;
	};
}
