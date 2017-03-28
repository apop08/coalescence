/**************************************************************************************************/
/*!
\file	bVolume.cpp
\par	Project: Coalescence
\date	12/6/2011
\authors
Primary : Lincoln Tosi

\brief
A button that cycles through the different levels of game volume.

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#include "Precompiled.h"
#include "bVolume.h"
#include "GameLogic.h"

#include "Core.h"

#include "Transform.h"
#include "DebugBars.h"
#include "Graphics.h"
#include "GameStateMgr.h"

#include "Camera.h"

#include "Audio.h"
#include "SoundNode.h"

namespace Scuttlemeyer
{
	/*****************************************************************************/
	/*!
		\brief
			Constructor - Initializes necessary variables.
	*/
	/*****************************************************************************/
	bVolume::bVolume()
	{
		bImage = "Medium";
		mName = "bVolume";
		selected = false;
	}

	/*****************************************************************************/
	/*!
		\brief
			Initializes variables.
			Adds this object to the logic objects list.
	*/
	/*****************************************************************************/
	void bVolume::Initialize()
	{
		LOGIC->mButtonList.push_back(reinterpret_cast<ILogical*>(this));
		FACTORY->RegisterForMessages(GetOwner());
		if (LOGIC->VOLUME == 3)
		{
			//set volume to high
			bImage = "High";
			AUDIO->SetVolume(1.0f);
			AUDIO->DecrementVolume();
			AUDIO->IncrementVolume();
			//LOGIC->VOLUME = 3;
		}
		else if (LOGIC->VOLUME == 0)
		{
			//set volume to off
			bImage = "Off";
			AUDIO->SetVolume(0.0f);
			AUDIO->IncrementVolume();
			AUDIO->DecrementVolume();
			LOGIC->VOLUME = 0;
		}
		else if (LOGIC->VOLUME == 1)
		{
			//set volume to low
			bImage = "Low";
			AUDIO->SetVolume(0.3f);
			AUDIO->DecrementVolume();
			AUDIO->IncrementVolume();
			//LOGIC->VOLUME = 1;
		}
		else if (LOGIC->VOLUME == 2)
		{
			//set volume to medium
			bImage = "Medium";
			AUDIO->SetVolume(0.6f);
			AUDIO->DecrementVolume();
			AUDIO->IncrementVolume();
			//LOGIC->VOLUME = 2;
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
	bool bVolume::isMouseOn()
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
	void bVolume::Triggered()
	{
		//LOGIC->Save();
		if (bImage == "Medium")
		{
			//set volume to high
			bImage = "High";
			AUDIO->SetVolume(1.0f);
			AUDIO->DecrementVolume();
			AUDIO->IncrementVolume();
			LOGIC->VOLUME = 3;
		}
		else if (bImage == "High")
		{
			//set volume to off
			bImage = "Off";
			AUDIO->SetVolume(0.0f);
			AUDIO->IncrementVolume();
			AUDIO->DecrementVolume();
			LOGIC->VOLUME = 0;
		}
		else if (bImage == "Off")
		{
			//set volume to low
			bImage = "Low";
			AUDIO->SetVolume(0.3f);
			AUDIO->DecrementVolume();
			AUDIO->IncrementVolume();
			LOGIC->VOLUME = 1;
		}
		else if (bImage == "Low")
		{
			//set volume to medium
			bImage = "Medium";
			AUDIO->SetVolume(0.6f);
			AUDIO->DecrementVolume();
			AUDIO->IncrementVolume();
			LOGIC->VOLUME = 2;
		}
		LOGIC->Save();
		GetOwner()->has(SoundNode)->Start();
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
	Mid::MessageIdType bVolume::SendMessage(Message* message)
	{
		if(CORE->mConfirmAction == false)
		{
			if (message->mMessageId == Mid::MOUSEMOVE)
			{
				Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
				if (isMouseOn())
					bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//Volume" + bImage + "_selected.png");
				else
					bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//Volume" + bImage + ".png");
			}
			else if (message->mMessageId == Mid::LBUTTONDOWN && isMouseOn())
			{
				Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
				bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//Volume" + bImage + ".png");

				selected = true;
				return Mid::LBUTTONDOWN;
			}
			else if (message->mMessageId == Mid::LBUTTONUP && isMouseOn())
			{
				Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
				bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//Volume" + bImage + "_selected.png");
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
	std::string bVolume::ToString()
	{
		return "Logic Component: bVolume";
	}

	/*****************************************************************************/
	/*!
		\brief
			Adds this object to the debug display.
	*/
	/*****************************************************************************/
	void bVolume::ToBar()
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
	bVolume::~bVolume()
	{
		LOGIC->mButtonList.erase(reinterpret_cast<ILogical*>(this));
		FACTORY->UnregisterForMessages(GetOwner());
	}
}