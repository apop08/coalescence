/**************************************************************************************************/
/*!
\file	Bullet.cpp
\par	Project: Coalescence
\date	10/3/2011
\authors
Primary  : Matt Sutherlin
Secondary: Lincoln Tosi

\brief
Logic for the bullets fired by the player.

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#include "Precompiled.h"
#include "Bullet.h"
#include "GameLogic.h"
#include "GameStateMgr.h"

#include "Core.h"

#include "Transform.h"
#include "PhysBody.h"
#include "CompSprite.h"
#include "DebugConsole.h"
#include "Camera.h"
#include "DebugBars.h"

namespace Scuttlemeyer
{
	/*****************************************************************************/
	/*!
		\brief
			Constructor - Initializes necessary variables.
	*/
	/*****************************************************************************/
	Bullet::Bullet()
	{
		mName = "Bullet";
	}

	/*****************************************************************************/
	/*!
		\brief
			Initializes variables.
			Adds this object to the logic objects list.
	*/
	/*****************************************************************************/
	void Bullet::Initialize()
	{
		mCollided = false;
		LOGIC->mLogicList.push_back(reinterpret_cast<ILogical*>(this));
		PhysBody* bPhysics = ((PhysBody*)GetOwner()->GetComponent(CT_PhysBody));
		bPhysics->mColFlags |= 1;
	}

	/*****************************************************************************/
	/*!
		\brief
			Hot bullets don't collide with blocks in the gas state.

		\param nState
			The state to change to.

		\param veloReset
			Whether or not to reset the objects velocity (only used in Block.cpp).
	*/
	/*****************************************************************************/
	int Bullet::ChangeState(EState nState, bool veloReset)
	{
		if (GetOwner()->state == Hot && nState == Gas)
			return 1;

		PhysBody * bPhysics = (PhysBody*)GetOwner()->GetComponent(CT_PhysBody);
		bPhysics->mVelocity.x = 0;
		bPhysics->mVelocity.y = 0;

		return 0;
	}

	/*****************************************************************************/
	/*!
		\brief
			Called every frame.

		\param dt
			The frame time.
	*/
	/*****************************************************************************/
	void Bullet::Update(float dt)
	{
	}

	/*****************************************************************************/
	/*!
		\brief
			Used to prevent multiple bullet collisions.

		\return
			If the bullet has already collided this frame.
	*/
	/*****************************************************************************/
	bool Bullet::CheckCollided()
	{
		return mCollided;
	}

	/*****************************************************************************/
	/*!
		\brief
			Keeps track of bullet collisions.
	*/
	/*****************************************************************************/
	void Bullet::SetCollided(bool collided)
	{
		mCollided = collided;
	}

	/*****************************************************************************/
	/*!
		\brief
			Recieves messages sent from other places in the code.

		\param message
			The message being recieved.

		\return
			The message type to be deleted.
	*/
	/*****************************************************************************/
	Mid::MessageIdType Bullet::SendMessage(Message* message)
	{
		return Mid::None;
	}

	/*****************************************************************************/
	/*!
		\brief
			Converts the type of this object into a string.

		\return
			The string containing this object type.
	*/
	/*****************************************************************************/
	std::string Bullet::ToString()
	{
		return "Logic Component: Bullet";
	}

	/*****************************************************************************/
	/*!
		\brief
			Adds this object to the debug display.
	*/
	/*****************************************************************************/
	void Bullet::ToBar()
	{
		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");

		TwAddVarRO(bar, "Logic Component", TW_TYPE_STDSTRING, &mName, " help='Type of logical component.' ");
	}

	/*****************************************************************************/
	/*!
		\brief
			Deconstructor - removes this object from the object list.
	*/
	/*****************************************************************************/
	Bullet::~Bullet()
	{
		LOGIC->mLogicList.erase(reinterpret_cast<ILogical*>(this));
	}
}