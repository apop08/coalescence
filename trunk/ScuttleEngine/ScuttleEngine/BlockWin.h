/******************************************************************************/
/*!
\file   BlockWin.h
\author Kevin Verholtz
\par    email: kevin.verholtz\@digipen.edu
\par    DigiPen login: kevin.verholtz
\par    Course: GAM200
\date   12/8/2011
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/
#pragma once

#include "ILogical.h"
#include "VMath.h"

namespace Scuttlemeyer
{
	class BlockWin : public ILogical
	{
	public:
		BlockWin();
		virtual void Initialize();
		virtual void Update(float dt);
		void Activate();
		void Deactivate();
		virtual Mid::MessageIdType SendMessage(Message* message);
		virtual std::string ToString();
		virtual void ToBar();
		~BlockWin();
	private:
		bool mBIsWin;
	};
}