/******************************************************************************/
/*!
\file   EntranceDoor.cpp
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM250
\date   3/24/2012
\brief  
	Logic control unit for level entrance doors.  Based on Kevin's work in BlockWin.
\copyright 
“All content © 2012 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#include "Precompiled.h"
#include "EntranceDoor.h"
#include "GameLogic.h"

#include "Core.h"

#include "SoundNode.h"

#include "DebugConsole.h"
#include "DebugBars.h"

namespace Scuttlemeyer
{
	/******************************************************************************/
	/*!
	  \brief
		Constructor sets mName to name of class
	*/
	/******************************************************************************/
	EntranceDoor::EntranceDoor()
	{
		mName = "EntranceDoor";
	}

	/******************************************************************************/
	/*!
	  \brief
		initialaizes this block
	*/
	/******************************************************************************/
	void EntranceDoor::Initialize()
	{
		GetOwner()->has(Sprite)->SetAnimation(PLAYONCE, FORWARD);
		GetOwner()->has(Sprite)->mAnimActive = true;
		GetOwner()->has(SoundNode)->Start(0);
	}

	/******************************************************************************/
	/*!
	  \brief
		Nothing happens here for this

	  \param dt
	    this is needed to overload the virtual function even though dt isn't used
	*/
	/******************************************************************************/
	void EntranceDoor::Update(float dt)
	{
	}

	void EntranceDoor::Activate()
	{
	}

	void EntranceDoor::Deactivate()
	{
	}

	/******************************************************************************/
	/*!
	  \brief
		empty for now maybe later will do something

	  \param message
	    message to process
	*/
	/******************************************************************************/
	Mid::MessageIdType EntranceDoor::SendMessage(Message* message)
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

	/******************************************************************************/
	/*!
	  \brief
		returns string of what this class is

	  \return
	    string of the class
	*/
	/******************************************************************************/
	std::string EntranceDoor::ToString()
	{
		return "Logic Component: EntranceDoor";
	}

	/******************************************************************************/
	/*!
	  \brief
		adds what this class is to the tweek bar
	*/
	/******************************************************************************/
	void EntranceDoor::ToBar()
	{
		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");

		TwAddVarRO(bar, "Logic Component", TW_TYPE_STDSTRING, &mName, " help='Type of logical component.' ");
	}

	/******************************************************************************/
	/*!
	  \brief
		destructor removes freed object from the list of logic objects
	*/
	/******************************************************************************/
	EntranceDoor::~EntranceDoor()
	{
	}
}