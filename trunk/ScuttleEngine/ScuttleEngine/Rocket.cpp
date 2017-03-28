/**************************************************************************************************/
/*!
\file	Rocket.cpp
\par	Project: Coalescence
\date	4/11/2012
\authors
Primary : Kevin Verholtz

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#include "Precompiled.h"
#include "Rocket.h"
#include "GameStateMgr.h"
#include "GameLogic.h"
#include "DebugConsole.h"
#include "Camera.h"
#include "DebugBars.h"
#include "PhysBody.h"
#include "Core.h"

namespace Scuttlemeyer
{
	Rocket::Rocket()
	{
		mName = "Rocket";
		rocketset = false;
	}

	void Rocket::Initialize()
	{
		LOGIC->mLogicList.push_back(reinterpret_cast<ILogical*>(this));
		PhysBody* bPhysics = ((PhysBody*)GetOwner()->GetComponent(CT_PhysBody));
		bPhysics->mColFlags |= 1;
		rocketset = false;
	}

	void Rocket::Update(float dt)
	{
		ClearCollided();
		if(rocketset == false)
		{
			Transform * trans = GetOwner()->has(Transform);
			GetOwner()->has(PhysBody)->AddForce(60.f, ((trans->mRotation + 90) * cDelicious)/180.f);
			rocketset = true;
		}
	}
	Mid::MessageIdType Rocket::SendMessage(Message* message)
	{
		return Mid::None;
	}

	std::string Rocket::ToString()
	{
		return "Logic Component: Rocket";
	}

	int Rocket::SetCheckCollided()
	{
		if(mCollided == 0)
		{
			mCollided = 1;
			return 1;
		}
		return 0;
	}
	void Rocket::ClearCollided()
	{
		mCollided = 0;
	}

	void Rocket::ToBar()
	{
		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");

		TwAddVarRO(bar, "Logic Component", TW_TYPE_STDSTRING, &mName, " help='Type of logical component.' ");
	}

	Rocket::~Rocket()
	{
		LOGIC->mLogicList.erase(reinterpret_cast<ILogical*>(this));
	}
}