/**************************************************************************************************/
/*!
\file	Thwomp.h
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
	class Thwomp : public ILogical
	{
	public:
		Thwomp();
		virtual void Initialize();
		virtual int ChangeState(EState nState, bool veloReset = false);
		virtual void Update(float dt);
		virtual Mid::MessageIdType SendMessage(Message* message);
		virtual std::string ToString();
		virtual void ToBar();
		void SmashAttack();
		void MoveToOriginalPos();
		int SetCheckCollided();
		~Thwomp();
	private:
		void ClearCollided();
	public:
		Vec2  mOriginalPosition;
		EState mAttackState;
		DWORD mAttackTimer;
	private:
		int mCollided;
	};
}