/**************************************************************************************************/
/*!
\file	Turret.cpp
\par	Project: Coalescence
\date	4/11/2012
\authors
Primary : Kevin Verholtz

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#include "Precompiled.h"
#include "Thwomp.h"
#include "GameLogic.h"
#include "DebugBars.h"
#include "PhysBody.h"
#define SMASH_FORCE 60.0f

namespace Scuttlemeyer
{
	/*****************************************************************************/
	/*!
	\brief
		Constructor
	*/
	/*****************************************************************************/
	Thwomp::Thwomp()
	{
		mName = "Thwomp";
	}

	/*****************************************************************************/
	/*!
	\brief
		Checks if collided on bottom
	*/
	/*****************************************************************************/
	int Thwomp::SetCheckCollided()
	{
		if(mCollided == 0)
		{
			mCollided = 1;
			return 1;
		}
		return 0;
	}

	/*****************************************************************************/
	/*!
	\brief
		Clears it
	*/
	/*****************************************************************************/
	void Thwomp::ClearCollided()
	{
		mCollided = 0;
	}

	/*****************************************************************************/
	/*!
	\brief
		Initializes Thwomp
	*/
	/*****************************************************************************/
	void Thwomp::Initialize()
	{
		LOGIC->mLogicList.push_back(reinterpret_cast<ILogical*>(this));
		PhysBody* pPhysics = ((PhysBody*)GetOwner()->GetComponent(CT_PhysBody));
		pPhysics->mColFlags |= 1;
		mOriginalPosition = GetOwner()->has(Transform)->mPosition;
		mAttackState = Done;
		mAttackTimer = timeGetTime();
	}

	/*****************************************************************************/
	/*!
	\brief
		Changes state of thwomp
	*/
	/*****************************************************************************/
	int Thwomp::ChangeState(EState nState, bool veloReset)
	{
		mAttackState = nState;
		return 1;
	}

	/*****************************************************************************/
	/*!
	\brief
		Updates the twomp to smash
	*/
	/*****************************************************************************/
	void Thwomp::Update(float dt)
	{
		if(timeGetTime() - mAttackTimer > 6000 && mAttackState == Done)
		{
			mAttackState = Attack;

			SmashAttack();

			mAttackTimer = timeGetTime();
		}
		else
		{
			if(mAttackState == None)
			{
				if(GetOwner()->has(Transform)->mPosition.y >= mOriginalPosition.y)
				{
					PhysBody* bPhysics = ((PhysBody*)GetOwner()->GetComponent(CT_PhysBody));
					bPhysics->mVelocity.x = 0.0f;
					bPhysics->mVelocity.y = 0.0f;
					mAttackState = Done;
				}
				else
				{
					MoveToOriginalPos();
				}
			}
		}
		ClearCollided();
	}

	/*****************************************************************************/
	/*!
	\brief
		SMASHHHH!!!
	*/
	/*****************************************************************************/
	void Thwomp::SmashAttack()
	{
		PhysBody* bPhysics = ((PhysBody*)GetOwner()->GetComponent(CT_PhysBody));
		//bPhysics->mPhysFlags = PhysFlags::FGravity;
		bPhysics->mVelocity.x = 0.0f;
		bPhysics->mVelocity.y = 0.0f;
		bPhysics->AddForce(SMASH_FORCE, 3 * cDelicious / 2);
	}

	/*****************************************************************************/
	/*!
	\brief
		Go back to pos then SMASH
	*/
	/*****************************************************************************/
	void Thwomp::MoveToOriginalPos()
	{
		PhysBody* bPhysics = ((PhysBody*)GetOwner()->GetComponent(CT_PhysBody));
		//bPhysics->mPhysFlags = PhysFlags::FGravity;
		bPhysics->mVelocity.x = 0.0f;
		bPhysics->mVelocity.y = 0.0f;
		bPhysics->AddForce(SMASH_FORCE / 2.5f, cDelicious / 2);
	}

	Mid::MessageIdType Thwomp::SendMessage(Message* message)
	{
		return Mid::None;
	}

	std::string Thwomp::ToString()
	{
		return "Logic Component: Thwomp";
	}

	void Thwomp::ToBar()
	{
		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");

		TwAddVarRO(bar, "Logic Component", TW_TYPE_STDSTRING, &mName, " help='Type of logical component.' ");
	}

	/*****************************************************************************/
	/*!
	\brief
		Destructor
	*/
	/*****************************************************************************/
	Thwomp::~Thwomp()
	{
		LOGIC->mLogicList.erase(reinterpret_cast<ILogical*>(this));
	}
}