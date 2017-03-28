/**************************************************************************************************/
/*!
\file	Player.h
\par	Project: Coalescence
\date	10/1/2011
\authors
Primary  : Lincoln Tosi

\brief
Logic for everything the player does (jumping, shooting, etc.).

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#pragma once

#include "ILogical.h"
#include "Transform.h"
#include "PhysBody.h"
#include "CompSprite.h"

namespace Scuttlemeyer
{
	#define PLAYER_MAX_HEALTH 100
	class Player : public ILogical
	{
	public:
		Player();
		virtual void Initialize();
		virtual int ChangeState(EState nState, bool veloReset = false);
		virtual void Update(float dt);
		virtual Mid::MessageIdType SendMessage(Message* message);
		virtual void HandleInput(const float dt);
		virtual std::string ToString();
		virtual void ToBar();
		~Player();
	public:
		EState mFireState;
		DWORD mHealthTimer;
		float mHealth;
		float mOldHealth;
		int mTutFlag;
		bool mInvincible;
		int mFaceDir;
	private:
		void HealthRegen(void);
		PhysBody* pPhysics;
		Transform* pTransform;
		CompSprite* pDrawable;
	};
} 