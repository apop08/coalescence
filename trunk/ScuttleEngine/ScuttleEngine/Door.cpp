/**************************************************************************************************/
/*!
\file	Door.cpp
\par	Project: Coalescence
\date	4/11/2012
\authors
Primary : Kevin Verholtz

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#include "Precompiled.h"
#include "Door.h"
#include "GameLogic.h"
#include "DebugBars.h"

namespace Scuttlemeyer
{
	/*****************************************************************************/
	/*!
	\brief
		Constructor
	*/
	/*****************************************************************************/
	Door::Door()
	{
		mName = "Door";
	}

	/*****************************************************************************/
	/*!
	\brief
		Initializes members
	*/
	/*****************************************************************************/
	void Door::Initialize()
	{
		LOGIC->mLogicList.push_back(reinterpret_cast<ILogical*>(this));
		mOriginalPosition = GetOwner()->has(Transform)->mPosition;
		mBIsActivated = false;
	}

	/*****************************************************************************/
	/*!
	\brief
		Opens or shuts door
	*/
	/*****************************************************************************/
	void Door::Update(float dt)
	{
		Transform * trans = GetOwner()->has(Transform);
		if(mBIsActivated)
		{
			if(trans->mPosition.y < mOriginalPosition.y + trans->mScale.y * GetOwner()->has(Sprite)->mSize.y)
			{
				trans->mPosition.y++;
			}
		}
		else
		{
			if(trans->mPosition.y > mOriginalPosition.y)
			{
				trans->mPosition.y--;
			}
		}
	}

	/*****************************************************************************/
	/*!
	\brief
		activate open
	*/
	/*****************************************************************************/
	void Door::Activate()
	{
		mBIsActivated = true;
	}


	/*****************************************************************************/
	/*!
	\brief
		deactivate close
	*/
	/*****************************************************************************/
	void Door::Deactivate()
	{

		mBIsActivated = false;
	}

	Mid::MessageIdType Door::SendMessage(Message* message)
	{
		if(message->mMessageId == Mid::Activate)
		{
			Activate();
		}
		else if(message->mMessageId == Mid::Deactivate)
		{
			Deactivate();
		}

		return Mid::None;
	}

	std::string Door::ToString()
	{
		return "Logic Component: Door";
	}

	void Door::ToBar()
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
	Door::~Door()
	{
		LOGIC->mLogicList.erase(reinterpret_cast<ILogical*>(this));
	}
}