/**************************************************************************************************/
/*!
\file	bArrowLeft.cpp
\par	Project: Coalescence
\date	4/19/2012
\authors
Primary  : Kevin Verholtz
\brief
Left Arrow For Level Select Screen

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#include "Precompiled.h"
#include "bArrowLeft.h"
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
		Constructor button

	*/
	/*****************************************************************************/
	bArrowLeft::bArrowLeft()
	{
		mName = "bArrowLeft";
		selected = false;
	}

	/*****************************************************************************/
	/*!
	\brief
		Initializes button

	*/
	/*****************************************************************************/
	void bArrowLeft::Initialize()
	{
		LOGIC->mButtonList.push_back(reinterpret_cast<ILogical*>(this));
		FACTORY->RegisterForMessages(GetOwner());
	}

	/*****************************************************************************/
	/*!
	\brief
		Button mouse on

	*/
	/*****************************************************************************/
	bool bArrowLeft::isMouseOn()
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
		If the button is pressed

	*/
	/*****************************************************************************/
	void bArrowLeft::Triggered()
	{
		GetOwner()->has(SoundNode)->Start();

		if(CORE->mLevelSelected != 1)
		{
			CORE->mLevelSelected--;
		}
		else
		{
			CORE->mLevelSelected = LOGIC->mMaxLevel;
		}
		Message * q = new Message(Mid::MOUSEMOVE, 0);
		CORE->EnqueueMessage(q);
	}

	/*****************************************************************************/
	/*!
	\brief
		Handles messages 

	*/
	/*****************************************************************************/
	Mid::MessageIdType bArrowLeft::SendMessage(Message* message)
	{
		if(CORE->mConfirmAction == false)
		{
			if (message->mMessageId == Mid::MOUSEMOVE)
			{
				Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
				if (isMouseOn())
					bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Levels//Arrow_Left_Selected.png");
				else
					bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Levels//Arrow_Left.png");
			}
			else if (message->mMessageId == Mid::LBUTTONDOWN && isMouseOn())
			{
				Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
				bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Levels//Arrow_Left.png");

				selected = true;
				return Mid::LBUTTONDOWN;
			}
			else if (message->mMessageId == Mid::LBUTTONUP && isMouseOn())
			{
				Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
				bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Levels//Arrow_Left_Selected.png");
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
	std::string bArrowLeft::ToString()
	{
		return "Logic Component: bArrowLeft";
	}
	void bArrowLeft::ToBar()
	{
		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");

		TwAddVarRO(bar, "Logic Component", TW_TYPE_STDSTRING, &mName, " help='Type of logical component.' ");
	}

	/*****************************************************************************/
	/*!
	\brief
		Removes the button

	*/
	/*****************************************************************************/
	bArrowLeft::~bArrowLeft()
	{
		LOGIC->mButtonList.erase(reinterpret_cast<ILogical*>(this));
		FACTORY->UnregisterForMessages(GetOwner());
	}
}