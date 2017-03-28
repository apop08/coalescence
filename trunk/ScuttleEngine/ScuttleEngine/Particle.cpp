/**************************************************************************************************/
/*!
\file	Particle.cpp
\par	Project: Coalescence
\date	12/7/2011
\authors
Primary   : Kevin Verholtz
Secondary : Alex Poplawski

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#include "Precompiled.h"
#include "Particle.h"
#include "PhysBody.h"
#include "Factory.h"
#include "Physics.h"

namespace Scuttlemeyer
{
	/*****************************************************************************/
	/*!
	\brief
		Initializes members
	*/
	/*****************************************************************************/
	Particle::Particle()
	{
		mLife = 0;
		mRotVel = 0;
		mVelocity.x = 0;
		mVelocity.y = 0;
		mActive = false;
		mLeftRight = LEFT;
	}

	/*****************************************************************************/
	/*!
	\brief
		Uses the Emitters information to initializes itself
	*/
	/*****************************************************************************/
	void Particle::Initialize(EmitData* init)
	{
		mSprite = GetOwner()->has(Sprite);
		
		mActive = true;
		
		mLife = 0;

		mLifetime = (rand() % (init->mMaxLifetime - init->mMinLifetime)) + init->mMinLifetime;

		float angle = ((rand() % (init->mMaxRot - init->mMinRot)) + init->mMinRot) / 10.0f * cDelicious/180.0f;

		int temp = (rand() % (init->mMaxVelocity - init->mMinVelocity)) + init->mMinVelocity;

		mVelocity.x = temp * cos(angle);
		mVelocity.y = temp * sin(angle);

		mSprite->SetFadeMode(1, 1);
	}

	/*****************************************************************************/
	/*!
	\brief
		Updates the Physics of the particles separate from physics
	*/
	/*****************************************************************************/
	void Particle::UpdatePhysics(float dt)
	{
		if(!mActiveForces.empty())
		{
			//walk through all active forces of this object
			for(std::list<Force*>::iterator it = mActiveForces.begin(); (it != mActiveForces.end()) && !mActiveForces.empty() && (*it) != NULL;)
			{
				//update force and check if needs to be deleted
				int destroy = (*it)->Update(dt, &mVelocity, 1.0f);
				if(destroy == TRUE)
				{
					//delete the force
					std::list<Force*>::iterator it2;
					it2 = it;
					++it;
					delete (*it2);
					mActiveForces.remove(*it2);
				}
				else
					++it;
			}
		}


		mTransform->mPosition.x += mVelocity.x * dt;
		mTransform->mPosition.y += mVelocity.y * dt;
		mTransform->mRotation += mRotVel * dt;

	}

	/*****************************************************************************/
	/*!
	\brief
		Updates the life of particles and physics
	*/
	/*****************************************************************************/
	bool Particle::Update(float dt)
	{
		UpdatePhysics(dt);
		++mLife;

		if(--mLifetime <= 0)
			return true;
		else
			return false;
	}

	/*****************************************************************************/
	/*!
	\brief
		Sets the particle inactive
	*/
	/*****************************************************************************/
	void Particle::SetInactive()
	{
		mLeftRight = LEFT;
		mActive = false;
		mLife = 0;
		mVelocity.x = 0;
		mVelocity.y = 0;
		mSprite->SetFadeMode(-1, 1.0f);
		mRotVel = 0;
		ClearForces();
	}

	/*****************************************************************************/
	/*!
	\brief
		Particles Physics
	*/
	/*****************************************************************************/
	void Particle::AddForce(const float magnitude,const float direction,const DWORD flags,const float activeTime)
	{
		mActiveForces.push_back(new Force(magnitude, direction, activeTime, flags));
	}

	/*****************************************************************************/
	/*!
	\brief
		Adds the force for particles
	*/
	/*****************************************************************************/
	void Particle::AddForce(Force* a)
	{
		mActiveForces.push_back(a);
	}

	/*****************************************************************************/
	/*!
	\brief
		Clears forces for particles
	*/
	/*****************************************************************************/
	void Particle::ClearForces()
	{
		for(std::list<Force*>::iterator it = mActiveForces.begin(); (it != mActiveForces.end()) && !mActiveForces.empty() && (*it) != NULL;)
		{
			//delete the force
			std::list<Force*>::iterator it2;
			it2 = it;
			++it;
			delete (*it2);
			mActiveForces.remove(*it2);
		}
	}

	/*****************************************************************************/
	/*!
	\brief
		Sets the particles Active
	*/
	/*****************************************************************************/
	void Particle::Activate(EmitData* init)
	{
		mLeftRight = LEFT;

		mActive = true;
		
		mLife = 0;

		mLifetime = (rand() % (init->mMaxLifetime - init->mMinLifetime)) + init->mMinLifetime;

		float angle = ((rand() % (init->mMaxRot - init->mMinRot)) + init->mMinRot) / 10.0f * cDelicious/180.0f;

		int temp = (rand() % (init->mMaxVelocity - init->mMinVelocity)) + init->mMinVelocity;

		mVelocity.x = temp * cos(angle);
		mVelocity.y = temp * sin(angle);

		mSprite->SetFadeMode(1, 1);
	}

	/*****************************************************************************/
	/*!
	\brief
		Clears forces of particle
	*/
	/*****************************************************************************/
	Particle::~Particle()
	{
		ClearForces();
	}
}