/******************************************************************************/
/*!
\file   BiDirectionalFan.h
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM250
\date   2/10/2012
\brief  
	Logic control unit for reversable fans.
\copyright 
“All content © 2012 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#pragma once

#include "ILogical.h"

namespace Scuttlemeyer
{
	class BiDirectionalFan : public ILogical
	{
	public:
		BiDirectionalFan();
		virtual void Initialize();
		virtual void Serialize(TextLoader& stream);
		virtual void Update(float dt);
		void Activate();
		void Deactivate();
		virtual Mid::MessageIdType SendMessage(Message* message);
		virtual std::string ToString();
		virtual void ToBar();
		~BiDirectionalFan();
	private:
		bool mBStarted;
		float mDefRegPower;
	};
}
