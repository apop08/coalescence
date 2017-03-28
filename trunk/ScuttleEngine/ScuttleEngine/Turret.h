/**************************************************************************************************/
/*!
\file	Turret.h
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
#include "PhysBody.h"
#include "GameLogic.h"

namespace Scuttlemeyer
{
	enum Scan
	{
		Up = 0,
		Down = 1
	};
	class Turret : public ILogical
	{
	public:
		Turret();
		virtual void Initialize();
		virtual int ChangeState(EState nState, bool veloReset = false);
		virtual void Update(float dt);
		virtual Mid::MessageIdType SendMessage(Message* message);
		virtual std::string ToString();
		virtual void ToBar();
		void RocketAttack(float dt);
		void LockedOn();
		void Scanning();
		bool BresenhamLine();
		bool TurretCol(Vec2 B1, PhysBody* B2);
		~Turret();
	public:
		bool  mLockedOn;
		bool  mScanDirection;
		Vec2 mOffsetCollided;
		float mRotSpeed;
		float mOriginalRotation;
		float mFieldOfView;
		EState mAttackState;
		DWORD mAttackTimer;
		DWORD mLockedOnTimer;
	private:
		int mShootCount;
		Transform* mPlayerTrans;
		Player* mPlayerPlayer;
		CompSprite * mCompSprite;
		Transform * mTransform;
	};
}