/**************************************************************************************************/
/*!
\file	Turret.cpp
\par	Project: Coalescence
\date	4/11/2012
\authors
Primary : Kevin Verholtz

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#include "Precompiled.h"
#include "Turret.h"
#include "DebugDraw.h"
#include "DebugBars.h"
#include "Core.h"
#include "Physics.h"
#include "SoundNode.h"

namespace Scuttlemeyer
{
	/*****************************************************************************/
	/*!
	\brief
		Name of ILogical
	*/
	/*****************************************************************************/
	Turret::Turret()
	{
		mName = "Turret";
	}

	/*****************************************************************************/
	/*!
	\brief
		Initializes lots of stuff so it doesn't have to call has all the time
	*/
	/*****************************************************************************/
	void Turret::Initialize()
	{
		LOGIC->mLogicList.push_back(reinterpret_cast<ILogical*>(this));
		PhysBody* pPhysics = ((PhysBody*)GetOwner()->GetComponent(CT_PhysBody));
		mPlayerTrans = LOGIC->mPlayer->has(Transform);
		mPlayerPlayer = LOGIC->mPlayer->has(Player);
		mCompSprite = GetOwner()->has(CompSprite);
		mTransform = GetOwner()->has(Transform);
		pPhysics->mColFlags |= 1;
		mScanDirection = Up;
		mRotSpeed = 2;
		mOriginalRotation = mTransform->mRotation;// + GetOwner()->has(CompSprite)->mComponents[0]->mLocalTransform->mRotation + GetOwner()->has(CompSprite)->mComponents[1]->mLocalTransform->mRotation;
		mAttackState = Done;
		mAttackTimer = timeGetTime();
		mLockedOnTimer = timeGetTime();
		mOffsetCollided.x = 0;
		mOffsetCollided.y = 0;
		mLockedOn = false;
		mShootCount = 3;
	}

	/*****************************************************************************/
	/*!
	\brief
		Sets the state to shooting sometimes
	*/
	/*****************************************************************************/
	int Turret::ChangeState(EState nState, bool veloReset)
	{
		mAttackState = nState;
		return 1;
	}

	/*****************************************************************************/
	/*!
	\brief
		Update the turrets Scanning and shooting algerithms
	*/
	/*****************************************************************************/
	void Turret::Update(float dt)
	{
		Scanning();
		if(mAttackState == Attack && mPlayerPlayer->mHealth > 0 && !mPlayerPlayer->mInvincible)
		{
			if(timeGetTime() - mLockedOnTimer >= 16000 * dt)
			{
				RocketAttack(dt);
			}
		}
	}

	/*****************************************************************************/
	/*!
	\brief
		Fires rockets at the players locked on spot
	*/
	/*****************************************************************************/
	void Turret::RocketAttack(float dt)
	{
		if(timeGetTime() - mAttackTimer >= 4000 * dt)
		{
			++mShootCount;

			if(mShootCount == 4)
			{
				GetOwner()->has(SoundNode)->Start(0);
				mShootCount = 0;
			}

			mAttackTimer = timeGetTime();
			Transform * trans = (mCompSprite)->mComponents[0]->mLocalTransform;
			Transform * trans2 = mTransform;

			float accuracy = trans->mRotation + (rand()%10 - 5);

			Vec2 offset;
			offset.x = cos(((trans->mRotation + trans2->mRotation + 90)/180.f) * cDelicious)*(425.f*trans2->mScale.x) - sin(((trans->mRotation + 90 + trans2->mRotation + 90)/180.f) * cDelicious)*8.f;
			offset.y = sin(((trans->mRotation + trans2->mRotation + 90)/180.f) * cDelicious)*(425.f*trans2->mScale.x) + cos(((trans->mRotation + 90 + trans2->mRotation + 90)/180.f) * cDelicious)*8.f;

			GOC* RocketObj = LOGIC->SpawnObj("Rocket", trans2->mPosition.x + offset.x, trans2->mPosition.y + offset.y, trans->mRotation, 0.8, 0.8, 0);
			RocketObj->has(Sprite)->mTransform->mRotation = accuracy + trans2->mRotation;
		}
	}

	/*****************************************************************************/
	/*!
	\brief
		Tracs the player when he moves.
	*/
	/*****************************************************************************/
	void Turret::LockedOn()
	{
		//Players Transform
		Transform * player = mPlayerTrans;
		Transform* trans = (mTransform);
		
		//player -> move to orgin... 
		//triangle width height
		//width = player->mPosition.x - trans->mPosition.x
		//height = player->mPosition.y - trans->mPosition.y
		//angle = atan(height/width)
		float width = (player->mPosition.x + mOffsetCollided.x) - trans->mPosition.x;
		float height = (player->mPosition.y + mOffsetCollided.y) - trans->mPosition.y;
		float angle = atan2(height, width);

		float rot = ((angle * 180)/cDelicious) - trans->mRotation - 90;

		if(rot + trans->mRotation < mOriginalRotation + mFieldOfView
		&& rot + trans->mRotation > mOriginalRotation - mFieldOfView)
		{
			(mCompSprite)->mComponents[0]->mLocalTransform->mRotation = rot;
		}
	}

	/*****************************************************************************/
	/*!
	\brief
		Scan to check if the turret can see the player to shoot at him
	*/
	/*****************************************************************************/
	void Turret::Scanning()
	{
		if(BresenhamLine() == true)
		{
			if(mLockedOn == false)
			{
				GetOwner()->has(SoundNode)->Start(1);
				mLockedOn = true;
			}

			if(mAttackState == Done)
			{
				mAttackState = Attack;
				mAttackTimer = timeGetTime();
			}
			LockedOn();
		}
		else
		{
			if(mLockedOn == true)
			{
				GetOwner()->has(SoundNode)->Start(2);
				mLockedOn = false;
			}
			
			mAttackState = Done;
			CompSprite* pDrawable = (mCompSprite);

			if(mScanDirection == Up)
			{
				if(pDrawable->mComponents[0]->mLocalTransform->mRotation + mTransform->mRotation < mOriginalRotation + mFieldOfView)
				{
					// Rotates the cannon CCW
					pDrawable->mComponents[0]->mLocalTransform->mRotation += mRotSpeed;
				}
				else
				{
					mScanDirection = Down;
				}
			}
			else
			{
				if(pDrawable->mComponents[0]->mLocalTransform->mRotation + mTransform->mRotation > mOriginalRotation - mFieldOfView)
				{
					// Rotates the cannon CW
					pDrawable->mComponents[0]->mLocalTransform->mRotation -= mRotSpeed;
				}
				else
				{
					mScanDirection = Up;
				}
			}
		}
	}

	/*****************************************************************************/
	/*!
	\brief
		tells you what the turret collides with when scanning
	*/
	/*****************************************************************************/
	bool Turret::BresenhamLine()
	{
		Transform* trans = mTransform;
		float rot = (mCompSprite)->mComponents[0]->mLocalTransform->mRotation + trans->mRotation + 90;

		int x0 = (int)trans->mPosition.x;
		int y0 = (int)trans->mPosition.y;
		int x1 = (int)trans->mPosition.x + (int)(cos((rot * cDelicious)/180) * (3*GRAPHICS->mScreenWidth/4));
		int y1 = (int)trans->mPosition.y + (int)(sin((rot * cDelicious)/180) * (3*GRAPHICS->mScreenWidth/4));

		if(mAttackState == Done)
		{
			Transform * player = mPlayerTrans;

			if(!(player->mPosition.x < x0 && player->mPosition.x > x1) && !(player->mPosition.x > x0 && player->mPosition.x < x1))
			{
				return false;
			}
			else if(!(player->mPosition.y < y0 && player->mPosition.y > y1) && !(player->mPosition.y > y0 && player->mPosition.y < y1))
			{
				return false;
			}		
		}

		int halfPlayerSize = (int)(3*(LOGIC->mPlayer->has(PhysBody)->mExtents.x * mTransform->mScale.x)/4);
		int y_step, x_step, dx, dy, i;

		//deals with left quadrants
		if(x0 > x1)
			x_step = -1;
		else
			x_step = 1;

		if(y0 > y1)
			y_step = -1;
		else
			y_step = 1;

		dx = (x1 - x0)*x_step; //done here to keep dx positive
		dy = (y1 - y0)*y_step; //done here to keep dy bassed of dx

		if(dy > dx) //slope is greater than 45
		{
			int x = x0,
				dn = 2*dx,
				d0 = dn - dy,
				dne = d0 - dy;

			//x_step *= halfPlayerSize;
			y_step *= halfPlayerSize;

			if(y0 < y1)
			{
				for(i = y0; i <= y1; i += y_step)
				{
					//SetPixel(x, i, r, g, b);
					for(std::list<PhysBody*>::iterator it = PHYSICS->mCollidableList.begin(); it != PHYSICS->mCollidableList.end(); ++it)
					{
						//get the first points

						//dont count objects that dont need to be considered
						if(!((*it)->mPhysFlags & PhysFlags::FUN) && !((*it)->mPhysFlags & PhysFlags::HUD) && !((*it)->mPhysFlags & PhysFlags::BUL)  && !((*it)->mPhysFlags & PhysFlags::HELD) && !((*it)->mPhysFlags & PhysFlags::FStand))
						{
							bool collided = TurretCol(Vec2((float)x, (float)i),(*it));
							if(collided == true)
							{
								if((*it)->GetOwner()->has(Player))
								{
									if(mLockedOn == false)
									{
										Transform * player = mPlayerTrans;
										mOffsetCollided.x = x - player->mPosition.x;
										mOffsetCollided.y = i - player->mPosition.y;

										mLockedOnTimer = timeGetTime();
									}
									return true;
								}
								else
								{
									return false;
								}
							}
						}
					}
					for(int j = 0; j < halfPlayerSize; ++j)
					{
						if(d0 < 0) //North
						{
							d0 += dn;
						}
						else //NorthEast
						{
							d0 += dne;
							x  += x_step;
						}
					}
				}
			}
			else
			{
				for(i = y0; i >= y1; i += y_step)
				{
					//SetPixel(x, i, r, g, b);
					for(std::list<PhysBody*>::iterator it = PHYSICS->mCollidableList.begin(); it != PHYSICS->mCollidableList.end(); ++it)
					{
						//get the first points

						//dont count objects that dont need to be considered
						if(!((*it)->mPhysFlags & PhysFlags::FUN) && !((*it)->mPhysFlags & PhysFlags::HUD) && !((*it)->mPhysFlags & PhysFlags::BUL)  && !((*it)->mPhysFlags & PhysFlags::HELD) && !((*it)->mPhysFlags & PhysFlags::FStand))
						{
							bool collided = TurretCol(Vec2((float)x, (float)i),(*it));
							if(collided == true)
							{
								if((*it)->GetOwner()->has(Player))
								{
									if(mLockedOn == false)
									{
										Transform * player = mPlayerTrans;
										mOffsetCollided.x = x - player->mPosition.x;
										mOffsetCollided.y = i - player->mPosition.y;

										mLockedOnTimer = timeGetTime();
									}
									return true;
								}
								else
								{
									return false;
								}
							}
						}
					}
					for(int j = 0; j < halfPlayerSize; ++j)
					{
						if(d0 < 0) //North
						{
							d0 += dn;
						}
						else //NorthEast
						{
							d0 += dne;
							x  += x_step;
						}
					}
				}
			}
		}
		else //slope is less than or equal to 45
		{
			int y = y0,
				de = 2*dy,
				d0 = de - dx,
				dne = d0 - dx;

			x_step *= halfPlayerSize;
			//y_step *= halfPlayerSize;

			if(x0 < x1)
			{
				for(i = x0; i <= x1; i+=x_step)
				{
					//SetPixel(i, y, r, g, b);
					for(std::list<PhysBody*>::iterator it = PHYSICS->mCollidableList.begin(); it != PHYSICS->mCollidableList.end(); ++it)
					{
						//get the first points

						//dont count objects that dont need to be considered
						if(!((*it)->mPhysFlags & PhysFlags::FUN) && !((*it)->mPhysFlags & PhysFlags::HUD) && !((*it)->mPhysFlags & PhysFlags::BUL)  && !((*it)->mPhysFlags & PhysFlags::HELD) && !((*it)->mPhysFlags & PhysFlags::FStand))
						{
							bool collided = TurretCol(Vec2((float)i, (float)y),(*it));
							if(collided == true)
							{
								if((*it)->GetOwner()->has(Player))
								{
									if(mLockedOn == false)
									{
										Transform * player = mPlayerTrans;
										mOffsetCollided.x = i - player->mPosition.x;
										mOffsetCollided.y = y - player->mPosition.y;

										mLockedOnTimer = timeGetTime();
									}
									return true;
								}
								else
								{
									return false;
								}
							}
						}
					}
					for(int j = 0; j < halfPlayerSize; ++j)
					{
						if(d0 < 0) //East
						{
							d0 += de;
						}
						else //NorthEast
						{
							d0 += dne;
							y  += y_step;
						}
					}
				}
			}
			else
			{
				for(i = x0; i >= x1; i+=x_step)
				{
					//SetPixel(i, y, r, g, b);
					for(std::list<PhysBody*>::iterator it = PHYSICS->mCollidableList.begin(); it != PHYSICS->mCollidableList.end(); ++it)
					{
						//get the first points

						//dont count objects that dont need to be considered
						if(!((*it)->mPhysFlags & PhysFlags::FUN) && !((*it)->mPhysFlags & PhysFlags::HUD) && !((*it)->mPhysFlags & PhysFlags::BUL)  && !((*it)->mPhysFlags & PhysFlags::HELD) && !((*it)->mPhysFlags & PhysFlags::FStand))
						{
							bool collided = TurretCol(Vec2((float)i, (float)y),(*it));
							if(collided == true)
							{
								if((*it)->GetOwner()->has(Player))
								{
									if(mLockedOn == false)
									{
										Transform * player = mPlayerTrans;
										mOffsetCollided.x = i - player->mPosition.x;
										mOffsetCollided.y = y - player->mPosition.y;

										mLockedOnTimer = timeGetTime();
									}
									return true;
								}
								else
								{
									return false;
								}
							}
						}
					}
					for(int j = 0; j < halfPlayerSize; ++j)
					{
						if(d0 < 0) //East
						{
							d0 += de;
						}
						else //NorthEast
						{
							d0 += dne;
							y  += y_step;
						}
					}
				}
			}
		}
		return false;
	}

	/*****************************************************************************/
	/*!
	\brief
		point in box col
	*/
	/*****************************************************************************/
	bool Turret::TurretCol(Vec2 B1, PhysBody* B2)
	{
		int x0 = (int)(B2->mTransform->mPosition.x + B2->mTransform->mScale.x * B2->mExtents.x);
		int x1 = (int)(B2->mTransform->mPosition.y + B2->mTransform->mScale.y * B2->mExtents.y);
		int x2 = (int)(B2->mTransform->mPosition.x - B2->mTransform->mScale.x * B2->mExtents.x);
		int x3 = (int)(B2->mTransform->mPosition.y - B2->mTransform->mScale.y * B2->mExtents.y);

		if(B1.x < x0 && B1.x > x2 && B1.y < x1 && B1.y > x3)
		{
			return true;
		}
		return false;
	}
	Mid::MessageIdType Turret::SendMessage(Message* message)
	{
		return Mid::None;
	}

	std::string Turret::ToString()
	{
		return "Logic Component: Turret";
	}

	void Turret::ToBar()
	{
		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");

		TwAddVarRO(bar, "Logic Component", TW_TYPE_STDSTRING, &mName, " help='Type of logical component.' ");
	}

	/*****************************************************************************/
	/*!
	\brief
		Removes turret from the logic list
	*/
	/*****************************************************************************/
	Turret::~Turret()
	{
		LOGIC->mLogicList.erase(reinterpret_cast<ILogical*>(this));
	}
}