/******************************************************************************/
/*!
\file   LevelPanCam.cpp
\author Matt Sutherlin
\par    email: matt.sutherlin\@digipen.edu
\par    DigiPen login: matt.sutherlin
\par    Course: GAM200
\date   12/15/2011
\brief  
	Logic control unit for scripted camera.
\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/******************************************************************************/

#include "Precompiled.h"
#include "LevelPanCam.h"

#include "GameLogic.h"
#include "Graphics.h"

#include "DebugBars.h"

namespace Scuttlemeyer
{
	LevelPanCam::LevelPanCam()
	{
		mName = "LevelPanCam";
		mCurPoint = 0;
	}

	void LevelPanCam::Initialize()
	{
		LOGIC->mLogicList.push_back(reinterpret_cast<ILogical*>(this));
	}

	void LevelPanCam::Serialize(TextLoader &stream)
	{
		TextLoader stream2;
		std::string name = "..//Content//Levels//Level";
		char tempChar[20];
		sprintf_s(tempChar, 20, "%i", LOGIC->curLevel);
		name.append(tempChar);
		name.append(".txt.data");

		stream2.Open(name);

		if(stream2.IsGood())
		{
			StreamRead(stream2, mNumPoints);

			for(unsigned i = 0; i < mNumPoints; ++i)
			{
				Vec2 tempPoint;
				StreamRead(stream2, tempPoint);
				mPoints.push_back(tempPoint);
			}
		}
	}

	void LevelPanCam::Update(float dt)
	{
		Transform* pTransform = GetOwner()->has(Transform);

		float incDirX;
		
		if(pTransform->mPosition.x > mPoints[mCurPoint].x)
		{
			incDirX = -5.f;
		}
		else if(pTransform->mPosition.x < mPoints[mCurPoint].x)
		{
			incDirX = 5.f;
		}
		else
		{
			incDirX = 0.f;
		}

		float incDirY;

		if(pTransform->mPosition.y > mPoints[mCurPoint].y)
		{
			incDirY = -5.f;
		}
		else if(pTransform->mPosition.y < mPoints[mCurPoint].y)
		{
			incDirY = 5.f;
		}
		else
		{
			incDirY = 0.f;
		}

		pTransform->mPosition.x += incDirX;
		pTransform->mPosition.y += incDirY;

		if(pTransform->mPosition.x == mPoints[mCurPoint].x && pTransform->mPosition.y == mPoints[mCurPoint].y)
		{
			++mCurPoint;

			if(mCurPoint == mPoints.size())
			{
				GetOwner()->Destroy();
			}
		}
	}

	Mid::MessageIdType LevelPanCam::SendMessage(Message* message)
	{
		return Mid::None;
	}

	std::string LevelPanCam::ToString()
	{
		return "Logic Component: LevelPanCam";
	}

	void LevelPanCam::ToBar()
	{
		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");

		TwAddVarRO(bar, "Logic Component", TW_TYPE_STDSTRING, &mName, " help='Type of logical component.' ");
	}

	LevelPanCam::~LevelPanCam()
	{
		Camera* temp = LOGIC->mPlayer->has(Camera);
		GRAPHICS->mCurrentCamera = temp;

		LOGIC->mLogicList.erase(reinterpret_cast<ILogical*>(this));
	}
}