/******************************************************************************/
/*!
\file   Checkpoint.cpp
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM250
\date   1/23/2012
\brief  
	Logic control unit for checkpoints.
\copyright 
“All content © 2012 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#include "Precompiled.h"
#include "Checkpoint.h"

#include "GameLogic.h"
#include "Graphics.h"

#include "DebugBars.h"

namespace Scuttlemeyer
{
	Checkpoint::Checkpoint()
	{
		mName = "Checkpoint";
	}

	void Checkpoint::Initialize()
	{
		GetOwner()->state = EState(0);
		LOGIC->mLogicList.push_back(reinterpret_cast<ILogical*>(this));
	}

	void Checkpoint::Serialize(TextLoader &stream)
	{
	}

	void Checkpoint::Update(float dt)
	{
		if(GetOwner()->state != 1)
		{
			if(LOGIC->mPlayer->has(Transform)->mPosition.x < GetOwner()->has(Transform)->mPosition.x + GetOwner()->has(Transform)->mScale.x 
				&& LOGIC->mPlayer->has(Transform)->mPosition.x > GetOwner()->has(Transform)->mPosition.x - GetOwner()->has(Transform)->mScale.x 
				&& LOGIC->mPlayer->has(Transform)->mPosition.y < GetOwner()->has(Transform)->mPosition.y + GetOwner()->has(Transform)->mScale.y 
				&& LOGIC->mPlayer->has(Transform)->mPosition.x > GetOwner()->has(Transform)->mPosition.y - GetOwner()->has(Transform)->mScale.y)
			{
				GetOwner()->state = EState(1);

				TextSaver stream;
				stream.Open("..//Content//Levels//Checkpoint.txt");
				FACTORY->SaveLevel(stream);
			}
		}
	}

	Mid::MessageIdType Checkpoint::SendMessage(Message* message)
	{
		return Mid::None;
	}

	std::string Checkpoint::ToString()
	{
		return "Logic Component: Checkpoint";
	}

	void Checkpoint::ToBar()
	{
		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");

		TwAddVarRO(bar, "Logic Component", TW_TYPE_STDSTRING, &mName, " help='Type of logical component.' ");
	}

	Checkpoint::~Checkpoint()
	{
		LOGIC->mLogicList.erase(reinterpret_cast<ILogical*>(this));
	}
}