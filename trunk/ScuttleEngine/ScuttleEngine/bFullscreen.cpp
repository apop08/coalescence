/**************************************************************************************************/
/*!
\file	bFullscreen.cpp
\par	Project: Coalescence
\date	4/3/2012
\authors
Primary : Kevin Verholtz

\brief
A button that cycles through the Fullscreen/Windowed.

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#include "Precompiled.h"
#include "bFullscreen.h"
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
	bFullscreen::bFullscreen()
	{
		mName = "bFullscreen";
		selected = false;
	}

	void bFullscreen::Initialize()
	{
		LOGIC->mButtonList.push_back(reinterpret_cast<ILogical*>(this));
		FACTORY->RegisterForMessages(GetOwner());

		if(GRAPHICS->GetD3DPP().Windowed)
		{
			Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
			bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//Windowed_Button.png");
		}
		else
		{
			Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
			bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//Fullscreen_Button.png");
		}
	}

	bool bFullscreen::isMouseOn()
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

	void bFullscreen::Triggered()
	{
		//LOGIC->Save();
		GRAPHICS->ToggleWindowed();
		LOGIC->FULLSCREEN = !LOGIC->FULLSCREEN;
		Message * q = new Message(Mid::MOUSEMOVE, 0);
		CORE->EnqueueMessage(q);
		LOGIC->Save();
	}

	Mid::MessageIdType bFullscreen::SendMessage(Message* message)
	{
		if(CORE->mConfirmAction == false)
		{
			if(GRAPHICS->GetD3DPP().Windowed)
			{
				if (message->mMessageId == Mid::MOUSEMOVE)
				{
					Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
					if (isMouseOn())
						bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//Windowed_Button_Selected.png");
					else
						bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//Windowed_Button.png");
				}
				else if (message->mMessageId == Mid::LBUTTONDOWN && isMouseOn())
				{
					Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
					bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//Windowed_Button.png");

					selected = true;
					return Mid::LBUTTONDOWN;
				}
				else if (message->mMessageId == Mid::LBUTTONUP && isMouseOn())
				{
					Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
					bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//Windowed_Button_Selected.png");
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
			else
			{
				if (message->mMessageId == Mid::MOUSEMOVE)
				{
					Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
					if (isMouseOn())
						bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//Fullscreen_Button_Selected.png");
					else
						bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//Fullscreen_Button.png");
				}
				else if (message->mMessageId == Mid::LBUTTONDOWN && isMouseOn())
				{
					Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
					bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//Fullscreen_Button.png");

					selected = true;
					return Mid::LBUTTONDOWN;
				}
				else if (message->mMessageId == Mid::LBUTTONUP && isMouseOn())
				{
					Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
					bSprite->mTexture = GRAPHICS->GetTexture("..//Content//Textures//Buttons//Fullscreen_Button_Selected.png");
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
		}

		return Mid::None;
	}

	std::string bFullscreen::ToString()
	{
		return "Logic Component: bFullscreen";
	}

	void bFullscreen::ToBar()
	{
		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");

		TwAddVarRO(bar, "Logic Component", TW_TYPE_STDSTRING, &mName, " help='Type of logical component.' ");
	}

	bFullscreen::~bFullscreen()
	{
		LOGIC->mButtonList.erase(reinterpret_cast<ILogical*>(this));
		FACTORY->UnregisterForMessages(GetOwner());
	}
}