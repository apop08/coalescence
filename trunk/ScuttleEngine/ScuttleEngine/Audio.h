/******************************************************************************/
/*!
\file   Audio.h
\author Matt Sutherlin
\author Kevin Verholtz (Reload routines)
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   7/21/2011
\brief  
FMOD based audio system.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#pragma once

#include "Engine.h"
#include "TextSerialization.h"
#include "fmod.hpp"
#include "SoundNode.h"
#include "AttenuatedSoundNode.h"

namespace Scuttlemeyer
{
	class Audio : public ISystem
	{
	public:
		Audio();
		void Unload();
		void Load();
		void Free();
		void Initialize();
		void Update(float dt);
		virtual std::string GetName() { return "Audio"; }

		void PlaySong(std::string song);
		void StopSong(std::string song);
		void PlaySound(FMOD::Sound* sound, FMOD::Channel** channel, float volume);
		void StopSound(FMOD::Sound* sound, FMOD::Channel** channel);
		Mid::MessageIdType SendMessage(Message *message);

		FMOD::Sound* GetSoundByName(std::string name);

		void IncrementVolume();
		void DecrementVolume();
		void SetVolume(float volume);
		float GetVolume();

		~Audio();
	public:
		ObjectLinkedList<AttenuatedSoundNode> mPosSoundList;
	private:
		FMOD_RESULT mResult;
		FMOD::System* mSystem;
		stdext::hash_map<std::string, FMOD::Sound*> mSounds;
		FMOD::Channel* mChannel;
		float mVolume;
		float mPreviousVolume;
		bool mBIsFirstRun;
	};

	extern Audio* AUDIO;
}