/**************************************************************************************************/
/*!
\file	bHowToPlay.cpp
\par	Project: Coalescence
\date	12/8/2011
\authors
Primary  : Lincoln Tosi
Secondary: Kevin Verholtz - Triggered() code
\brief
Button that takes you to the How to Play screen.

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#include "Precompiled.h"
#include "bHowToPlay.h"
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

	/******************************************************************************/
	/*!
	  \brief
		Constructor sets mName to name of class
	*/
	/******************************************************************************/
	bHowToPlay::bHowToPlay()
	{
		mName = "bHowToPlay";
		selected = false;
	}
	/******************************************************************************/
	/*!
	  \brief
		initialaizes this button
	*/
	/******************************************************************************/
	void bHowToPlay::Initialize()
	{
		LOGIC->mButtonList.push_back(reinterpret_cast<ILogical*>(this));
		FACTORY->RegisterForMessages(GetOwner());
	}

	/******************************************************************************/
	/*!
	  \brief
		checks if the mouse is on the button
	*/
	/******************************************************************************/
	bool bHowToPlay::isMouseOn()
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
	/******************************************************************************/
	/*!
	  \brief
		if the button was triggered go to how to play screen
	*/
	/******************************************************************************/
	void bHowToPlay::Triggered()
	{
		GetOwner()->has(SoundNode)->Start();

		if (gGameStateCurr == GS_PAUSE)
		{
			// load the options buttons
			LOGIC->mKevinISAJEWPACABRAMEXICAN = true;
			LOGIC->pListEnd = LOGIC->PauseObjList.size();
			LOGIC->LoadPauseFile("..//Content//Levels//PMenu2.txt");
		}
		//if (gGameStateCurr == GS_PAUSE)
		//	q = new Message(Mid::Menu, 0);
		else
		{
			//change menu to how to play menu
			LOGIC->curMenu = 2;
			Message * q;
			q = new Message(Mid::Reload, 0);//reload new menu screen
			CORE->EnqueueMessage(q);
		}
	}
	/******************************************************************************/
	/*!
	  \brief
		highlites button bassed of mouse pos

	  \param message
			message to process
	*/
	/******************************************************************************/
	Mid::MessageIdType bHowToPlay::SendMessage(Message* message)
	{
		if(CORE->mConfirmAction == false)
		{
			if (message->mMessageId == Mid::MOUSEMOVE)
			{
				Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
				if (isMouseOn())
					bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//HowToPlay_selected.png");
				else
					bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//HowToPlay.png");
			}
			else if (message->mMessageId == Mid::LBUTTONDOWN && isMouseOn() && !(LOGIC->mKevinISAJEWPACABRAMEXICAN))
			{
				Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
				bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//HowToPlay.png");

				selected = true;
				return Mid::LBUTTONDOWN;
			}
			else if (message->mMessageId == Mid::LBUTTONUP && isMouseOn() && !(LOGIC->mKevinISAJEWPACABRAMEXICAN))
			{
				Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
				bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//HowToPlay_selected.png");
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
	/******************************************************************************/
	/*!
	  \brief
		returns string of what this class is

	  \return
			string of the class
	*/
	/******************************************************************************/
	std::string bHowToPlay::ToString()
	{
		return "Logic Component: bHowToPlay";
	}
	/******************************************************************************/
	/*!
	  \brief
		adds what this class is to the tweek bar
	*/
	/******************************************************************************/
	void bHowToPlay::ToBar()
	{
		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");

		TwAddVarRO(bar, "Logic Component", TW_TYPE_STDSTRING, &mName, " help='Type of logical component.' ");
	}
	/******************************************************************************/
	/*!
	  \brief
		destructor removes freed button from the list of button objects
	*/
	/******************************************************************************/
	bHowToPlay::~bHowToPlay()
	{
		LOGIC->mButtonList.erase(reinterpret_cast<ILogical*>(this));
		FACTORY->UnregisterForMessages(GetOwner());
	}
}