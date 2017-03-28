/**************************************************************************************************/
/*!
\file	bLevel.cpp
\par	Project: Coalescence
\date	4/4/2012
\authors
Primary : Kevin Verholtz

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#include "Precompiled.h"
#include "bLevel.h"
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
	bLevel::bLevel()
	{
		mName = "bLevel";
		selected = false;
	}

	void bLevel::Initialize()
	{
		LOGIC->mButtonList.push_back(reinterpret_cast<ILogical*>(this));
		FACTORY->RegisterForMessages(GetOwner());
	}

	bool bLevel::isMouseOn()
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

	void bLevel::Triggered()
	{
		if(CORE->mLevelSelected <= CORE->mLevelUnlocked)
		{
			GetOwner()->has(SoundNode)->Start(0);

			LOGIC->curLevel = CORE->mLevelSelected;
			LOGIC->mBGameEnd = false;
			LOGIC->mBDied = false;
			LOGIC->nextState = GS_PLAY;
			LOGIC->curTransition = 1;

			Message * q = new Message(Mid::Transition, 0);
			CORE->EnqueueMessage(q);
		}
		else
		{
			GetOwner()->has(SoundNode)->Start(1);
			//PLAY EHHHH "horn" SOUND
		}
	}

	Mid::MessageIdType bLevel::SendMessage(Message* message)
	{
		if(CORE->mConfirmAction == false)
		{
			if (message->mMessageId == Mid::MOUSEMOVE)
			{
				std::string Level("..//Content//Textures//Levels//Level");
				char temp[12];
				_itoa_s(CORE->mLevelSelected, temp, 12, 10);
				Level += temp;
				
				if(CORE->mLevelSelected <= CORE->mLevelUnlocked)
				{
					Level += ".png";
				}
				else
				{
					Level += "_Locked.png";
				}

				Sprite * bSprite = (Sprite*)GetOwner()->GetComponent(CT_Sprite);
				bSprite->mTexture = GRAPHICS->GetTexture(Level);
			}
			else if (message->mMessageId == Mid::LBUTTONDOWN && isMouseOn())
			{
				selected = true;
				return Mid::LBUTTONDOWN;
			}
			else if (message->mMessageId == Mid::LBUTTONUP && isMouseOn())
			{
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

	std::string bLevel::ToString()
	{
		return "Logic Component: bLevel";
	}

	void bLevel::ToBar()
	{
		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");

		TwAddVarRO(bar, "Logic Component", TW_TYPE_STDSTRING, &mName, " help='Type of logical component.' ");
	}

	bLevel::~bLevel()
	{
		LOGIC->mButtonList.erase(reinterpret_cast<ILogical*>(this));
		FACTORY->UnregisterForMessages(GetOwner());
	}
}