/**************************************************************************************************/
/*!
\file	Switch.cpp
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
#include "Precompiled.h"
#include "Switch.h"
#include "GameLogic.h"

#include "Core.h"

#include "Transform.h"
#include "PhysBody.h"
#include "DebugConsole.h"
#include "ParticleEmitter.h"
#include "DebugBars.h"
#include "GameStateMgr.h"
#include "Factory.h"

#include "SoundNode.h"

namespace Scuttlemeyer
{
	/*****************************************************************************/
	/*!
		\brief
			Constructor - Initializes necessary variables.
	*/
	/*****************************************************************************/
	Switch::Switch()
	{
		mName = "Switch";
		hold_time = 0.0f;
		max_time = 0.2f;
		collision_side = 0;
		collision_flag = 4;
		mBIsActivated = false;
	}

	/*****************************************************************************/
	/*!
		\brief
			Initializes variables along with the switch image.
			Adds this object to the logic objects list.
	*/
	/*****************************************************************************/
	void Switch::Initialize()
	{
		activation_override = false;
		GetOwner()->has(Sprite)->SetAnimation(STOP, FORWARD);
		GetOwner()->has(Sprite)->mAnimActive = false;
		GetOwner()->has(Sprite)->GoToFrame(0.f);
		LOGIC->mLogicList.push_back(reinterpret_cast<ILogical*>(this));
	}

	/*****************************************************************************/
	/*!
		\brief
			Reads in configuration data from a file.

		\param stream
			The stream to read in from.
	*/
	/*****************************************************************************/
	void Switch::Serialize(TextLoader& stream)
	{
		TextLoader stream2;

		std::string name = "..//Content//Objects//";
		char tempChar[20];
		sprintf_s(tempChar, 20, "%i", LOGIC->curLevel);
		name.append(tempChar);
		name.append("//");
		name.append(LOGIC->mCurInstName);
		name.append(".swt.data");

		stream2.Open(name);

		if(stream2.IsGood())
		{
			StreamRead(stream2, max_time);
			SetTimer(max_time);
			StreamRead(stream2, collision_side);
			SetCollisionSide(collision_side);
		}

		while(stream2.IsGood())
		{
			std::string tempName;
			StreamRead(stream2, tempName);

			unsigned numObj = FACTORY->GetLastObjectId();

			for(unsigned i = 1; i < numObj + 1; ++i)
			{
				GOC* tempObj = FACTORY->GetObjectWithId(i);
				if(tempObj->mObjectName == tempName)
				{
					Link(tempObj);
				}
			}
		}
	}

	/*****************************************************************************/
	/*!
		\brief
			The state of switches is never changed.

		\param nState
			The state to change to.

		\param veloReset
			Whether or not to reset the objects velocity (only used in Block.cpp).
	*/
	/*****************************************************************************/
	int Switch::ChangeState(EState nState, bool veloReset)
	{
		return 0;
	}

	/*****************************************************************************/
	/*!
		\brief
			Called every frame. Checks if the button is being pressed or not and
			takes the appropriate action.

		\param dt
			The frame time.
	*/
	/*****************************************************************************/
	void Switch::Update(float dt)
	{
		PhysBody* sPhysics = ((PhysBody*)GetOwner()->GetComponent(CT_PhysBody));

			// if the switch is being pressed
			// potential problem: linking an object after activating
		if (sPhysics->mColFlags & collision_flag)
		{
			if (max_time > 0.f)
				hold_time = max_time;

			if (!mBIsActivated)
				Activate();

			sPhysics->mColFlags -= collision_flag;
		}
		else if (activation_override)
		{
			if (max_time > 0.f)
				hold_time = max_time;

			if (!mBIsActivated)
				Activate();
		}
		else
		{
			if (max_time > 0.f)
				hold_time -= dt;

			if (mBIsActivated && hold_time <= 0.0f)
				Deactivate();
		}
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
	Mid::MessageIdType Switch::SendMessage(Message* message)
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
	std::string Switch::ToString()
	{
		return "Logic Component: Switch";
	}

	/*****************************************************************************/
	/*!
		\brief
			Adds this object to the debug display.
	*/
	/*****************************************************************************/
	void Switch::ToBar()
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
	Switch::~Switch()
	{
		LOGIC->mLogicList.erase(reinterpret_cast<ILogical*>(this));
	}

	/*****************************************************************************/
	/*!
		\brief
			Sets the switch timer.

		\param time
			The time to set to.
	*/
	/*****************************************************************************/
	void Switch::SetTimer(float time)
	{
		max_time = time < 0.2f ? 0.2f : time;
	}

	/*****************************************************************************/
	/*!
		\brief
			Sets the collision flag variable based on the side being checked for
			collision.

		\param side
			The side top, right, bottom, left (0-3) to detect collision with.
	*/
	/*****************************************************************************/
	void Switch::SetCollisionSide(int side)
	{
			// (0) top, (1) left, (2) bottom, (3) right
		collision_side = (side < 4 && side >=0) ? side : 0;

			// CHANGE - magic numbers
		if (collision_side == 0)		// top
			collision_flag = 4;
		
		else if (collision_side == 2)	// bottom
			collision_flag = 2;
		//else if (collision_side == 1)	// left - don't know flag
		//	collision_flag = ???;
		//else if (collision_side == 3)	// right - don't know flag
		//	collision_flag = ???;
	}

	/*****************************************************************************/
	/*!
		\brief
			Keeps track of the objects to be activated when the switch is pressed.

		\param target
			The object to be added to the activate list.
	*/
	/*****************************************************************************/
	void Switch::Link(GOC* target)
	{
		activate_list.push_back(target);
	}

	/*****************************************************************************/
	/*!
		\brief
			Removes an object from the activate list so pressing the button doesn't
			affect it.

		\param target
			The object to be removed to the activate list.
	*/
	/*****************************************************************************/
	void Switch::UnLink(GOC* target)
	{
		for (unsigned i = 0; i < activate_list.size(); ++i)
		{
			if (activate_list[i] == target)
			{
				activate_list.erase(activate_list.begin() + i);
				break;
			}
		}
	}

	/*****************************************************************************/
	/*!
		\brief
			Removes all objects from the activate list so pressing the button
			doesn't affect them.
	*/
	/*****************************************************************************/
	void Switch::UnLinkAll()
	{
		activate_list.clear();
	}

	/*****************************************************************************/
	/*!
		\brief
			Activates all objects linked to this switch.

		\param _override
			Overrides collision detection in the update function so the switch
			stays activated.
	*/
	/*****************************************************************************/
	void Switch::Activate(bool _override)
	{
		GetOwner()->has(SoundNode)->Start(0);
		GetOwner()->has(Sprite)->GoToFrame(1.f);

		// Activate each of the links
		for (unsigned i = 0; i < activate_list.size(); ++i)
		{
			activate_list[i]->SendMessage(new Message(Mid::Activate, 0));
		}

		mBIsActivated = true;
		if (_override)
			activation_override = true; // override active
	}

	/*****************************************************************************/
	/*!
		\brief
			Deactivates all objects linked to this switch.

		\param _override
			Overrides collision detection in the update function so the switch
			stays deactivated.
	*/
	/*****************************************************************************/
	void Switch::Deactivate(bool _override)
	{
		GetOwner()->has(Sprite)->GoToFrame(0.f);

		// Deactivate each of the links
		for (unsigned i = 0; i < activate_list.size(); ++i)
		{
			activate_list[i]->SendMessage(new Message(Mid::Deactivate, 0));
		}

		mBIsActivated = false;
		if (_override)
			activation_override = false; // override active
	}
}