/**************************************************************************************************/
/*!
\file	BrokenPipe.h
\par	Project: Coalescence
\date	2/24/2012
\authors
Primary  : Lincoln Tosi

\brief
Logic for "broken" pipes that spew a special state changing gas.

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#pragma once

#include "ILogical.h"
#include "VMath.h"

namespace Scuttlemeyer
{
	class BrokenPipe : public ILogical
	{
	public:
		BrokenPipe();
		virtual void Initialize();
		virtual void Serialize(TextLoader& stream);
		virtual void Update(float dt);
		void Activate();
		void Deactivate();
		int GetSide(void);
		int GetState();
		virtual Mid::MessageIdType SendMessage(Message* message);
		virtual std::string ToString();
		virtual void ToBar();
		~BrokenPipe();
	private:
		void CalculateExtents();
		bool CheckExtents(GOC* parent);
	private:
		Vec2 mExtents;
		Vec2 maxExtents;
		Vec2 minExtents;
		int open_side;	// (0) top, (1) left, (2) bottom, (3) right
		int pipe_state;	// (0)Solid, (1)Liquid, (2)Gas, (3)Hot, (4)Warm, (5)Cold
		bool mBActivateIsActivate;
		bool mBStarted;
	};
}
