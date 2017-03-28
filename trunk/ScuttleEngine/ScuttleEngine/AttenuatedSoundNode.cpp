/******************************************************************************/
/*!
\file   AttenuatedSoundNode.cpp
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM250
\date   3/1/2011
\brief  
		Positional sound node.
\copyright 
		“All content © 2012 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#include "Precompiled.h"
#include "AttenuatedSoundNode.h"
#include "Audio.h"
#include "Transform.h"
#include "GameStateMgr.h"

#include "Graphics.h"
#include "Camera.h"

namespace Scuttlemeyer
{
	AttenuatedSoundNode::AttenuatedSoundNode()
	{
		mSounds.clear();
		mChannels.clear();
		mTransform = NULL;

		mInnerOffset = 0.f;
		mOuterOffset = 1.f;
	}

	void AttenuatedSoundNode::Unload()
	{
		for(unsigned i = 0; i < mSounds.size(); ++i)
		{
			AUDIO->StopSound(mSounds[i], &mChannels[i]);
		}

		mSounds.clear();
		mChannels.clear();
		mTransform = NULL;
		AUDIO->mPosSoundList.erase(this);
	}

	void AttenuatedSoundNode::Initialize()
	{
		mTransform = GetOwner()->has(Transform);
		AUDIO->mPosSoundList.push_back(this);
	}

	void AttenuatedSoundNode::Serialize(TextLoader &stream)
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

		StreamRead(stream, mInnerOffset);
		StreamRead(stream, mOuterOffset);
	}

	void AttenuatedSoundNode::Start(int num)
	{
		//float p = DistanceSquared(GRAPHICS->mCurrentCamera->mTransform->mPosition, mTransform->mPosition);
		//float inner = mInnerOffset * mInnerOffset;
		//float outer = mOuterOffset * mOuterOffset;
		//float sp = (mOuterOffset * mOuterOffset) - (mInnerOffset * mInnerOffset);

		//if(p < inner)
		//{
		//	//Play at current system volume if within the inner radius.
		//	AUDIO->PlaySound(mSounds[num], &mChannels[num], AUDIO->GetVolume());
		//}
		//else if(p > outer)
		//{
		//	//Play at 0 volume if outside of the outer radius.
		//	AUDIO->PlaySound(mSounds[num], &mChannels[num], 0.f);
		//}
		//else
		//{
		//	//((sp + outer) - p) / ((sp + outer) - (sp + inner))
		//	float attenVol = AUDIO->GetVolume() * (((sp + outer) - p) / ((sp + outer) - (sp + inner)));

		//	//Play at an attenuated volume level if between the inner and outer radius.
		//	AUDIO->PlaySound(mSounds[num], &mChannels[num], attenVol);
		//}

		AUDIO->PlaySound(mSounds[num], &mChannels[num], AUDIO->GetVolume());
	}

	void AttenuatedSoundNode::Stop(int num)
	{
		AUDIO->StopSound(mSounds[num], &mChannels[num]);
	}

	void AttenuatedSoundNode::Update()
	{
		if(gGameStateCurr == GS_PAUSE)
		{
			for(unsigned i = 0; i < mChannels.size(); ++i)
			{
				mChannels[i]->setVolume(0.f);
			}
		}
		else
		{
			//float p = DistanceSquared(GRAPHICS->mCurrentCamera->mTransform->mPosition, mTransform->mPosition);
			//float inner = mInnerOffset * mInnerOffset;
			//float outer = mOuterOffset * mOuterOffset;
			//float sp = (mOuterOffset * mOuterOffset) - (mInnerOffset * mInnerOffset);

			//if(p < inner)
			//{
			//	for(unsigned i = 0; i < mChannels.size(); ++i)
			//	{
			//		//Play at current system volume if within the inner radius.
			//		mChannels[i]->setVolume(AUDIO->GetVolume());
			//	}
			//}
			//else if(p > outer)
			//{
			//	for(unsigned i = 0; i < mChannels.size(); ++i)
			//	{
			//		//Play at 0 volume if outside of the outer radius.
			//		mChannels[i]->setVolume(0.f);
			//	}
			//}
			//else
			//{
			//	//((sp + outer) - p) / ((sp + outer) - (sp + inner))
			//	float attenVol = AUDIO->GetVolume() * (((sp + outer) - p) / ((sp + outer) - (sp + inner)));

			//	for(unsigned i = 0; i < mChannels.size(); ++i)
			//	{
			//		//Play at an attenuated volume level if between the inner and outer radius.
			//		mChannels[i]->setVolume(attenVol);
			//	}
			//}

			for(unsigned i = 0; i < mChannels.size(); ++i)
			{
				mChannels[i]->setVolume(AUDIO->GetVolume());
			}
		}
	}

	AttenuatedSoundNode::~AttenuatedSoundNode()
	{
		for(unsigned i = 0; i < mSounds.size(); ++i)
		{
			AUDIO->StopSound(mSounds[i], &mChannels[i]);
		}

		AUDIO->mPosSoundList.erase(this);
	}
}