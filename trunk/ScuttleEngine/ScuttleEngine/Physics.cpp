/**************************************************************************************************/
/*!
\file	Physics.cpp
\par	Project: Coalescence
\date	12/7/2011
\authors
Primary  : Alex Poplawski,
Secondary: Kevin Verholtz - Game state manager functions
\brief
Core of the Physics engine
Need to remove the legacy game state manager things as physics engine should not be
unloaded/loaded EVER

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/

#include "Precompiled.h"
#include "Physics.h"
#include "ComponentCreator.h"
#include "GameStateMgr.h"
#include "PhysBody.h"
#include "Regions.h"
#include "Core.h"
#include "DebugConsole.h"
#include "float.h"
#include "DebugDraw.h"
#include "PhysVars.h"
#include "Force.h"

#include "GameLogic.h"

const float sqrt2 = sqrt(2.0f);

namespace Scuttlemeyer
{
	//drag in physics flags
	using namespace PhysFlags;
	//extern the physics system, this is bad
	Physics* PHYSICS = NULL;
	/*****************************************************************************/
	/*!
	\brief
	Physics system constructor
	*/
	/*****************************************************************************/
	Physics::Physics()
	{
		//register the subcomponents of physics
		RegisterComponent(PhysBody);
		RegisterComponent(Region);
		//update the extern
		PHYSICS = this;
	}

	/*****************************************************************************/
	/*!
	\brief
	Physics unload function
	*/
	/*****************************************************************************/
	void Physics::Unload()
	{
#ifdef BLARG
		Message* q = new Message(Mid::DebugCNSL, 1, new std::string("Physics Unload."));
		CORE->EnqueueMessage(q);
#endif
	}

	/*****************************************************************************/
	/*!
	\brief
	Physics load function
	*/
	/*****************************************************************************/
	void Physics::Load()
	{
#ifdef BLARG
		Message* q = new Message(Mid::DebugCNSL, 1, new std::string("Physics Load."));
		CORE->EnqueueMessage(q);
#endif
	}
	/*****************************************************************************/
	/*!
	\brief
	Physics free function
	*/
	/*****************************************************************************/
	void Physics::Free()
	{
#ifdef BLARG
		Message* q = new Message(Mid::DebugCNSL, 1, new std::string("Physics Free."));
		CORE->EnqueueMessage(q);
#endif
	}
	/*****************************************************************************/
	/*!
	\brief
	Physics initialize function
	*/
	/*****************************************************************************/
	void Physics::Initialize()
	{
#ifdef BLARG
		Message* q = new Message(Mid::DebugCNSL, 1, new std::string("Physics Init."));
		CORE->EnqueueMessage(q);
#endif
		mForces = 0;
	}


	/*****************************************************************************/
	/*!
	\brief
	Check collision function held blocks
	\param[in] B1
	the block physics body

	\param[in] dt
	frame time

	\return
	the resolution vector
	*/
	/*****************************************************************************/
	Vec2* Physics::CheckBlock(PhysBody* b1, float dt)
	{
		b1->UpdatePoints(dt);
		std::list<PhysBody*>::iterator it3 = mStaticList.begin();
		while(mStaticList.end() != it3)
		{
			
			float tempY = ((*it3)->mTransform->mPosition.y > b1->mTransform->mPosition.y) 
				? (*it3)->mTransform->mPosition.y - ((*it3)->mExtents.y * (*it3)->mTransform->mScale.y) 
				: b1->mTransform->mPosition.y - (b1->mExtents.y * b1->mTransform->mScale.y);
			float tempY2 = (tempY == ((*it3)->mTransform->mPosition.y - ((*it3)->mExtents.y) * (*it3)->mTransform->mScale.y)) 
				? b1->mTransform->mPosition.y + (b1->mExtents.y * b1->mTransform->mScale.y) 
				: (*it3)->mTransform->mPosition.y + ((*it3)->mExtents.y * (*it3)->mTransform->mScale.y);

			if(tempY - tempY2 < 10.0f)
			{
				Vec2* temp = NULL;
				if(temp = CheckColHeld(b1, *it3, dt))
					return temp;
			}	
			++it3;
		}
		return NULL;
	}


	/*****************************************************************************/
	/*!
	\brief
	Physics update function for every game loop
	Need to fix dt limiting

	\param[in] dt
	frame time to use in calculations
	*/
	/*****************************************************************************/
	void Physics::Update(float dt)
	{


		//if dt is greater than .016 then limit it (bad way to do it)
		if(dt > cOneFrame)
		{
			dt = cOneFrame;
		}

		int i = 0;
		dt /= (float) cNumPhysicsLoops;
		while(i < cNumPhysicsLoops)
		{

			//if your in the debug console you shouldnt be running physics
			if((DBGCNSL->DBConsole == 0 || DBGCNSL->mSteps > 0) && gGameStateCurr != GS_PAUSE && !CORE->mBEditorActive)
			{
				//update the forces then update the points based off the velocity created
				for(std::list<PhysBody*>::iterator it = mPhysBodyList.begin(); it != mPhysBodyList.end(); ++it)
				{
					if(!((*it)->mPhysFlags & HELD))
					(*it)->UpdateForces(dt);

					(*it)->UpdatePoints(dt);
				}
				std::list<PhysBody*>::iterator it3 = mStaticList.begin();
				while(mStaticList.end() != it3)
				{
					std::list<PhysBody*>::iterator it2 = mNonStaticList.begin();
					while(it2 != mNonStaticList.end())
					{
						if(!((*it2)->mPhysFlags & FUN) && !((*it3)->mPhysFlags & HUD))//SHHH && !((*it2)->mPhysFlags & HELD))
						{
							//some optimization but not that much, bounding box collision
							/*float tempX = ((*it)->mTransform->mPosition.x > (*it2)->mTransform->mPosition.x) 
							? (*it)->mTransform->mPosition.x - ((*it)->mExtents.x * (*it)->mTransform->mScale.x) 
							: (*it2)->mTransform->mPosition.x - ((*it2)->mExtents.x * (*it2)->mTransform->mScale.x);
							float tempX2 = (tempX == ((*it)->mTransform->mPosition.x - ((*it)->mExtents.x) * (*it)->mTransform->mScale.x)) 
							? (*it2)->mTransform->mPosition.x + ((*it2)->mExtents.x * (*it2)->mTransform->mScale.x) 
							: (*it)->mTransform->mPosition.x + ((*it)->mExtents.x * (*it)->mTransform->mScale.x);

							if(tempX - tempX2 < 10.0f)
							{*/
							//float tempY = ((*it3)->mTransform->mPosition.y > (*it2)->mTransform->mPosition.y) 
							//	? (*it3)->mTransform->mPosition.y - ((*it3)->mExtents.y * (*it3)->mTransform->mScale.y) 
							//	: (*it2)->mTransform->mPosition.y - ((*it2)->mExtents.y * (*it2)->mTransform->mScale.y);
							//float tempY2 = (tempY == ((*it3)->mTransform->mPosition.y - ((*it3)->mExtents.y) * (*it3)->mTransform->mScale.y)) 
							//	? (*it2)->mTransform->mPosition.y + ((*it2)->mExtents.y * (*it2)->mTransform->mScale.y) 
							//	: (*it3)->mTransform->mPosition.y + ((*it3)->mExtents.y * (*it3)->mTransform->mScale.y);

							//if(tempY - tempY2 < 10.0f)
							//{
							if(!(((*it2)->mPhysFlags & GAS) && ((*it3)->mPhysFlags & SWITCH)))
								CheckCol(*it2, *it3, dt);
						/*	}*/
							/*}*/
						}
						++it2;
					}
					++it3;
				}
				for(std::list<PhysBody*>::iterator it = mNonStaticList.begin(); it != mNonStaticList.end(); ++it)
				{
					int numCol = 0;
					(*it)->Resolve(dt, numCol);
				}
				//update each physics body and check for collision
				for(std::list<PhysBody*>::iterator it = mNonStaticList.begin(); it != mNonStaticList.end(); ++it)
				{
					//dont count objects that dont need to be considered
					if(!((*it)->mPhysFlags & FUN) && !((*it)->mPhysFlags & HUD) && !((*it)->mPhysFlags & HELD))
					{
						std::list<PhysBody*>::iterator it2 = it;
						++it2;
						while(it2 != mNonStaticList.end())
						{
							if(!((*it2)->mPhysFlags & FUN) && !((*it)->mPhysFlags & HUD) && !((*it2)->mPhysFlags & HELD))
							{
								//some optimization but not that much, bounding box collision
								/*float tempX = ((*it)->mTransform->mPosition.x > (*it2)->mTransform->mPosition.x) 
								? (*it)->mTransform->mPosition.x - ((*it)->mExtents.x * (*it)->mTransform->mScale.x) 
								: (*it2)->mTransform->mPosition.x - ((*it2)->mExtents.x * (*it2)->mTransform->mScale.x);
								float tempX2 = (tempX == ((*it)->mTransform->mPosition.x - ((*it)->mExtents.x) * (*it)->mTransform->mScale.x)) 
								? (*it2)->mTransform->mPosition.x + ((*it2)->mExtents.x * (*it2)->mTransform->mScale.x) 
								: (*it)->mTransform->mPosition.x + ((*it)->mExtents.x * (*it)->mTransform->mScale.x);

								if(tempX - tempX2 < 10.0f)
								{*/
	/*							float tempY = ((*it)->mTransform->mPosition.y > (*it2)->mTransform->mPosition.y) 
									? (*it)->mTransform->mPosition.y - ((*it)->mExtents.y * (*it)->mTransform->mScale.y) 
									: (*it2)->mTransform->mPosition.y - ((*it2)->mExtents.y * (*it2)->mTransform->mScale.y);
								float tempY2 = (tempY == ((*it)->mTransform->mPosition.y - ((*it)->mExtents.y) * (*it)->mTransform->mScale.y)) 
									? (*it2)->mTransform->mPosition.y + ((*it2)->mExtents.y * (*it2)->mTransform->mScale.y) 
									: (*it)->mTransform->mPosition.y + ((*it)->mExtents.y * (*it)->mTransform->mScale.y);

								if(tempY - tempY2 < 10.0f)
								{*/
									CheckCol(*it, *it2, dt);
								/*}*/
								/*}*/
							}
							++it2;
						}

					}
					//check regions onto objects
					if(!((*it)->mPhysFlags & FUN) && !((*it)->mPhysFlags & HUD))
					{
						std::list<Region*>::iterator it3 = mRegionsList.begin();
						while(it3 != mRegionsList.end())
						{
							if((*it)->mPhysFlags & (*it3)->mFlags)
								CheckRegion(*it, *it3, dt);
							++it3;
						}
					}
				}
				//go back to the start of the frame then update all objects based on the outcome of the resolutions
				for(std::list<PhysBody*>::iterator it = mPhysBodyList.begin(); it != mPhysBodyList.end(); ++it)
				{
					int numCol = 0;
					(*it)->Resolve(dt, numCol);
				if(CORE->mMode > 2)
				{
					(*it)->ReversePoints(dt, numCol);
				}
					(*it)->Update(dt);
				}
			}
			++i;
		}

#ifdef BLARG
		//draw debug is needed
		if(CORE->mMode == 3 || CORE->mMode == 5)
		{
			DebugDraw();
		}
		//draw the debug regions
		if(CORE->mMode >= 4)
		{
			DebugDrawR();
		}
#endif
	}

	/*****************************************************************************/
	/*!
	\brief
	Check collision function for rigid bodies
	\param[in,out] B1
	First physics body

	\param[in,out] B2
	Second physics body

	\param[in] dt
	frame time

	*/
	/*****************************************************************************/
	void Physics::CheckCol(PhysBody* B1, PhysBody* B2,const float dt)
	{
		//rec2rec collision for removing unneccisary collision
		Vec2 velocity = B1->mVelocity;
		Vec2 velocity2 = B2->mVelocity;
		if(!(B1->mPhysFlags & BUL))
		{
			velocity.x *= dt;
			velocity.y *= dt;
		}
		else
		{
			velocity.x = 0;
			velocity.y = 0;
		}
		if(!(B2->mPhysFlags & BUL))
		{
			velocity2.x *= dt;
			velocity2.y *= dt;
		}
		else
		{
			velocity2.x = 0;
			velocity2.y = 0;
		}
		
		if(RecCol(B1->mTransform->mPosition + velocity, B1->mExtents.x * B1->mTransform->mScale.x * sqrt2, B1->mExtents.y * B1->mTransform->mScale.y * sqrt2,
			B2->mTransform->mPosition + velocity2, B2->mExtents.x * B2->mTransform->mScale.x * sqrt2, B2->mExtents.y * B2->mTransform->mScale.y * sqrt2))
		{
			/*if ((B1->mPhysFlags & LIQ && B2->mPhysFlags & FGravity) || (B2->mPhysFlags & LIQ && B1->mPhysFlags & FGravity))
			{
					//SAT collision
				Vec2* temp = SAT(B1,B2, dt);
				if(temp != NULL )
				{
					ApplyCol(B1, B2, dt, temp);
					if (!(B1->mPhysFlags & HUD) && !(B2->mPhysFlags & HUD))
					{
							//process special collision logic
						Message* q = new Message(Mid::Collision, 3, B1->GetOwner(), B2->GetOwner(), &dt);
						CORE->EnqueueMessage(q);
					}
				}
				if(temp != NULL)
					delete temp;
			}
			else */if(!(B1->mPhysFlags & BUL) && !(B2->mPhysFlags & BUL))// && !(B1->mPhysFlags & LIQ) && !(B2->mPhysFlags & LIQ))
			{
				//SAT collision
				Vec2* temp = SAT(B1,B2, dt);
				if(temp != NULL )
				{
					//if((temp->x != 0 && temp->y != 0))
					//B1->mTransform->mPosition.x += temp->x;
					//B1->mTransform->mPosition.y += temp->y;
					//apply if necissary
					
						ApplyCol(B1, B2, dt, temp);
					if (!(B1->mPhysFlags & HUD) && !(B2->mPhysFlags & HUD))
					{
						//process special collision logic
						Message* q = new Message(Mid::Collision, 3, B1->GetOwner(), B2->GetOwner(), &dt);
						CORE->EnqueueMessage(q);
						//if (!(LOGIC->ProcessCollision(B1->GetOwner(), B2->GetOwner())))
						//	ApplyCol(B1, B2, dt);
					}
				}
				if(temp != NULL)
					delete temp;
			}
			else if (((B1->mPhysFlags & HELD) || (B2->mPhysFlags & HELD) || !(B1->mPhysFlags & FGravity) || !(B2->mPhysFlags & FGravity) || (B1->mPhysFlags & LIQ) || (B2->mPhysFlags & LIQ)))// && !(B1->mPhysFlags & BUL && B2->mPhysFlags & BUL))
			{
				if(RecCol(B1->mTransform->mPosition + velocity, B1->mExtents.x * B1->mTransform->mScale.x, B1->mExtents.y * B1->mTransform->mScale.y,
					B2->mTransform->mPosition + velocity2, B2->mExtents.x * B2->mTransform->mScale.x , B2->mExtents.y * B2->mTransform->mScale.y ))
					if (!(B1->mPhysFlags & HUD) && !(B2->mPhysFlags & HUD))
					{
						//process special collision logic
						Message* q = new Message(Mid::Collision, 3, B1->GetOwner(), B2->GetOwner(), &dt);
						CORE->EnqueueMessage(q);
						//if (!(LOGIC->ProcessCollision(B1->GetOwner(), B2->GetOwner(), &dt)))
						//{
						//		//SAT collision
						//	Vec2* temp = SAT(B1,B2, dt);
						//	if(temp != NULL )
						//		ApplyCol(B1, B2, dt, temp);
						//	if(temp != NULL)
						//		delete temp;
						//}
						//if (!(LOGIC->ProcessCollision(B1->GetOwner(), B2->GetOwner())))
						//	ApplyCol(B1, B2, dt);
					}
			}


		}
	}

	/*****************************************************************************/
	/*!
	\brief
	checks collision with held objects

	\param[in,out] B1
	First physics body

	\param[in,out] B2
	Second physics body

	\param[in] dt
	frame time

	\return
	the resolution vector

	*/
	/*****************************************************************************/
	Vec2* Physics::CheckColHeld(PhysBody* B1, PhysBody* B2,const float dt)
	{
		//rec2rec collision for removing unneccisary collision
		Vec2 velocity = B1->mVelocity;
		Vec2 velocity2 = B2->mVelocity;
		float sqrt2 = sqrt(2.0f);
		if(RecCol(B1->mTransform->mPosition + velocity, B1->mExtents.x * B1->mTransform->mScale.x * sqrt2, B1->mExtents.y * B1->mTransform->mScale.y * sqrt2,
			B2->mTransform->mPosition + velocity2, B2->mExtents.x * B2->mTransform->mScale.x * sqrt2, B2->mExtents.y * B2->mTransform->mScale.y * sqrt2))
		{
				B2->UpdatePoints(dt);
				//SAT collision
				Vec2* temp = SAT(B1,B2, dt);
				if(temp)
				{
					return temp;
				}
		}
		return NULL;
	}


	/*****************************************************************************/
	/*!
	\brief
	Check Region function for applying regions onto objects
	\param[in,out] B1
	First physics body

	\param[in,out] B2
	Region to check against

	\param[in] dt
	frame time

	*/
	/*****************************************************************************/
	void Physics::CheckRegion(PhysBody* B1, Region* B2, const float dt)
	{
		//do SAT on the region and body and if its true apply the region
		Vec2 velocity = B1->mVelocity;
		//Vec2 velocity2 = B2->mVelocity;
		velocity.x *= dt;
		//velocity2.x *= dt;
		velocity.y *= dt;
		//velocity2.y *= dt;
		{
			Vec2* temp = SATR(B1,B2, dt);
			if(temp != NULL )
			{
				//if((temp->x != 0 && temp->y != 0))
				//B1->mTransform->mPosition.x += temp->x;
				//B1->mTransform->mPosition.y += temp->y;

				B2->AddForce(B1);
			}
			if(temp != NULL)
				delete temp;
		}
	}


	/*****************************************************************************/
	/*!
	\brief
	Apply collision function for objects that collide and have used SAT to determine so
	also utilizes flags to show correct response

	\param[in,out] B1
	First physics body

	\param[in,out] B2
	Second physics body

	\param[in] dt
	frame time

	\param[in] direction
	the MTV outcome of the collision

	*/
	/*****************************************************************************/
	void Physics::ApplyCol(PhysBody* B1, PhysBody* B2, const float dt, const Vec2* direction)
	{
		float scale;
		if(cNumPhysicsLoops == 1)
			scale = dt / cOneFrame;
		else
			scale = (dt * cNumPhysicsLoops) / (cOneFrame * cOneFrame);
		if(CORE->mMode == 3 || CORE->mMode == 5)
		{
			Drawer::Instance.SetColor(Vec4(1,0,1,1));
			Vec2 temp2 = B1->mPoints[0];
			temp2.x += B1->mTransform->mPosition.x;
			temp2.y += B1->mTransform->mPosition.y;
			Vec2 temp = temp2;
			if(direction->x > 100)
			{
				return;
			}
			temp.x +=  direction->x * scale;
			temp.y +=  direction->y * scale;

			Drawer::Instance.MoveTo(temp2);
			Drawer::Instance.LineTo(temp);

		}
		if(B1->mRotVel > -.01 && B1->mRotVel < .01)
			B1->mRotVel = 0;
		if(!(B1->mPhysFlags & HUD) && !(B2->mPhysFlags & HUD))
		{
			//apply correct collision response based on flags
			//Velocity temp = Velocity(B2->mVelocity.x, B2->mVelocity.y);
			//if(B2->mPhysFlags & FStand)
			//{



			//	/*int side = Side(B1->mTransform->mPosition + velocity, B1->mExtents.x * B1->mTransform->mScale.x, B1->mExtents.y * B1->mTransform->mScale.y,
			//	B2->mTransform->mPosition + velocity2, B2->mExtents.x * B2->mTransform->mScale.x, B2->mExtents.y * B2->mTransform->mScale.y);

			//	int side2 = Side(B1->mTransform->mPosition, B1->mExtents.x * B1->mTransform->mScale.x, B1->mExtents.y * B1->mTransform->mScale.y,
			//	B2->mTransform->mPosition, B2->mExtents.x * B2->mTransform->mScale.x, B2->mExtents.y * B2->mTransform->mScale.y);*/

			//	//if((side & 1 && side & 2))// && !((side & 8) || (side & 4)))
			//	//	if(side & 4 && side2 & 8)
			//	if(direction->x != 0)
			//	{
			//		
			//		//B1->mVelocity.x = B2->mVelocity.x;
			//		//B1->mRotVel = 0;
			//	}
			//	if(direction->y != 0)
			//	{
			//		if(B1->mColFlags & 1 && (B1->mTransform->mPosition.y > B2->mTransform->mPosition.y))
			//			B1->mColFlags |= 2;
			//		if(B2->mVelocity.y > 0)
			//			//B1->AddForce((direction->x * direction->x + direction->y * direction->y), atan(direction->x / direction->y));
			//			B1->mVelocity.y = B2->mVelocity.y;
			//		else
			//			B1->mVelocity.y = 0;
			//		//B1->mRotVel = 0;

			//	}
			//	/*if((side & 4) && (side & 1 && side & 2))
			//	B1->mVelocity.y = 0;*/
			//	if(B1->mRotVel != 0)
			//	{
			//		//if(!(B1->mPhysFlags & FStand))
			//		//{
			//		//	B1->mTransform->mPosition.x += direction->x;// * dt * 10;
			//		//	B1->mTransform->mPosition.y += direction->y;// * dt * 10;
			//		//}
			//		//if(!(B2->mPhysFlags & FStand))
			//		//{
			//		//B2->mTransform->mPosition.x -= direction->x;// * dt * 10;
			//		//B2->mTransform->mPosition.y -= direction->y;// * dt * 10;
			//		//}
			//		}
			//}
			//else if(B1->mPhysFlags & FStand)
			//{
			//	if((B2->mVelocity.y > 0 && B2->mTransform->mPosition.y < B1->mTransform->mPosition.y) || (B2->mVelocity.y < 0 && B2->mTransform->mPosition.y > B1->mTransform->mPosition.y))
			//		B2->mVelocity.y = 0;
			//	if((B2->mVelocity.x > 0 && B2->mTransform->mPosition.x < B1->mTransform->mPosition.x) || (B2->mVelocity.x < 0 && B2->mTransform->mPosition.x > B1->mTransform->mPosition.x))
			//		B2->mVelocity.x = 0;
			//	B1->mColFlags |= 2;
			//	B2->mColFlags |= 2;
			//}
			if(B2->mPhysFlags & FStaticObj)
			{
				if(B1->mTransform->mRotation != 0)
				{
					if(direction->y > .05f)
					{
						B1->AddResponse(direction->y * scale,-cDirDown, 0 , dt);
						B1->mColFlags |= 2;
						B2->mColFlags |= 4;
						B1->mStack |= 2;
					}
					else if(direction->y < -.05f)
					{
						B1->AddResponse(direction->y * scale,-cDirDown, 0 , dt);
						B1->mColFlags |= 4;
						B2->mColFlags |= 2;
						B1->mStack |= 2;
					}

					if(direction->x > .05f || direction->x < -.05f)
					{
						B1->AddResponse(direction->x * scale , cDirRight, 0 , dt);
						B1->mStack |= 1;
					}
				}
				else
				{
					if(direction->y > .05f)
					{
						B1->AddResponse(direction->y * scale,cDirDown, 0 , dt);
						B1->mColFlags |= 4;
						B2->mColFlags |= 2;
						B1->mStack |= 2;
					}
					else if(direction->y < -.05f)
					{
						B1->AddResponse(direction->y * scale,cDirDown, 0 , dt);
						B1->mColFlags |= 2;
						B2->mColFlags |= 4;
						B1->mStack |= 2;
					}

					if(direction->x > .05f || direction->x < -.05f)
					{
						B1->AddResponse(direction->x * scale, cDirLeft, 0 , dt);
						B1->mStack |= 1;
					}
				}
			}
			else if(B1->mPhysFlags & FStaticObj)
			{
				if(B2->mTransform->mRotation != 0)
				{
					if(direction->y > .05f)
					{
						B2->AddResponse(direction->y * scale,-cDirDown, 0 , dt);
						B2->mColFlags |= 2;
					}
					else if(direction->y < -.05f)
					{
						B2->AddResponse(direction->y * scale,-cDirDown, 0 , dt);
					}

					if(direction->x > .05f || direction->x < -.05f)
					{
						B2->AddResponse(direction->x * scale, -cDirLeft, 0 , dt);
					}
				}
				else
				{
					if(direction->y > .05f)
					{
						B1->AddResponse(direction->y * scale,cDirDown , 0 , dt);
					}
					else if(direction->y < -.05f)
					{
						B2->AddResponse(direction->y * scale,cDirDown, 0 , dt);
						B2->mColFlags |= 2;
					}

					if(direction->x > .05f || direction->x < -.05f)
					{
						B2->AddResponse(direction->x * scale, cDirLeft, 0 , dt);
					}
				}
			}
			else if(((B1->mPhysFlags & PLA && B2->mPhysFlags & FGravity) || (B2->mPhysFlags & PLA && B1->mPhysFlags & FGravity)
				|| (B1->mPhysFlags & FGravity && B2->mPhysFlags & FGravity)) || (!(B1->mPhysFlags & FStand) && !(B2->mPhysFlags & FStand)))
			{
				if(!((B1->mPhysFlags & PLA) && ((B2->mPhysFlags & GAS) || (B2->mPhysFlags & LIQ))))
				{
					if((direction->y / 2.0f) < -.05f)
					{
						if(B1->mStack & 2)
							B2->AddResponse((direction->y) * scale * B2->mMass / B1->mMass, cDirUp, 0 , dt);
						else
							B2->AddResponse((direction->y / 2.0f) * scale * B2->mMass / B1->mMass, cDirUp, 0 , dt);
						if(B2->mStack & 2)
							B1->AddResponse((direction->y) * scale * B1->mMass / B2->mMass, cDirDown, 0 , dt);
						else 
							B1->AddResponse((direction->y / 2.0f) * scale * B1->mMass / B2->mMass, cDirDown, 0 , dt);
						B1->mColFlags |= 2;
						B2->mColFlags |= 4;
					}
					else if((direction->y / 2.0f) > .05f)
					{
						if(B1->mStack & 2)
							B2->AddResponse((direction->y) * scale * B2->mMass / B1->mMass, cDirUp, 0 , dt);
						else
							B2->AddResponse((direction->y / 2.0f) * scale * B2->mMass / B1->mMass, cDirUp, 0 , dt);
						if(B2->mStack & 2)
							B1->AddResponse((direction->y) * scale * B1->mMass / B2->mMass, cDirDown, 0 , dt);
						else 
							B1->AddResponse((direction->y / 2.0f) * scale * B1->mMass / B2->mMass, cDirDown, 0 , dt);		
					}

					if((direction->x / 2.0f) > .05f || (direction->x / 2.0f) < -.05f)
					{
						if(B1->mTransform->mRotation != 0)
							B2->AddResponse((direction->x / 2.0f) * scale * B2->mMass / B1->mMass, cDirLeft, 0 , dt);
						else
							B2->AddResponse((direction->x / 2.0f) * scale * B2->mMass / B1->mMass, cDirRight, 0 , dt);
						if(B1->mTransform->mRotation != 0)
						{
							B1->AddResponse((direction->x / 2.0f) * scale * B1->mMass / B2->mMass, cDirRight, 0 , dt);		
						}
						else
							B1->AddResponse((direction->x / 2.0f)* scale * B1->mMass / B2->mMass, cDirLeft, 0 , dt);

						//B2->AddResponse(direction->x * dt / cOneFrame * B2->mMass / B1->mMass, cDirRight, 0 , dt);
						//B1->AddResponse(direction->x * dt / cOneFrame * B1->mMass / B2->mMass,  cDirLeft, 0 , dt);
					}
				}
			}

			//else
			//{
			//	B1->mTransform->mPosition.x -= direction->x * .5f;
			//	B1->mTransform->mPosition.y += direction->y * .5f;
			//	//B2->mTransform->mPosition.x += direction->x * dt * 12;
			//	//B2->mTransform->mPosition.y -= direction->y * .5;
			//	if(B1->mTransform->mPosition.y - (B2->mTransform->mPosition.y) < 50 && B1->mTransform->mPosition.y - (B2->mTransform->mPosition.y) > -50)
			//	{
			//	B2->mVelocity.x = B1->mVelocity.x / B2->mMass;
			//	B1->mVelocity.x = B2->mVelocity.x;
			//	}
			//	else
			//	{
			//	B1->mColFlags |= 2;
			//	B2->mColFlags |= 2;
			//	}
			//	B1->mVelocity.y = B2->mVelocity.y;
			//	if(B1->mVelocity.y > 0)
			//		B2->mVelocity.y = B1->mVelocity.y;
			//	else
			//		B2->mVelocity.y = 0;

			//	//B1->mVelocity.y = B2->mVelocity.y;

			//}
		}
	}

	/*****************************************************************************/
	/*!
	\brief
	Apply collision function for objects that collide and used bounding box
	also utilizes flags to show correct response
	LEGACY in a way

	\param[in,out] B1
	First physics body

	\param[in,out] B2
	Second physics body

	\param[in] dt
	frame time
	*/
	/*****************************************************************************/
	void Physics::ApplyCol(PhysBody* B1, PhysBody* B2,const float dt)
	{
		Vec2 velocity = B1->mVelocity;
		Vec2 velocity2 = B2->mVelocity;
		velocity.x *= dt;
		velocity2.x *= dt;
		velocity.y *= dt;
		velocity2.y *= dt;

		if(B1->mRotVel > -.01 && B1->mRotVel < .01)
			B1->mRotVel = 0;
		if(!(B1->mPhysFlags & HUD) && !(B2->mPhysFlags & HUD))
		{
			Velocity temp = Velocity(B2->mVelocity.x, B2->mVelocity.y);
			if(B2->mPhysFlags & FStand)
			{
				int side = Side(B1->mTransform->mPosition + velocity, B1->mExtents.x * B1->mTransform->mScale.x, B1->mExtents.y * B1->mTransform->mScale.y,
					B2->mTransform->mPosition + velocity2, B2->mExtents.x * B2->mTransform->mScale.x, B2->mExtents.y * B2->mTransform->mScale.y);

				int side2 = Side(B1->mTransform->mPosition, B1->mExtents.x * B1->mTransform->mScale.x, B1->mExtents.y * B1->mTransform->mScale.y,
					B2->mTransform->mPosition, B2->mExtents.x * B2->mTransform->mScale.x, B2->mExtents.y * B2->mTransform->mScale.y);

				if((side & 1 && side & 2))// && !((side & 8) || (side & 4)))
					if(side & 4 && side2 & 8)
					{
						B1->mVelocity.x = 0;
						B1->mRotVel = 0;
					}
					if(side & 4 && side2 & 8)
					{
						if(B1->mColFlags & 1 && (B1->mTransform->mPosition.y > B2->mTransform->mPosition.y))
							B1->mColFlags |= 2;
						B1->mVelocity.y = 0;
						B1->mRotVel = 0;

					}
					if((side & 4) && (side & 1 && side & 2))
						B1->mVelocity.y = 0;
					/*if(B1->mRotVel != 0)
					{
					B1->mTransform->mPosition.x += direction->x * dt * 5;
					B1->mTransform->mPosition.y += direction->y * dt * 5;
					}*/
			}
			else if(B1->mPhysFlags & FStand)
			{
				if((B2->mVelocity.y > 0 && B2->mTransform->mPosition.y < B1->mTransform->mPosition.y) || (B2->mVelocity.y < 0 && B2->mTransform->mPosition.y > B1->mTransform->mPosition.y))
					B2->mVelocity.y = 0;
				if((B2->mVelocity.x > 0 && B2->mTransform->mPosition.x < B1->mTransform->mPosition.x) || (B2->mVelocity.x < 0 && B2->mTransform->mPosition.x > B1->mTransform->mPosition.x))
					B2->mVelocity.x = 0;
			}
			else if(B2->mPhysFlags & FStaticObj)
			{
				B1->mVelocity.x *= -1;
				B1->mVelocity.y *= -1;
			}
			else if(B1->mPhysFlags & FStaticObj)
			{
				B2->mVelocity.x *= -1;
				B2->mVelocity.y *= -1;
			}
			else
			{
				B2->mVelocity.x = B1->mVelocity.x / B2->mMass;
				B2->mVelocity.y = B1->mVelocity.y / B2->mMass;
				B1->mVelocity.x = temp.x / B1->mMass;
				B1->mVelocity.y = temp.y / B1->mMass;
			}
		}
	}

	/*****************************************************************************/
	/*!
	\brief
	Performs Seperated Axis Theorm collision to check if and object is inside a region 

	\param[in,out] B1
	First physics body

	\param[in,out] B2
	Region to check collision on

	\param[in] dt
	frame time

	\return 
	the normal of collision

	*/
	/*****************************************************************************/
	Vec2* Physics::SATR(PhysBody* B1, Region* B2,const float dt)
	{
		float Overlap = 99999;

		int faces1 = B1->mPoints.size();
		int faces2 = B2->mPoints.size();
		Vec2 good;

		for(int i = 0; i < faces1; i++)
		{


			//Grab a face (face x, face y)
			float x = B1->mPoints[i].x - B1->mPoints[(i + 1) % faces1].x;
			float y = B1->mPoints[i].y - B1->mPoints[(i + 1) % faces1].y;

			Vec2 temp;
			temp.x = -y;
			temp.y = x;

			float norm = Normalize(temp);


			float B1min = FLT_MAX, B1max = -B1min;
			float B2min = FLT_MAX, B2max = -B2min;


			//Project every point in c1 on the axis and store min and max
			for(int j = 0; j < faces1; j++)
			{
				float B1proj = (temp.x * (B1->mPoints[j].x + B1->mTransform->mPosition.x) + temp.y * (B1->mPoints[j].y + B1->mTransform->mPosition.y)) / (LengthSquared(temp));
				B1min = min(B1proj, B1min);
				B1max = max(B1proj, B1max);
			}

			//Project every point in c2 on the axis and store min and max
			for(int j = 0; j < faces2; j++)
			{
				float B2proj = (temp.x * (B2->mPoints[j].x + B2->mTransform->mPosition.x) + temp.y * (B2->mPoints[j].y +  B2->mTransform->mPosition.y)) / (LengthSquared(temp));
				B2min = min(B2proj, B2min);
				B2max = max(B2proj, B2max);
			}

			//Return if the projections do not overlap
			if(!(B1max >= B2min && B1min <= B2max))
				return NULL;
			if(B1max - B2min < Overlap)
			{
				Overlap = abs(B1max - B2min);
				good = temp;
			}
		}


		for(int i = 0; i < faces2; i++)
		{


			//Grab a face (face x, face y)
			float x = B2->mPoints[i].x - B2->mPoints[(i + 1) % faces2].x;
			float y = B2->mPoints[i].y - B2->mPoints[(i + 1) % faces2].y;

			Vec2 temp;
			temp.x = -y;
			temp.y = x;

			float norm = Normalize(temp);


			float B1min = FLT_MAX, B1max = -B1min;
			float B2min = FLT_MAX, B2max = -B2min;


			//Project every point in c1 on the axis and store min and max
			for(int j = 0; j < faces2; j++)
			{
				float B1proj = (temp.x * (B2->mPoints[j].x + B2->mTransform->mPosition.x) + temp.y * (B2->mPoints[j].y + B2->mTransform->mPosition.y)) / (LengthSquared(temp));
				B1min = min(B1proj, B1min);
				B1max = max(B1proj, B1max);
			}

			//Project every point in c2 on the axis and store min and max
			for(int j = 0; j < faces1; j++)
			{
				float B2proj = (temp.x * (B1->mPoints[j].x + B1->mTransform->mPosition.x) + temp.y * (B1->mPoints[j].y + B1->mTransform->mPosition.y)) / (LengthSquared(temp));
				B2min = min(B2proj, B2min);
				B2max = max(B2proj, B2max);
			}

			//Return if the projections do not overlap
			if(!(B1max >= B2min && B1min <= B2max))
				return NULL;
			if(B1max - B2min < Overlap)
			{
				Overlap = abs(B1max - B2min);
				good = temp;
			}
		}



		Vec2* MTV = new Vec2(good);
		MTV->x *= Overlap; 
		if(MTV->x < .001 && MTV->x > -.001)
			MTV->x = 0;
		MTV->y *= Overlap; 
		if(MTV->y < .001 && MTV->y > -.001)
			MTV->y = 0;


		return MTV; //return true;
	}

	/*****************************************************************************/
	/*!
	\brief
	Performs Seperated Axis Theorm collision to check if and object is colliding with another object

	\param[in,out] B1
	First physics body

	\param[in,out] B2
	Region to check collision on

	\param[in] dt
	frame time

	\return
	the amount its colliding
	*/
	/*****************************************************************************/
	Vec2* Physics::SAT(PhysBody* B1, PhysBody* B2, const float dt)
	{
		float Overlap = FLT_MAX;

		int faces1 = B1->mPoints.size();
		int faces2 = B2->mPoints.size();
		Vec2 good;
		good.x = 0;
		good.y = 0;
		for(int i = 0; i < faces1; i++)
		{


			//Grab a face (face x, face y)
			float x = B1->mPoints[i].x - B1->mPoints[(i + 1) % faces1].x;
			float y = B1->mPoints[i].y - B1->mPoints[(i + 1) % faces1].y;

			Vec2 temp;
			temp.x = 0;
			temp.y = 0;
			temp.x = -y;
			temp.y = x;

			float norm = Normalize(temp);


			float B1min = FLT_MAX, B1max = -B1min;
			float B2min = FLT_MAX, B2max = -B2min;


			//Project every point in c1 on the axis and store min and max
			for(int j = 0; j < faces1; j++)
			{
				float B1proj = (temp.x * (B1->mPoints[j].x + B1->mTransform->mPosition.x) + temp.y * (B1->mPoints[j].y + B1->mTransform->mPosition.y)) / (LengthSquared(temp));
				B1min = min(B1proj, B1min);
				B1max = max(B1proj, B1max);
			}

			//Project every point in c2 on the axis and store min and max
			for(int j = 0; j < faces2; j++)
			{
				float B2proj = (temp.x * (B2->mPoints[j].x + B2->mTransform->mPosition.x) + temp.y * (B2->mPoints[j].y +  B2->mTransform->mPosition.y)) / (LengthSquared(temp));
				B2min = min(B2proj, B2min);
				B2max = max(B2proj, B2max);
			}

			//Return if the projections do not overlap
			if(!(B1max >= B2min && B1min <= B2max))
				return NULL;
			if(B1max - B2min < Overlap)
			{
				Overlap = abs(B1max - B2min);
				good = temp;
			}
		}


		for(int i = 0; i < faces2; i++)
		{


			//Grab a face (face x, face y)
			float x = B2->mPoints[i].x - B2->mPoints[(i + 1) % faces2].x;
			float y = B2->mPoints[i].y - B2->mPoints[(i + 1) % faces2].y;

			Vec2 temp;
			temp.x = 0;
			temp.y = 0;
			temp.x = -y;
			temp.y = x;

			float norm = Normalize(temp);


			float B1min = FLT_MAX, B1max = -B1min;
			float B2min = FLT_MAX, B2max = -B2min;


			//Project every point in c1 on the axis and store min and max
			for(int j = 0; j < faces2; j++)
			{
				float B1proj = (temp.x * (B2->mPoints[j].x + B2->mTransform->mPosition.x) + temp.y * (B2->mPoints[j].y + B2->mTransform->mPosition.y)) / (LengthSquared(temp));
				B1min = min(B1proj, B1min);
				B1max = max(B1proj, B1max);
			}

			//Project every point in c2 on the axis and store min and max
			for(int j = 0; j < faces1; j++)
			{
				float B2proj = (temp.x * (B1->mPoints[j].x + B1->mTransform->mPosition.x) + temp.y * (B1->mPoints[j].y + B1->mTransform->mPosition.y)) / (LengthSquared(temp));
				B2min = min(B2proj, B2min);
				B2max = max(B2proj, B2max);
			}

			//Return if the projections do not overlap
			if(!(B1max >= B2min && B1min <= B2max))
				return NULL;
			if(B1max - B2min < Overlap)
			{
				Overlap = abs(B1max - B2min);
				good = temp;
			}
		}

		//calculate the shortest distance to get out of the object
		Vec2* MTV = new Vec2(good);


		MTV->x *= Overlap; 
		if((MTV->x < .001f && MTV->x > -.001f))
			MTV->x = 0;
		MTV->y *= Overlap; 
		if((MTV->y < .001 && MTV->y > -.001))
			MTV->y = 0;


		return MTV; //return true;
	}


	/*****************************************************************************/
	/*!
	\brief
	Performs box box collision to tell if two objects are colliding or not

	\param[in,out] Rect0
	rectangle 1

	\param[in] Width
	Region to check collision on

	\param[in] dt
	frame time

	\return
	if they collide or not
	*/
	/*****************************************************************************/
	bool Physics::RecCol(Vec2 &Rect0, float Width0, float Height0, Vec2 &Rect1, float Width1, float Height1)
	{
		if((Rect0.x + Width0) <= (Rect1.x - Width1) || (Rect0.x - Width0) >= (Rect1.x + Width1) 
			|| (Rect0.y + Height0) <= (Rect1.y - Height1) || (Rect0.y - Height0) >= (Rect1.y + Height1))
			return false;
		return true;
	}
	/*****************************************************************************/
	/*!
	\brief
	Tell what side an object is colliding on

	\param[in,out] B1
	First physics body

	\param[in,out] B2
	Region to check collision on

	\param[in] dt
	frame time

	*/
	/*****************************************************************************/
	int Physics::Side(Vec2 &Rect0, float Width0, float Height0, Vec2 &Rect1, float Width1, float Height1)
	{
		int flag = 0;
		if((Rect0.x + Width0) > (Rect1.x - Width1))
			flag |= 1;//right
		if((Rect0.x - Width0) < (Rect1.x + Width1))
			flag |= 2; //left
		if((Rect0.y + Height0) > (Rect1.y - Height1))
			flag |= 4; //top
		if((Rect0.y - Height0) < (Rect1.y + Height1))
			flag |= 8;//down
		return flag;
	}

	/*****************************************************************************/
	/*!
	\brief
	debug draw for physics system

	*/
	/*****************************************************************************/
	void Physics::DebugDraw()
	{
		for(std::list<PhysBody*>::iterator it = mPhysBodyList.begin(); it != mPhysBodyList.end(); ++it)
		{
			(*it)->DebugDraw();
		}
	}
	/*****************************************************************************/
	/*!
	\brief
	draw the debug regions

	*/
	/*****************************************************************************/
	void Physics::DebugDrawR()
	{
		for(std::list<Region*>::iterator it = mRegionsList.begin(); it != mRegionsList.end(); ++it)
		{
			(*it)->DebugDraw();
		}
	}


	/*****************************************************************************/
	/*!
	\brief
	message system for physics

	\param[in] message
	message to use

	\return
	none just inheratice handling

	*/
	/*****************************************************************************/
	Mid::MessageIdType Physics::SendMessage(Message *message)
	{
		return Mid::None;
	}

	/*****************************************************************************/
	/*!
	\brief
	physics system destructor
	*/
	/*****************************************************************************/
	Physics::~Physics()
	{

	}
}