/**************************************************************************************************/
/*!
\file	Regions.cpp
\par	Project: Coalescence
\date	12/7/2011
\authors
Primary : Alex Poplawski

\brief
Region components for the physics engine

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/

#include "Precompiled.h"
#include "Physics.h"
#include "PhysBody.h"
#include "Core.h"
#include "Regions.h"
#include "GameLogic.h"
#include "Graphics.h"
#include "Force.h"
#include "DebugDraw.h"

namespace Scuttlemeyer
{
	/*****************************************************************************/
	/*!
	\brief
	Region constructor
	- Needs to allow for non-rectangle regions
	*/
	/*****************************************************************************/
	Region::Region() : mPower(0), mNormal(0,0) 
	{
		//rectangle
		Vec2 temp;
		temp.x = 0; temp.y = -.5;
		mPoints.push_back(temp); mCPoints.push_back(temp);
		temp.x = 0; temp.y = .5;
		mPoints.push_back(temp); mCPoints.push_back(temp);
		temp.x = 150; temp.y = .5;
		mPoints.push_back(temp); mCPoints.push_back(temp);
		temp.x = 150; temp.y = -.5;
		mPoints.push_back(temp); mCPoints.push_back(temp);
	}

	/*****************************************************************************/
	/*!
	\brief
	Unload region
	- Not really used correctly for recycling regions

	*/
	/*****************************************************************************/
	void Region::Unload()
	{
		PHYSICS->mRegionsList.remove(this);
		mTransform = NULL;
	}

	/*****************************************************************************/
	/*!
	\brief
	Initialize regions
	- Not really used correctly for recycling regions

	*/
	/*****************************************************************************/
	void Region::Initialize()
	{
		mTransform = GetOwner()->has(Transform);
		PHYSICS->mRegionsList.push_back(this);
		UpdatePoints();
	}

	/*****************************************************************************/
	/*!
	\brief
	Unload region
	- Not really used correctly for recycling regions

	\param[in,out] stream
	Reference to stream to read from
	*/
	/*****************************************************************************/
	void Region::Serialize(TextLoader &stream)
	{
		//read in rotation of object and calculate directional normal
		StreamRead(stream, mRot);
		mNormal.x = cos(mRot * cDelicious / 180);
		mNormal.y = sin(mRot * cDelicious / 180);

		//read in power and max distance of region
		StreamRead(stream, mFlags);

		//read in extents of object not really used if I remember correctly
		StreamRead(stream, mExtents);

		TextLoader stream2;

		std::string name = "..//Content//Objects//";
		char tempChar[20];
		sprintf_s(tempChar, 20, "%i", LOGIC->curLevel);
		name.append(tempChar);
		name.append("//");
		name.append(LOGIC->mCurInstName);
		name.append(".rgn.data");

		stream2.Open(name);

		if(stream2.IsGood())
		{
			StreamRead(stream2, mPower);
			StreamRead(stream2, mMaxDist);
		}
	}

	/*****************************************************************************/
	/*!
	\brief
	Debug draw region
	- Not correct, just draws a large rectangle rather than factoring the distance
	*/
	/*****************************************************************************/
	void Region::DebugDraw() 
	{
		if(GetOwner()->GetId() == CORE->GetCurObj())
		{
			Drawer::Instance.SetColor(Vec4(1,0,0,1));
		}
		else
		{
			Drawer::Instance.SetColor(Vec4(0,1,0,1));
		}
		
		Vec2 temp;
		temp.x = mPoints[0].x + mTransform->mPosition.x;
		temp.y = mPoints[0].y + mTransform->mPosition.y;
		Drawer::Instance.MoveTo(temp);
		for(unsigned int i = 1; i < mPoints.size(); ++i)
		{
			temp.x = mPoints[i].x + mTransform->mPosition.x;
			temp.y = mPoints[i].y + mTransform->mPosition.y;
			Drawer::Instance.LineTo(temp);
		}
		temp.x = mPoints[0].x + mTransform->mPosition.x;
		temp.y = mPoints[0].y + mTransform->mPosition.y;
		Drawer::Instance.LineTo(temp);
	}

	/*****************************************************************************/
	/*!
	\brief
	Region To string function for debug
	*/
	/*****************************************************************************/
	std::string Region::ToString() const
	{
		char tempStr[400];

		sprintf_s(tempStr, 400, "Physics Component: PhysBody\nExtents:");// %.2f, %.2f\nMass: %.2f", 
		//	mExtents.x, mExtents.y, mMass);
		std::string tempString(tempStr);
		return tempString;
	}

	/*****************************************************************************/
	/*!
	\brief
	Add force to object based on their distance from you
	- Needs to be more bouncy so the player doesnt reach the equalibrium as fast
	
	\param[in,out] b1
	physics body to apply the force onto
	*/
	/*****************************************************************************/
	void Region::AddForce(PhysBody *b1) const
	{
		float dist = DistanceSquared(mTransform->mPosition, ((*b1).mTransform->mPosition));
		int maxdist = mMaxDist * mMaxDist;//calc distance
		if(dist < maxdist && dist > 0)
			b1->mActiveForces.push_back(new Force((mPower /** (maxdist - dist) / maxdist)*/), ((mTransform->mRotation  * cDelicious/180) + (mRot * cDelicious/180)), cOneFrame, 0));
	}
	/*****************************************************************************/
	/*!
	\brief
	Update points based off of time for both SAT and debug purposes

	\param[in] dt
	frame time to use when calculating new coords
	*/
	/*****************************************************************************/
	void Region::UpdatePoints()
	{
		for(unsigned int i = 0; i < mPoints.size(); ++i)
		{
			Vec2 u;
			u.x = cos((mTransform->mRotation  * cDelicious/180) + mRot * cDelicious/180);
			u.y = sin((mTransform->mRotation  * cDelicious/180) + mRot * cDelicious/180);
			Vec2 start;
			start = Vec2(mCPoints[i].x, mCPoints[i].y);


			start.x *= mExtents.x;
			start.y *= mExtents.y;

			start.x *= 2;
			start.y *= 2;

			start.x *= mTransform->mScale.x;
			start.y *= mTransform->mScale.y;

			float tempx = start.x;
			start.x = start.x * u.x - start.y * u.y;
			start.y = tempx * u.y + start.y * u.x;

			//start.x += mTransform->mPosition.x;
			//start.y += mTransform->mPosition.y;

			mPoints[i].x = floor(start.x);
			mPoints[i].y = floor(start.y);
		}
	}
	/*****************************************************************************/
	/*!
	\brief
	Region deconstructor
	*/
	/*****************************************************************************/
	Region::~Region()
	{
		PHYSICS->mRegionsList.remove(this);
	}
}