/**************************************************************************************************/
/*!
\file	CreditsText.h
\par	Project: Coalescence
\date	4/11/2012
\authors
Primary : Kevin Verholtz

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#pragma once

#include "ILogical.h"

#include "Transform.h"
#include "Sprite.h"

namespace Scuttlemeyer
{
	class CreditsText : public ILogical
	{
	public:
		CreditsText();
		virtual void Initialize();
		virtual Mid::MessageIdType SendMessage(Message* message);
		virtual std::string ToString();
		virtual void ToBar();
		~CreditsText();
	private:
		Transform * mTransform;
		Sprite * mSprite;
	};
}