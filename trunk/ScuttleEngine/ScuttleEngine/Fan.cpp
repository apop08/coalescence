/******************************************************************************/
/*!
\file   Fan.cpp
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM250
\date   4/04/2012
\brief  
	Logic control unit for on/off fans.
\copyright 
“All content © 2012 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#include "Precompiled.h"
#include "Fan.h"

#include "GameLogic.h"
#include "DebugBars.h"
#include "ParticleEmitter.h"
#include "Regions.h"
#include "AttenuatedSoundNode.h"

#define SPEED 5

namespace Scuttlemeyer
{
	Fan::Fan()
	{
		mName = "Fan";
		mBStarted = false;
		mDefRegPower = 0.f;
	}

	void Fan::Initialize()
	{
		LOGIC->mLogicList.push_back(reinterpret_cast<ILogical*>(this));
		mDefRegPower = GetOwner()->has(Region)->GetPower();
		GetOwner()->has(Region)->SetPower(0.f);
	}

	void Fan::Serialize(TextLoader &stream)
	{
		TextLoader stream2;

		std::string name = "..//Content//Objects//";
		char tempChar[20];
		sprintf_s(tempChar, 20, "%i", LOGIC->curLevel);
		name.append(tempChar);
		name.append("//");
		name.append(LOGIC->mCurInstName);
		name.append(".fan.data");

		stream2.Open(name);

		if(stream2.IsGood())
		{
			int tempBool;
			StreamRead(stream2, tempBool);
			mBActivateIsActivate = tempBool == 0 ? false : true;
			StreamRead(stream2, tempBool);
			mBIsActivated = tempBool == 0 ? false : true;
		}
		else
		{
			mBActivateIsActivate = true;
			mBIsActivated = false;
		}
	}

	void Fan::Update(float dt)
	{
		if(!mBStarted)
		{
			if ((mBIsActivated && mBActivateIsActivate) || (mBIsActivated && !mBActivateIsActivate))
			{
				GetOwner()->has(Sprite)->mAnimActive = true;
				GetOwner()->has(Sprite)->SetAnimation(PLAYLOOPED, FORWARD);
				Emitter * emit = GetOwner()->has(Emitter);
				if(emit)
				{
					emit->mJetPack = true;
				}

				GetOwner()->has(Region)->SetPower(mDefRegPower);

				GetOwner()->has(AttenuatedSoundNode)->Start(0);
			}
			else
			{
				GetOwner()->has(Sprite)->SetAnimation(STOP, FORWARD);
				Emitter * emit = GetOwner()->has(Emitter);
				if(emit)
				{
					emit->mJetPack = false;
				}

				GetOwner()->has(Region)->SetPower(0.f);

				GetOwner()->has(AttenuatedSoundNode)->Stop(0);
			}

			mBStarted = true;
		}
	}

	void Fan::Activate()
	{
		if (!mBIsActivated && mBActivateIsActivate)
		{
			GetOwner()->has(Sprite)->mAnimActive = true;
			GetOwner()->has(Sprite)->SetAnimation(PLAYLOOPED, FORWARD);
			Emitter * emit = GetOwner()->has(Emitter);
			if(emit)
			{
				emit->mJetPack = true;
			}

			GetOwner()->has(Region)->SetPower(mDefRegPower);

			GetOwner()->has(AttenuatedSoundNode)->Start(0);

			mBIsActivated = true;
		}
		else if(mBIsActivated && !mBActivateIsActivate)
		{
			GetOwner()->has(Sprite)->SetAnimation(STOP, FORWARD);
			Emitter * emit = GetOwner()->has(Emitter);
			if(emit)
			{
				emit->mJetPack = false;
			}

			GetOwner()->has(Region)->SetPower(0.f);

			GetOwner()->has(AttenuatedSoundNode)->Stop(0);

			mBIsActivated = false;
		}
	}

	void Fan::Deactivate()
	{
		if (mBIsActivated && mBActivateIsActivate)
		{
			GetOwner()->has(Sprite)->SetAnimation(STOP, FORWARD);
			Emitter * emit = GetOwner()->has(Emitter);
			if(emit)
			{
				emit->mJetPack = false;
			}

			GetOwner()->has(Region)->SetPower(0.f);

			GetOwner()->has(AttenuatedSoundNode)->Stop(0);

			mBIsActivated = false;
		}
		else if(!mBIsActivated && !mBActivateIsActivate)
		{
			GetOwner()->has(Sprite)->mAnimActive = true;
			GetOwner()->has(Sprite)->SetAnimation(PLAYLOOPED, FORWARD);
			Emitter * emit = GetOwner()->has(Emitter);
			if(emit)
			{
				emit->mJetPack = true;
			}

			GetOwner()->has(Region)->SetPower(mDefRegPower);

			GetOwner()->has(AttenuatedSoundNode)->Start(0);

			mBIsActivated = true;
		}
	}

	Mid::MessageIdType Fan::SendMessage(Message* message)
	{
		if(message->mMessageId == Mid::Activate)
		{
			Activate();
		}
		else if(message->mMessageId == Mid::Deactivate)
		{
			Deactivate();
		}

		return Mid::None;
	}

	std::string Fan::ToString()
	{
		return "Logic Component: Fan";
	}

	void Fan::ToBar()
	{
		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");

		TwAddVarRO(bar, "Logic Component", TW_TYPE_STDSTRING, &mName, " help='Type of logical component.' ");
	}

	Fan::~Fan()
	{
		LOGIC->mLogicList.erase(reinterpret_cast<ILogical*>(this));
	}
}