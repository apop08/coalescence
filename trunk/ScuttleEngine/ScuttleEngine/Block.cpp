/**************************************************************************************************/
/*!
\file	Block.cpp
\par	Project: Coalescence
\date	10/26/2011
\authors
Primary  : Lincoln Tosi

\brief
The logic for the block states (sollid, gel, and gas).

\copyright 
“All content © 2011 DigiPen (USA) Corporation, all rights reserved.”
*/
/**************************************************************************************************/
#include "Precompiled.h"
#include "Block.h"
#include "GameLogic.h"

#include "Core.h"

#include "TutBox.h"
#include "DebugConsole.h"
#include "ParticleEmitter.h"
#include "DebugBars.h"
#include "GameStateMgr.h"

#include "SoundNode.h"


namespace Scuttlemeyer
{
	const float PICKUP_RANGE = 140.0f;
	const float HOLDING_OFFSET_SCALE_X = 1.12f;
	const float HOLDING_OFFSET_SCALE_Y = 1.15f;
	const float ARM_OFFSET_X = 0.0f;
	const float ARM_OFFSET_Y = 20.0f;
	const float THROW_FORCE = 100.0f;
	const float DROP_FORCE = 1.0f;

	/*****************************************************************************/
	/*!
		\brief
			Constructor - Initializes necessary variables.
	*/
	/*****************************************************************************/
	Block::Block()
	{
		mName = "Block";
		mSelected = false;
		mHolding = false;
		mFixed = false;
		mStateChanged = false;
		mDropSound = false;
		mDropSoundFlag = false;
	}

	/*****************************************************************************/
	/*!
		\brief
			Initializes variables.
			Adds this object to the logic objects list.
	*/
	/*****************************************************************************/
	void Block::Initialize()
	{
		LOGIC->mLogicList.push_back(reinterpret_cast<ILogical*>(this));
		FACTORY->RegisterForMessages(GetOwner());

		mParent = GetOwner();
		pDrawable = LOGIC->mPlayer->has(CompSprite);
		pTransform = LOGIC->mPlayer->has(Transform);
		mTransform = mParent->has(Transform);
		mPhysics = mParent->has(PhysBody);
		mSprite = mParent->has(Sprite);

		mPhysics->mColFlags |= 1;
		mStateChanged = false;
	}

	/*****************************************************************************/
	/*!
		\brief
			Swaps this objects state to nState and makes the necessary changes
			(like unsticking and resetting velocity).

		\param nState
			The state to change to.

		\param veloReset
			Whether or not to reset the objects velocity (only used in Block.cpp).
	*/
	/*****************************************************************************/
	int Block::ChangeState(EState nState, bool veloReset)
	{
		if (mHolding == true && !veloReset)
			return 1;

		if (mStateChanged == true)
			return 0;

		switch (nState)
		{
		case Hot:
			{
				if (mParent->state == Gas)
				{
					return 1;
				}
				else if (mParent->state == Solid || mParent->state == Liquid)
				{
					mDropSound = false;
					mStateChanged = true;

					if (mParent->state == Solid)
						mParent->has(SoundNode)->Start(0);
					else
						mParent->has(SoundNode)->Start(0);

					GOC* nBlock = LOGIC->SpawnObj("BlockGas", mTransform, Gas);
					PhysBody* nPhysics = nBlock->has(PhysBody);

					if (!veloReset)
						nPhysics->mVelocity = mPhysics->mVelocity;
					if (mHolding)
						Release();
					mParent->Destroy();
				}
				break;
			}
		case Cold:
			{
				if (mParent->state == Gas || mParent->state == Liquid)
				{
					mStateChanged = true;

					if(mParent->state == Gas)
						mParent->has(SoundNode)->Start(1);
					else
						mParent->has(SoundNode)->Start(1);

					GOC* nBlock = LOGIC->SpawnObj("Block", mTransform, Solid);
					PhysBody* nPhysics = nBlock->has(PhysBody);

					if (!veloReset)
						nPhysics->mVelocity = mPhysics->mVelocity;
					if (mHolding)
						Release();
					mParent->Destroy();
				}
				break;
			}
		case Warm:
			{
				if (mParent->state == Solid || mParent->state == Gas)
				{
					mDropSound = false;
					mStateChanged = true;

					if(mParent->state == Solid)
						GetOwner()->has(SoundNode)->Start(1);
					else
						GetOwner()->has(SoundNode)->Start(0);

					GOC* nBlock = LOGIC->SpawnObj("BlockLiquid", mTransform, Liquid);
					PhysBody* nPhysics = nBlock->has(PhysBody);

					if (!veloReset)
						nPhysics->mVelocity = mPhysics->mVelocity;
					if (mHolding)
						Release();
					mParent->Destroy();
				}
				break;
			}
		}

		return 0;
	}

	/*****************************************************************************/
	/*!
		\brief
			Called every frame. If the block is being held by the player, update
			its position accordingly. If the block is a liquid, update the sticks.

		\param dt
			The frame time.
	*/
	/*****************************************************************************/
	void Block::Update(float dt)
	{
		if(mDropSoundFlag)
		{
			mDropSound = true;
			mDropSoundFlag = false;
		}

		if (mHolding == true)
		{
			int facingScaler = pDrawable->GetFacing() == true ? 1 : -1;
			float cannonRotation = ((pDrawable->mComponents[1]->mLocalTransform->mRotation * facingScaler * cDelicious) / 180) + cDirUp + pTransform->mRotation;

			// This offsets the hold more, based on the size of the block
			float bx = floor(pTransform->mPosition.x) + (ARM_OFFSET_X * facingScaler) + (HOLDING_OFFSET_SCALE_X * mSprite->mSize.x * mTransform->mScale.x) * cos(cannonRotation);
			float by = floor(pTransform->mPosition.y) + ARM_OFFSET_Y + (HOLDING_OFFSET_SCALE_Y * mSprite->mSize.y * mTransform->mScale.y) * sin(cannonRotation);

			// Velocity must be set to zero or else the collision box will fall
			ResetVelocity();

			Transform temp = *mTransform;

			// Hold the block at the cannon's tip
			mTransform->mPosition.x = bx;
			mTransform->mPosition.y = by;

			Vec2* temp2;

			if (temp2 = PHYSICS->CheckBlock(mPhysics, dt))
			{
				mTransform->mPosition.x -= temp2->x * 1.001f;
				mTransform->mPosition.y -= temp2->y * 1.001f;

				if (temp2 = PHYSICS->CheckBlock(mPhysics, dt))
				{
					mTransform->mPosition.x -= temp2->x * 1.001f;
					mTransform->mPosition.y -= temp2->y * 1.001f;
				}
				if (DistanceSquared(mTransform->mPosition, pTransform->mPosition) > (120 * 120))
				{
					mTransform->mPosition.x = temp.mPosition.x;
					mTransform->mPosition.y = temp.mPosition.y;
				}
				if (DistanceSquared(mTransform->mPosition, pTransform->mPosition) > (150 * 150))
				{
					Throw(0);
				}
			}
		}
		else if (mParent->state == Liquid)
		{
			if (!UpdateStuckList())
				return;

			bool stuck = false;

			for (unsigned i = 0; i < stuckList.size(); ++i)
			{
				Transform* aTransform = stuckList[i].anchor->has(Transform);
				PhysBody* aPhysics = stuckList[i].anchor->has(PhysBody);
				Block* aBlock = stuckList[i].anchor->has(Block);

				if (aTransform)
				{
					if (i == 0)
					{
						// Base position off of the first thing in the list
						// Calculate original offsets if they don't exist
						if (!stuckList[i].offset.x && !stuckList[i].offset.y)
							stuckList[i].offset = mTransform->mPosition - aTransform->mPosition;

						if (aBlock)
						{
							if (stuckList[i].anchor->state == Solid)
							{
								// Case 1: Stuck to a solid
								if (mTransform->mPosition.y >= aTransform->mPosition.y)
								{
									// Above the solid (no collision threat), set our position
									mTransform->mPosition = aTransform->mPosition + stuckList[i].offset;
									mFixed = true;
									stuck = true;
								}
								else if (aPhysics && aPhysics->mVelocity.x < 0.00001f && aPhysics->mVelocity.y < 0.00001f)
								{
									// Below the solid (solid has stopped moving for some reason), set our position
									mTransform->mPosition = aTransform->mPosition + stuckList[i].offset;
									mFixed = true;
									stuck = true;
								}
								else
								{
									// Below the solid (solid is falling), don't make it fall slowly
									mFixed = false;
								}
							}
							else
							{
								// Case 2: Stuck to a liquid
								if (mTransform->mPosition.y > aTransform->mPosition.y || mFixed)
								{
									// Above the liquid or fixed, set our position
									mTransform->mPosition = aTransform->mPosition + stuckList[i].offset;
									stuck = true;
								}
								else
								{
									// Below the liquid and not fixed, don't make it fall slowly
									mFixed = false;
									stuck = false;
								}
							}
						}
						else
						{
							// Case 3: Stuck to a static object, set our position and reset velocity
							mTransform->mPosition = aTransform->mPosition + stuckList[i].offset;
							mFixed = true;
							stuck = true;
						}
					}
					else if (aBlock && stuckList[i].anchor->state == Solid)
					{
						// Everything else in the list is stuck to us
						// Calculate original offsets if they don't exist
						if (!stuckList[i].offset.x && !stuckList[i].offset.y)
							stuckList[i].offset = aTransform->mPosition - mTransform->mPosition;

						// Set the position of the object
						aTransform->mPosition = mTransform->mPosition + stuckList[i].offset;

						// Check the position, if colliding with static fix and offset liquid
						Vec2* temp2;

						if (temp2 = PHYSICS->CheckBlock(aPhysics, dt))
						{
							aTransform->mPosition.x -= temp2->x * 1.001f;
							aTransform->mPosition.y -= temp2->y * 1.001f;

							if (temp2 = PHYSICS->CheckBlock(aPhysics, dt))
							{
								aTransform->mPosition.x -= temp2->x * 1.001f;
								aTransform->mPosition.y -= temp2->y * 1.001f;
							}

							Transform* cTransform = stuckList[0].anchor->has(Transform);
							Block* cBlock = stuckList[0].anchor->has(Block);

							if (cBlock && stuckList[0].anchor->state == Solid)
								SwapAnchor(stuckList[i].anchor);

							mFixed = true;
							stuck = true;
						}

						aBlock->ResetVelocity();
					}
					else
						stuck = true;
				}
			}

			if (stuck)
				ResetVelocity();
		}
		else if(mParent->state == Solid)
		{
			if(!mDropSound)
			{
				if(abs(mPhysics->mVelocity.y) > 50.f)
				{
					mDropSoundFlag = true;
				}
			}
		}

		if((LOGIC->mPlayer->has(Player)->mTutFlag & PickUp) && (gGameStateCurr != GS_PAUSE))
		{
			if ((mHolding == false) && (LOGIC->isHolding == false) && (GetOwner()->state != Gas))
			{
				if(isWithinRange() && isMouseOn())
				{
					LOGIC->mCurCursor = 1;
				}
			}
			else if(mHolding == true)
			{
				if(!inWall())
				{
					LOGIC->mCurCursor = 3;
				}
				else
				{
					LOGIC->mCurCursor = 4;
				}
			}
		}
	}

	/*****************************************************************************/
	/*!
		\brief
			Recieves messages sent from other places in the code. Used for
			grabbing and throwing/dropping the blocks.

		\param message
			The message being recieved.

		\return
			The message type to be deleted.
	*/
	/*****************************************************************************/
	Mid::MessageIdType Block::SendMessage(Message* message)
	{
		if((LOGIC->mPlayer->has(Player)->mTutFlag & PickUp) && (gGameStateCurr != GS_PAUSE))
		{
			// To grab an object we can't already be holding one, and it can't be a gas
			if ((mHolding == false) && (LOGIC->isHolding == false) && (GetOwner()->state != Gas))
			{
				// Make sure we're selecting it and it's within our reach
				// This check doubles as a size limiter (except for liquids)
				if (isWithinRange() && isMouseOn())
				{
					if (message->mMessageId == Mid::RBUTTONDOWN)
					{
						mSelected = true;
						return Mid::RBUTTONDOWN;
					}
					else if (message->mMessageId == Mid::RBUTTONUP && mSelected)
					{
						Grab();
						mSelected = false;
						return Mid::RBUTTONUP;
					}
					//else if (message->mMessageId == Mid::MOUSEMOVE)
					//{
					//	// Cursor hover
					//}
				}
				else if (message->mMessageId == Mid::RBUTTONUP)
					mSelected = false;
			}
			else if (mHolding == true)
			{
				if (message->mMessageId == Mid::RBUTTONDOWN)
				{
					if (!inWall())
					{
						Throw(DROP_FORCE);
					}
					else
					{
						mParent->has(SoundNode)->Start(5);
					}

					return Mid::RBUTTONDOWN;
				}
				if (message->mMessageId == Mid::LBUTTONDOWN)
				{
					if ((LOGIC->mPlayer->has(Player)->mTutFlag & Throwable) && !inWall())
					{
						LOGIC->fireTimer = timeGetTime();
						Throw(THROW_FORCE);
						mParent->has(SoundNode)->Start(4);

						return Mid::LBUTTONDOWN;
					}
					else
					{
						mParent->has(SoundNode)->Start(5);
					}
				}
			}
		}

		return Mid::None;
	}

	/*****************************************************************************/
	/*!
		\brief
			Converts the type of this object into a string.

		\return
			The string containing this object type.
	*/
	/*****************************************************************************/
	std::string Block::ToString()
	{
		return "Logic Component: Block";
	}

	/*****************************************************************************/
	/*!
		\brief
			Adds this object to the debug display.
	*/
	/*****************************************************************************/
	void Block::ToBar()
	{
		TwBar* bar = DEBUGBARS->GetBarByName("ObjectProperties");
		TwAddVarRO(bar, "Logic Component", TW_TYPE_STDSTRING, &mName, " help='Type of logical component.' ");
	}

	/*****************************************************************************/
	/*!
		\brief
			Deconstructor - removes this object from the object list.
	*/
	/*****************************************************************************/
	Block::~Block()
	{
		UnStickAll();
		LOGIC->mLogicList.erase(reinterpret_cast<ILogical*>(this));
		FACTORY->UnregisterForMessages(GetOwner());
	}

	/*****************************************************************************/
	/*!
		\brief
			Checks if the block is inside of another object.

		\return
			true if inside something else, false if not.
	*/
	/*****************************************************************************/
	bool Block::inWall()
	{
		float width = (mSprite->mSize.x * mTransform->mScale.x) / 2.0f;
		maxExtents.x = mTransform->mPosition.x + width;
		minExtents.x = mTransform->mPosition.x - width;
		float height = (mSprite->mSize.y * mTransform->mScale.y) / 2.0f;
		maxExtents.y = mTransform->mPosition.y + height;
		minExtents.y = mTransform->mPosition.y - height;

		ObjectLinkedList<ILogical>::iterator it;
		for (it = LOGIC->mLogicList.begin(); it != LOGIC->mLogicList.end(); ++it)
		{
			GOC* parent = it->GetOwner();
			if (!(parent->has(BlockWin)) && !(parent->has(TutBox)) && !(parent->has(BiDirectionalFan)) && !(parent->has(Fan)) && !(parent->has(Rocket)) && (parent != GetOwner()) && CheckExtents(parent))
				return true;
		}
		return false;
	}

	/*****************************************************************************/
	/*!
		\brief
			Checks if the block is inside of another object.

		\param parent
			The object to check against.

		\return
			true if colliding, false if not.
	*/
	/*****************************************************************************/
	bool Block::CheckExtents(GOC* parent)
	{
		PhysBody * aPhysics = parent->has(PhysBody);
		Transform* aTransform = parent->has(Transform);

		if (aPhysics && aTransform)
		{
			// check bounds
			float width = aPhysics->mExtents.x * aTransform->mScale.x;
			float x_max = aTransform->mPosition.x + width;
			float x_min = aTransform->mPosition.x - width;

			float height = aPhysics->mExtents.y * aTransform->mScale.y;
			float y_max = aTransform->mPosition.y + height;
			float y_min = aTransform->mPosition.y - height;

			if ((maxExtents.x < x_max && maxExtents.x > x_min)
				|| (minExtents.x < x_max && minExtents.x > x_min)
				|| (maxExtents.x > x_max && minExtents.x < x_min))
			{
				if ((maxExtents.y < y_max && maxExtents.y > y_min)
					|| (minExtents.y < y_max && minExtents.y > y_min)
					|| (maxExtents.y > y_max && minExtents.y < y_min))
					return true;
			}
		}

		return false;
	}

	/*****************************************************************************/
	/*!
		\brief
			Checks if the mouse is positioned over the block.

		\return
			true if over the block, false if not.
	*/
	/*****************************************************************************/
	bool Block::isMouseOn()
	{
		if (gGameStateCurr != GS_PLAY)
			return false;

		float mx = CORE->MouseInput.MousePosition.x - GRAPHICS->mScreenWidth / 2.f;
		float my = CORE->MouseInput.MousePosition.y - GRAPHICS->mHeightRatio * GRAPHICS->mCameraOffset - GRAPHICS->mScreenHeight / 2.f;
		float dx = abs(mx - GRAPHICS->mWidthRatio * mTransform->mPosition.x + GRAPHICS->mWidthRatio * pTransform->mPosition.x);	// CHANGE
		float dy = abs(my + GRAPHICS->mHeightRatio * mTransform->mPosition.y - GRAPHICS->mHeightRatio * pTransform->mPosition.y);	// CHANGE

		if ((dx < GRAPHICS->mWidthRatio * mSprite->mSize.x * mTransform->mScale.x / 2.f)
			&& (dy < GRAPHICS->mHeightRatio * mSprite->mSize.y * mTransform->mScale.y / 2.f))
			return true;

		return false;
	}

	/*****************************************************************************/
	/*!
		\brief
			Checks if the block is close enough for the player to grab.

		\return
			true if close enough, false if not.
	*/
	/*****************************************************************************/
	bool Block::isWithinRange()
	{
		if ((abs(mTransform->mPosition.x - pTransform->mPosition.x) < PICKUP_RANGE)
			&& (abs(mTransform->mPosition.y - pTransform->mPosition.y) < PICKUP_RANGE))
			return true;

		return false;
	}

	/*****************************************************************************/
	/*!
		\brief
			Makes the player hold the block.
	*/
	/*****************************************************************************/
	void Block::Grab()
	{
		UnStickAll();
		mPhysics->mPhysFlags = PhysFlags::HELD;
		pDrawable->mComponents[1]->GoToFrame(0.0f);
		LOGIC->isHolding = true;
		mHolding = true;
		mParent->has(SoundNode)->Start(3);

		if(mParent->state == Solid)
		{
			mDropSound = true;
		}
	}

	/*****************************************************************************/
	/*!
		\brief
			Makes the player throw the block.

		\param force
			The velocity to apply to the block.
	*/
	/*****************************************************************************/
	void Block::Throw(float force)
	{
		ResetVelocity();

		if (mParent->state == Solid)
			mPhysics->mPhysFlags = PhysFlags::FGravity;
		else
			mPhysics->mPhysFlags = PhysFlags::LIQ | PhysFlags::FGravity;

		int facingScaler = pDrawable->GetFacing() == true ? 1 : -1;
		float cannonRotation = ((pDrawable->mComponents[1]->mLocalTransform->mRotation * facingScaler * cDelicious) / 180) + cDirUp + pTransform->mRotation;
		mPhysics->AddForce(force, cannonRotation);
		Release();
	}

	/*****************************************************************************/
	/*!
		\brief
			Resets the holding flags and the player's arm to holding a gun.
	*/
	/*****************************************************************************/
	void Block::Release()
	{
		pDrawable->mComponents[1]->GoToFrame(1.0f);
		LOGIC->isHolding = false;
		mHolding = false;
	}

	/*****************************************************************************/
	/*!
		\brief
			Update the list containing all of the objects this block is stuck to
			(both in solid and liquid form).

		\param target
			The object to stick this block to.

		\param notAgain
			If this is false, call stick on the target to add this block to its
			stuck list as well. This way they are both aware of the connection.
	*/
	/*****************************************************************************/
	void Block::Stick(GOC* target, bool notAgain)
	{
		if (mHolding
			|| target->has(Player)
			|| target->has(Bullet)
			|| target->has(Rocket)
			|| target->has(BiDirectionalFan)
			|| target->has(Fan)
			|| target->has(BrokenPipe)
			|| target->has(BlockWin)
			|| target->has(KillZone)
			)
			return;


		// don't store duplicate anchors
		for (unsigned i = 0; i < stuckList.size(); ++i)
		{
			if (stuckList[i].anchor == target)
				return;
		}

		if (Switch* aSwitch = target->has(Switch))
		{
				aSwitch->Activate(true);
		}

		if(!notAgain)
		{
			mParent->has(SoundNode)->Start(2);
			
		}

		Block* aBlock = target->has(Block);

		if (stuckList.size() > 0 && stuckList[0].anchor->has(Block) && !(aBlock))
		{
			// If stuck to a movable object and trying to stick to a static one
			// swap so that we stick to the static object
			Sticker temp = stuckList[0];
			temp.offset = -(temp.offset);
			stuckList.push_back(temp);

			stuckList[0].anchor = target;
			stuckList[0].offset.x = 0.0f;
			stuckList[0].offset.y = 0.0f;
			ResetVelocity();
			mFixed = true;
		}
		else if (aBlock)
		{
			// If trying to stick to another block
			if (!(aBlock->mHolding) && target->state != Gas)
			{
				Sticker temp;
				temp.anchor = target;
				temp.offset.x = 0.0f;
				temp.offset.y = 0.0f;
				stuckList.push_back(temp);

				ResetVelocity();
				if (stuckList.size() == 1)
					mFixed = aBlock->isFixed();

				// Add this block to the other one's stuck list
				// In other words make sure they both know about the stick in case we grab one
				if (!notAgain)
					aBlock->Stick(mParent, true);
			}
		}
		else
		{
			// Otherwise, stick normally
			Sticker temp;
			temp.anchor = target;
			temp.offset.x = 0.0f;
			temp.offset.y = 0.0f;
			stuckList.push_back(temp);
			ResetVelocity();
			mFixed = true;
		}
	}

	/*****************************************************************************/
	/*!
		\brief
			Removes a specific object from this block's stuck list.

		\param target
			The object to unstick from.
	*/
	/*****************************************************************************/
	void Block::UnStick(GOC* target)
	{
		// Remove a single connection from a specific target
		for (unsigned i = 0; i < stuckList.size(); ++i)
		{
			if (stuckList[i].anchor == target)
			{
				// If we remove the base anchor, update the offset to be correct
				if (i == 0)
				{
					mFixed = false;
					if (stuckList.size() > 1)
						stuckList[1].offset = -(stuckList[1].offset);
				}

				stuckList.erase(stuckList.begin() + i);
				UpdateStuckList();
				break;
			}
		}
	}

	/*****************************************************************************/
	/*!
		\brief
			Remove all objects from this block's stuck list as well as removing
			this block from their lists.
	*/
	/*****************************************************************************/
	void Block::UnStickAll()
	{
		// Remove all connections to other objects
		for (unsigned i = 0; i < stuckList.size(); ++i)
		{
			if (Block* aBlock = stuckList[i].anchor->has(Block))
				aBlock->UnStick(GetOwner());
			if (Switch* aSwitch = stuckList[i].anchor->has(Switch))
				aSwitch->Deactivate(true);
		}

		stuckList.clear();
		mFixed = false;
	}

	/*****************************************************************************/
	/*!
		\brief
			Resets this block's velocity to (0,0).
	*/
	/*****************************************************************************/
	void Block::ResetVelocity()
	{
		if (mPhysics)
		{
			mPhysics->mVelocity.x = 0.0f;
			mPhysics->mVelocity.y = 0.0f;
		}
	}

	/*****************************************************************************/
	/*!
		\brief
			Updates this block's stuck list to reflect the best anchor possible.
	*/
	/*****************************************************************************/
	bool Block::UpdateStuckList()
	{
		// Some sort of error, return false to avoid extra calculations
		if (!mTransform || !(stuckList.size()))
			return false;

		// No need to update if the list has one/static anchor
		Block* anchorObj = stuckList[0].anchor->has(Block);
		if (!anchorObj)
		{
			mFixed = true;
			return true;
		}
		if (stuckList.size() == 1 || anchorObj->isFixed())
			return true;

		float min_y = mTransform->mPosition.y;
		int min = 0;

		for (unsigned i = 1; i < stuckList.size(); ++i)
		{
			Transform* aTransform = stuckList[i].anchor->has(Transform);
			if (aTransform)
			{
				Block* aBlock = stuckList[i].anchor->has(Block);

				// (Best Case) Swap to a static anchor and finish
				if (!aBlock)
				{
					// Create a copy of the old anchor
					Sticker temp = stuckList[0];
					temp.offset = -(temp.offset);
					stuckList.push_back(temp);

					// Replace the old anchor with the static one
					stuckList[0].anchor = stuckList[i].anchor;
					stuckList[0].offset = -(stuckList[i].offset);

					// Remove the old entry from the list
					stuckList.erase(stuckList.begin() + i);

					// Stuck to a static object, let other stuck blocks know
					for (unsigned j = 1; j < stuckList.size(); ++j)
					{
						Block* cBlock = stuckList[j].anchor->has(Block);
						if (cBlock)
							cBlock->SwapAnchor(GetOwner());
					}

					ResetVelocity();
					mFixed = true;
					return true;
				}
				// Keep track of the object with the lowest y value
				else if (aTransform->mPosition.y < min_y)
				{
					min_y = aTransform->mPosition.y;
					min = i;
				}
			}
		}

		// Lower anchor available, swap
		if ((min_y < mTransform->mPosition.y) && !(stuckList[0].anchor->state == Solid && stuckList[min].anchor->state == Solid))
		{
			// Create a copy of the old anchor
			Sticker temp = stuckList[0];
			temp.offset = -(temp.offset);
			stuckList.push_back(temp);

			// Replace the old anchor with the static one
			stuckList[0].anchor = stuckList[min].anchor;
			stuckList[0].offset = -(stuckList[min].offset);

			// Remove the old entry from the list
			stuckList.erase(stuckList.begin() + min);
		}

		return true;
	}

	/*****************************************************************************/
	/*!
		\brief
			Manually changes this object's anchor to the target object.

		\param target
			The object to set as this block's new anchor.
	*/
	/*****************************************************************************/
	void Block::SwapAnchor(GOC* target)
	{
		// Make this object the new anchor
		for (unsigned i = 0; i < stuckList.size(); ++i)
		{
			if (stuckList[i].anchor == target)
			{
				// Create a copy of the old anchor
				Sticker temp = stuckList[0];
				temp.offset = -(temp.offset);
				stuckList.push_back(temp);

				// Replace the old anchor with the new one
				stuckList[0].anchor = stuckList[i].anchor;
				stuckList[0].offset = -(stuckList[i].offset);

				// Remove the old entry from the list
				stuckList.erase(stuckList.begin() + i);

				ResetVelocity();
				mFixed = true;
				break;
			}
		}
	}

	/*****************************************************************************/
	/*!
		\brief
			Returns if the object is fixed (velocity and position based off of
			another object) or not.

		\return
			If the object is fixed (velocity and position based off of
			another object) or not.
	*/
	/*****************************************************************************/
	bool Block::isFixed()
	{
		return mFixed;
	}
}