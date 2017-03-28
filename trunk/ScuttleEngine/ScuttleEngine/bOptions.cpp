/**************************************************************************************************/
/*!
\file	bOptions.cpp
\par	Project: Coalescence
\date	12/6/2011
\authors
Primary : Lincoln Tosi
Secondary: Kevin Verholtz

\brief
A button that opens the game options (both in pause and main menu).

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#include "Precompiled.h"
#include "bOptions.h"
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
	bOptions::bOptions()
	{
		mName = "bOptions";
		selected = false;
		bOffset = (int) (240 / GRAPHICS->mZoom);
		pListEnd = 0;
		isLeft = false;
	}

	/*****************************************************************************/
	/*!
		\brief
			Initializes variables.
			Adds this object to the logic objects list.
	*/
	/*****************************************************************************/
	void bOptions::Initialize()
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
	bool bOptions::isMouseOn()
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
	void bOptions::Triggered()
	{
		if (gGameStateCurr == GS_PAUSE)
		{
			GetOwner()->has(SoundNode)->Start();

			if (!isLeft)
			{
				// move pause buttons to the left
				pListEnd = LOGIC->PauseObjList.size();
				for (unsigned i = 1; i < LOGIC->PauseObjList.size(); ++i)
				{
					GOC *button = FACTORY->GetObjectWithId(LOGIC->PauseObjList[i]->GetId());
					Transform* bTransform = ((Transform*)button->GetComponent(CT_Transform));
					if (bTransform)
						bTransform->mPosition.x -= bOffset;
				}
				// load the options buttons
				LOGIC->LoadPauseFile("..//Content//Levels//PauseOptions.txt");
				isLeft = true;
			}
			else if (pListEnd)
			{
				// delete the options buttons
				while (LOGIC->PauseObjList.size() > pListEnd)
				{
					std::vector<GOC*>::iterator it = LOGIC->PauseObjList.begin() + pListEnd;
					FACTORY->Destroy(FACTORY->GetObjectWithId(LOGIC->PauseObjList[pListEnd]->GetId()));
					LOGIC->PauseObjList.erase(it);
				}

				// reset the button positions
				for (unsigned i = 1; i < LOGIC->PauseObjList.size(); ++i)
				{
					GOC *button = FACTORY->GetObjectWithId(LOGIC->PauseObjList[i]->GetId());
					Transform* bTransform = ((Transform*)button->GetComponent(CT_Transform));
					if (bTransform)
						bTransform->mPosition.x += bOffset;
				}
				isLeft = false;
			}
			Message * q = new Message(Mid::MOUSEMOVE, 0);
			CORE->EnqueueMessage(q);
		}
		else
		{
			GetOwner()->has(SoundNode)->Start();
			LOGIC->curMenu = 4;
			Message * q = new Message(Mid::Reload, 0);//reload new menu screen
			CORE->EnqueueMessage(q);
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
	Mid::MessageIdType bOptions::SendMessage(Message* message)
	{
		if(CORE->mConfirmAction == false)
		{
			if (message->mMessageId == Mid::MOUSEMOVE)
			{
				Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
				if (isMouseOn())
					bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//Options_selected.png");
				else
					bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//Options.png");
			}
			else if (message->mMessageId == Mid::LBUTTONDOWN && isMouseOn() && !(LOGIC->mKevinISAJEWPACABRAMEXICAN))
			{
				Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
				bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//Options.png");

				selected = true;
				return Mid::LBUTTONDOWN;
			}
			else if (message->mMessageId == Mid::LBUTTONUP && isMouseOn() && !(LOGIC->mKevinISAJEWPACABRAMEXICAN))
			{
				Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
				bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//Options_selected.png");
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
	std::string bOptions::ToString()
	{
		return "Logic Component: bOptions";
	}

	/*****************************************************************************/
	/*!
		\brief
			Adds this object to the debug display.
	*/
	/*****************************************************************************/
	void bOptions::ToBar()
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
	bOptions::~bOptions()
	{
		LOGIC->mButtonList.erase(reinterpret_cast<ILogical*>(this));
		FACTORY->UnregisterForMessages(GetOwner());
	}
}