/******************************************************************************/
/*!
\file   TutBox.cpp
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   11/20/2011
\brief  
	Logic control unit for tutorial boxes.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#include "Precompiled.h"
#include "TutBox.h"
#include "GameLogic.h"

#include "Core.h"

#include "Graphics.h"
#include "Transform.h"
#include "Camera.h"
#include "DebugConsole.h"
#include "DebugBars.h"

namespace Scuttlemeyer
{
	TutBox::TutBox()
	{
		mName = "TutorialBox";
	}

	void TutBox::Initialize()
	{
		LOGIC->mLogicList.push_back(reinterpret_cast<ILogical*>(this));
		GetOwner()->has(Transform)->mScale.x /= GRAPHICS->mZoom;
		GetOwner()->has(Transform)->mScale.y /= GRAPHICS->mZoom;
	}

	void TutBox::Serialize(TextLoader &stream)
	{
		StreamRead(stream, mTriggerPosition);
		StreamRead(stream, mTriggerRadius);
	}

	void TutBox::Update(float dt)
	{
		if(GetOwner()->has(Sprite)->mColor.w > 0.0f)
		{
			if((LOGIC->mPlayer->has(Transform)->mPosition.x > (mTriggerPosition.x + mTriggerRadius)) 
				|| (LOGIC->mPlayer->has(Transform)->mPosition.x < (mTriggerPosition.x - mTriggerRadius)) 
				|| (LOGIC->mPlayer->has(Transform)->mPosition.y > (mTriggerPosition.y + mTriggerRadius)) 
				|| (LOGIC->mPlayer->has(Transform)->mPosition.y < (mTriggerPosition.y - mTriggerRadius)))
			{
				GetOwner()->has(Sprite)->SetFadeMode(-1, 0.08f);
			}
			else
			{
				GetOwner()->has(Sprite)->SetFadeMode(1, 0.08f);
				GetOwner()->has(Transform)->mPosition.x = floor(LOGIC->mPlayer->has(Transform)->mPosition.x - floor(400.f / GRAPHICS->mZoom));
				GetOwner()->has(Transform)->mPosition.y = floor(LOGIC->mPlayer->has(Transform)->mPosition.y + floor(250.f / GRAPHICS->mZoom) + 150.f);
			}
		}
		else
		{
			if((LOGIC->mPlayer->has(Transform)->mPosition.x < (mTriggerPosition.x + mTriggerRadius)) 
				&& (LOGIC->mPlayer->has(Transform)->mPosition.x > (mTriggerPosition.x - mTriggerRadius)) 
				&& (LOGIC->mPlayer->has(Transform)->mPosition.y < (mTriggerPosition.y + mTriggerRadius)) 
				&& (LOGIC->mPlayer->has(Transform)->mPosition.y > (mTriggerPosition.y - mTriggerRadius)))
			{
				LOGIC->mPlayer->has(Player)->mTutFlag |= GetOwner()->state;
				LOGIC->mPlayer->state = (EState)LOGIC->mPlayer->has(Player)->mTutFlag;
				GetOwner()->has(Sprite)->SetFadeMode(1, 0.08f);
				GetOwner()->has(Transform)->mPosition.x = floor(LOGIC->mPlayer->has(Transform)->mPosition.x - floor(400.f / GRAPHICS->mZoom));
				GetOwner()->has(Transform)->mPosition.y = floor(LOGIC->mPlayer->has(Transform)->mPosition.y + floor(250.f / GRAPHICS->mZoom) + 150.f);
			}
		}
	}

	Mid::MessageIdType TutBox::SendMessage(Message* message)
	{
		return Mid::None;
	}

	std::string TutBox::ToString()
	{
		return "Logic Component: TutorialBox";
	}

	void TutBox::ToBar()
	{
		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");

		TwAddVarRO(bar, "Logic Component", TW_TYPE_STDSTRING, &mName, " help='Type of logical component.' ");
	}

	TutBox::~TutBox()
	{
		LOGIC->mLogicList.erase(reinterpret_cast<ILogical*>(this));
	}
}