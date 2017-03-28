/**************************************************************************************************/
/*!
\file	Switch.h
\par	Project: Coalescence
\date	2/6/2012
\authors
Primary  : Lincoln Tosi

\brief
Logic for a flexible switch that can activate/deactivate multiple game objects at once. Can be
set to trigger on holding the button down or for a set amount of time.

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#pragma once

#include "ILogical.h"
#include "VMath.h"

namespace Scuttlemeyer
{
	class Switch : public ILogical
	{
	public:
		Switch();
		virtual void Initialize();
		virtual void Serialize(TextLoader& stream);
		virtual int ChangeState(EState nState, bool veloReset = false);
		virtual void Update(float dt);
		virtual Mid::MessageIdType SendMessage(Message* message);
		virtual std::string ToString();
		virtual void ToBar();
		~Switch();

		void SetTimer(float time);
		void SetCollisionSide(int side); // (0) top, (1) left, (2) bottom, (3) right
		void Link(GOC* target);
		void UnLink(GOC* target);
		void UnLinkAll();
		void Activate(bool _override = false);
		void Deactivate(bool _override = false);
	private:
		std::vector<GOC*> activate_list;
		float hold_time;
		float max_time;
		int collision_side;
		int collision_flag;
		bool activation_override;
	};
}