/**************************************************************************************************/
/*!
\file	ParticleManager.cpp
\par	Project: Coalescence
\date	12/7/2011
\authors
Primary : Kevin Verholtz

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#include "Precompiled.h"
#include "ParticleManager.h"
#include "ComponentCreator.h"
#include "DebugConsole.h"
#include "GameStateMgr.h"
#include "Core.h"
#include "Force.h"

namespace Scuttlemeyer
{
	ParticleManager* PARTICLEMANAGER = NULL;

	EmitData::EmitData(){}

	EmitData::~EmitData(){}

	/*****************************************************************************/
	/*!
	\brief
		Make this the Particle Manager System
	*/
	/*****************************************************************************/
	ParticleManager::ParticleManager()
	{
		PARTICLEMANAGER = this;
		RegisterComponent(Particle);
		RegisterComponent(Emitter);
	}

	/*****************************************************************************/
	/*!
	\brief
		Nothing to do here
	*/
	/*****************************************************************************/
	void ParticleManager::Unload()
	{
#ifdef BLARG
		Message* q = new Message(Mid::DebugCNSL, 1, new std::string("ParticleManager Unload."));
		CORE->EnqueueMessage(q);
#endif
	}
	/*****************************************************************************/
	/*!
	\brief
		Nothing to do here
	*/
	/*****************************************************************************/
	void ParticleManager::Load()
	{
#ifdef BLARG
		Message* q = new Message(Mid::DebugCNSL, 1, new std::string("ParticleManager Load."));
		CORE->EnqueueMessage(q);
#endif
	}

	/*****************************************************************************/
	/*!
	\brief
		Clear the Emiter Map
	*/
	/*****************************************************************************/
	void ParticleManager::Free()
	{
#ifdef BLARG
		Message* q = new Message(Mid::DebugCNSL, 1, new std::string("ParticleManager Free."));
		CORE->EnqueueMessage(q);
#endif
		mEmitMap.clear();
	}
	
	/*****************************************************************************/
	/*!
	\brief
		Loads the Emitters from the file for all types of Emitters
	*/
	/*****************************************************************************/
	void ParticleManager::Initialize()
	{
#ifdef BLARG
		Message* q = new Message(Mid::DebugCNSL, 1, new std::string("ParticleManager Init."));
		CORE->EnqueueMessage(q);
#endif
		LoadEmitters("..//Content//Levels//Level_0_emit.txt");
	}

	/*****************************************************************************/
	/*!
	\brief
		Function that loads the emitters given the file as a parameter
	*/
	/*****************************************************************************/
	void ParticleManager::LoadEmitters(std::string filename)	
	{
		TextLoader stream;
		bool fileOpened = stream.Open(filename);
		
		while(stream.IsGood())
		{
			EmitData data;// = new EmitData();
			std::string emitType;
			StreamRead(stream, emitType);
			StreamRead(stream, std::string());
			StreamRead(stream, data.mParticleFile);
			StreamRead(stream, std::string());
			StreamRead(stream, data.mSpawnTime);
			StreamRead(stream, std::string());
			StreamRead(stream, data.mMaxParticles);
			StreamRead(stream, std::string());
			StreamRead(stream, data.mMaxLifetime);
			StreamRead(stream, std::string());
			StreamRead(stream, data.mMinLifetime);
			StreamRead(stream, std::string());
			StreamRead(stream, data.mMaxRot);
			StreamRead(stream, std::string());
			StreamRead(stream, data.mMinRot);
			StreamRead(stream, std::string());
			StreamRead(stream, data.mMaxVelocity);
			StreamRead(stream, std::string());
			StreamRead(stream, data.mMinVelocity);
			StreamRead(stream, std::string());
			StreamRead(stream, data.mStatus);
			mEmitMap.insert(std::pair<std::string, EmitData>(emitType, data));
		}
	}

	/*****************************************************************************/
	/*!
	\brief
		Updates all the emitters in the level
	*/
	/*****************************************************************************/
	void ParticleManager::Update(float dt)
	{
		if((DBGCNSL->DBConsole == 0 || DBGCNSL->mSteps > 0) && gGameStateCurr != GS_PAUSE && !CORE->mBEditorActive)
		{
			for(std::list<Emitter*>::iterator it = mEmitterList.begin(); it !=mEmitterList.end(); ++it)
			{
				(*it)->Update(dt);
			}
		}

	}

	Mid::MessageIdType ParticleManager::SendMessage(Message *message)
	{
		return Mid::None;
	}

	ParticleManager::~ParticleManager()
	{}
}