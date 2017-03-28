/**************************************************************************************************/
/*!
\file	Door.h
\par	Project: Coalescence
\date	3/11/2012
\authors
Primary : Kevin Verholtz

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#pragma once

#include "ILogical.h"
#include "VMath.h"

namespace Scuttlemeyer
{
	class Door : public ILogical
	{
	public:
		Door();
		virtual void Initialize();
		virtual void Update(float dt);
		void Activate();
		void Deactivate();
		virtual Mid::MessageIdType SendMessage(Message* message);
		virtual std::string ToString();
		virtual void ToBar();
		~Door();
	public:
		Vec2  mOriginalPosition;
	};
}