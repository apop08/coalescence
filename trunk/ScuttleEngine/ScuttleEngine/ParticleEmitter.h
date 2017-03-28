/**************************************************************************************************/
/*!
\file	ParticleEmitter.h
\par	Project: Coalescence
\date	12/7/2011
\authors
Primary : Kevin Verholtz

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#pragma once

#include "Sprite.h"
#include "Physics.h"
#include "Graphics.h"
#include "PhysBody.h"
#include "ParticleManager.h"
#include "Particle.h"


namespace Scuttlemeyer
{
	//Need to forward declare Transform to use it here without stupid includes.
	class Transform;
	class Particle;
	class EmitData;

	class Emitter : public GameComponent
	{
	public:
		static void* operator new(unsigned)                                        
		{                                                                          
		   typedef select_allocator<sizeof(Emitter), false>::type alloc_type;  
		   return alloc_type::malloc();      
		}                                                                          
																					
		static void operator delete(void* data)                                    
		{                                                                              	
		   typedef select_allocator<sizeof(Emitter), false>::type alloc_type;  
		   alloc_type::free(data);                                                  
		}

		Emitter();
		virtual void Serialize(TextLoader& stream);
		void Initialize();
		void TypeUpdate(float dt);
		void TypeFire();
		void TypeCircle(float dt);
		void TypeFan();
		void TypePipe();
		void Update(float dt);
		void SpawnParticle(Transform * trans);
		~Emitter();
	public:
		bool mJetPack;
		int mDirection;
		int mPoolLoop;
		std::list<Particle*> mParticleList;
		EmitData* mEmitType;
	private:
		std::vector<Particle*> mParticlePool;
		Transform* mTransform;
		PhysBody* mPhysBod;
		int mRegionMaxDist;
		int mPipeSide;
		std::string mType;
		int mActiveCount;
		int mSpawnCount;
		int mCount;
	};
}