/******************************************************************************/
/*!
\file   EditCam.cpp
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM250
\date   12/05/2011
\brief  
	Logic control unit for the level editor camera.
\copyright 
“All content © 2012 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#include "Precompiled.h"
#include "EditCam.h"

#include "GameLogic.h"
#include "Graphics.h"

#include "DebugBars.h"

namespace Scuttlemeyer
{
	EditCam::EditCam()
	{
		mName = "EditCam";
	}

	void EditCam::Initialize()
	{
		FACTORY->RegisterForMessages(GetOwner());
	}

	void EditCam::Update(float dt)
	{
	}

	Mid::MessageIdType EditCam::SendMessage(Message* message)
	{
		if(message->mMessageId == Mid::KeyDown)
		{
			int key = atoi(((std::string*)(message->mMessageData[0]))->c_str());
			Transform* pTransform = GetOwner()->has(Transform);

			if(key == VK_UP)
			{
				pTransform->mPosition.y += 5.f;
			}

			if(key == VK_DOWN)
			{
				pTransform->mPosition.y -= 5.f;
			}

			if(key == VK_LEFT)
			{
				pTransform->mPosition.x -= 5.f;
			}

			if(key == VK_RIGHT)
			{
				pTransform->mPosition.x += 5.f;
			}
		}

		return Mid::None;
	}

	std::string EditCam::ToString()
	{
		return "Logic Component: EditCam";
	}

	void EditCam::ToBar()
	{
		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");

		TwAddVarRO(bar, "Logic Component", TW_TYPE_STDSTRING, &mName, " help='Type of logical component.' ");
	}

	EditCam::~EditCam()
	{
		Camera* temp = LOGIC->mPlayer->has(Camera);
		GRAPHICS->mCurrentCamera = temp;
		FACTORY->UnregisterForMessages(GetOwner());
	}
}