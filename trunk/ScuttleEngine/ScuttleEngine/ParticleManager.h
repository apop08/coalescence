/**************************************************************************************************/
/*!
\file	ParticleManager.h
\par	Project: Coalescence
\date	12/7/2011
\authors
Primary : Kevin Verholtz

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#pragma once

#include "Engine.h"
#include "ParticleEmitter.h"
#include "Particle.h"

namespace Scuttlemeyer
{
	class Emitter;
	class Force;
	class EmitData
	{
	public:
		EmitData();
		~EmitData();
	public:
		int mMaxLifetime;
		int mMinLifetime;
		int mMaxVelocity;
		int mMinVelocity;
		int mMaxRot;
		int mMinRot;
		int mSpawnTime;
		unsigned int mMaxParticles;
		DWORD mStatus;
		std::string mParticleFile;
	};

	class ParticleManager : public ISystem
	{
	public:
		ParticleManager();
		void Initialize();
		void LoadEmitters(std::string filename);
		void Update(float dt);
		virtual std::string GetName() { return "Particles"; }
		Mid::MessageIdType SendMessage(Message *message);
		void Unload();

		void Load();
		void Free();
		
		~ParticleManager();
	public:
		std::list<Emitter*> mEmitterList;
		stdext::hash_map<std::string, EmitData> mEmitMap;
	};

	extern ParticleManager* PARTICLEMANAGER;
}