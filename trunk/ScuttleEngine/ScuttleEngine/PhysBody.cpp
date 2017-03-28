/**************************************************************************************************/
/*!
\file	PhysBody.cpp
\par	Project: Coalescence
\date	12/7/2011
\authors
Primary : Alex Poplawski

\brief
Force class for force based movement inside physics engine
Rigid body physics component for objects that require any physics inside the game

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/

#include "Precompiled.h"
#include "PhysBody.h"
#include "Core.h"
#include "Regions.h"
#include "Physics.h"
#include "Force.h"

#include "GameLogic.h"
#include "Graphics.h"

#include "DebugDraw.h"
#include "DebugBars.h"
#include "PhysVars.h"
namespace Scuttlemeyer
{
	//drag in physics flags
	using namespace PhysFlags;

	/*****************************************************************************/
	/*!
	\brief
	Force constructor for moving most objects through non velocity based movement
	- Still need to factor in distance from center of mass however this
	will be done through the rewrite of the physics engine

	\param[in] magnitude
	Strength of the force

	\param[in] direction
	Direction of force to be applied to on object

	\param[in] activeTime
	Length of time force is to be applied
	- Defaults to one frame

	\param[in] flags
	Special flags for specific types of forces
	- Only used for gravity at this time
	- Defaults to none
	*/
	/*****************************************************************************/
	Force::Force(const float magnitude, const float direction,const float activeTime,const DWORD flags) 
		: mMagnitude(magnitude), mDirection(direction),
		mActiveTime(activeTime), flag(flags){}//, void* owner)

	/*****************************************************************************/
	/*!
	\brief
	Update function for force objects.

	\param[in] dt
	Frame time to use for updating the force

	\param[in] velocity
	Velocity of the object that the force is applied to

	\param[in] mass
	Mass of the object to factor how to force affects the object

	\return
	TRUE - The force has expired and should be deleted
	FALSE - The force is still active
	*/
	/*****************************************************************************/
	int Force::Update(const float dt, Velocity* velocity,const float mass) 
	{
		//force isnt active for the whole frame
		if(mActiveTime <= dt && mActiveTime != cTInfinity)
		{
			//deal with some rounding problems
			float roundCheck = (mMagnitude * cos(mDirection) * mActiveTime * 1000) / mass;
			if(roundCheck < .01 && roundCheck > -.01)
			{
				roundCheck = 0;
			}
			velocity->x += roundCheck;

			roundCheck = (mMagnitude * sin(mDirection) * mActiveTime * 1000) / mass;
			if(roundCheck < .01 && roundCheck > -.01)
			{
				roundCheck = 0;
			}
			velocity->y += roundCheck; 

			return TRUE;
		}
		//force continues on after frame
		else
		{

			float roundCheck = (mMagnitude * cos(mDirection) * dt * 1000) / mass;
			if(flag == 1)
			{
				roundCheck *= mass;
				//roundCheck *= dt / cOneFrame;
			}
			if(roundCheck < .01 && roundCheck > -.01)
			{
				roundCheck = 0;
			}
			velocity->x += roundCheck;

			roundCheck = (mMagnitude * sin(mDirection) * dt * 1000) / mass;
			if(flag == 1)
			{
				roundCheck *= mass;
				//roundCheck *= dt / cOneFrame;
			}
			if(roundCheck < .01 && roundCheck > -.01)
			{
				roundCheck = 0;
			}
			velocity->y += roundCheck;
			//decrement time
			if(mActiveTime != cTInfinity)
				mActiveTime -= dt;
			return FALSE;
		}
	}

	/*****************************************************************************/
	/*!
	\brief
	Deconstructor for forces
	*/
	/*****************************************************************************/
	Force::~Force()
	{
		//debug force counter
		PHYSICS->mForces++;
	}

	/*****************************************************************************/
	/*!
	\brief
	Rotational force constructor
	- Will merge with normal forces in next iteration on physics engine so limit use

	\param[in] magnitude
	Strength of the force

	\param[in] direction
	Direction of force to be applied to on object

	\param[in] activeTime
	Length of time force is to be applied
	- Defaults to one frame

	\param[in] flags
	Special flags for specific types of forces
	- Only used for gravity at this time
	- Defaults to none
	*/
	/*****************************************************************************/
	RotForce::RotForce(const float magnitude,const float direction,const float activeTime,const DWORD flags)
		: mMagnitude(magnitude), mDirection(direction),
		mActiveTime(activeTime), flag(flags){}//, void* owner)

	/*****************************************************************************/
	/*!
	\brief
	Update function for rotational force objects.

	\param[in] dt
	Frame time to use for updating the force

	\param[in,out] rotVel
	Rotational Velocity of the object that the force is applied to

	\return
	TRUE - The force has expired and should be deleted
	FALSE - The force is still active
	*/
	/*****************************************************************************/
	int RotForce::Update(const float dt, float* rotVel)
	{
		if(mActiveTime <= dt && mActiveTime != cTInfinity)
		{
			//deal with some rounding problems
			float roundCheck = (cDelicious * mMagnitude / 180) * mDirection * mActiveTime * 1000;

			*rotVel += roundCheck; 
			return TRUE;
		}
		else
		{
			float roundCheck = (cDelicious * mMagnitude / 180) * mDirection * dt * 1000;
			*rotVel += roundCheck; 
			if(mActiveTime != cTInfinity)
				mActiveTime -= dt;
			return FALSE;
		}
	}

	/*****************************************************************************/
	/*!
	\brief
	Deconstructor for forces
	*/
	/*****************************************************************************/
	RotForce::~RotForce()
	{
		PHYSICS->mRotForces++;
	}


	/*****************************************************************************/
	/*!
	\brief
	Constructor for physics body components just initializes data members does
	not serialize.
	*/
	/*****************************************************************************/
	PhysBody::PhysBody() : mVelocity(0,0), mMass(1.0f), mPhysFlags(0), mColFlags(0), mRotVel(0)
	{
		mActiveForces.clear();
		mResponseForces.clear();
	}

	/*****************************************************************************/
	/*!
	\brief
	Unload function for physics bodies
	*/
	/*****************************************************************************/
	void PhysBody::Unload()
	{
		//unhook from physics body list
		//should not do this when recycling
		PHYSICS->mPhysBodyList.remove(this);
		//set the transform pointer to null
		mTransform = NULL;
		//delete all active forces on the object
		//needs to put the forces to inactive to allow for recycling
		if(!mActiveForces.empty())
		{
			Force* deleteme = *(mActiveForces.begin());
			mActiveForces.clear();
			delete deleteme;
		}
	}

	/*****************************************************************************/
	/*!
	\brief
	Initialize function for physics bodies
	*/
	/*****************************************************************************/
	void PhysBody::Initialize()
	{
		//Set up transform matrix
		mTransform = GetOwner()->has(Transform);
		//Add to the active physics body list

		PHYSICS->mPhysBodyList.push_back(this);

		GOC * parent = GetOwner();
		if(parent->has(Block))
		{
			std::list<PhysBody *>::const_iterator it = PHYSICS->mCollidableList.begin();
			std::list<PhysBody *>::const_iterator it2 = it;
			if(it != PHYSICS->mCollidableList.end())
			{
				++it;
			}
			if(it != PHYSICS->mCollidableList.end())
			{
				it2 = it;
				++it;
			}
			if(it != PHYSICS->mCollidableList.end())
			{
				it2 = it;
			}
			PHYSICS->mCollidableList.insert(it2, this);
		}
		else
		{
			PHYSICS->mCollidableList.push_back(this);
		}
		if(mPhysFlags & FStaticObj)
			PHYSICS->mStaticList.push_back(this);
		else
			PHYSICS->mNonStaticList.push_back(this);

		//Add gravity to the object if it is required
		if(mPhysFlags & FGravity)
		{
			AddForce(1.0f * mMass, cDirDown, 1, cTInfinity);
		}
		mStack = 0;
	}

	/*****************************************************************************/
	/*!
	\brief
	Serialization for physics body components
	- Needs to be used in recycling of objects

	\param[in,out] stream
	Reference to the stream to read from
	*/
	/*****************************************************************************/
	void PhysBody::Serialize(TextLoader &stream)
	{
		//extents of object, size of the object used in bounding box collision occlution
		StreamRead(stream, mExtents);//2D vector

		//Physics flags that the object has
		StreamRead(stream, mPhysFlags);//DWORD

		int points;
		//number of points to be read in
		StreamRead(stream, points);//int
		//Rigid body point definitions used in SAT collision detection/resolution
		for(int i = 0; i < points; ++i)
		{
			Vec2 temp;
			StreamRead(stream, temp);//2D vector
			mPoints.push_back(temp);
			mCPoints.push_back(temp);
		}
	}

	/*****************************************************************************/
	/*!
	\brief
	Update all forces that the object
	- Needs to not worry about the rotational forces

	\param[in] dt
	Frame time to factor in
	*/
	/*****************************************************************************/
	void PhysBody::UpdateForces(const float dt)
	{

		//check list
		if(!mActiveForces.empty())
		{
			//walk through all active forces of this object
			for(std::list<Force*>::iterator it = mActiveForces.begin(); (it != mActiveForces.end()) && !mActiveForces.empty() && (*it) != NULL;)
			{
				//update force and check if needs to be deleted
				int destroy = (*it)->Update(dt, &mVelocity, mMass);
				if(destroy == TRUE)
				{
					//delete the force
					std::list<Force*>::iterator it2;
					it2 = it;
					++it;
					delete (*it2);
					mActiveForces.remove(*it2);
				}
				else
					++it;
			}
		}
		//do for all rotational forces
		if(!mActiveRotForces.empty())
		{
			for(std::list<RotForce*>::iterator it = mActiveRotForces.begin(); (it != mActiveRotForces.end()) && !mActiveRotForces.empty() && (*it) != NULL;)
			{
				int destroy = (*it)->Update(dt, &mRotVel);
				if(destroy == TRUE)
				{
					std::list<RotForce*>::iterator it2;
					it2 = it;
					++it;
					delete (*it2);
					mActiveRotForces.remove(*it2);
				}
				else
					++it;
			}
		}
	}

	/*****************************************************************************/
	/*!
	\brief
	resolves collisions for an object

	\param[in] dt
	Frame time to factor in

	\param[in, out] numCol
	number of collisions
	*/
	/*****************************************************************************/
	void PhysBody::Resolve(const float dt, int& numCol)
	{
		//check list
		if(!mResponseForces.empty())
		{
			//walk through all active forces of this object
			for(std::list<Force*>::iterator it = mResponseForces.begin(); (it != mResponseForces.end()) && !mResponseForces.empty() && (*it) != NULL;)
			{
				if((*it)->mMagnitude > -1000 && (*it)->mMagnitude < 1000)
				{
					//update force and check if needs to be deleted
					(*it)->Update(dt, &mVelocity, mMass);
					//delete the force
					std::list<Force*>::iterator it2;
					it2 = it;
					++it;
					delete (*it2);
					mResponseForces.remove(*it2);
					++numCol;
				}
				else
				{
					std::list<Force*>::iterator it2;
					it2 = it;
					++it;
					delete (*it2);
					mResponseForces.remove(*it2);
				}
			}
		}
	}

	/*****************************************************************************/
	/*!
	\brief
	To string function for Physics body debug printing

	\return
	String used for writing debug info
	*/
	/*****************************************************************************/
	std::string PhysBody::ToString() const
	{
		char tempStr[400];

		sprintf_s(tempStr, 400, "Physics Component: PhysBody\nExtents: %.2f, %.2f\nMass: %.2f", 
			mExtents.x, mExtents.y, mMass);
		std::string tempString(tempStr);
		return tempString;
	}

	/*****************************************************************************/
	/*!
	\brief
	To bar function for Physics body debug printing with antweakbar
	*/
	/*****************************************************************************/
	void PhysBody::ToBar() const
	{
		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");

		TwAddVarRO(bar, "Extents", DEBUGBARS->vec2Type, &mExtents, " group='PhysBody' label='Extents' help='Extents of the physics body.' ");
		TwAddVarRO(bar, "Mass", TW_TYPE_FLOAT, &mMass, " group='PhysBody' label='Mass' help='Mass of the physics body.' ");

		TwDefine(" ObjProp/PhysBody opened=false ");
	}

	/*****************************************************************************/
	/*!
	\brief
	Physics body update function called every frame

	\param[in] dt
	Frame time to use with calculations
	*/
	/*****************************************************************************/
	void PhysBody::Update(const float dt)
	{

		//check flags for setting the positions
		if(!(mPhysFlags & HUD))
		{
			if(mPhysFlags & GAS)
			{
				if(mVelocity.y < 0)
				{
					mVelocity.y = 0;
				}
			}
			//liquid does not move
			if(mPhysFlags & FStaticObj) //|| GetOwner()->state == Liquid
			{
				mVelocity.x = 0;
				mVelocity.y = 0;
			}
			if(mVelocity.x > -.05f && mVelocity.x < .05f)
			{
				mVelocity.x = 0;
			}
			if(mVelocity.y > -.05f && mVelocity.y < .05f)
			{
				mVelocity.y = 0;
			}

			//move objects by velocity
			mTransform->mPosition.x += mVelocity.x * dt;
			mTransform->mPosition.y += mVelocity.y * dt;
			mTransform->mRotation += mRotVel * dt;



			//calculate drag
			float drag = 1 - .05f * dt / cOneFrame;
			if(!(mPhysFlags & FUN))
			{
				//apply to all non bullet moving objects
				if(!(mPhysFlags & BUL))
				{
					mVelocity.x *= drag;
					if(mVelocity.x > -.1 && mVelocity.x < .1)
					{
						mVelocity.x = 0;
					}
					mVelocity.y *= drag;
					if(mVelocity.y > -.1 && mVelocity.y < .1)
					{
						mVelocity.y = 0;
					}
					mRotVel *= drag;
				}
			}
		}
		else
		{
			//move player up by 150 pixels
			mTransform->mPosition = ((Transform*)LOGIC->mPlayer->GetComponent(CT_Transform))->mPosition;
			mTransform->mPosition.y += 150;

		}
		mStack = 0;
	}

	/*****************************************************************************/
	/*!
	\brief
	Debug draw for physics collision lines
	*/
	/*****************************************************************************/
	void PhysBody::DebugDraw()
	{
		//set the color, differs if the object is selected for debug drawing
		if(GetOwner()->GetId() == CORE->GetCurObj())
		{
			Drawer::Instance.SetColor(Vec4(1,0,0,1));
		}
		else
		{
			Drawer::Instance.SetColor(Vec4(1,1,1,1));
		}
		//get the first points
		Drawer::Instance.MoveTo(mPoints[0]);
		for(unsigned int i = 1; i < mPoints.size(); ++i)
		{
			//move to the next point
			Drawer::Instance.LineTo(mPoints[i]);
		}
		//loop back to first point
		Drawer::Instance.LineTo(mPoints[0]);

	}

	/*****************************************************************************/
	/*!
	\brief
	Add force to this object based on params for the force

	\param[in] magnitude
	Magnitude of new force

	\param[in] direction
	Direction that force is applied in

	\param[in] flags
	Flags to provide force
	- Defaults to 0

	\param[in] activeTime
	Time that force exists
	- Defaults to one frame
	*/
	/*****************************************************************************/
	void PhysBody::AddForce(const float magnitude,const float direction,const DWORD flags,const float activeTime)
	{
		mActiveForces.push_back(new Force(magnitude, direction, activeTime, flags));
	}

	/*****************************************************************************/
	/*!
	\brief
	Add response forces to an object

	\param[in] magnitude
	Magnitude of new force

	\param[in] direction
	Direction that force is applied in

	\param[in] flags
	Flags to provide force
	- Defaults to 0

	\param[in] activeTime
	Time that force exists
	- Defaults to one frame
	*/
	/*****************************************************************************/
	void PhysBody::AddResponse(const float magnitude,const float direction,const DWORD flags,const float activeTime)
	{
		mResponseForces.push_back(new Force(magnitude, direction, activeTime, flags));
	}

	/*****************************************************************************/
	/*!
	\brief
	Copies a force onto a physics body
	

	\param[in] a
	Force to copy
	*/
	/*****************************************************************************/
	void PhysBody::AddForce(Force* a)
	{
		mActiveForces.push_back(a);
	}

	/*****************************************************************************/
	/*!
	\brief
	Add rotational force to this object based on params for the force

	\param[in] magnitude
	Magnitude of new force

	\param[in] direction
	Direction that force is applied in

	\param[in] flags
	Flags to provide force
	- Defaults to 0

	\param[in] activeTime
	Time that force exists
	- Defaults to one frame
	*/
	/*****************************************************************************/
	void PhysBody::AddRotForce(const float magnitude,const float direction,const DWORD flags,const float activeTime)
	{
		mActiveRotForces.push_back(new RotForce(magnitude, direction, activeTime, flags));
	}

	/*****************************************************************************/
	/*!
	\brief
	Returns points to the position they were at the start of the frame for debug drawing
	- Might be faster to just copy over points at the start, NEEDS testing

	\param[in] dt
	Frame time to factor into calculations

	\param[in] numCol
	number of collisions on the object
	*/
	/*****************************************************************************/
	void PhysBody::ReversePoints(float dt, int numCol)
	{

		//modify each point
		for(unsigned int i = 0; i < mPoints.size(); ++i)
		{
			if(numCol)
			{
				//Vec2 u;
				//u.x = cos(mTransform->mRotation * (cDelicious / 180) + mRotVel * dt);
				//u.y = sin(mTransform->mRotation * (cDelicious / 180) + mRotVel * dt);
				Vec2 start;
				start = Vec2(mCPoints[i].x, mCPoints[i].y);


				start.x *= mExtents.x;
				start.y *= mExtents.y;

				start.x *= 2;
				start.y *= 2;

				start.x *= mTransform->mScale.x;
				start.y *= mTransform->mScale.y;

				//store x for use in y calculations
				//float tempx = start.x;
				//start.x = start.x * u.x - start.y * u.y;
				//start.y = tempx * u.y + start.y * u.x;

				start.x += mVelocity.x * dt + mTransform->mPosition.x;
				start.y += mVelocity.y * dt + mTransform->mPosition.y;

				mPoints[i].x = floor(start.x);
				mPoints[i].y = floor(start.y);
			}
			else
			{
				mPoints[i].x += mTransform->mPosition.x;
				mPoints[i].y += mTransform->mPosition.y;

				mPoints[i].x = floor(mPoints[i].x);
				mPoints[i].y = floor(mPoints[i].y);
			}
		}
	}

	/*****************************************************************************/
	/*!
	\brief
	Update points to the position they will be at end of the frame for debug drawing

	\param[in] dt
	Frame time to factor into calculations
	*/
	/*****************************************************************************/
	void PhysBody::UpdatePoints(const float dt)
	{
		if(GetOwner()->state == Gas)
		{
			//move gasses up
			AddForce(1.25f * mMass, cDirUp, 0 , dt);
		}

		for(unsigned int i = 0; i < mPoints.size(); ++i)
		{
			//Vec2 u;
			//u.x = cos(mTransform->mRotation * (cDelicious / 180) + mRotVel * dt);
			//u.y = sin(mTransform->mRotation * (cDelicious / 180) + mRotVel * dt);

			Vec2 start;
			start = Vec2(mCPoints[i].x, mCPoints[i].y);


			start.x *= mExtents.x;
			start.y *= mExtents.y;

			start.x *= 2;
			start.y *= 2;

			start.x *= mTransform->mScale.x;
			start.y *= mTransform->mScale.y;

			//float tempx = start.x;
			//start.x = start.x * u.x - start.y * u.y;
			//start.y = tempx * u.y + start.y * u.x;

			start.x += mVelocity.x * dt;
			start.y += mVelocity.y * dt;

			mPoints[i].x = start.x;
			mPoints[i].y = start.y;
		}
	}

	/*****************************************************************************/
	/*!
	\brief
	Physics body deconstructor
	- Needs to recycle rather than delete however this function still is correct
	in cleaning up

	*/
	/*****************************************************************************/
	PhysBody::~PhysBody()
	{
		//delete all forces
		if(!mActiveForces.empty())
		{
			Force* deleteme = *(mActiveForces.begin());
			mActiveForces.clear();
			delete deleteme;

		}

		if(!mResponseForces.empty())
		{
			Force* deleteme = *(mResponseForces.begin());
			mResponseForces.clear();
			delete deleteme;

		}
		//unhook from physics body list
		PHYSICS->mPhysBodyList.remove(this);
		PHYSICS->mCollidableList.remove(this);
		PHYSICS->mStaticList.remove(this);
		PHYSICS->mNonStaticList.remove(this);
		//delete rot forces
		if(!mActiveRotForces.empty())
		{
			RotForce* deleteme = *(mActiveRotForces.begin());
			mActiveRotForces.clear();
			delete deleteme;

		}

		//PHYSICS->mPhysBodyList.remove(this);
		//delete the points
		if(!mPoints.empty())
		{
			mPoints.clear();
			mCPoints.clear();
		}
		//PHYSICS->mPhysBodyList.remove(this);

	}
}