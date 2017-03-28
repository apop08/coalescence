/******************************************************************************/
/*!
\file   Sprite.cpp
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   8/20/2011
\brief  
Basic graphical component.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#include "Precompiled.h"
#include "Sprite.h"
#include "VertexTypes.h"
#include "Transform.h"
#include "Graphics.h"
#include "QuadBatcher.h"
#include "DebugBars.h"
#include "GameStateMgr.h"

#include "Core.h"

#include "DebugConsole.h"

namespace Scuttlemeyer
{
	Sprite::Sprite()
	{
		mTransform = NULL;
		mTexture = NULL;
		mColor = Vec4(0, 0, 0, 1);

		mFadeMode = 0;

		mNumFrames = 1.f;
		mCurFrame = 0.f;
		mNumAnims = 1.f;
		mCurAnim = 0.f;
		mTimer = 1;
		mTimerMax = 1;

		mAnimType = PLAYLOOPED;
		mAnimDir = FORWARD;
		mAnimActive = true;

		mBFacingRight = true;

		mBRedActive = false;
	}

	void Sprite::Unload()
	{
		mTransform = NULL;
		mTexture = NULL;
		GRAPHICS->mDrawableList.erase(reinterpret_cast<IDrawable*>(this));
	}

	void Sprite::Initialize()
	{
		mTransform = GetOwner()->has(Transform);
		GRAPHICS->mDrawableList.insert(reinterpret_cast<IDrawable*>(this));
		mTexture = GRAPHICS->GetTexture(mName);
	}

	void Sprite::Serialize(TextLoader &stream)
	{
		StreamRead(stream, mName);
		StreamRead(stream, mSize);
		StreamRead(stream, mColor);
		StreamRead(stream, mDepth);

		TextLoader stream2;
		std::string name2 = mName + ".data";

		stream2.Open(name2);

		if(stream2.IsGood())
		{
			StreamRead(stream2, mNumFrames);
			StreamRead(stream2, mNumAnims);
			StreamRead(stream2, mTimerMax);
		}
	}

	void Sprite::Draw(IDirect3DDevice9* pDevice, ID3DXEffect* shader)
	{
		if(mFadeMode)
		{
			mColor.w += (mFadeMode * mFadeRate);

			if(mColor.w >= 1.0f)
			{
				mColor.w = 1.0f;
				mFadeMode = 0;
			}
			else if(mColor.w <= 0.0f)
			{
				mColor.w = 0.0f;
				mFadeMode = 0;
			}
		}

		if(mBRedActive)
		{
			mColor.y -= 0.03f;
			mColor.z -= 0.03f;
		}

		if(mColor.w > 0.0f)
		{
			//Transform the sprite.
			Mat4 matSprite, matTranslate, matRotate;

			int facingScaler = mBFacingRight == true ? 1 : -1;

			//First, scale it to the proper width and height.
			D3DXMatrixScaling(&matSprite, (mSize.x * mTransform->mScale.x * facingScaler), (mSize.y * mTransform->mScale.y), 1.0f);

			//Then, rotate it to the proper angle.
			D3DXMatrixRotationZ(&matRotate, ((mTransform->mRotation * cDelicious) / 180));
			D3DXMatrixMultiply(&matSprite, &matSprite, &matRotate);

			//Finally, move it to the proper location (note the floor functions on the positions).
			D3DXMatrixTranslation(&matTranslate, floor(mTransform->mPosition.x), floor(mTransform->mPosition.y), 0.0f);
			D3DXMatrixMultiply(&matSprite, &matSprite, &matTranslate);

			if(DBGCNSL->DBConsole == 0 && gGameStateCurr != GS_PAUSE && !CORE->mBEditorActive)
			{
				if(mAnimActive)
				{
					++mTimer;

					if(mTimer >= mTimerMax)
					{
						if(mAnimDir == FORWARD)
						{
							mCurFrame = mCurFrame + 1 >= mNumFrames ? 0.0f : mCurFrame + 1;
						}
						else
						{
							mCurFrame = mCurFrame - 1 >= 0.0f ? mCurFrame - 1 : mNumFrames - 1;
						}

						mTimer = 1;

						if((mAnimDir == FORWARD && mCurFrame == mNumFrames - 1) || (mAnimDir == REVERSE && mCurFrame == 0.f))
						{
							mAnimActive = mAnimType == PLAYLOOPED ? true : false;
						}
					}
				}
			}

			GRAPHICS->mBatcher->Draw(mTexture, shader, mDepth, 6, &matSprite, mCurFrame, mNumFrames, mCurAnim, mNumAnims, &GRAPHICS->mViewProjMatrix, mColor);
		}
	}

	void Sprite::SetRandomAlpha(float low, float high)
	{
		mFadeMode = 0;
		float random = ((float)rand()) / (float)RAND_MAX;
		float diff = high - low;
		float r = random * diff;
		mColor.w = low + r;
	}

	std::string Sprite::ToString()
	{
		char tempStr[400];

		sprintf_s(tempStr, 400, "Graphics Component: Sprite\nSize: %.2f, %.2f\nColor: %.1f, %.1f, %.1f, %.1f", 
			mSize.x, mSize.y, mColor.x, mColor.y, mColor.z, mColor.w);
		std::string tempString(tempStr);
		return tempString;
	}

	void Sprite::ToBar()
	{
		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");

		TwAddVarRO(bar, "Size", DEBUGBARS->vec2Type, &mSize, " group='Sprite' label='Size' help='Size of the sprite.' ");
		TwAddVarRO(bar, "Color", DEBUGBARS->vec4Type, &mColor, " group='Sprite' label='Color' help='Color of the sprite.' ");

		TwDefine(" ObjProp/Sprite opened=false ");
	}

	Sprite::~Sprite()
	{
		GRAPHICS->mDrawableList.erase(reinterpret_cast<IDrawable*>(this));
	}
}