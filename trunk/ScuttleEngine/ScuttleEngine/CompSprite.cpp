/******************************************************************************/
/*!
\file   CompSprite.cpp
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   9/22/2011
\brief  
Complex graphical component.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#include "Precompiled.h"
#include "CompSprite.h"
#include "VertexTypes.h"
#include "Transform.h"
#include "Graphics.h"
#include "QuadBatcher.h"
#include "DebugBars.h"
#include "GameStateMgr.h"
#include "DebugConsole.h"
#include "Core.h"

namespace Scuttlemeyer
{
	CompSprite::CompSprite()
	{
		mTransform = NULL;

		mFadeMode = 0;

		mBRedActive = false;

		mComponents.clear();
	}

	void CompSprite::Unload()
	{
		mTransform = NULL;
		GRAPHICS->mDrawableList.erase(reinterpret_cast<IDrawable*>(this));
		for(unsigned int i = 0; i < mComponents.size(); ++i)
		{
			delete mComponents[i]->mLocalTransform;
			delete mComponents[i];
		}
	}

	void CompSprite::Initialize()
	{
		mTransform = GetOwner()->has(Transform);
		GRAPHICS->mDrawableList.insert(reinterpret_cast<IDrawable*>(this));
		SetFacing(true);
		for(unsigned int i = 0; i < mComponents.size(); ++i)
		{
			mComponents[i]->mTexture = GRAPHICS->GetTexture(mComponents[i]->mName);
			mComponents[i]->mTransform = mTransform;
			mComponents[i]->SetFadeMode(0, 0.f);
			mComponents[i]->SetFacing(true);
		}
	}

	void CompSprite::Serialize(TextLoader &stream)
	{
		std::string name;
		do
		{
			StreamRead(stream, name);
			if(name != "endlist")
			{
				CompSpriteComp* tempComp = new CompSpriteComp();
				tempComp->mName = name;
				StreamRead(stream, tempComp->mSize);
				StreamRead(stream, tempComp->mColor);
				Transform* tempTrans = new Transform();
				StreamRead(stream, tempTrans->mPosition);
				StreamRead(stream, tempTrans->mScale);
				StreamRead(stream, tempTrans->mRotation);
				tempComp->mLocalTransform = tempTrans;

				TextLoader stream2;
				std::string name2 = name + ".data";

				stream2.Open(name2);

				if(stream2.IsGood())
				{
					StreamRead(stream2, tempComp->mNumFrames);
					StreamRead(stream2, tempComp->mNumAnims);
					StreamRead(stream2, tempComp->mTimerMax);
				}
				else
				{
					tempComp->mNumFrames = 1.f;
					tempComp->mNumAnims = 1.f;
					tempComp->mTimerMax = 1;
				}

				tempComp->mCurFrame = 0.f;
				tempComp->mCurAnim = 0.f;
				tempComp->mTimer = 1;
				tempComp->mAnimType = PLAYLOOPED;
				tempComp->mAnimDir = FORWARD;
				tempComp->mAnimActive = true;

				mComponents.push_back(tempComp);
			}
		}while(name != "endlist");

		mName = mComponents[0]->mName;
		StreamRead(stream, mDepth);
	}

	void CompSprite::Draw(IDirect3DDevice9* pDevice, ID3DXEffect* shader)
	{
		Mat4 matObject, matObjTrans, matObjRot;

		int facingScaler = mBFacingRight == true ? 1 : -1;

		D3DXMatrixScaling(&matObject, mTransform->mScale.x * facingScaler, mTransform->mScale.y, 1.0f);

		D3DXMatrixRotationZ(&matObjRot, ((mTransform->mRotation * cDelicious) / 180));
		D3DXMatrixMultiply(&matObject, &matObject, &matObjRot);

		D3DXMatrixTranslation(&matObjTrans, floor(mTransform->mPosition.x), floor(mTransform->mPosition.y), 0.0f);
		D3DXMatrixMultiply(&matObject, &matObject, &matObjTrans);

		Mat4 worldViewProj = matObject * GRAPHICS->mViewProjMatrix;

		if(mFadeMode)
		{
			for(unsigned i = 0; i <  mComponents.size(); ++i)
			{
				mComponents[i]->SetFadeMode(mFadeMode, mFadeRate);
			}
		}

		for(unsigned int i = 0; i < mComponents.size(); ++i)
		{
			if(mComponents[i]->GetFadeMode())
			{
				mComponents[i]->mColor.w += (mComponents[i]->GetFadeMode() * mComponents[i]->GetFadeRate());

				if(mComponents[i]->mColor.w >= 1.0f)
				{
					mComponents[i]->mColor.w = 1.0f;
					mComponents[i]->SetFadeMode(0, 0.0f);
				}
				else if(mComponents[i]->mColor.w <= 0.0f)
				{
					mComponents[i]->mColor.w = 0.0f;
					mComponents[i]->SetFadeMode(0, 0.0f);
				}
			}

			if(mComponents[i]->mColor.w > 0.0f)
			{
				if(mBRedActive)
				{
					mComponents[i]->mColor.y -= 0.03f;
					mComponents[i]->mColor.z -= 0.03f;
				}

				//Transform the sprite.
				Mat4 matSprite, matTranslate, matRotate;

				//First, scale it to the proper width and height.
				D3DXMatrixScaling(&matSprite, (mComponents[i]->mSize.x * mComponents[i]->mLocalTransform->mScale.x), 
					(mComponents[i]->mSize.y * mComponents[i]->mLocalTransform->mScale.y), 1.0f);
				//Then, rotate it to the proper angle.
				D3DXMatrixRotationZ(&matRotate, ((mComponents[i]->mLocalTransform->mRotation * cDelicious) / 180));
				D3DXMatrixMultiply(&matSprite, &matSprite, &matRotate);

				//Finally, move it to the proper location (note the floor functions on the positions).
				D3DXMatrixTranslation(&matTranslate, floor(mComponents[i]->mLocalTransform->mPosition.x), 
					floor(mComponents[i]->mLocalTransform->mPosition.y), 0.0f);
				D3DXMatrixMultiply(&matSprite, &matSprite, &matTranslate);

				if(DBGCNSL->DBConsole == 0 && gGameStateCurr != GS_PAUSE && !CORE->mBEditorActive)
				{
					if(mComponents[i]->mAnimActive)
					{
						++mComponents[i]->mTimer;

						if(mComponents[i]->mTimer >= mComponents[i]->mTimerMax)
						{
							if(mComponents[i]->mAnimDir == FORWARD)
							{
								mComponents[i]->mCurFrame = mComponents[i]->mCurFrame + 1 >= mComponents[i]->mNumFrames ? 0.0f : mComponents[i]->mCurFrame + 1;
							}
							else
							{
								mComponents[i]->mCurFrame = mComponents[i]->mCurFrame - 1 >= 0.0f ? mComponents[i]->mCurFrame - 1 : mComponents[i]->mNumFrames - 1;
							}

							mComponents[i]->mTimer = 1;

							if((mComponents[i]->mAnimDir == FORWARD && mComponents[i]->mCurFrame == mComponents[i]->mNumFrames - 1) || 
								(mComponents[i]->mAnimDir == REVERSE && mComponents[i]->mCurFrame == 0.f))
							{
								mComponents[i]->mAnimActive = mComponents[i]->mAnimType == PLAYLOOPED ? true : false;
							}
						}
					}
				}

				GRAPHICS->mBatcher->Draw(mComponents[i]->mTexture, shader, mDepth, 6, &matSprite, mComponents[i]->mCurFrame, mComponents[i]->mNumFrames, mComponents[i]->mCurAnim, mComponents[i]->mNumAnims, &worldViewProj, mComponents[i]->mColor);
			}
		}
	}

	std::string CompSprite::ToString()
	{
		std::string tempString = "Graphics Component: Composite Sprite\n";

		for(unsigned int i = 0; i < mComponents.size(); ++i)
		{
			char tempStr[400];

			sprintf_s(tempStr, 400, "Composite Sprite Component\nSize: %.2f, %.2f\nColor: %.1f, %.1f, %.1f, %.1f\nLocal Position: %.2f, %.2f\nLocal Scale: %.2f, %.2f\nLocal Rotation: %.2f", 
				mComponents[i]->mSize.x, mComponents[i]->mSize.y, 
				mComponents[i]->mColor.x, mComponents[i]->mColor.y, mComponents[i]->mColor.z, mComponents[i]->mColor.w, 
				mComponents[i]->mLocalTransform->mPosition.x, mComponents[i]->mLocalTransform->mPosition.y, 
				mComponents[i]->mLocalTransform->mScale.x, mComponents[i]->mLocalTransform->mScale.y, 
				mComponents[i]->mLocalTransform->mRotation);
			std::string tempString2(tempStr);
			tempString.append(tempString2);
			if(i < mComponents.size() - 1)
			{
				tempString.append("\n");
			}
		}

		return tempString;
	}

	void CompSprite::ToBar()
	{
		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");

		for(unsigned int i = 0; i < mComponents.size(); ++i)
		{
			char groupName[25];
			sprintf_s(groupName, 25, " group='Component%i' ", i);

			char sizeName[10];
			sprintf_s(sizeName, 10, "Size%i", i);
			char sizeGroupName[50];
			sprintf_s(sizeGroupName, 50, "%slabel='Size' ", &groupName);
			TwAddVarRO(bar, sizeName, DEBUGBARS->vec2Type, &mComponents[i]->mSize, sizeGroupName);

			char colorName[10];
			sprintf_s(colorName, 10, "Color%i", i);
			char colorGroupName[50];
			sprintf_s(colorGroupName, 50, "%slabel='Color' ", &groupName);
			TwAddVarRO(bar, colorName, DEBUGBARS->vec4Type, &mComponents[i]->mColor, colorGroupName);

			char subGroup[255];
			sprintf_s(subGroup, 255, " ObjProp/Component%i group='CompSprite' ", i);
			TwDefine(subGroup);
		}

		TwDefine(" ObjProp/CompSprite opened=false ");
	}

	CompSprite::~CompSprite()
	{
		GRAPHICS->mDrawableList.erase(reinterpret_cast<IDrawable*>(this));
		for(unsigned int i = 0; i < mComponents.size(); ++i)
		{
			delete mComponents[i]->mLocalTransform;
			delete mComponents[i];
		}
	}
}