/**************************************************************************************************/
/*!
\file	bNewGame.cpp
\par	Project: Coalescence
\date	12/6/2011
\authors
Primary : Lincoln Tosi
Secondary: Kevin Verholtz

\brief
A button that starts a new game from level 1.

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#include "Precompiled.h"
#include "bNewGame.h"
#include "GameLogic.h"

#include "Core.h"

#include "Transform.h"
#include "DebugBars.h"
#include "Graphics.h"
#include "GameStateMgr.h"

#include "Camera.h"

#include "SoundNode.h"

namespace Scuttlemeyer
{
	/*****************************************************************************/
	/*!
		\brief
			Constructor - Initializes necessary variables.
	*/
	/*****************************************************************************/
	bNewGame::bNewGame()
	{
		mName = "bNewGame";
		selected = false;
	}

	/*****************************************************************************/
	/*!
		\brief
			Initializes variables.
			Adds this object to the logic objects list.
	*/
	/*****************************************************************************/
	void bNewGame::Initialize()
	{
		LOGIC->mButtonList.push_back(reinterpret_cast<ILogical*>(this));
		FACTORY->RegisterForMessages(GetOwner());
	}

	/*****************************************************************************/
	/*!
		\brief
			Checks if the mouse is currently positioned over the button.

		\return
			true if the mouse is over, false if it's not.
	*/
	/*****************************************************************************/
	bool bNewGame::isMouseOn()
	{
		Transform* bTransform = ((Transform*)GetOwner()->GetComponent(CT_Transform));
		Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
		float mx = CORE->MouseInput.MousePosition.x - GRAPHICS->mScreenWidth / 2.f;
		float my = CORE->MouseInput.MousePosition.y - GRAPHICS->mHeightRatio * GRAPHICS->mCameraOffset - GRAPHICS->mScreenHeight / 2.f;
		float dx, dy;

		if(gGameStateCurr == GS_PAUSE)
		{
			Transform* pTransform = GRAPHICS->mCurrentCamera->GetOwner()->has(Transform);
			dx = abs(mx - GRAPHICS->mWidthRatio * bTransform->mPosition.x + GRAPHICS->mWidthRatio * pTransform->mPosition.x);	// CHANGE
			dy = abs(my + GRAPHICS->mHeightRatio * bTransform->mPosition.y - GRAPHICS->mHeightRatio * pTransform->mPosition.y);	// CHANGE
		}
		else
		{
			dx = abs(mx - GRAPHICS->mWidthRatio * bTransform->mPosition.x);	// CHANGE
			dy = abs(my + GRAPHICS->mHeightRatio * bTransform->mPosition.y);	// CHANGE
		}

		if (dx < GRAPHICS->mWidthRatio * bSprite->mSize.x * bTransform->mScale.x / 2.f && dy < GRAPHICS->mHeightRatio * bSprite->mSize.y * bTransform->mScale.y / 2.f)
			return true;
		else
			return false;
	}

	/*****************************************************************************/
	/*!
		\brief
			Process the button logic.
	*/
	/*****************************************************************************/
	void bNewGame::Triggered()
	{
		//GetOwner()->has(SoundNode)->Start();
		//Message* q = new Message(Mid::DebugCNSL, 1, new std::string("Button Pressed, enter 'PLAY' state."));
		//CORE->EnqueueMessage(q);

		//LOGIC->curLevel = 0;
		////////////////LOGIC->mBGameEnd = false;
		////////////////LOGIC->mBDied = false;
		////////////////LOGIC->nextState = GS_PLAY;
		////////////////LOGIC->curTransition = 1;
		//LOGIC->mBCheckpointActive = false;
		
		//change menu to how to play menu
		CORE->mLevelSelected = 1;
		LOGIC->curMenu = 5;
		GetOwner()->has(SoundNode)->Start();
		Message * q;
		q = new Message(Mid::Reload, 0);//reload new menu screen
		CORE->EnqueueMessage(q);

		//q = new Message(Mid::Transition, 0);
		//CORE->EnqueueMessage(q);
		/*q = new Message(Mid::Play, 1, 0);
		CORE->EnqueueMessage(q);*/
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
	Mid::MessageIdType bNewGame::SendMessage(Message* message)
	{
		if(CORE->mConfirmAction == false)
		{
			if (message->mMessageId == Mid::MOUSEMOVE)
			{
				Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
				if (isMouseOn())
					bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//NewGame_selected.png");
				else
					bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//NewGame.png");
			}
			else if (message->mMessageId == Mid::LBUTTONDOWN && isMouseOn())
			{
				Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
				bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//NewGame.png");

				selected = true;
				return Mid::LBUTTONDOWN;
			}
			else if (message->mMessageId == Mid::LBUTTONUP && isMouseOn())
			{
				Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
				bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//NewGame_selected.png");
				// activate button logic
				if (selected)
					Triggered();

				selected = false;
				return Mid::LBUTTONUP;
			}
			else if (message->mMessageId == Mid::LBUTTONUP)
			{
				selected = false;
			}
		}

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
	std::string bNewGame::ToString()
	{
		return "Logic Component: bNewGame";
	}

	/*****************************************************************************/
	/*!
		\brief
			Adds this object to the debug display.
	*/
	/*****************************************************************************/
	void bNewGame::ToBar()
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
	bNewGame::~bNewGame()
	{
		LOGIC->mButtonList.erase(reinterpret_cast<ILogical*>(this));
		FACTORY->UnregisterForMessages(GetOwner());
	}
}