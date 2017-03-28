/**************************************************************************************************/
/*!
\file	bYes.cpp
\par	Project: Coalescence
\date	12/6/2011
\authors
Primary : Kevin Verholtz

\brief
A button that quits the game.

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#include "Precompiled.h"
#include "bYes.h"
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
	bYes::bYes()
	{
		mName = "bYes";
		selected = false;
	}

	void bYes::Initialize()
	{
		LOGIC->mButtonList.push_back(reinterpret_cast<ILogical*>(this));
		FACTORY->RegisterForMessages(GetOwner());
	}

	bool bYes::isMouseOn()
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

	void bYes::Triggered()
	{
		GetOwner()->has(SoundNode)->Start();
		Message* q = new Message(Mid::Quit, 0);
		CORE->EnqueueConfirmMessage(q);
	}

	Mid::MessageIdType bYes::SendMessage(Message* message)
	{
		if (message->mMessageId == Mid::MOUSEMOVE)
		{
			Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
			if (isMouseOn())
				bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//Yes_selected.png");
			else
				bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//Yes.png");
		}
		else if (message->mMessageId == Mid::LBUTTONDOWN && isMouseOn())
		{
			Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
			bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//Yes.png");

			selected = true;
			return Mid::LBUTTONDOWN;
		}
		else if (message->mMessageId == Mid::LBUTTONUP && isMouseOn())
		{
			Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
			bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//Yes_selected.png");
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

		return Mid::None;
	}

	std::string bYes::ToString()
	{
		return "Logic Component: bYes";
	}

	void bYes::ToBar()
	{
		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");

		TwAddVarRO(bar, "Logic Component", TW_TYPE_STDSTRING, &mName, " help='Type of logical component.' ");
	}

	bYes::~bYes()
	{
		LOGIC->mButtonList.erase(reinterpret_cast<ILogical*>(this));
		FACTORY->UnregisterForMessages(GetOwner());
	}
}