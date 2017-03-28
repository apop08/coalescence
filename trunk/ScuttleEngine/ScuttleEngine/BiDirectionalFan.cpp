/******************************************************************************/
/*!
\file   BiDirectionalFan.cpp
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM250
\date   2/10/2012
\brief  
	Logic control unit for reversable fans.
\copyright 
“All content © 2012 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#include "Precompiled.h"
#include "BiDirectionalFan.h"

#include "GameLogic.h"
#include "DebugBars.h"
#include "ParticleEmitter.h"
#include "Regions.h"
#include "AttenuatedSoundNode.h"

#define SPEED 5

namespace Scuttlemeyer
{
	BiDirectionalFan::BiDirectionalFan()
	{
		mName = "BiDirectionalFan";
		mBStarted = false;
		mDefRegPower = 0.f;
		mBIsActivated = false;
	}

	void BiDirectionalFan::Initialize()
	{
		LOGIC->mLogicList.push_back(reinterpret_cast<ILogical*>(this));
		mDefRegPower = GetOwner()->has(Region)->GetPower();
	}

	void BiDirectionalFan::Serialize(TextLoader &stream)
	{
	}

	void BiDirectionalFan::Update(float dt)
	{
		if(!mBStarted)
		{
			GetOwner()->has(AttenuatedSoundNode)->Start(0);
			GetOwner()->has(Emitter)->mJetPack = true;
			mBStarted = true;
		}
	}

	void BiDirectionalFan::Activate()
	{
		GetOwner()->has(Sprite)->mAnimActive = true;
		GetOwner()->has(Sprite)->SetAnimation(PLAYLOOPED, REVERSE);
		Emitter * emit = GetOwner()->has(Emitter);//->mDirection = 0;
		if(emit)
		{
			emit->mDirection = 0;
		
			for(std::list<Particle*>::iterator it = emit->mParticleList.begin(); it != emit->mParticleList.end(); ++it)
			{
				int maxDist  = ((Region*)GetOwner()->has(Region))->GetMaxDistance();
				int moveAmount = SPEED;
				int totalLifetime = maxDist/moveAmount;
				
				(*it)->mLifetime = totalLifetime - (*it)->mLifetime;
			}
		}

		GetOwner()->has(Region)->SetPower(mDefRegPower * -1.f);

		mBIsActivated = true;
	}

	void BiDirectionalFan::Deactivate()
	{
		GetOwner()->has(Sprite)->mAnimActive = true;
		GetOwner()->has(Sprite)->SetAnimation(PLAYLOOPED, FORWARD);
		Emitter * emit = GetOwner()->has(Emitter);//->mDirection = 1;
		if(emit)
		{
			emit->mDirection = 1;
			//emit->mPoolLoop = 0;//emit->mEmitType->mMaxParticles;
			for(std::list<Particle*>::iterator it = emit->mParticleList.begin(); it != emit->mParticleList.end(); ++it)
			{
				int maxDist  = ((Region*)GetOwner()->has(Region))->GetMaxDistance();
				int moveAmount = SPEED;
				int totalLifetime = maxDist/moveAmount;
				
				(*it)->mLifetime = totalLifetime - (*it)->mLifetime;
				//std::list<Particle*>::iterator it2 = it;
				//++it;
				//(*it2)->mActive = false;
				//(*it2)->GetOwner()->has(Sprite)->SetFadeMode(-1, 1);
				//emit->mParticleList.remove(*it2);
			}
		}

		GetOwner()->has(Region)->SetPower(mDefRegPower);

		mBIsActivated = false;
	}

	Mid::MessageIdType BiDirectionalFan::SendMessage(Message* message)
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

	std::string BiDirectionalFan::ToString()
	{
		return "Logic Component: BiDirectionalFan";
	}

	void BiDirectionalFan::ToBar()
	{
		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");

		TwAddVarRO(bar, "Logic Component", TW_TYPE_STDSTRING, &mName, " help='Type of logical component.' ");
	}

	BiDirectionalFan::~BiDirectionalFan()
	{
		LOGIC->mLogicList.erase(reinterpret_cast<ILogical*>(this));
	}
}