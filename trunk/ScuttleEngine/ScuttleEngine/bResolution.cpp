/**************************************************************************************************/
/*!
\file	bResolution.cpp
\par	Project: Coalescence
\date	12/6/2011
\authors
Primary : Lincoln Tosi
Secondary: Kevin Verholtz

\brief
A button that cycles through the different game resolutions.

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#include "Precompiled.h"
#include "bResolution.h"
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
	bResolution::bResolution()
	{
		bImage = "1024x768";
		mName = "bResolution";
		selected = false;
	}

	/*****************************************************************************/
	/*!
		\brief
			Initializes variables.
			Adds this object to the logic objects list.
	*/
	/*****************************************************************************/
	void bResolution::Initialize()
	{
		LOGIC->mButtonList.push_back(reinterpret_cast<ILogical*>(this));
		FACTORY->RegisterForMessages(GetOwner());

		if (LOGIC->RESOLUTION == 2)
		{
			//set resolution to 1600x1200
			bImage = "1600x1200";
			//if(GRAPHICS->GetD3DPP().Windowed)
			//{
			//	GRAPHICS->ChangeResolution(GRAPHICS->mMaxResWindowed.Width, GRAPHICS->mMaxResWindowed.Height);
			//}
			//else
			//{
			//	GRAPHICS->ChangeResolution(GRAPHICS->mMaxRes.Width, GRAPHICS->mMaxRes.Height);
			//}
			//LOGIC->RESOLUTION = 2;
		}
		else if (LOGIC->RESOLUTION == 0)
		{
			//set resolution to 800x600
			bImage = "800x600";
			//GRAPHICS->ChangeResolution(800, 600);
			LOGIC->RESOLUTION = 0;
		}
		else if (LOGIC->RESOLUTION == 1)
		{
			//set resolution to 1024x768
			bImage = "1024x768";
			//GRAPHICS->ChangeResolution(1024, 768);
			LOGIC->RESOLUTION = 1;
		}
	}

	/*****************************************************************************/
	/*!
		\brief
			Checks if the mouse is currently positioned over the button.

		\return
			true if the mouse is over, false if it's not.
	*/
	/*****************************************************************************/
	bool bResolution::isMouseOn()
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
	void bResolution::Triggered()
	{
		//LOGIC->Save();
		GetOwner()->has(SoundNode)->Start();

		if (bImage == "1024x768")
		{
			//set resolution to 1600x1200
			bImage = "1600x1200";
			if(GRAPHICS->GetD3DPP().Windowed)
			{
				GRAPHICS->ChangeResolution(GRAPHICS->mMaxResWindowed.Width, GRAPHICS->mMaxResWindowed.Height);
			}
			else
			{
				GRAPHICS->ChangeResolution(GRAPHICS->mMaxRes.Width, GRAPHICS->mMaxRes.Height);
			}
			LOGIC->RESOLUTION = 2;
		}
		else if (bImage == "1600x1200")
		{
			//set resolution to 800x600
			bImage = "800x600";
			GRAPHICS->ChangeResolution(800, 600);
			LOGIC->RESOLUTION = 0;
		}
		else if (bImage == "800x600")
		{
			//set resolution to 1024x768
			bImage = "1024x768";
			GRAPHICS->ChangeResolution(1024, 768);
			LOGIC->RESOLUTION = 1;
		}
		LOGIC->Save();
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
	Mid::MessageIdType bResolution::SendMessage(Message* message)
	{
		if(CORE->mConfirmAction == false)
		{
			if (message->mMessageId == Mid::MOUSEMOVE)
			{
				Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
				if (isMouseOn())
					bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//Resolution" + bImage + "_selected.png");
				else
					bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//Resolution" + bImage + ".png");
			}
			else if (message->mMessageId == Mid::LBUTTONDOWN && isMouseOn())
			{
				Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
				bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//Resolution" + bImage + ".png");

				selected = true;
				return Mid::LBUTTONDOWN;
			}
			else if (message->mMessageId == Mid::LBUTTONUP && isMouseOn())
			{
				Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
				bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//Resolution" + bImage + "_selected.png");
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
	std::string bResolution::ToString()
	{
		return "Logic Component: bResolution";
	}

	/*****************************************************************************/
	/*!
		\brief
			Adds this object to the debug display.
	*/
	/*****************************************************************************/
	void bResolution::ToBar()
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
	bResolution::~bResolution()
	{
		LOGIC->mButtonList.erase(reinterpret_cast<ILogical*>(this));
		FACTORY->UnregisterForMessages(GetOwner());
	}
}