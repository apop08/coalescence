/**************************************************************************************************/
/*!
\file	ParticleEmitter.cpp
\par	Project: Coalescence
\date	12/7/2011
\authors
Primary : Kevin Verholtz
Secondary : Alex Poplawski

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#include "Precompiled.h"
#include "ParticleEmitter.h"
#include "Factory.h"
#include "GameStateMgr.h"
#include "Force.h"
#include "Regions.h"
#include "BrokenPipe.h"
#include "SoundNode.h"

#define SPEED 5
#define SPEED2 3

namespace Scuttlemeyer
{

	/*****************************************************************************/
	/*!
	\brief
		Makes a new empty emitter, sets direction to forward
	*/
	/*****************************************************************************/
	Emitter::Emitter()
	{
		mEmitType = new EmitData();
		mDirection = 1;
	}

	/*****************************************************************************/
	/*!
	\brief
		Initializes the emitter bassed on what the type is.
		Types are Labled in file
	*/
	/*****************************************************************************/
	void Emitter::Initialize()
	{
		mRegionMaxDist = -1;//for seeing if it has a region

		mCount = 0;

		mSpawnCount = mEmitType->mStatus;
		mPoolLoop = 0;
		mJetPack = false;
		mTransform = GetOwner()->has(Transform);
		mPhysBod = GetOwner()->has(PhysBody);

		std::list<Emitter*> check = PARTICLEMANAGER->mEmitterList;
		PARTICLEMANAGER->mEmitterList.push_back(this);

		//Setting up the Particle pool
		if(mType == "FIRE")
		{
			for(unsigned i = 0; i < mEmitType->mMaxParticles/4; ++i)
			{
				GOC* particleObj = LOGIC->SpawnObj(mEmitType->mParticleFile, mTransform, 0);
				mParticlePool.push_back(particleObj->has(Particle));
				(particleObj->has(Sprite))->SetFadeMode(-1, 1);
				particleObj = LOGIC->SpawnObj(mEmitType->mParticleFile, mTransform, 0);
				mParticlePool.push_back(particleObj->has(Particle));
				(particleObj->has(Sprite))->SetFadeMode(-1, 1);
				particleObj = LOGIC->SpawnObj(mEmitType->mParticleFile, mTransform, 0);
				mParticlePool.push_back(particleObj->has(Particle));
				(particleObj->has(Sprite))->SetFadeMode(-1, 1);
				particleObj = LOGIC->SpawnObj(mEmitType->mParticleFile, mTransform, 0);
				mParticlePool.push_back(particleObj->has(Particle));
				(particleObj->has(Sprite))->SetFadeMode(-1, 1);
			}
		}
		else if(mType == "FAN")
		{
			mRegionMaxDist = ((Region*)GetOwner()->has(Region))->GetMaxDistance();
			int moveAmount = SPEED;
			int mLifetime = mRegionMaxDist/moveAmount;
			int numParticles = mRegionMaxDist/(mEmitType->mStatus * moveAmount);

			mEmitType->mMaxParticles = numParticles;

			mJetPack = false;
			for(unsigned i = 0; i < mEmitType->mMaxParticles; ++i)
			{	
				GOC* particleObj;

				particleObj = LOGIC->SpawnObj(mEmitType->mParticleFile, mTransform, 0);
				mParticlePool.push_back(particleObj->has(Particle));
				(particleObj->has(Sprite))->SetFadeMode(-1, 1);
				(particleObj->has(Particle))->mLifetime = 0;
			}
		}
		else if(mType == "PIPE")
		{
			mPipeSide = GetOwner()->has(BrokenPipe)->GetSide();
			int mPipeState = GetOwner()->has(BrokenPipe)->GetState();
			//mJetPack = true;

			std::string particlefile;
			if(mPipeState == 3)
			{
				particlefile = "Particle6";
			}
			else if(mPipeState == 4)
			{
				particlefile = "Particle2";
			}
			else
			{
				particlefile = "Particle4";
			}
			for(unsigned i = 0; i < mEmitType->mMaxParticles; ++i)
			{
				GOC* particleObj = LOGIC->SpawnObj(particlefile, mTransform, 0);
				mParticlePool.push_back(particleObj->has(Particle));
				(particleObj->has(Sprite))->SetFadeMode(-1, 1);
			}
		}
		else
		{
			mJetPack = true;
			for(unsigned i = 0; i < mEmitType->mMaxParticles; ++i)
			{
				GOC* particleObj = LOGIC->SpawnObj(mEmitType->mParticleFile, mTransform, 0);
				mParticlePool.push_back(particleObj->has(Particle));
				(particleObj->has(Sprite))->SetFadeMode(-1, 1);
			}
		}
	}

	/*****************************************************************************/
	/*!
	\brief
		Initializes the Emitter bassed off the file
	*/
	/*****************************************************************************/
	void Emitter::Serialize(TextLoader& stream)
	{
		StreamRead(stream, mType);

		if(mType == "FIRE")
		{
			*mEmitType = PARTICLEMANAGER->mEmitMap.find("FIRE")->second;
		}
		else if(mType == "CIRCLE")
		{
			*mEmitType = PARTICLEMANAGER->mEmitMap.find("CIRCLE")->second;
		}
		else if(mType == "FAN")
		{
			*mEmitType = PARTICLEMANAGER->mEmitMap.find("FAN")->second;
		}
		else if(mType == "PIPE")
		{
			*mEmitType = PARTICLEMANAGER->mEmitMap.find("PIPE")->second;
		}
	}

	/*****************************************************************************/
	/*!
	\brief
		Call update function for each type of emitter
	*/
	/*****************************************************************************/
	void Emitter::TypeUpdate(float dt)
	{
		if(mType == "FIRE")
		{
			TypeFire();
		}
		else if(mType == "CIRCLE")
		{
			TypeCircle(dt);
		}
		else if(mType == "FAN")
		{
			TypeFan();
		}
		else if(mType == "PIPE")
		{
			TypePipe();
		}
	}

	/*****************************************************************************/
	/*!
	\brief
		Update for the fire type of emitter
	*/
	/*****************************************************************************/
	void Emitter::TypeFire()
	{
		for(std::list<Particle*>::iterator it = mParticleList.begin(); it != mParticleList.end(); ++it)
		{
			if((*it)->mLife > 0)
			{
				(*it)->mSprite->SetFadeMode(-1, 0.1f);
			}
			if(((PhysBody*)LOGIC->mPlayer->has(PhysBody))->mVelocity.y < 250.f)
			{
				mJetPack = false;
				LOGIC->mPlayer->has(SoundNode)->Stop(1);
			}
		}
	}

	/*****************************************************************************/
	/*!
	\brief
		Update for the Gas emitter
	*/
	/*****************************************************************************/
	void Emitter::TypeCircle(float dt)
	{
		if(mSpawnCount++ == mEmitType->mStatus && mParticleList.size() < mEmitType->mMaxParticles - 1)
		{
			mSpawnCount = 0;
			int i = 0;
			while(i < 10)
			{
				SpawnParticle(mTransform);
				++i;
			}
		}
		for(std::list<Particle*>::iterator it = mParticleList.begin(); it != mParticleList.end(); ++it)
		{
			Transform * trans = (*it)->mTransform;
			Sprite * sprite = (*it)->mSprite;
			(*it)->mLife++;
			
			PhysBody * emitPhys = mPhysBod;
			trans->mPosition.x += emitPhys->mVelocity.x * dt;
			trans->mPosition.y += emitPhys->mVelocity.y * dt;

			//Circle
			if((mTransform->mPosition.x - trans->mPosition.x) * (mTransform->mPosition.x - trans->mPosition.x) + (mTransform->mPosition.y - trans->mPosition.y) * (mTransform->mPosition.y - trans->mPosition.y)
				 > (((emitPhys->mExtents.x * mTransform->mScale.x) * (emitPhys->mExtents.x * mTransform->mScale.x)))/2.f)
			{
				trans->mPosition.x += (mTransform->mPosition.x - trans->mPosition.x)/2.5f;
				trans->mPosition.y += (mTransform->mPosition.y - trans->mPosition.y)*1.2f;
				(*it)->mVelocity.y *= -1.f;
			}
			else
			{
				trans->mPosition.x += (mTransform->mPosition.x - trans->mPosition.x)/500.f;
				trans->mPosition.y += (mTransform->mPosition.y - trans->mPosition.y)/500.f;
			}

			if((*it)->mLife > 9)
			{			
				if(trans->mScale.x < (emitPhys->mExtents.x * mTransform->mScale.x)/30.f)
					trans->mScale *= 1.119999f;
			}

			if((*it)->mLife%200 == 0)
			{
				sprite->SetFadeMode(1, 0.010f);
			}
			else if((*it)->mLife%100 == 0)
			{
				sprite->SetFadeMode(-1, 0.010f);
			}
		}
	}

	/*****************************************************************************/
	/*!
	\brief
		Update for the Fans emitters
	*/
	/*****************************************************************************/
	void Emitter::TypeFan()
	{
		PhysBody * FanPhys = mPhysBod;
		Transform* FanTran = mTransform;

		if(mDirection)//fan if pushing or pulling
		{
			for(std::list<Particle*>::iterator it = mParticleList.begin(); it != mParticleList.end(); ++it)
			{
				((*it)->mSprite)->SetFadeMode(1, 0.04f);

				(*it)->mVelocity.x = 0;
				(*it)->mVelocity.y = 0;

				Transform * trans = ((*it)->mTransform);

				/////Spot One
				//Left Right Up Down
				if((FanTran->mRotation > 45 && FanTran->mRotation < 135))
				{
					trans->mPosition.x -= SPEED;
					if((*it)->mLeftRight == LEFT)
					{
						if(trans->mPosition.y > FanTran->mPosition.y - ((FanPhys->mExtents.x/2) * FanTran->mScale.x))
						{
							trans->mPosition.y -= SPEED2;
						}
						else
						{
							(*it)->mLeftRight = RIGHT;
						}
					}
					else
					{
						if(trans->mPosition.y < FanTran->mPosition.y + ((FanPhys->mExtents.x/2) * FanTran->mScale.x))
						{
							trans->mPosition.y += SPEED2;
						}
						else
						{
							(*it)->mLeftRight = LEFT;
						}						
					}

					if(trans->mPosition.y <= FanTran->mPosition.y + SPEED2 && trans->mPosition.y >= FanTran->mPosition.y - SPEED2)
					{
						if((*it)->mLifetime > 30)
						{
							((*it)->mSprite)->SetFadeMode(-1, 1.f);
						}
					}
				}
				else if(FanTran->mRotation < -45 && FanTran->mRotation > -135)
				{
					trans->mPosition.x += SPEED;
					if((*it)->mLeftRight == LEFT)
					{
						if(trans->mPosition.y < FanTran->mPosition.y + ((FanPhys->mExtents.x/2) * FanTran->mScale.x))
						{
							trans->mPosition.y += SPEED2;
						}
						else
						{
							(*it)->mLeftRight = RIGHT;
						}
					}
					else
					{
						if(trans->mPosition.y > FanTran->mPosition.y - ((FanPhys->mExtents.x/2) * FanTran->mScale.x))
						{
							trans->mPosition.y -= SPEED2;
						}
						else
						{
							(*it)->mLeftRight = LEFT;
						}						
					}

					if(trans->mPosition.y <= FanTran->mPosition.y + SPEED2 && trans->mPosition.y >= FanTran->mPosition.y - SPEED2)
					{
						if((*it)->mLifetime > 30)
						{
							((*it)->mSprite)->SetFadeMode(-1, 1.f);
						}
					}
				}
				else if(FanTran->mRotation < 45 && FanTran->mRotation > -45)
				{
					trans->mPosition.y += SPEED;
					if((*it)->mLeftRight == LEFT)
					{
						if(trans->mPosition.x > FanTran->mPosition.x - ((FanPhys->mExtents.x/2) * FanTran->mScale.x))
						{
							trans->mPosition.x -= SPEED2;
						}
						else
						{
							(*it)->mLeftRight = RIGHT;
						}
					}
					else
					{
						if(trans->mPosition.x < FanTran->mPosition.x + ((FanPhys->mExtents.x/2) * FanTran->mScale.x))
						{
							trans->mPosition.x += SPEED2;
						}
						else
						{
							(*it)->mLeftRight = LEFT;
						}						
					}

					if(trans->mPosition.x <= FanTran->mPosition.x + SPEED2 && trans->mPosition.x >= FanTran->mPosition.x - SPEED2)
					{
						if((*it)->mLifetime > 30)
						{
							((*it)->mSprite)->SetFadeMode(-1, 1.f);
						}
					}
				}
				else
				{
					trans->mPosition.y -= SPEED;
					if((*it)->mLeftRight == LEFT)
					{
						if(trans->mPosition.x < FanTran->mPosition.x + ((FanPhys->mExtents.x/2) * FanTran->mScale.x))
						{
							trans->mPosition.x += SPEED2;
						}
						else
						{
							(*it)->mLeftRight = RIGHT;
						}
					}
					else
					{
						if(trans->mPosition.x > FanTran->mPosition.x - ((FanPhys->mExtents.x/2) * FanTran->mScale.x))
						{
							trans->mPosition.x -= SPEED2;
						}
						else
						{
							(*it)->mLeftRight = LEFT;
						}						
					}

					if(trans->mPosition.x <= FanTran->mPosition.x + SPEED2 && trans->mPosition.x >= FanTran->mPosition.x - SPEED2)
					{
						if((*it)->mLifetime > 30)
						{
							((*it)->mSprite)->SetFadeMode(-1, 1.f);
						}
					}
				}
				
				if((*it)->mLifetime < 25)
				{
					((*it)->mSprite)->SetFadeMode(-1, 0.04f);
				}
			}
		}
		else
		{
			for(std::list<Particle*>::iterator it = mParticleList.begin(); it != mParticleList.end(); ++it)
			{
				((*it)->mSprite)->SetFadeMode(1, 0.04f);
		
				(*it)->mVelocity.x = 0;
				(*it)->mVelocity.y = 0;
				
				Transform * trans = ((*it)->mTransform);

				/////Spot 2
				//Left Right Up Down
				if(FanTran->mRotation > 45 && FanTran->mRotation < 135)
				{
					trans->mPosition.x += SPEED;
					if((*it)->mLeftRight == LEFT)
					{
						if(trans->mPosition.y > FanTran->mPosition.y - ((FanPhys->mExtents.x/2) * FanTran->mScale.x))
						{
							trans->mPosition.y -= SPEED2;
						}
						else
						{
							(*it)->mLeftRight = RIGHT;
						}
					}
					else
					{
						if(trans->mPosition.y < FanTran->mPosition.y + ((FanPhys->mExtents.x/2) * FanTran->mScale.x))
						{
							trans->mPosition.y += SPEED2;
						}
						else
						{
							(*it)->mLeftRight = LEFT;
						}						
					}

					if(trans->mPosition.y <= FanTran->mPosition.y + SPEED2 && trans->mPosition.y >= FanTran->mPosition.y - SPEED2)
					{
						((*it)->mSprite)->SetFadeMode(-1, 1.f);
					}
				}
				else if(FanTran->mRotation < -45 && FanTran->mRotation > -135)
				{
					trans->mPosition.x -= SPEED;
					if((*it)->mLeftRight == LEFT)
					{
						if(trans->mPosition.y < FanTran->mPosition.y + ((FanPhys->mExtents.x/2) * FanTran->mScale.x))
						{
							trans->mPosition.y += SPEED2;
						}
						else
						{
							(*it)->mLeftRight = RIGHT;
						}
					}
					else
					{
						if(trans->mPosition.y > FanTran->mPosition.y - ((FanPhys->mExtents.x/2) * FanTran->mScale.x))
						{
							trans->mPosition.y -= SPEED2;
						}
						else
						{
							(*it)->mLeftRight = LEFT;
						}						
					}

					if(trans->mPosition.y <= FanTran->mPosition.y + SPEED2 && trans->mPosition.y >= FanTran->mPosition.y - SPEED2)
					{
						((*it)->mSprite)->SetFadeMode(-1, 1.f);
					}
				}
				else if(FanTran->mRotation < 45 && FanTran->mRotation > -45)
				{
					trans->mPosition.y -= SPEED;
					if((*it)->mLeftRight == LEFT)
					{
						if(trans->mPosition.x > FanTran->mPosition.x - ((FanPhys->mExtents.x/2) * FanTran->mScale.x))
						{
							trans->mPosition.x -= SPEED2;
						}
						else
						{
							(*it)->mLeftRight = RIGHT;
						}
					}
					else
					{
						if(trans->mPosition.x < FanTran->mPosition.x + ((FanPhys->mExtents.x/2) * FanTran->mScale.x))
						{
							trans->mPosition.x += SPEED2;
						}
						else
						{
							(*it)->mLeftRight = LEFT;
						}						
					}
			
					if(trans->mPosition.x <= FanTran->mPosition.x + SPEED2 && trans->mPosition.x >= FanTran->mPosition.x - SPEED2)
					{
						((*it)->mSprite)->SetFadeMode(-1, 1.f);
					}
				}
				else
				{
					trans->mPosition.y += SPEED;
					if((*it)->mLeftRight == LEFT)
					{
						if(trans->mPosition.x < FanTran->mPosition.x + ((FanPhys->mExtents.x/2) * FanTran->mScale.x))
						{
							trans->mPosition.x += SPEED2;
						}
						else
						{
							(*it)->mLeftRight = RIGHT;
						}
					}
					else
					{
						if(trans->mPosition.x > FanTran->mPosition.x - ((FanPhys->mExtents.x/2) * FanTran->mScale.x))
						{
							trans->mPosition.x -= SPEED2;
						}
						else
						{
							(*it)->mLeftRight = LEFT;
						}						
					}

					if(trans->mPosition.x <= FanTran->mPosition.x + SPEED2 && trans->mPosition.x >= FanTran->mPosition.x - SPEED2)
					{
						((*it)->mSprite)->SetFadeMode(-1, 1.f);
					}
				}	
			}
		}
	}

	/*****************************************************************************/
	/*!
	\brief
		Update for the Pipes that change states of objects emitter
	*/
	/*****************************************************************************/
	void Emitter::TypePipe()
	{
		for(std::list<Particle*>::iterator it = mParticleList.begin(); it != mParticleList.end(); ++it)
		{
			(*it)->mVelocity.x = 0;
			(*it)->mVelocity.y = 0;

			Transform * trans = ((*it)->mTransform);

			/////Spot One
			//Left Right Up Down
			if(mPipeSide == 1)
			{
				trans->mPosition.x -= SPEED;
			}
			else if(mPipeSide == 3)
			{
				trans->mPosition.x += SPEED;
			}
			else if(mPipeSide == 0)
			{
				trans->mPosition.y += SPEED;
			}
			else
			{
				trans->mPosition.y -= SPEED;
			}
			
			if((*it)->mLifetime < 25)
			{
				((*it)->mSprite)->SetFadeMode(-1, 0.04f);
			}
		}
	}

	/*****************************************************************************/
	/*!
	\brief
		Spawns particles and updates all the emitter and remove particles
	*/
	/*****************************************************************************/
	void Emitter::Update(float dt)
	{
		TypeUpdate(dt);

		if(mType != "CIRCLE")
		{
			if(mJetPack == true)
			{
				if(mEmitType->mStatus == 1 || mEmitType->mStatus == 0)
				{
					int i = 0;
					while((mParticleList.size()) < mEmitType->mMaxParticles - 1 && i < mEmitType->mSpawnTime && mEmitType->mStatus == 1)
					{
						SpawnParticle(mTransform);
						++i;
					}
				}
				else
				{
					if(mSpawnCount++ == mEmitType->mStatus)
					{
						mSpawnCount = 0;
						int i = 0;
						while((mParticleList.size()) < mEmitType->mMaxParticles - 1 && i < mEmitType->mSpawnTime)
						{
							SpawnParticle(mTransform);
							++i;
						}
					}
				}
			}

			for(std::list<Particle*>::iterator it = mParticleList.begin(); it != mParticleList.end();)
			{
				if((*it)->Update(dt))
				{
					std::list<Particle*>::iterator it2 = it;
					++it;

					(*it2)->mActive = false;
					
					(*it2)->mSprite->SetFadeMode(-1, 1);
					mParticleList.remove(*it2);
				}
				else ++it;
			}
		}
		else
		{
			for(std::list<Particle*>::iterator it = mParticleList.begin(); it != mParticleList.end();++it)
			{
				(*it)->UpdatePhysics(dt);
			}
		}
	}

	/*****************************************************************************/
	/*!
	\brief
		Spawns particle bassed off of the Type of the emitter
	*/
	/*****************************************************************************/
	void Emitter::SpawnParticle(Transform * trans)
	{
		if(mRegionMaxDist != -1)//fan spin
		{
			while(mParticlePool[mPoolLoop]->mLifetime != 0)
			{
				if(mPoolLoop >= (int)mEmitType->mMaxParticles - 1)
				{
					mPoolLoop = 0;
				}
				else
				{
					++mPoolLoop;
				}
			}
		}
		else
		{
			if(mPoolLoop >= (int)mEmitType->mMaxParticles - 1)
			{
				mPoolLoop = 0;
			}
			else
			{
				++mPoolLoop;
			}
		}

		mParticleList.push_back(mParticlePool[mPoolLoop]);
		Particle * particle = mParticlePool[mPoolLoop];
		particle->mTransform->mPosition = mTransform->mPosition;
		particle->mTransform->mRotation = mTransform->mRotation;

		if(mType == "CIRCLE")
		{
			PhysBody * emitPhys = mPhysBod;

			particle->mTransform->mPosition.x += (rand() % (int)(emitPhys->mExtents.x * mTransform->mScale.x * 2)) - emitPhys->mExtents.x * mTransform->mScale.x;
			particle->mTransform->mPosition.y += (rand() % (int)(emitPhys->mExtents.y * mTransform->mScale.y * 2)) - emitPhys->mExtents.y * mTransform->mScale.y;
		}
		else if(mType == "FAN")
		{
			bool sideLeft = true;
			mCount++;
			if(mCount%2 == 0)
			{
				sideLeft = false;
			}

			if(mDirection)//fan spin
			{
				PhysBody * emitPhys = mPhysBod;
				
				//LEFT RIGHT UP DOWN
				if(mTransform->mRotation > 45 && mTransform->mRotation < 135)
				{
					if(sideLeft)
					{
						particle->mTransform->mPosition.y -= (rand() % (int)(emitPhys->mExtents.x * mTransform->mScale.x)/2);
					}
					else
					{
						particle->mTransform->mPosition.y += (rand() % (int)(emitPhys->mExtents.x * mTransform->mScale.x)/2);
					}

					if(particle->mTransform->mPosition.y < mTransform->mPosition.y)
					{
						particle->mLeftRight = RIGHT;
					}
					else
					{
						particle->mLeftRight = LEFT;
					}
				}
				else if(mTransform->mRotation < -45 && mTransform->mRotation > -135)
				{
					if(sideLeft)
					{
						particle->mTransform->mPosition.y += (rand() % (int)(emitPhys->mExtents.x * mTransform->mScale.x)/2);
					}
					else
					{
						particle->mTransform->mPosition.y -= (rand() % (int)(emitPhys->mExtents.x * mTransform->mScale.x)/2);
					}

					if(particle->mTransform->mPosition.y > mTransform->mPosition.y)
					{
						particle->mLeftRight = RIGHT;
					}
					else
					{
						particle->mLeftRight = LEFT;
					}
				}
				else if(mTransform->mRotation < 45 && mTransform->mRotation > -45)
				{
					if(sideLeft)
					{
						particle->mTransform->mPosition.x += (rand() % (int)(emitPhys->mExtents.x * mTransform->mScale.x)/2);
					}
					else
					{
						particle->mTransform->mPosition.x -= (rand() % (int)(emitPhys->mExtents.x * mTransform->mScale.x)/2);
					}

					if(particle->mTransform->mPosition.x < mTransform->mPosition.x)
					{
						particle->mLeftRight = RIGHT;
					}
					else
					{
						particle->mLeftRight = LEFT;
					}
				}
				else
				{
					if(sideLeft)
					{
						particle->mTransform->mPosition.x -= (rand() % (int)(emitPhys->mExtents.x * mTransform->mScale.x)/2);
					}
					else
					{
						particle->mTransform->mPosition.x -= (rand() % (int)(emitPhys->mExtents.x * mTransform->mScale.x)/2);
					}
				
					if(particle->mTransform->mPosition.x > mTransform->mPosition.x)
					{
						particle->mLeftRight = RIGHT;
					}
					else
					{
						particle->mLeftRight = LEFT;
					}
				}
			}
			else
			{
				/////Spot 4
				PhysBody * emitPhys = mPhysBod;

				int maxDist  = mRegionMaxDist;

				//LEFT RIGHT UP DOWN
				if(mTransform->mRotation > 45 && mTransform->mRotation < 135)
				{
					if(sideLeft)
					{
						particle->mTransform->mPosition.y -= (rand() % (int)(emitPhys->mExtents.x * mTransform->mScale.x)/2);
					}
					else
					{
						particle->mTransform->mPosition.y += (rand() % (int)(emitPhys->mExtents.x * mTransform->mScale.x)/2);
					}

					particle->mTransform->mPosition.x -= (maxDist);

					if(particle->mTransform->mPosition.y < mTransform->mPosition.y)
					{
						particle->mLeftRight = RIGHT;
					}
					else
					{
						particle->mLeftRight = LEFT;
					}
				}
				else if(mTransform->mRotation < -45 && mTransform->mRotation > -135)
				{
					if(sideLeft)
					{
						particle->mTransform->mPosition.y += (rand() % (int)(emitPhys->mExtents.x * mTransform->mScale.x)/2);
					}
					else
					{
						particle->mTransform->mPosition.y -= (rand() % (int)(emitPhys->mExtents.x * mTransform->mScale.x)/2);
					}
					particle->mTransform->mPosition.x += maxDist;

					if(particle->mTransform->mPosition.y > mTransform->mPosition.y)
					{
						particle->mLeftRight = RIGHT;
					}
					else
					{
						particle->mLeftRight = LEFT;
					}
				}
				else if(mTransform->mRotation < 45 && mTransform->mRotation > -45)
				{
					if(sideLeft)
					{
						particle->mTransform->mPosition.x += (rand() % (int)(emitPhys->mExtents.x * mTransform->mScale.x)/2);
					}
					else
					{
						particle->mTransform->mPosition.x -= (rand() % (int)(emitPhys->mExtents.x * mTransform->mScale.x)/2);
					}

					particle->mTransform->mPosition.y += maxDist;
					
					if(particle->mTransform->mPosition.x < mTransform->mPosition.x)
					{
						particle->mLeftRight = RIGHT;
					}
					else
					{
						particle->mLeftRight = LEFT;
					}
				}
				else
				{
					if(sideLeft)
					{
						particle->mTransform->mPosition.x -= (rand() % (int)(emitPhys->mExtents.x * mTransform->mScale.x)/2);
					}
					else
					{
						particle->mTransform->mPosition.x += (rand() % (int)(emitPhys->mExtents.x * mTransform->mScale.x)/2);
					}

					particle->mTransform->mPosition.y -= maxDist;
					
					if(particle->mTransform->mPosition.x > mTransform->mPosition.x)
					{
						particle->mLeftRight = RIGHT;
					}
					else
					{
						particle->mLeftRight = LEFT;
					}
				}
			}
		}
		else if(mType == "FIRE")
		{
			PhysBody * emitPhys = mPhysBod;
			particle->mTransform->mPosition.x += ((rand() % (int)(emitPhys->mExtents.x * mTransform->mScale.x) * 100)/200.f) - (emitPhys->mExtents.x * mTransform->mScale.x)/4.f;

			if(GetOwner()->has(CompSprite)->GetFacing() == true)
			{

				float diff = particle->mTransform->mPosition.x - (mTransform->mPosition.x + (emitPhys->mExtents.x * mTransform->mScale.x)/2);
				particle->mTransform->mPosition.y -= diff;
				particle->mTransform->mPosition.y -= 6;

				particle->mTransform->mPosition.x -= (emitPhys->mExtents.x * mTransform->mScale.x) + 4;
			}
			else
			{
				float diff = (mTransform->mPosition.x - (emitPhys->mExtents.x * mTransform->mScale.x)/2) -  particle->mTransform->mPosition.x;
				particle->mTransform->mPosition.y -= diff;
				particle->mTransform->mPosition.y -= 6;

				particle->mTransform->mPosition.x += (emitPhys->mExtents.x * mTransform->mScale.x) + 4;
			}
			//(mParticlePool[mPoolLoop])->mVelocity.x *= 1 + mTransform->mScale.x;
		}
		else if(mType == "PIPE")
		{
			bool sideLeft = true;
			mCount++;
			if(mCount%2 == 0)
			{
				sideLeft = false;
			}

			PhysBody * emitPhys = mPhysBod;
			
			//LEFT RIGHT UP DOWN
			if(mPipeSide == 1)
			{				
				if(sideLeft)
				{
					particle->mTransform->mPosition.y -= (rand() % (int)(emitPhys->mExtents.x * mTransform->mScale.x)/2);
				}
				else
				{
					particle->mTransform->mPosition.y += (rand() % (int)(emitPhys->mExtents.x * mTransform->mScale.x)/2);
				}
				particle->mTransform->mPosition.x -= ((int)(emitPhys->mExtents.y * mTransform->mScale.y));


				if(particle->mTransform->mPosition.y < mTransform->mPosition.y)
				{
					particle->mLeftRight = RIGHT;
				}
				else
				{
					particle->mLeftRight = LEFT;
				}
			}
			else if(mPipeSide == 3)
			{
				if(sideLeft)
				{
					particle->mTransform->mPosition.y -= (rand() % (int)(emitPhys->mExtents.x * mTransform->mScale.x)/2);
				}
				else
				{
					particle->mTransform->mPosition.y += (rand() % (int)(emitPhys->mExtents.x * mTransform->mScale.x)/2);
				}

				if(particle->mTransform->mPosition.y > mTransform->mPosition.y)
				{
					particle->mLeftRight = RIGHT;
				}
				else
				{
					particle->mLeftRight = LEFT;
				}
			}
			else if(mPipeSide == 0)
			{
				if(sideLeft)
				{
					particle->mTransform->mPosition.x -= (rand() % (int)(emitPhys->mExtents.x * mTransform->mScale.x)/2);
				}
				else
				{
					particle->mTransform->mPosition.x += (rand() % (int)(emitPhys->mExtents.x * mTransform->mScale.x)/2);
				}

				if(particle->mTransform->mPosition.x < mTransform->mPosition.x)
				{
					particle->mLeftRight = RIGHT;
				}
				else
				{
					particle->mLeftRight = LEFT;
				}
			}
			else
			{
				if(sideLeft)
				{
					particle->mTransform->mPosition.x -= (rand() % (int)(emitPhys->mExtents.x * mTransform->mScale.x)/2);
				}
				else
				{
					particle->mTransform->mPosition.x += (rand() % (int)(emitPhys->mExtents.x * mTransform->mScale.x)/2);
				}
			
				if(particle->mTransform->mPosition.x > mTransform->mPosition.x)
				{
					particle->mLeftRight = RIGHT;
				}
				else
				{
					particle->mLeftRight = LEFT;
				}
			}
		}
		mParticlePool[mPoolLoop]->Initialize(mEmitType);

		if(mType == "FAN")
		{
			int moveAmount = SPEED;
			mParticlePool[mPoolLoop]->mLifetime = mRegionMaxDist/moveAmount;
		}
		else if(mType == "PIPE")
		{
			mParticlePool[mPoolLoop]->mSprite->SetRandomAlpha(.6f, .9f);
		}
		else if(mType == "FIRE")
		{
			mParticlePool[mPoolLoop]->mVelocity.x = 0;
			mParticlePool[mPoolLoop]->mVelocity.y = 0;
			Vec2 posOutside = particle->mTransform->mPosition;

			Vec2 posCenter;
			if(GetOwner()->has(CompSprite)->GetFacing() == true)
			{
				posCenter.x = mTransform->mPosition.x - (mPhysBod->mExtents.x * mTransform->mScale.x + 4) - 15;
			}
			else
			{
				posCenter.x = mTransform->mPosition.x + (mPhysBod->mExtents.x * mTransform->mScale.x + 4) + 15;
			}
			posCenter.y = mTransform->mPosition.y - 20;
			Vec2 dir = posCenter - posOutside;
			//normilize
			float distance = sqrt(dir.x * dir.x + dir.y * dir.y);
			dir.x /= distance;
			dir.y /= distance;

			float direction = atan2(dir.y, dir.x);
			
			Velocity* velocity = &((PhysBody*)LOGIC->mPlayer->has(PhysBody))->mVelocity;
			float temp = (velocity->y / 15.f - mEmitType->mMaxVelocity - mEmitType->mMinVelocity );
			int temp2 =  (1000 * (int)temp + 1);
			int temp3 = 800 - (int)velocity->y;
			if(temp3 == 0)
				temp3 = 1;
			float mag = (rand() % (temp3) + mEmitType->mMinVelocity)/10.f;
			mParticlePool[mPoolLoop]->AddForce((float)mag, direction);
			mParticlePool[mPoolLoop]->mSprite->SetRandomAlpha(.5f, .9f);
		}

		if(!mDirection)
		{
			mParticlePool[mPoolLoop]->mSprite->SetFadeMode(-1, 1);
		}
	}

	/*****************************************************************************/
	/*!
	\brief
		Fade destroys all the Particles for the emitter
	*/
	/*****************************************************************************/
	Emitter::~Emitter()
	{
		mParticleList.clear();
		int test = gGameStateNext;
		if(gGameStateNext == GS_PLAY)
		{
			for(std::vector<Particle *>::iterator it = mParticlePool.begin(); it != mParticlePool.end(); ++it)
			{
				(*it)->GetOwner()->FadeDestroy(0.02f);
			}
		}
		mParticlePool.clear();

		PARTICLEMANAGER->mEmitterList.remove(this);
	}

}