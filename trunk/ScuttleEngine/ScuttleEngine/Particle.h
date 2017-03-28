/**************************************************************************************************/
/*!
\file	Particle.h
\par	Project: Coalescence
\date	12/7/2011
\authors
Primary : Kevin Verholtz

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#pragma once

#include "ParticleEmitter.h"
#include "ParticleManager.h"
#include "PhysVars.h"
#include "GameLogic.h"
#include "Factory.h"
namespace Scuttlemeyer
{
	//Need to forward declare Transform to use it here without stupid includes.
	class Transform;
	class EmitData;

	enum LEFTRIGHT{
		LEFT = 0,
		RIGHT
	};

	class Particle : public GameComponent
	{
	public:
		static void* operator new(unsigned)                                        
		{                                                                          
		   typedef select_allocator<sizeof(Particle), false>::type alloc_type;  
		   return alloc_type::malloc();      
		}                                                                          
																					
		static void operator delete(void* data)                                    
		{                                                                              	
		   typedef select_allocator<sizeof(Particle), false>::type alloc_type;  
		   alloc_type::free(data);                                                  
		}
		Particle();
		void Initialize(EmitData* init);
		void Activate(EmitData* init);
		bool Update(float dt);
		void UpdatePhysics(float dt);
		void SetInactive();
		void AddForce(Force* a);
		void ClearForces();
		void AddForce(const float magnitude,const float direction,const DWORD flags = 0,const float activeTime = cOneFrame);
		~Particle();
		
	public:
		int mLifetime;
		int mLife;
		bool mLeftRight;
		bool mActive;
		float mRotVel;
		Transform * mTransform;
		Sprite * mSprite;
		Vec2 mVelocity;
		std::list<Force*> mActiveForces;
	};
}