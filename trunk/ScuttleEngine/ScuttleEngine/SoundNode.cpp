/******************************************************************************/
/*!
\file   SoundNode.cpp
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM250
\date   3/1/2011
\brief  
		Non-positional sound node.
\copyright 
		“All content © 2012 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#include "Precompiled.h"
#include "SoundNode.h"
#include "Audio.h"
#include "Transform.h"

namespace Scuttlemeyer
{
	SoundNode::SoundNode()
	{
		mSounds.clear();
		mChannels.clear();
	}

	void SoundNode::Unload()
	{
		mSounds.clear();
		mChannels.clear();
	}

	void SoundNode::Initialize()
	{
	}

	void SoundNode::Serialize(TextLoader &stream)
	{
		std::string tempName;

		do
		{
			StreamRead(stream, tempName);

			if(tempName != "endlist")
			{
				FMOD::Sound* tempSound = AUDIO->GetSoundByName(tempName);

				if(tempSound)
				{
					mSoundNames.push_back(tempName);
					mSounds.push_back(tempSound);
				}
			}
		}while(tempName != "endlist");

		mChannels.resize(mSoundNames.size());
	}

	void SoundNode::Start(int num)
	{
		AUDIO->PlaySound(mSounds[num], &mChannels[num], AUDIO->GetVolume());
	}

	void SoundNode::Stop(int num)
	{
		AUDIO->StopSound(mSounds[num], &mChannels[num]);
	}

	SoundNode::~SoundNode()
	{
	}
}