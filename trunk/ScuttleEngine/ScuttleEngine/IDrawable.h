/******************************************************************************/
/*!
\file   IDrawable.h
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   9/20/2011
\brief  
Interface for drawable components.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#pragma once

#include "Component.h"

namespace Scuttlemeyer
{
	class IDrawable : public GameComponent
	{
	public:
		virtual void Unload() {};
		virtual void Initialize() {};
		virtual Mid::MessageIdType SendMessage(Message *) { return Mid::None; }
		virtual void Serialize(TextLoader& stream) {};
		virtual void Draw(IDirect3DDevice9* pDevice, ID3DXEffect* shader) {};
		int GetFadeMode() { return mFadeMode; }
		float GetFadeRate() { return mFadeRate; }
		void SetFadeMode(int fadeMode, float fadeRate) {
			mFadeMode = fadeMode;
			mFadeRate = fadeRate;
		}
		void SetAnimation(int type, int direction = FORWARD) {
			mAnimType = type;
			mAnimDir = direction;
		}
		void GoToFrame(float frame, float anim = 0.f) {
			mCurFrame = frame;
			mCurAnim = anim;
		}
		void GoToFrameLevel(float level) { mCurAnim = level; }
		bool GetFacing() { return mBFacingRight; }
		void SetFacing(bool facing) { mBFacingRight = facing; }
		void SetRed(bool red) { mBRedActive = red; }
	public:
		IDrawable* Next;
		IDrawable* Prev;
		std::string mName;
		float mNumFrames;
		float mCurFrame;
		float mNumAnims;
		float mCurAnim;
		int mTimer;
		int mTimerMax;
		int mAnimType;
		int mAnimDir;
		bool mAnimActive;
		enum eAnimModes {PLAYLOOPED, PLAYONCE, STOP, FORWARD, REVERSE};
	protected:
		virtual ~IDrawable() {};
	protected:
		int mDepth;
		int mFadeMode;
		float mFadeRate;
		bool mBFacingRight;
		bool mBRedActive;
	public:
		bool operator<(const IDrawable& rhs) {
			if(mDepth > rhs.mDepth)
			{
				return true;
			}
			else if(mDepth == rhs.mDepth)
			{
				if(mName < rhs.mName)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
	};
}