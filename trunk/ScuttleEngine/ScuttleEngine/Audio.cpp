/******************************************************************************/
/*!
\file   Audio.cpp
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

#include "Precompiled.h"
#include "Audio.h"
#include "ComponentCreator.h"
#include "TextSerialization.h"

#include "Core.h"

#pragma comment(lib, "fmodex_vc.lib")

namespace Scuttlemeyer
{
	Audio* AUDIO = NULL;

	Audio::Audio()
	{
		mVolume = 1.0f;
		mPreviousVolume = mVolume;

		//Create the FMOD system when the Audio system starts.
		mResult = FMOD::System_Create(&mSystem);
		mResult = mChannel->setVolume(mVolume);

		mBIsFirstRun = true;

		RegisterComponent(SoundNode);
		RegisterComponent(AttenuatedSoundNode);

		AUDIO = this;
	}

	void Audio::Unload()
	{
#ifdef BLARG
		Message* q = new Message(Mid::DebugCNSL, 1, new std::string("Audio Unload."));
		CORE->EnqueueMessage(q);
#endif
		//mSongs.clear();
		//mSfxs.clear();
		//mResult = mSystem->close();
	}
	void Audio::Load()
	{
		if(mBIsFirstRun)
		{
			//Initialize the FMOD system.
			//Currently defaulted to 100 channels.  Feel free to increase this later.
			mResult = mSystem->init(500, FMOD_INIT_NORMAL, 0);

			TextLoader stream;
			FMOD::Sound* tempSound;

			//Currently loading the audio assets from it's own file.
			//This is pretty subject to change once object comp is running.
			stream.Open("..//Content//Levels//AudioList.txt");

			while(stream.IsGood())
			{
				std::string tempType;
				std::string tempPath;

				//Get the filepath and type from the list file.
				StreamRead(stream, tempType);
				StreamRead(stream, tempPath);

				//If the type is Song, we create it in memory as a looped stream.
				if(tempType == "Song")
				{
					mResult = mSystem->createStream(tempPath.c_str(), FMOD_LOOP_NORMAL, 0, &tempSound);
				}
				//If the type is a standard sound effect, we create it in memory as an unlooped sound.
				else if(tempType == "Sfx")
				{
					mResult = mSystem->createSound(tempPath.c_str(), FMOD_DEFAULT, 0, &tempSound);
				}
				//Otherwise it's a looped sound effect, and we create it in memory as a looped sound.
				else
				{
					mResult = mSystem->createSound(tempPath.c_str(), FMOD_LOOP_NORMAL, 0, &tempSound);
				}

				//Whatever it was, add it to the map.
				mSounds.insert(std::pair<std::string, FMOD::Sound*>(tempPath, tempSound));
			}

			PlaySong("..//Content//Audio//AutoPilot.mp3");
			mBIsFirstRun = false;
		}

		//If everything went down in flames, we let the console know that!
		if(mResult != FMOD_OK)
		{
#ifdef BLARG
			Message* q = new Message(Mid::DebugCNSL, 1, new std::string("Audio failed to initialize."));
			CORE->EnqueueMessage(q);
#endif
		}
		//Or we send a message that audio started successfully.
		else
		{
#ifdef BLARG
			Message* q = new Message(Mid::DebugCNSL, 1, new std::string("Audio Load."));
			CORE->EnqueueMessage(q);
#endif
		}
	}
	void Audio::Free()
	{
#ifdef BLARG
		Message* q = new Message(Mid::DebugCNSL, 1, new std::string("Audio Free."));
		CORE->EnqueueMessage(q);
#endif
		//StopSong(0);
	}
	void Audio::Initialize()
	{
#ifdef BLARG
		Message* q = new Message(Mid::DebugCNSL, 1, new std::string("Audio Init."));
		CORE->EnqueueMessage(q);
#endif
		//PlaySong(0);
	}

	void Audio::Update(float dt)
	{
		for(ObjectLinkedList<AttenuatedSoundNode>::iterator it = mPosSoundList.begin(); it != mPosSoundList.end(); ++it)
		{
			it->Update();
		}
		//FMOD update needs to run once a frame, not less, not more.
		mResult = mSystem->update();
	}

	void Audio::PlaySong(std::string song)
	{
		stdext::hash_map<std::string, FMOD::Sound*>::iterator it = mSounds.find(song);

		if(it != mSounds.end())
		{
			mResult = mSystem->playSound(FMOD_CHANNEL_FREE, it->second, false, &mChannel);
			mChannel->setVolume(mVolume);
		}
	}

	void Audio::StopSong(std::string song)
	{
		stdext::hash_map<std::string, FMOD::Sound*>::iterator it = mSounds.find(song);

		if(it != mSounds.end())
		{
			mResult = mSystem->playSound(FMOD_CHANNEL_FREE, it->second, true, &mChannel);
			mChannel->setVolume(mVolume);
		}
	}

	void Audio::PlaySound(FMOD::Sound* sound, FMOD::Channel** channel, float volume)
	{
		mResult = mSystem->playSound(FMOD_CHANNEL_FREE, sound, false, channel);
		(*channel)->setVolume(volume);
	}

	void Audio::StopSound(FMOD::Sound* sound, FMOD::Channel** channel)
	{
		mResult = (*channel)->stop();
	}

	FMOD::Sound* Audio::GetSoundByName(std::string name)
	{
		stdext::hash_map<std::string, FMOD::Sound*>::iterator it = mSounds.find(name);

		FMOD::Sound* tempSound = it != mSounds.end() ? it->second : NULL;

		return tempSound;
	}

	Mid::MessageIdType Audio::SendMessage(Message *message)
	{
		if(message->mMessageId == Mid::Pause)
		{
			if(mVolume - 0.5f >= 0.0f)
			{
				SetVolume(mVolume - 0.5f);
			}
			else
			{
				SetVolume(0.0f);
			}
		}

		if(message->mMessageId == Mid::Play || message->mMessageId == Mid::AudioMenu)
		{
			SetVolume(mPreviousVolume);
		}

		return Mid::None;
	}

	void Audio::IncrementVolume()
	{
		if((mVolume + 0.1f) <= 1.0f)
		{
			mVolume += 0.1f;
			mPreviousVolume = mVolume;
			mChannel->setVolume(mVolume);
		}
	}

	void Audio::DecrementVolume()
	{
		if((mVolume - 0.1f) >= 0.0f)
		{
			mVolume -= 0.1f;
			mPreviousVolume = mVolume;
			mChannel->setVolume(mVolume);
		}
	}

	void Audio::SetVolume(float volume)
	{
		if(volume <= 1.0f && volume >= 0.0f)
		{
			mPreviousVolume = mVolume;
			mVolume = volume;
			mChannel->setVolume(mVolume);
		}
	}

	float Audio::GetVolume()
	{
		return mVolume;
	}

	Audio::~Audio()
	{
		mResult = mSystem->release();
	}
}