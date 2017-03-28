/**************************************************************************************************/
/*!
\file	KillZone.cpp
\par	Project: Coalescence
\date	2/29/2012
\authors
Primary  : Lincoln Tosi

\brief
Logic to kill the player if they enter one of these zones while playing.

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#include "Precompiled.h"
#include "KillZone.h"

#include "GameLogic.h"
#include "DebugBars.h"

namespace Scuttlemeyer
{
	/*****************************************************************************/
	/*!
		\brief
			Constructor - Initializes necessary variables.
	*/
	/*****************************************************************************/
	KillZone::KillZone()
	{
		mName = "KillZone";
	}

	/*****************************************************************************/
	/*!
		\brief
			Initializes variables.
			Adds this object to the logic objects list.
	*/
	/*****************************************************************************/
	void KillZone::Initialize()
	{
		mBIsActivated = true;
		LOGIC->mLogicList.push_back(reinterpret_cast<ILogical*>(this));
	}

	/*****************************************************************************/
	/*!
		\brief
			Called every frame. If the player enters a kill zone, kill the player.

		\param dt
			The frame time.
	*/
	/*****************************************************************************/
	void KillZone::Update(float dt)
	{
		if (mBIsActivated && CheckExtents())
		{
			// kill player
			LOGIC->DamagePlayer(PLAYER_MAX_HEALTH);
		}
	}

	/*****************************************************************************/
	/*!
		\brief
			Checks if a block is inside the region.

		\param parent
			The object to check against.

		\return
			true if inside, false if not.
	*/
	/*****************************************************************************/
	bool KillZone::CheckExtents()
	{
		Transform* kTransform = ((Transform*)GetOwner()->GetComponent(CT_Transform));
		Transform* pTransform = ((Transform*)LOGIC->mPlayer->GetComponent(CT_Transform));

		if (kTransform && pTransform)
		{
			float width = kTransform->mScale.x / 2.0f;
			float height = kTransform->mScale.y / 2.0f;

				// check only the midpoint (can change later if precision is needed)
			if (pTransform->mPosition.x < (kTransform->mPosition.x + width)
			 && pTransform->mPosition.x > (kTransform->mPosition.x - width)
			 && pTransform->mPosition.y < (kTransform->mPosition.y + height)
			 && pTransform->mPosition.y > (kTransform->mPosition.y - height))
				return true;
		}
		return false;
	}

	/*****************************************************************************/
	/*!
		\brief
			Turns on the kill zone.
	*/
	/*****************************************************************************/
	void KillZone::Activate()
	{
		if (!mBIsActivated)
			mBIsActivated = true;
	}

	/*****************************************************************************/
	/*!
		\brief
			Turns off the kill zone.
	*/
	/*****************************************************************************/
	void KillZone::Deactivate()
	{
		if (mBIsActivated)
			mBIsActivated = false;
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
	Mid::MessageIdType KillZone::SendMessage(Message* message)
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
	std::string KillZone::ToString()
	{
		return "Logic Component: KillZone";
	}

	/*****************************************************************************/
	/*!
		\brief
			Adds this object to the debug display.
	*/
	/*****************************************************************************/
	void KillZone::ToBar()
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
	KillZone::~KillZone()
	{
		LOGIC->mLogicList.erase(reinterpret_cast<ILogical*>(this));
	}
}