/**************************************************************************************************/
/*!
\file	Static.cpp
\par	Project: Coalescence
\date	11/2/2011
\authors
Primary  : Lincoln Tosi

\brief
Logic object for all static blocks.

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#include "Precompiled.h"
#include "Static.h"
#include "GameLogic.h"

#include "Core.h"

#include "DebugConsole.h"
#include "DebugBars.h"

namespace Scuttlemeyer
{
	/*****************************************************************************/
	/*!
		\brief
			Constructor - Initializes necessary variables.
	*/
	/*****************************************************************************/
	Static::Static()
	{
		mName = "Static";
	}

	/*****************************************************************************/
	/*!
		\brief
			Adds this object to the logic objects list.
	*/
	/*****************************************************************************/
	void Static::Initialize()
	{
		LOGIC->mLogicList.push_back(reinterpret_cast<ILogical*>(this));
	}

	/*****************************************************************************/
	/*!
		\brief
			Static objects don't change states.

		\param nState
			The state to change to.

		\param veloReset
			Whether or not to reset the objects velocity (only used in Block.cpp).
	*/
	/*****************************************************************************/
	int Static::ChangeState(EState nState, bool veloReset)
	{
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
	void Static::Update(float dt)
	{
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
	Mid::MessageIdType Static::SendMessage(Message* message)
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
	std::string Static::ToString()
	{
		return "Logic Component: Static";
	}

	/*****************************************************************************/
	/*!
		\brief
			Adds this object to the debug display.
	*/
	/*****************************************************************************/
	void Static::ToBar()
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
	Static::~Static()
	{
		LOGIC->mLogicList.erase(reinterpret_cast<ILogical*>(this));
	}
}